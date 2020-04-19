#include <stdio.h>
#include <cuda_runtime.h>
 
 int main(void)
 {
     cudaError_t err = cudaSuccess;
 
     //* cudaMalloc3D
     cudaPitchedPtr  cudaPitchedPtrCase;
     cudaExtent cudaExtentCase = {900, 800, 513};
     err = cudaMalloc3D(&cudaPitchedPtrCase,cudaExtentCase);
     if (err != cudaSuccess)
     {
         fprintf(stderr, "Failed to allocate device vector C (error code %s)!\n", cudaGetErrorString(err));
         exit(EXIT_FAILURE);
     }
     printf("cudaPitchedPtrCase.pitch = %d\n",cudaPitchedPtrCase.pitch);
     printf("cudaPitchedPtrCase.xsize = %d\n",cudaPitchedPtrCase.xsize);
     printf("cudaPitchedPtrCase.ysize = %d\n",cudaPitchedPtrCase.ysize);
     printf("Done\n");

    //* cudaMallocPitch
    // void *cudaptr;
    // size_t pitch;
    // cudaMallocPitch((float **)&cudaptr,&pitch,100,900);
    // printf("pitch = %d\n",pitch);
    // cudaFree(cudaptr);
     return 0;
 }
 
 