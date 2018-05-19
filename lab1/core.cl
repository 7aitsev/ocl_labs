__kernel void
inout(__global int* out, int in)
{
    *out = in; // out[0] = in;
}
