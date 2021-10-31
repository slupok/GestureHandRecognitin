__kernel void test(__global int* A, __global int *B, __global int *res)
{
    int i = get_global_id(0);
    res[i] = A[i] + B[i];
}
