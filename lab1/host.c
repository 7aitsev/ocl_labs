#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* 1. Add program header */
#include "CL/opencl.h"

#include "../common/errorcodes.c"
#include "../common/utils.c"

#define KERNEL_FILE_NAME "lab1.aocx"

int
main(int argc, char** argv)
{
    cl_int rv;
    cl_device_id device;
    cl_context context;
    const char* kernel_file_name;

    // check if it's possible to open the kernel program
    kernel_file_name = (2 == argc) ? argv[1] : KERNEL_FILE_NAME;
    if(access(kernel_file_name, R_OK))
    {
        printf("Provided kernel name (%s) in not found\n", kernel_file_name);
        return CL_INVALID_VALUE;
    }

    /* 2. Create an OpenCL context (see common/utils.c) */
    if(CL_SUCCESS != (rv = platform_layer(&device, &context)))
    {
        return rv;
    }

    /* 3. Allocate the host memory buffer(s) */
    cl_int data_in = 0xCAFE;
    cl_int data_out;
    size_t data_size = sizeof(cl_int);

    /* 4. Create a command queue with profiling enabled */
    cl_command_queue cmd_q = clCreateCommandQueue(context, device, 
            CL_QUEUE_PROFILING_ENABLE, &rv);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("Failed to create a command queue:", rv, stderr);
        return rv;
    }

    /* 5. Allocate the device memory buffer(s) */
    cl_mem buf_out =
        clCreateBuffer(context, CL_MEM_WRITE_ONLY, data_size, NULL, &rv);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("Failed to create a buffer object:", rv, stderr);
        return rv;
    }

    /* 6. Write the data from the host buffers to the device buffers */
    // don't need this step (except for task3)

    /* 7-8. Read the kernel file and build a program (see common/utils.c) */
    cl_program program;
    rv = build_program(&program, &context, &device, kernel_file_name);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("Failed to build a program:", rv, stderr);
        return rv;
    }

    /* 9. Create the kernel object */
    cl_kernel kernel = clCreateKernel(program, "inout", &rv);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("Failed to create a kernel object:", rv, stderr);
        return rv;
    }

    /* 10. Setup the arguments to the kernel object */
    rv  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buf_out);
    rv |= clSetKernelArg(kernel, 1, sizeof(cl_int), &data_in);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("Failed to set an argument:", rv, stderr);
        return rv;
    }

    /* 11. Enqueue the kernel object for execution */
    //cl_event kernel_event; /* task1 */
    rv = clEnqueueTask(cmd_q, kernel, 0, NULL, NULL);
    /*
    // the same as above using clEnqueueNDRangeKernel() call
    size_t global_work_size = 1;
    rv = clEnqueueNDRangeKernel(cmd_q, kernel, 1, NULL,
            &global_work_size, NULL, 0, NULL, NULL);
    */
    if(CL_SUCCESS != rv)
    {
        print_cl_error("Couldn\'t enqueue a task:", rv, stderr);
        return rv;
    }

    // synchronization point
    if(CL_SUCCESS != (rv = clFinish(cmd_q)))
        print_cl_error("Failed to set a synchronization point:", rv, stderr);

    // get kernel time using the OpenCL event profiling API
    /* task1 */
    
    /* 12. Read out the buffer back from the device to the host */
    rv = clEnqueueReadBuffer(cmd_q, buf_out, CL_TRUE, 0, data_size, &data_out,
            0, NULL, NULL);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("Failed to enqueue the command to read the data:",
                rv, stderr);
        return rv;
    }

    printf("Read value: %X\n", data_out);

    /* 13. Finalization */
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(cmd_q);
    clReleaseContext(context);
    clReleaseMemObject(buf_out);
    // free host buffers bellow if necessary

    return 0;
}
