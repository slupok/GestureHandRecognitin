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
