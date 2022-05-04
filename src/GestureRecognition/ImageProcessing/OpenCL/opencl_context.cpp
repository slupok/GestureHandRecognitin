#include "opencl_context.h"
#include <QDebug>

OpenclContext::OpenclContext() : IPContext()
{
    //m_device = new OpenclDevice();
    m_device = new OpenclDevice();

    cl_int error;
    m_queue = clCreateCommandQueue(m_device->m_context, m_device->m_device, 0, &error);
}
OpenclContext::~OpenclContext()
{
    delete m_device;
    //очереди еще хранятся в openclImage и как с ними быть?
    //выносить операции из IPImage в IPContext? такие как Read/Write
    clReleaseCommandQueue(m_queue);
}

IPImage* OpenclContext::CreateImage(uint w, uint h, PixelType pixelType, void *data)
{
    cl_int error;
    cl_mem imageBuffer;
    //cl_mem bitmap;
    if(data)
        imageBuffer = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                             IPImage::IP_GetBytesPerPixel(pixelType) * w * h, data, &error);
    else
        imageBuffer = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE,
                             IPImage::IP_GetBytesPerPixel(pixelType) * w * h, nullptr, &error);
    //bitmap = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE,
                            //sizeof(char) * w * h, nullptr, &error);

    IPImage *img = new OpenclImage(m_queue, imageBuffer, w, h, pixelType);
    return img;
}

IPEnum OpenclContext::GetDeviceType()
{
    return IPDeviceOpenCL;
}

typedef struct Cluster{
    RGB_format center;
    int *positions;//хранит индексы
    int pointsCount;
} Cluster;

IPError OpenclContext::kMeans(int clustersCount, int iterationsCount, RGB_format recognitionColor)

{
    //доработать
    cl_int errorCode;

    OpenclImage *clImage;// = dynamic_cast<OpenclImage*>(m_image);
    int width = clImage->GetWidth();
    int height = clImage->GetHeight();

    Cluster *clusters = (Cluster*)malloc(sizeof(Cluster) * width * height * clustersCount);
    int step = 255/clustersCount;
    for(int i = 0; i < clustersCount; i++ )
    {
        clusters[i].positions = (int*)malloc(sizeof (int) * width * height);
        clusters[i].pointsCount = 0;
        clusters[i].center = {(unsigned char)(i*step),
                              (unsigned char)(i*step),
                              (unsigned char)(i*step)};
    }

    long *sum_r = (long*)malloc(sizeof(long) * clustersCount);
    long *sum_g = (long*)malloc(sizeof(long) * clustersCount);
    long *sum_b = (long*)malloc(sizeof(long) * clustersCount);

    cl_mem sum_r_mem = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE, sizeof(long) * clustersCount, 0x0, &errorCode);
    cl_mem sum_g_mem = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE, sizeof(long) * clustersCount, 0x0, &errorCode);
    cl_mem sum_b_mem = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE, sizeof(long) * clustersCount, 0x0, &errorCode);

    long zeroPattern = 0;
    for(int i = 0; i < iterationsCount; i++)
    {
        clEnqueueFillBuffer(clImage->m_queue, sum_r_mem, &zeroPattern, sizeof(long), 0, sizeof(long) * clustersCount, 0, 0x0, 0x0);
        clEnqueueFillBuffer(clImage->m_queue, sum_g_mem, &zeroPattern, sizeof(long), 0, sizeof(long) * clustersCount, 0, 0x0, 0x0);
        clEnqueueFillBuffer(clImage->m_queue, sum_b_mem, &zeroPattern, sizeof(long), 0, sizeof(long) * clustersCount, 0, 0x0, 0x0);


    }


    free(clusters);
    free(sum_r);
    free(sum_g);
    free(sum_b);
    clReleaseMemObject(sum_r_mem);
    clReleaseMemObject(sum_g_mem);
    clReleaseMemObject(sum_b_mem);
    return IPNoError;
}

IPError OpenclContext::FrameDifference(IPImage *currentImage, IPImage *previousImage, IPImage *resultMask, uchar theshold)
{
    cl_int errorCode = 0;
    OpenclImage *clCurImage = dynamic_cast<OpenclImage*>(currentImage);
    OpenclImage *clPrevImage = dynamic_cast<OpenclImage*>(previousImage);
    OpenclImage *clMask = dynamic_cast<OpenclImage*>(resultMask);
    if(!clPrevImage || !clPrevImage || !clMask)
        return IPErrorExecute;
    //мб нужна проверка, что размеры равны
    size_t width = clCurImage->GetWidth();
    size_t height = clCurImage->GetHeight();

    size_t gridSize[2] = {(1 + (width - 1) / 256) * 256, (1 + (height - 1) / 256) * 256};
    size_t threadsPerBlock[2] = {16, 16};

    int arg = 0;
    errorCode = clSetKernelArg(m_device->m_frameDifferenceKernel, arg++, sizeof (cl_mem), &clCurImage->m_image);
    errorCode |= clSetKernelArg(m_device->m_frameDifferenceKernel, arg++, sizeof (cl_mem), &clPrevImage->m_image);
    errorCode |= clSetKernelArg(m_device->m_frameDifferenceKernel, arg++, sizeof (cl_mem), &clMask->m_image);
    errorCode |= clSetKernelArg(m_device->m_frameDifferenceKernel, arg++, sizeof (uchar), &theshold);
    errorCode |= clSetKernelArg(m_device->m_frameDifferenceKernel, arg++, sizeof (PixelType), &clCurImage->m_pixelType);
    errorCode |= clSetKernelArg(m_device->m_frameDifferenceKernel, arg++, sizeof (PixelType), &clPrevImage->m_pixelType);
    errorCode |= clSetKernelArg(m_device->m_frameDifferenceKernel, arg++, sizeof (int), &width);
    errorCode |= clSetKernelArg(m_device->m_frameDifferenceKernel, arg++, sizeof (int), &height);
    if(errorCode != 0)
        return IPErrorExecute;

    errorCode = clEnqueueNDRangeKernel(m_queue, m_device->m_frameDifferenceKernel, 2, 0x0, gridSize, threadsPerBlock, 0, 0x0, 0x0);
    if(errorCode != 0)
        return IPErrorExecute;

    errorCode = clFinish(m_queue);
    if(errorCode != 0)
        return IPErrorExecute;

    return errorCode == 0 ? IPNoError : IPErrorExecute;
}

IPError OpenclContext::ColorThresholdConversion(IPImage *image, IPImage *resultMask)
{
    if(image->GetPixelType() != PixelType::RGB24 || resultMask->GetPixelType() != PixelType::Grayscale8)
        return IPErrorDevice;


    cl_int errorCode;

    OpenclImage *clImage = dynamic_cast<OpenclImage*>(image);
    OpenclImage *clMask = dynamic_cast<OpenclImage*>(resultMask);
    size_t width = clImage->GetWidth();
    size_t height = clImage->GetHeight();

#if 0
    unsigned char *bytes = (unsigned char*)malloc(clImage->GetWidth() * clImage->GetHeight() * GetBytesPerPixel(clImage->GetPixelType()));
    for(int i = 0; i < clImage->GetWidth() * clImage->GetHeight() * GetBytesPerPixel(clImage->GetPixelType()); i++)
    {
        bytes[i] = 200;
    }
    clImage->WriteImage(bytes, image->GetWidth() * image->GetHeight() * GetBytesPerPixel(image->GetPixelType()));
    return IPNoError;
#endif

    size_t gridSize[2] = {(1 + (width - 1) / 256) * 256, (1 + (height - 1) / 256) * 256};
    size_t threadsPerBlock[2] = {16, 16};
    int arg = 0;
    errorCode = clSetKernelArg(m_device->m_thresholdColorKernel, arg++, sizeof (cl_mem), &clImage->m_image);
    errorCode |= clSetKernelArg(m_device->m_thresholdColorKernel, arg++, sizeof (cl_mem), &clMask->m_image);
    errorCode |= clSetKernelArg(m_device->m_thresholdColorKernel, arg++, sizeof (int), &width);
    errorCode |= clSetKernelArg(m_device->m_thresholdColorKernel, arg++, sizeof (int), &height);
    if(errorCode != 0)
        return IPErrorDevice;

    errorCode = clEnqueueNDRangeKernel(m_queue, m_device->m_thresholdColorKernel, 2, 0x0, gridSize, threadsPerBlock, 0, 0x0, 0x0);
    if(errorCode != 0)
        return IPErrorDevice;

    errorCode = clFinish(m_queue);
    if(errorCode != 0)
        return IPErrorDevice;

    return IPNoError;
}

IPError OpenclContext::GaussianBlur(IPImage *image, int radius, float sigma)
{
    cl_int errorCode;

    OpenclImage *clImage = dynamic_cast<OpenclImage*>(image);
    size_t width = clImage->GetWidth();
    size_t height = clImage->GetHeight();

    size_t gridSize[2] = {(1 + (width - 1) / 256) * 256, (1 + (height - 1) / 256) * 256};
    size_t threadsPerBlock[2] = {16, 16};
    int arg = 0;

    size_t length = width * height * GetBytesPerPixel(clImage->GetPixelType());
    cl_mem tmp_image = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE, length, 0x0, &errorCode);
    if(!tmp_image)
        return IPErrorOutOfMemory;
    clEnqueueCopyBuffer(m_queue, clImage->m_image, tmp_image, 0, 0,length, 0, 0x0, 0x0);

    int isHorizontal;

    int kDim = radius * 2 + 1;
    float *filter = (float*)malloc(kDim  * kDim * sizeof(float));
    float sum = 0.0;
    int index = 0;
    for (int y = 0; y < kDim; y++)
        for (int x = 0; x < kDim; x++)
        {
            index = x + kDim * y;
            filter[index] = exp( -0.5 * (pow((x-radius)/sigma, 2.0) + pow((y-radius)/sigma,2.0)) )
                             / (2 * M_PI * sigma * sigma);

            sum += filter[index];
        }
    for (int y = 0; y < kDim; y++)
        for (int x = 0; x < kDim; x++)
        {
            index = x + kDim * y;
            filter[index] /= sum;
        }

    cl_mem clFilter = clCreateBuffer(m_device->m_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*kDim*kDim, filter, &errorCode);
    if(!clFilter)
        goto finished;

    isHorizontal = 0;
    arg = 0;
    errorCode |= clSetKernelArg(m_device->m_separableGaussianBlurKernel, arg++, sizeof (cl_mem), &clImage->m_image);
    errorCode |= clSetKernelArg(m_device->m_separableGaussianBlurKernel, arg++, sizeof (cl_mem), &tmp_image);
    errorCode |= clSetKernelArg(m_device->m_separableGaussianBlurKernel, arg++, sizeof (cl_mem), &clFilter);
    errorCode |= clSetKernelArg(m_device->m_separableGaussianBlurKernel, arg++, sizeof (int), &radius);
    errorCode |= clSetKernelArg(m_device->m_separableGaussianBlurKernel, arg++, sizeof (int), &isHorizontal);
    errorCode |= clSetKernelArg(m_device->m_separableGaussianBlurKernel, arg++, sizeof (int), &width);
    errorCode |= clSetKernelArg(m_device->m_separableGaussianBlurKernel, arg++, sizeof (int), &height);
    errorCode |= clSetKernelArg(m_device->m_separableGaussianBlurKernel, arg++, sizeof (PixelType), &clImage->m_pixelType);
    if(errorCode != 0)
        goto finished;

    errorCode = clEnqueueNDRangeKernel(m_queue, m_device->m_separableGaussianBlurKernel, 2, 0x0, gridSize, threadsPerBlock, 0, 0x0, 0x0);
    if(errorCode != 0)
        goto finished;

    errorCode = clFinish(m_queue);
    if(errorCode != 0)
        goto finished;

finished:
    if(filter)
        free(filter);
    if(tmp_image)
        clReleaseMemObject(tmp_image);
    if(clFilter)
        clReleaseMemObject(clFilter);
    return errorCode == 0 ? IPNoError : IPErrorExecute;
}

IPError OpenclContext::MorphologicalDilation(IPImage *image, int radius)
{
    cl_int errorCode;

    OpenclImage *clImage = dynamic_cast<OpenclImage*>(image);
    size_t width = clImage->GetWidth();
    size_t height = clImage->GetHeight();

    size_t length = width * height * GetBytesPerPixel(clImage->GetPixelType());
    cl_mem tmp_image = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE, length, 0x0, &errorCode);
    if(!tmp_image)
        return IPErrorOutOfMemory;
    clEnqueueCopyBuffer(m_queue, clImage->m_image, tmp_image, 0, 0,length, 0, 0x0, 0x0);

    size_t gridSize[2] = {(1 + (width - 1) / 256) * 256, (1 + (height - 1) / 256) * 256};
    size_t threadsPerBlock[2] = {16, 16};
    int arg = 0;
    errorCode = clSetKernelArg(m_device->m_morphologicalDilationKernel, arg++, sizeof (cl_mem), &clImage->m_image);
    errorCode |= clSetKernelArg(m_device->m_morphologicalDilationKernel, arg++, sizeof (cl_mem), &tmp_image);
    errorCode |= clSetKernelArg(m_device->m_morphologicalDilationKernel, arg++, sizeof (int), &radius);
    errorCode |= clSetKernelArg(m_device->m_morphologicalDilationKernel, arg++, sizeof (int), &width);
    errorCode |= clSetKernelArg(m_device->m_morphologicalDilationKernel, arg++, sizeof (int), &height);
    //errorCode |= clSetKernelArg(m_device->m_separableGaussianBlurKernel, arg++, sizeof (PixelType), &clImage->m_pixelType);
    if(errorCode != 0)
        goto finished;

    errorCode = clEnqueueNDRangeKernel(m_queue, m_device->m_morphologicalDilationKernel, 2, 0x0, gridSize, threadsPerBlock, 0, 0x0, 0x0);
    if(errorCode != 0)
        goto finished;

    errorCode = clFinish(m_queue);
    if(errorCode != 0)
        goto finished;

finished:
    if(tmp_image)
        clReleaseMemObject(tmp_image);
    return errorCode == 0 ? IPNoError : IPErrorExecute;
}
IPError OpenclContext::MorphologicalErosion(IPImage *image, int radius)
{

    cl_int errorCode;

    OpenclImage *clImage = dynamic_cast<OpenclImage*>(image);
    size_t width = clImage->GetWidth();
    size_t height = clImage->GetHeight();

    size_t length = width * height * GetBytesPerPixel(clImage->GetPixelType());
    cl_mem tmp_image = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE, length, 0x0, &errorCode);
    if(!tmp_image)
        return IPErrorOutOfMemory;
    clEnqueueCopyBuffer(m_queue, clImage->m_image, tmp_image, 0, 0,length, 0, 0x0, 0x0);

    size_t gridSize[2] = {(1 + (width - 1) / 256) * 256, (1 + (height - 1) / 256) * 256};
    size_t threadsPerBlock[2] = {16, 16};
    int arg = 0;
    errorCode = clSetKernelArg(m_device->m_morphologicalErosionKernel, arg++, sizeof (cl_mem), &clImage->m_image);
    errorCode |= clSetKernelArg(m_device->m_morphologicalErosionKernel, arg++, sizeof (cl_mem), &tmp_image);
    errorCode |= clSetKernelArg(m_device->m_morphologicalErosionKernel, arg++, sizeof (int), &radius);
    errorCode |= clSetKernelArg(m_device->m_morphologicalErosionKernel, arg++, sizeof (int), &width);
    errorCode |= clSetKernelArg(m_device->m_morphologicalErosionKernel, arg++, sizeof (int), &height);
    //добавить передачу pixelType
    if(errorCode != 0)
        goto finished;

    errorCode = clEnqueueNDRangeKernel(m_queue, m_device->m_morphologicalErosionKernel, 2, 0x0, gridSize, threadsPerBlock, 0, 0x0, 0x0);
    if(errorCode != 0)
        goto finished;

    errorCode = clFinish(m_queue);
    if(errorCode != 0)
        goto finished;

finished:
    if(tmp_image)
        clReleaseMemObject(tmp_image);
    return errorCode == 0 ? IPNoError : IPErrorExecute;
}

IPError OpenclContext::CenterOfBitmap(IPImage *mask, int &x, int &y)
{
    cl_int errorCode = 0;
    int arg = 0;

    OpenclImage *clMask = dynamic_cast<OpenclImage*>(mask);
    if(!clMask)
        return IPErrorExecute;
    int width = 100;//clMask->GetWidth();
    int height = 100;//clMask->GetHeight();
    int imageSize = width * height;

    cl_kernel kernel = m_device->m_coordinateSummingKernel;

    int rank = (int)(log2(imageSize)) + 1;
    size_t gridSize = pow(2, rank) > 256 ? pow(2, rank) : 256;//(1 + ((width * height * 2) - 1) / 256) * 256;//в 2 раза будет достаточно
    size_t threadsPerBlock = 256;
    qDebug() << gridSize;
    int numBlock = gridSize / threadsPerBlock;

    uint count = 0;
    uint *partialCount = nullptr;
    uint *partialSum = nullptr;

    cl_mem resultSum = clCreateBuffer(m_device->m_context, CL_MEM_READ_ONLY, sizeof(uint) * numBlock, 0x0, &errorCode);
    cl_mem resultCount = clCreateBuffer(m_device->m_context, CL_MEM_READ_ONLY, sizeof(uint) * numBlock, 0x0, &errorCode);
    if(!resultSum || !resultCount)
        goto finish;

    arg = 0;
    errorCode = clSetKernelArg(kernel, arg++, sizeof (cl_mem), &clMask->m_image);
    errorCode |= clSetKernelArg(kernel, arg++, sizeof(uint) * threadsPerBlock, nullptr);
    errorCode |= clSetKernelArg(kernel, arg++, sizeof (cl_mem), &resultSum);
    errorCode |= clSetKernelArg(kernel, arg++, sizeof(uint) * threadsPerBlock, nullptr);
    errorCode |= clSetKernelArg(kernel, arg++, sizeof (cl_mem), &resultCount);
    errorCode |= clSetKernelArg(kernel, arg++, sizeof (int), &width);
    errorCode |= clSetKernelArg(kernel, arg++, sizeof (int), &height);
    if(errorCode != 0)
        goto finish;

    errorCode = clEnqueueNDRangeKernel(m_queue, kernel, 1, 0x0, &gridSize, &threadsPerBlock, 0, 0x0, 0x0);
    if(errorCode != 0)
        goto finish;

    errorCode = clFinish(m_queue);
    if(errorCode != 0)
        goto finish;

    partialCount = (uint*)malloc(numBlock * sizeof (uint));
    partialSum = (uint*)malloc(numBlock * sizeof (uint));

    clEnqueueReadBuffer(m_queue, resultCount, CL_TRUE, 0, numBlock * sizeof (uint), partialCount, 0, 0x0, 0x0);
    clEnqueueReadBuffer(m_queue, resultSum, CL_TRUE, 0, numBlock * sizeof (uint), partialSum, 0, 0x0, 0x0);

    count = 0;
    x = 0;
    y = 0;
    for(int i = 0; i < numBlock; i++)
    {
        count += partialCount[i];
        x += (partialSum[i] % width) / partialCount[i];
        y += (partialSum[i] / width) / partialCount[i];
    }
    qDebug() << count << " = " << width * height;
    qDebug() << x << " = " <<  x;
    qDebug() << y << " = " <<  y;

finish:
    if(partialCount)
        free(partialCount);
    if(partialSum)
        free(partialSum);
    if(resultSum)
        clReleaseMemObject(resultSum);
    if(resultCount)
        clReleaseMemObject(resultCount);
    return errorCode == 0 ? IPNoError : IPErrorExecute;
}

void OpenclContext::RunTest()
{
    cl_int error;
    cl_command_queue queue = clCreateCommandQueue(m_device->m_context, m_device->m_device, 0, &error);

    int sizeArray = 10;
    int* A = (int*)malloc(sizeArray * sizeof (int));
    int* B = (int*)malloc(sizeArray * sizeof (int));
    int* res = (int*)malloc(sizeArray * sizeof (int));

    for(int i = 0; i < sizeArray; i++)
    {
        A[i] = i;
        B[i] = sizeArray - i;
    }

    cl_mem a_mem = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeArray * sizeof (int), A, &error);
    cl_mem b_mem = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeArray * sizeof (int), B, &error);
    cl_mem res_mem = clCreateBuffer(m_device->m_context, CL_MEM_READ_WRITE, sizeArray * sizeof (int), 0x0, &error);

    error = clSetKernelArg(m_device->m_testKernel, 0, sizeof (cl_mem), &a_mem);
    error = clSetKernelArg(m_device->m_testKernel, 1, sizeof (cl_mem), &b_mem);
    error = clSetKernelArg(m_device->m_testKernel, 2, sizeof (cl_mem), &res_mem);

    size_t gridSize = sizeArray;
    size_t threadsPerBlock = sizeArray < 256? sizeArray : 256;

    clEnqueueNDRangeKernel(queue, m_device->m_testKernel, 1, 0x0, &gridSize, &threadsPerBlock, 0, 0x0, 0x0);
    clFinish(queue);

    clEnqueueReadBuffer(queue, res_mem, CL_TRUE, 0, sizeArray * sizeof (int), res, 0, 0x0, 0x0);

    for(int i = 0; i < sizeArray; i ++)
    {
        qDebug() << A[i] << " + " << B[i] << " = " << res[i];
    }
}

#if 0
IPError OpenclContext::AttachImage(IPImage *image)
{
    if(image->GetDeviceType() != IPDeviceOpenCL)
        return IPErrorDevice;
    m_image = image;

    return IPNoError;
}
#endif
