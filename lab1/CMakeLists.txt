set(LAB_DESCRIPTION "demo on running a trivial kernel on an OpenCL device"
        PARENT_SCOPE)
add_executable(lab1 host.c)
target_compile_options(lab1 PUBLIC -Wno-unused-parameter)
