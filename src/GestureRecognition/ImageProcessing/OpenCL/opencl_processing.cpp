#include "../ipOpenCL.h"
#include <CL/cl.h>
#include <stdio.h>

//удалить можно

int test()
{
    cl_int error;

    cl_platform_id platform_id = 0x0;
    cl_uint num_platforms;
    error = clGetPlatformIDs(1, &platform_id, &num_platforms);

    cl_device_id device_id = 0x0;
    cl_uint num_devices;
    error = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);

    cl_context context = 0x0;
    context = clCreateContext(0x0, 1, &device_id, 0x0, 0x0, &error);
    cl_command_queue queue = clCreateCommandQueue(context, device_id, 0, &error);

    cl_program program = 0x0;
    //program = clCreateProgramWithSource(context, 1, (const char **)&source, (const size_t *)&sourceSize, &error);
    error = clBuildProgram(program, 1, &device_id, 0x0, 0x0, 0x0);
    cl_kernel kernel = clCreateKernel(program, "test", &error);

    int sizeArray = 10;
    int* A = (int*)malloc(sizeArray * sizeof (int));
    int* B = (int*)malloc(sizeArray * sizeof (int));
    int* res = (int*)malloc(sizeArray * sizeof (int));

    for(int i = 0; i < sizeArray; i++)
    {
        A[i] = i;
        B[i] = sizeArray - i;
    }


    cl_mem a_mem = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeArray * sizeof (int), A, &error);
    cl_mem b_mem = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeArray * sizeof (int), B, &error);
    cl_mem res_mem = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeArray * sizeof (int), 0x0, &error);

    error = clSetKernelArg(kernel, 0, sizeof (cl_mem), &a_mem);
    error = clSetKernelArg(kernel, 1, sizeof (cl_mem), &b_mem);
    error = clSetKernelArg(kernel, 2, sizeof (cl_mem), &res_mem);


    size_t gridSize = sizeArray;
    size_t threadsPerBlock = sizeArray < 256? sizeArray : 256;

    clEnqueueNDRangeKernel(queue, kernel, 1, 0x0, &gridSize, &threadsPerBlock, 0, 0x0, 0x0);
    clFinish(queue);

    clEnqueueReadBuffer(queue, res_mem, CL_TRUE, 0, sizeArray * sizeof (int), res, 0, 0x0, 0x0);

    for(int i = 0; i < sizeArray; i ++)
    {
        printf("%d + %d = %d\n",A[i], B[i], res[i]);
    }
    return 1;
}
