/*
 * @Author: xpt 
 * @Date: 2019-12-13 19:04:52 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-23 00:52:51
 */

#ifndef _CUDA_COMMON_H
#define _CUDA_COMMON_H

#include <cuda_runtime.h>
#include <stdio.h>
#include <iostream>

#define CHECK(cudaError) cudaCheckError((cudaError), __FILE__, __func__, __LINE__)

inline void cudaCheckError(const cudaError_t call, const std::string & filePath, const std::string &function, int line) 
{
    const cudaError_t error = call;
    unsigned long long int pos=filePath.find_last_of('/');
    std::string filename(filePath.substr(pos+1));
    if (error != cudaSuccess)
    {
        std::cout<<"cudaError: $file "<<filename<<", $function "<<function<<", $line "<<line
                 <<", $errorcode "<<error<<", $reason "<<cudaGetErrorString(error)<<std::endl;
        exit(1);
    }
}

template<class T>
__inline__ __host__ void setupConstant(const T &symbol, const T &src, size_t count = 1)
{
    CHECK(cudaMemcpyToSymbol( symbol, &src, count * sizeof(T)));
}

#endif // _CUDA_COMMON_H