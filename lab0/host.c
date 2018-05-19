#include <stdio.h>
#include <stdlib.h>

#include "CL/opencl.h"

#include "../common/errorcodes.c"

#define CHECK_AND_PRINT(format, str, val) { \
    if(CL_SUCCESS != rv) \
        printf("\t\t%s: parameter is not available\n", str); \
    else \
        printf(format, str, val); \
}

#define ARR_SIZE(a) (sizeof((a)) / sizeof((*a)))

#define INFO_SIZE 512
char* g_info;

void
print_extensions(char * const ext_list, const char* indents)
{
    char* p = ext_list;
    char* prev = p;
    while('\0' != *p)
    {
        if(' ' == *p)
        {
            *p = '\0';
            printf("%s* %s\n", indents, prev);
            prev = p + 1;
        }
        ++p;
    }
    printf("%s* %s\n", indents, prev);
}

void
print_platform_info(const cl_platform_id platform)
{
    cl_int rv;

    /* List of requested parameters about a platform */
    static cl_platform_info pl_params[] = {
        CL_PLATFORM_NAME, CL_PLATFORM_VENDOR, CL_PLATFORM_VERSION,
        CL_PLATFORM_PROFILE
    };
    static const char * const pl_params_str[] = {
        "Name", "Vendor", "Version", "Profile"
    };

    for(size_t i = 0; i < ARR_SIZE(pl_params); ++i)
    {
        rv = clGetPlatformInfo(platform, pl_params[i],
                INFO_SIZE, g_info, NULL);
        CHECK_AND_PRINT("\t%s: %s\n", pl_params_str[i], g_info);
    }

    // get information on extensions
    size_t ext_size;
    char* ext_list;
    rv = clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS, 0, NULL,
            &ext_size);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("\tFailed to query platfrom\'s extensions size:",
                rv, stderr);
        return;
    }
    ext_list = malloc(ext_size);
    rv = clGetPlatformInfo(platform, CL_PLATFORM_EXTENSIONS,
            ext_size, ext_list, NULL);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("\nFailed to query the list of platform extensions:",
                rv, stderr);
        return;
    }
    puts("\tPlatform extensions:");
    print_extensions(ext_list, "\t");
    free(ext_list);
}

void
print_device_info(const cl_device_id device)
{
    cl_int rv;
    size_t i;

    /* Lists of requested parameter about an OpenCL device */
    static cl_device_info dev_params_char[] = {
        CL_DEVICE_NAME, CL_DEVICE_VENDOR, CL_DEVICE_VERSION,
        CL_DRIVER_VERSION
    };
    static const char * const dev_params_char_str[] = {
        "Name", "Vendor", "Version",
        "Driver version"
    };

    static cl_device_info dev_params_uint[] = {
        CL_DEVICE_MAX_COMPUTE_UNITS, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
        CL_DEVICE_MAX_WORK_GROUP_SIZE, CL_DEVICE_MAX_CLOCK_FREQUENCY,
        CL_DEVICE_ADDRESS_BITS 
    };
    static const char * const dev_params_uint_str[] = {
        "Max compute units", "Max work item dimensions",
        "Max work group size", "Clock frequency",
        "Address bits" 
    };

    static cl_ulong dev_params_ulong[] = {
        CL_DEVICE_MAX_MEM_ALLOC_SIZE, CL_DEVICE_GLOBAL_MEM_SIZE,
        CL_DEVICE_LOCAL_MEM_SIZE, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
    };
    static const char * const dev_params_ulong_str[] = {
        "Max mem alloc size", "Global mem size",
        "Local mem size", "Max constant buffer size"
    };

    for(i = 0; i < ARR_SIZE(dev_params_char); ++i)
    {
        rv = clGetDeviceInfo(device, dev_params_char[i],
                INFO_SIZE, g_info, NULL);
        CHECK_AND_PRINT("\t\t%s: %s\n", dev_params_char_str[i], g_info);
    }

    cl_device_type type;
    rv = clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
    printf("\t\tType: ");
    if(CL_SUCCESS != rv)
        printf("parameter is not available\n");
    else
    {
        // the device may belong to many types
        if(CL_DEVICE_TYPE_DEFAULT & type) printf("default ");
        if(CL_DEVICE_TYPE_CPU & type) printf("CPU ");
        if(CL_DEVICE_TYPE_GPU & type) printf("GPU ");
        if(CL_DEVICE_TYPE_ACCELERATOR & type) printf("accelerator");
        putchar('\n');
    }

    cl_uint ret_number;
    for(i = 0; i < ARR_SIZE(dev_params_uint); ++i)
    {
        rv = clGetDeviceInfo(device, dev_params_uint[i],
                sizeof(cl_uint), &ret_number, NULL);
        CHECK_AND_PRINT("\t\t%s: %u\n", dev_params_uint_str[i], ret_number);
    }

    size_t sizes[3] = {0};
    printf("\t\tMax work item sizes: ");
    rv = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES,
            sizeof(sizes), sizes, NULL);
    if(CL_SUCCESS != rv)
    {
        printf("parameter is not available\n");
    }   
    else
    {
        for(i = 0; i < 2 && 0 != sizes[i + 1]; ++i)
            printf("%zdx", sizes[i]);
        printf("%zd\n", sizes[i]);
    }

    cl_ulong ret_mem_size;
    for(i = 0; i < ARR_SIZE(dev_params_ulong); ++i)
    {
        rv = clGetDeviceInfo(device, dev_params_ulong[i],
                sizeof(cl_ulong), &ret_mem_size, NULL);
        CHECK_AND_PRINT("\t\t%s: %llu\n", dev_params_ulong_str[i],
                ret_mem_size);
    }

    cl_device_local_mem_type local_mem_type;
    printf("\t\tLocal mem type: ");
    rv = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE,
            sizeof(local_mem_type), &local_mem_type, NULL);
    if(CL_SUCCESS != rv)
        printf("parameter is not available\n");
    else
        switch(local_mem_type)
        {
            case CL_LOCAL:
                puts("local");
                break;
            case CL_GLOBAL:
                puts("global");
                break;
            default:
                puts("none");
        }

    cl_command_queue_properties cmd_q_props;
    rv = clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES,
            sizeof(cmd_q_props), &cmd_q_props, NULL);
    if(CL_SUCCESS == rv)
        printf("\t\tOut-of-order execution support: %s\n",
                (CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE & cmd_q_props)
                ? "yes" : "no");

    /*
     * type here your OpenCL API call
     */

    // get information on extensions
    size_t ext_size;
    char* ext_list;
    rv = clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 0, NULL, &ext_size);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("\t\tFailed to query the size of extensions list:",
                rv, stderr);
        return;
    }
    ext_list = malloc(ext_size);
    rv = clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS,
            ext_size, ext_list, NULL);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("\t\tFailed to query the list of device extensions:",
                rv, stderr);
        return;
    }
    puts("\t\tDevice extensions:");
    print_extensions(ext_list, "\t\t");
    free(ext_list);
}

cl_int
platform_layer_info(void)
{
    cl_int rv;
    cl_platform_id* platforms; // list of platforms
    cl_uint num_platforms;
    cl_device_id* devices; // list of devices on the specific platform
    cl_uint num_devices;
    g_info = malloc(INFO_SIZE * sizeof(char)); // buffer for text values

    /* Find out how many platforms are attached on SOC-DE1 and get their ids */
    rv = clGetPlatformIDs(0, NULL, &num_platforms);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("clGetPlatformIDs(1) failed:", rv, stderr);
        return rv;
    }
    printf("Information about the platform:\n" \
            "Number of platforms: %d\n", num_platforms);

    /* Query all available platforms */
    platforms = malloc(num_platforms * sizeof(cl_platform_id));
    rv = clGetPlatformIDs(num_platforms, platforms, NULL);
    if(CL_SUCCESS != rv)
    {
        print_cl_error("clGetPlatformIDs(2) failed:", rv, stderr);
        return rv;
    }

    // print out information about each platform
    for(size_t i = 0; i < num_platforms; ++i)
    {
        printf("Platform #%d:\n", i + 1);
        print_platform_info(platforms[i]);
        
        /* How many devices are attached to each platform & get their ids */
        rv = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL,
                &num_devices);
        if(CL_SUCCESS != rv)
        {
            print_cl_error("clGetDeviceIDs(1) failed:", rv, stderr);
            return rv;
        }
        printf("\tNumber of devices: %d\n", num_devices);

        /* Query all available OpenCL devices on the platform */
        devices = malloc(num_devices * sizeof(cl_device_id));
        rv = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices, 
                devices, NULL);
        if(CL_SUCCESS != rv)
        {
            print_cl_error("clGetDeviceIDs(2) failed:", rv, stderr);
            return rv;
        }

        // print some useful information about each device
        for(size_t j = 0; j < num_devices; ++j)
        {
            printf("\tDevice #%d:\n", j + 1);
            print_device_info(devices[j]);
        }
        free(devices);
    }

    free(platforms);
    free(g_info);
    return CL_SUCCESS;
}

cl_int
main(void)
{
    cl_int rv;

    if(CL_SUCCESS != (rv = platform_layer_info()))
    {
        fprintf(stderr, "Critical error on platform layer: %d\n", rv);
        return rv;
    }

    return CL_SUCCESS;
}
