#include<stdio.h>
__global__ void HelloFromGPU(void)
{
    printf("Hello World from GPU!\n");
}

int main(void)
{
    printf("Hello World from CPU!\n");
    HelloFromGPU<<<1,10>>>();
    cudaDeviceReset();
    return 0;
}