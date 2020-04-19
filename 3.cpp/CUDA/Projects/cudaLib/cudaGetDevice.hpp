/*
 * @Author: xpt 
 * @Date: 2019-12-13 11:24:06 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-13 18:31:15
 */

#ifndef _CUDA_DEVICE_HPP
#define _CUDA_DEVICE_HPP

#include <string>
#include <memory>
#include"common.h"
#include <cuda_runtime.h>
#include <iostream>
using namespace std;

#if CUDART_VERSION < 5000
#include <cuda.h>
template <class T>
inline void getCudaAttribute(T *attribute, CUdevice_attribute device_attribute, int device)
{
    CUresult error =    cuDeviceGetAttribute(attribute, device_attribute, device);

    if (CUDA_SUCCESS != error)
    {
        printf("cuSafeCallNoSync() Driver API error = %04d from file <%s>, line %i.\n",error, __FILE__, __LINE__);
        fprintf(stderr, "cuSafeCallNoSync() Driver API error = %04d from file <%s>, line %i.\n",error, __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
}
#endif

struct _cudaDevice
{
    int DevNo;
    int DriverVersion;
    int RuntimeVersion;
    cudaDeviceProp deviceProp;
};

class _cudaGetDevice
{
    int deviceCount;
    _cudaDevice *cudaDevice;

    _cudaGetDevice():cudaDevice(NULL)
    {
        Reset();
        CHECK(cudaGetDeviceCount(&deviceCount));
        if (deviceCount == 0)
        {
            COUTENDL("There are no available device(s) that support CUDA");
        }
        else
        {
            COUTSTRVALENDL("Detected CUDA Capable device(s)", deviceCount);
        }
        cudaDevice = new _cudaDevice[deviceCount];
    
        for (int devNo = 0; devNo < deviceCount; devNo++)
        {
            cudaSetDevice(devNo);
            cudaDevice[devNo].DevNo = devNo;
            cudaDriverGetVersion(&(cudaDevice[devNo].DriverVersion));
            cudaRuntimeGetVersion(&(cudaDevice[devNo].RuntimeVersion));
            cudaGetDeviceProperties(&cudaDevice[devNo].deviceProp, devNo);

        }
    }

    ~_cudaGetDevice()
    {
        if(cudaDevice != NULL)
        {
            delete[] cudaDevice;
        }
    }

    void Reset()
    {
        if(cudaDevice != NULL)
        {
            delete[] cudaDevice;
        }
        deviceCount = 0;
        cudaDevice = NULL;
    }
};
#endif //_CUDA_DEVICE_HPP