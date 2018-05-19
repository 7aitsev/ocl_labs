#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "CL/opencl.h"

struct errorcode
{
    cl_int status_code;
    char* meaning;
};

static struct errorcode error_codes[] = {
{ CL_SUCCESS,                         ""                                    },
{ CL_DEVICE_NOT_FOUND,                "Device not found"                    },
{ CL_DEVICE_NOT_AVAILABLE,            "Dvice not available"                 },
{ CL_COMPILER_NOT_AVAILABLE,          "Compiler not available"              },
{ CL_MEM_OBJECT_ALLOCATION_FAILURE,   "Memory object allocation failure"    },
{ CL_OUT_OF_RESOURCES,                "Out of resources"                    },
{ CL_OUT_OF_HOST_MEMORY,              "Out of host memory"                  },
{ CL_PROFILING_INFO_NOT_AVAILABLE,    "Profiling information not available" },
{ CL_MEM_COPY_OVERLAP,                "Memory copy overlap"                 },
{ CL_IMAGE_FORMAT_MISMATCH,           "Image format mismatch"               },
{ CL_IMAGE_FORMAT_NOT_SUPPORTED,      "Image format not supported"          },
{ CL_BUILD_PROGRAM_FAILURE,           "Build program failure"               },
{ CL_MAP_FAILURE,                     "Map failure"                         },
{ CL_INVALID_VALUE,                   "Invalid value"                       },
{ CL_INVALID_DEVICE_TYPE,             "Invalid device type"                 },
{ CL_INVALID_PLATFORM,                "Invalid platform"                    },
{ CL_INVALID_DEVICE,                  "Invalid device"                      },
{ CL_INVALID_CONTEXT,                 "Invalid context"                     },
{ CL_INVALID_QUEUE_PROPERTIES,        "Invalid queue properties"            },
{ CL_INVALID_COMMAND_QUEUE,           "Invalid command queue"               },
{ CL_INVALID_HOST_PTR,                "Invalid host pointer"                },
{ CL_INVALID_MEM_OBJECT,              "Invalid memory object"               },
{ CL_INVALID_IMAGE_FORMAT_DESCRIPTOR, "Invalid image format descriptor"     },
{ CL_INVALID_IMAGE_SIZE,              "Invalid image size"                  },
{ CL_INVALID_SAMPLER,                 "Invalid sampler"                     },
{ CL_INVALID_BINARY,                  "Invalid binary"                      },
{ CL_INVALID_BUILD_OPTIONS,           "Invalid build options"               },
{ CL_INVALID_PROGRAM,                 "Invalid program"                     },
{ CL_INVALID_PROGRAM_EXECUTABLE,      "Invalid program executable"          },
{ CL_INVALID_KERNEL_NAME,             "Invalid kernel name"                 },
{ CL_INVALID_KERNEL_DEFINITION,       "Invalid kernel definition"           },
{ CL_INVALID_KERNEL,                  "Invalid kernel"                      },
{ CL_INVALID_ARG_INDEX,               "Invalid argument index"              },
{ CL_INVALID_ARG_VALUE,               "Invalid argument value"              },
{ CL_INVALID_ARG_SIZE,                "Invalid argument size"               },
{ CL_INVALID_KERNEL_ARGS,             "Invalid kernel arguments"            },
{ CL_INVALID_WORK_DIMENSION,          "Invalid work dimension"              },
{ CL_INVALID_WORK_GROUP_SIZE,         "Invalid work group size"             },
{ CL_INVALID_WORK_ITEM_SIZE,          "Invalid work item size"              },
{ CL_INVALID_GLOBAL_OFFSET,           "Invalid global offset"               },
{ CL_INVALID_EVENT_WAIT_LIST,         "Invalid event wait list"             },
{ CL_INVALID_EVENT,                   "Invalid event"                       },
{ CL_INVALID_OPERATION,               "Invalid operation"                   },
{ CL_INVALID_GL_OBJECT,               "Invalid gl object"                   },
{ CL_INVALID_BUFFER_SIZE,             "Invalid buffer size"                 },
{ CL_INVALID_MIP_LEVEL,               "Invalid mip level"                   },
{ CL_INVALID_GLOBAL_WORK_SIZE,        "Invalid global work size"            },
};

void
print_cl_error(char* prefix, cl_int status, FILE* fp)
{
    if(CL_SUCCESS == status)
        return;

    const int numErrorCodes = sizeof(error_codes) / sizeof(struct errorcode);
    char* meaning = "";
    for(int i = 0; i < numErrorCodes; i++)
    {
        if(error_codes[i].status_code == status)
        {
            meaning = error_codes[i].meaning;
            break;
        }
    }

    fprintf(fp, "%s %s\n", prefix, meaning);
}
