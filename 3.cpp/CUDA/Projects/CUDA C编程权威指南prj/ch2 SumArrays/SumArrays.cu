/*
 * @Author: xpt 
 * @Date: 2019-12-12 11:28:26 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-12 11:38:47
 */
#include<cuda_runtime.h>
#include<stdio.h>

void sumArrayOnHost(float *A, float *B, float *C, const int N)
{
    int idx = 0;
    for(; idx < N; idx++)
    {
        C[idx] = A[idx] + B[idx];
    }
}

__global__ void sumArrayOnGPU(float *A, float *B, float *C)
{
    int i = threadIdx.x;
    C[i] = A[i] + B[i];
}

void initialData(float *ip, int size)
{
    time_t t;
    srand((unsigned int) time(&t));
    for (int t = 0; t < size; t++)
    {
        ip[t] = (float)(rand() & 0xFF)/10.0f;
    }
}

void checkResult(float *hostRef, float *gpuRef, const int N)
{
    double epsilon = 1.0E-8;
    bool match = true;
    for(int i = 0; i < N; i++)
    {
        if(abs(hostRef[i] - gpuRef[i]) > epsilon)
        {
            match = false;
            printf("Arrays do not match!\n");
            break;
        }
    }
    if(match)
    {
        printf("Arrays  match\n\n");
    }
}

int main(int argc, char **argv)
{
    printf("%s Starting...\n", argv[0]);
    int dev = 0;
    cudaSetDevice(dev);

    int nElem = 1024;
    int nBytes = nElem * sizeof(float);
    float *h_A, *h_B, *hostRef, *gpuRef;
    h_A = (float *)malloc(nBytes);
    h_B = (float *)malloc(nBytes);
    hostRef = (float *)malloc(nBytes);
    gpuRef = (float *)malloc(nBytes);
    initialData(h_A,nElem);
    initialData(h_B,nElem);
    memset(hostRef, 0 , nBytes);
    memset(gpuRef, 0 , nBytes);

    float *d_A, *d_B, *d_C;
    cudaMalloc((float **)&d_A, nBytes);
    cudaMalloc((float **)&d_B, nBytes);
    cudaMalloc((float **)&d_C, nBytes);
    cudaMemcpy(d_A, h_A, nBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, nBytes, cudaMemcpyHostToDevice);

    dim3 block(nElem);
    dim3 grid((nElem + block.x - 1)/block.x);
    sumArrayOnGPU<<<grid, block>>>(d_A, d_B, d_C);
    printf("Execution configuration <<<%d, %d>>>\n", grid.x, block.x);
    cudaMemcpy(gpuRef, d_C, nBytes, cudaMemcpyDeviceToHost);

    sumArrayOnHost(h_A, h_B, hostRef,nElem);

    checkResult(hostRef, gpuRef, nElem);

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    free(h_A);
    free(h_B);
    free(hostRef);
    free(gpuRef);
    return 0;
}