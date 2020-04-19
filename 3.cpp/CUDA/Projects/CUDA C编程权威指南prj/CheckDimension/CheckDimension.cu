#include<cuda_runtime.h>
#include<stdio.h>
__global__ void CheckIndex(const int KernelNo)
{
    printf("KernelNo %d, threadIdx:(%d, %d, %d) blockIdx:(%d, %d, %d) blockDim:(%d, %d, %d) gridDim:(%d, %d, %d)\n",
            KernelNo,
            threadIdx.x, threadIdx.y, threadIdx.z,
            blockIdx.x, blockIdx.y, blockIdx.z,
            blockDim.x, blockDim.y, blockDim.z,
            gridDim.x, gridDim.y, gridDim.z);
}

int main(void)
{
    int nElm = 512;
    dim3 block(3);
    dim3 grid  ((nElm + block.x - 1)/block.x);
    printf("grid.x %d, grid.y %d, grid.z %d\n", grid.x, grid.y, grid.z);
    printf("block.x %d, block.y %d, block.z %d\n", block.x, block.y, block.z);
    CheckIndex<<<grid, block>>>(0);
    block.x = 2;
    grid.x = ((nElm + block.x - 1)/block.x);
    CheckIndex<<<grid, block>>>(1);
    cudaDeviceReset();
    return 0;
}