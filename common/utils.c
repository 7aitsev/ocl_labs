/**
 * A callback to be used by OpenCL implementation to report information
 * on errors that occur in this context.
 */
void
ocl_context_cb(const char* errinfo,
        const void* private_info, size_t cb, void* user_data)
{
    printf("[context-error] %s\n", errinfo); // not thread safe
}

/**
 * Get an availiable OpenCL device and create a context.
 */
cl_int
platform_layer(cl_device_id* device, cl_context* context)
{
    cl_int rv;
    cl_platform_id platform;
    cl_uint num_platforms;
    cl_uint num_devices;

    /* Get an available platform on SOC-DE1 */
    rv = clGetPlatformIDs(1, &platform, &num_platforms);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("clGetPlatfromIDs failed:\n", rv, stderr);
        return rv;
    }

    /* Get an OpenCL device */
    rv = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, 
            device, &num_devices);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("clGetDeviceIDs failed:\n", rv, stderr);
        return rv;
    }

    /* Actually create an OpenCL context */
    *context = clCreateContext(NULL, 1, device,
            &ocl_context_cb, NULL, &rv);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("Cannot create an OpenCL context:", rv, stderr);
        return rv;
    }

    return CL_SUCCESS;
}

/**
 * For a given file name return its content and size.
 */
size_t
read_binary_kernel(const char* kernel_file_name, unsigned char** kernel_binary)
{
    FILE* kernel_file = fopen(kernel_file_name, "r");
    fseek(kernel_file, 0, SEEK_END);
    size_t kernel_file_size = ftell(kernel_file);
    rewind(kernel_file);
    *kernel_binary = malloc(kernel_file_size);
    fread(*kernel_binary, sizeof(unsigned char), kernel_file_size, kernel_file);
    fclose(kernel_file);
    return kernel_file_size;
}

/**
 * Build a program for a given file name.
 */
cl_int
build_program(cl_program *program,
        const cl_context* context, const cl_device_id* device,
        const char* kernel_file_name)
{   
    cl_int rv;
    size_t kernel_file_size;
    unsigned char* kernel_binary;

    /* 7. Read the kernel binary code from a file (see common/utils.c) */
    kernel_file_size = read_binary_kernel(kernel_file_name, &kernel_binary);

    /* 8. Creating program object and building an executable for the device */
    const size_t lengths[1] = {kernel_file_size};
    const unsigned char* binaries[1] = {kernel_binary};
    *program = clCreateProgramWithBinary(*context, 1, device,
            lengths, binaries, NULL, &rv);
    if(CL_SUCCESS != NULL)
    {
        print_cl_error("Couldn\'t create the program", rv, stderr);
        return rv;
    }
    free(kernel_binary);

    rv = clBuildProgram(*program, 1, device, "", NULL, NULL);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("Couldn\'t build an executable", rv, stderr);
        return rv;
    }  

    return CL_SUCCESS;
}
