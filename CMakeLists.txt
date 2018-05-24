cmake_minimum_required(VERSION 2.4)
project(OpenCL_LAB C)

# OpenCL compile and link flags
execute_process(COMMAND aocl compile-config --arm OUTPUT_VARIABLE
        __AOCL_COMPILE_CONFIG)
execute_process(COMMAND aocl ldflags --arm OUTPUT_VARIABLE __AOCL_LINK_FLAGS)
execute_process(COMMAND aocl ldlibs --arm OUTPUT_VARIABLE _AOCL_LINK_LIBS)
string(REGEX REPLACE "-I|\n" "" _AOCL_COMPILE_CONFIG ${__AOCL_COMPILE_CONFIG})
string(REGEX REPLACE "-L|\n" "" _AOCL_LINK_FLAGS ${__AOCL_LINK_FLAGS})
string(REPLACE " " ";" AOCL_COMPILE_CONFIG ${_AOCL_COMPILE_CONFIG})
string(REPLACE " " ";" AOCL_LINK_FLAGS ${_AOCL_LINK_FLAGS})
string(STRIP "${_AOCL_LINK_LIBS}" AOCL_LINK_LIBS)

message("List of directories to be searched for OpenCL header files:")
foreach(path ${AOCL_COMPILE_CONFIG})
    message("\t${path}")
    include_directories(SYSTEM ${AOCL_COMPILE_CONFIG})
endforeach(path)
message("List of directories to be searced for OpenCL libraries:")
foreach(path ${AOCL_LINK_FLAGS})
    message("\t${path}")
    link_directories(${path})
endforeach(path)
message("Host runtime libraries and linker options:\n\t${AOCL_LINK_LIBS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${AOCL_LINK_LIBS}")

# Set compilation flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -std=c99 -Wall -Wextra")
message("Compiler flags:\n\t${CMAKE_C_FLAGS}")

# Define a varilable with common sources
file(GLOB COMMON_SRC common/*.c)
# List all folders which are the targets
file(GLOB LAB_DIRS lab*)
message("Availiable build targets:")
foreach(target ${LAB_DIRS})
    # Include target-specific CMake build scripts
    add_subdirectory(${target})
    get_filename_component(target_name ${target} NAME)
    message("\t* ${target_name} - ${LAB_DESCRIPTION}")
endforeach(target)

# Remove all patch-files
file(GLOB PATCH_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} lab?/*.patch)
if(NOT "${PATCH_FILES}" STREQUAL "")
    file(REMOVE ${PATCH_FILES})
endif()
