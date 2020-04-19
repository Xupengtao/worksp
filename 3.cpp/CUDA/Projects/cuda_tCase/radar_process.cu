/*
* Copyright 1993-2015 NVIDIA Corporation.  All rights reserved.
*
* Please refer to the NVIDIA end user license agreement (EULA) associated
* with this source code for terms and conditions that govern your use of
* this software. Any use, reproduction, disclosure, or distribution of
* this software and related documentation outside the terms of the EULA
* is strictly prohibited.
*
*/

/*
* Copyright 1993-2014 NVIDIA Corporation.  All rights reserved.
*
* Please refer to the NVIDIA end user license agreement (EULA) associated
* with this source code for terms and conditions that govern your use of
* this software. Any use, reproduction, disclosure, or distribution of
* this software and related documentation outside the terms of the EULA
* is strictly prohibited.
*
*/

/* Example showing the use of CUFFT for fast 1D-convolution using FFT. */

// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// includes, project
#include <cuda_runtime.h>
#include <cufft.h>
#include <cufftXt.h>

#include "helper_functions.h"
#include <helper_cuda.h>

#include "rsp_base.cuh"
#include "radar_process.h"






////////////////////////////////////////////////////////////////////////////////

// The filter size is assumed to be a number smaller than the signal size
#define SIGNAL_ROW	        16
#define SIGNAL_COL			64

////////////////////////////////////////////////////////////////////////////////
// Complex operations
////////////////////////////////////////////////////////////////////////////////

// Complex addition
//static __device__ __host__ inline Complex ComplexAdd(Complex a, Complex b)
//{
//	Complex c;
//	c.x = a.x + b.x;
//	c.y = a.y + b.y;
//	return c;
//}
//
//// Complex scale
//static __device__ __host__ inline Complex ComplexScale(Complex a, float s)
//{
//	Complex c;
//	c.x = s * a.x;
//	c.y = s * a.y;
//	return c;
//}
//
//// Complex multiplication
//static __device__ __host__ inline Complex ComplexMul(Complex a, Complex b)
//{
//	Complex c;
//	c.x = a.x * b.x - a.y * b.y;
//	c.y = a.x * b.y + a.y * b.x;
//	return c;
//}
//
//// Complex multiplication
//static __device__ __host__ inline Complex ComplexMulReal(Complex a, float b)
//{
//	Complex c;
//	c.x = a.x * b;
//	c.y = a.y * b;
//	return c;
//}
//
//
//// Complex multiplication
//static __device__ __host__ inline float ComplexAbs(Complex a)
//{
//	float c;
//	
//	c = fabsf(a.x * a.x + a.y * a.y);
//
//	return c;
//}
//
//
//// Complex pointwise multiplication
//static __global__ void ComplexPointwiseMulAndScale(Complex *a, const Complex *b, int size, float scale)
//{
//	const int numThreads = blockDim.x * gridDim.x;
//	const int threadID = blockIdx.x * blockDim.x + threadIdx.x;
//
//	for (int i = threadID; i < size; i += numThreads)
//	{
//		a[i] = ComplexScale(ComplexMul(a[i], b[i]), scale);
//	}
//}
//
//
//
////static __global__ void MatrixWiseMulVec(Complex* a, const Complex *b, int row, int col, float scale)
////{
////
////}
//
//static __global__ void MatrixWiseMulVecRow(Complex* m, Complex *v, Complex* result, int row, int col, float scale)
//{
//	int crow, ccol;
//	//int cstride;
//	int offset;
//	Complex vv;
//
//	crow = threadIdx.x;
//	ccol = blockIdx.x;
//	offset = crow*col + ccol;
//
//	crow = blockIdx.y * blockDim.y + threadIdx.y;
//	ccol = blockIdx.x * blockDim.x + threadIdx.x;
//	//cstride = blockDim.x * gridDim.x;
//	offset = crow * col + ccol;
//
//	if ((crow < row) && (ccol < col))
//	{
//		vv = ComplexMulReal(v[ccol],scale);
//		result[offset] = ComplexMul(m[offset], vv);
//	}
//}
//
//static __global__ void MatrixWiseRealMulVecCol(Complex* m, float *v, Complex* result, int row, int col)
//{
//	int crow, ccol;
//	//int cstride;
//	int offset;
//
//	crow = threadIdx.x;
//	ccol = blockIdx.x;
//	offset = crow*col + ccol;
//
//	crow = blockIdx.y * blockDim.y + threadIdx.y;
//	ccol = blockIdx.x * blockDim.x + threadIdx.x;
//	//cstride = blockDim.x * gridDim.x;
//	offset = crow * col + ccol;
//
//	if ((crow < row) && (ccol < col))
//	{
//		result[offset] = ComplexMulReal(m[offset], v[crow]);
//	}
//}


void rsp_blackman(int nSize, float* pWin)
{
	int i;

	for (i = 0; i < nSize; i++)
	{
		pWin[i] = 0.52 - 0.5*cos(2 * pi*i / (nSize - 1)) + 0.08*cos(4*pi*i/(nSize-1));
	}
}

void pulse_compress(float* rx, const float* win, int row, int col)
{
	
	int M_ROW = 32;
	int M_COL = 8192;

	Complex* fp_rx, *fp_result;
	float* fp_win;
	Complex* dev_rx, *dev_result;
	float* dev_win;

	cudaEvent_t start;
	cudaEvent_t stop;
	cudaStream_t stream0;
	float ElapsedTime;

	const int threadsPerBlock = M_ROW;
	const int blocksPerGrid = M_COL;


	int count;
	cudaDeviceProp prop;

	checkCudaErrors(cudaGetDeviceCount(&count));

	checkCudaErrors(cudaGetDeviceProperties(&prop, 0));

	fp_rx = (Complex*)malloc(M_ROW * M_COL * sizeof(Complex));
	fp_win = (float*)malloc(M_COL * sizeof(float));
	fp_result = (Complex*)malloc(M_ROW * M_COL * sizeof(Complex));

	checkCudaErrors(cudaMalloc((void**)&dev_rx, M_ROW * M_COL * sizeof(Complex)));
	checkCudaErrors(cudaMalloc((void**)&dev_win, M_ROW * M_COL * sizeof(float)));
	checkCudaErrors(cudaMalloc((void**)&dev_result, M_ROW * M_COL * sizeof(Complex)));

	for (int i = 0; i < M_ROW; i++)
	{
		for (int j = 0; j < M_COL; j++)
		{
			fp_rx[i*M_COL + j].x = (float)j;
			fp_rx[i*M_COL + j].y = 0;
		}

	}

	for (int j = 0; j < M_COL; j++)
		fp_win[j] = (float)j;


	checkCudaErrors(cudaEventCreate(&start));
	checkCudaErrors(cudaEventCreate(&stop));
	checkCudaErrors(cudaEventRecord(start, 0));


	checkCudaErrors(cudaMemcpy(dev_rx, fp_rx, M_ROW * M_COL * sizeof(Complex), cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(dev_win, fp_win, M_COL * sizeof(float), cudaMemcpyHostToDevice));
	
	//MatrixWiseMulVec <<<blocksPerGrid, threadsPerBlock >>>(dev_rx, dev_win, dev_result, M_ROW, M_COL, 1.0f);

	checkCudaErrors(cudaMemcpy(fp_result, dev_result, M_ROW * M_COL * sizeof(Complex), cudaMemcpyDeviceToHost));

	checkCudaErrors(cudaEventRecord(stop, 0));
	checkCudaErrors(cudaEventSynchronize(stop));

	checkCudaErrors(cudaEventElapsedTime(&ElapsedTime, start, stop));





	checkCudaErrors(cudaFree(dev_result));
	checkCudaErrors(cudaFree(dev_win));
	checkCudaErrors(cudaFree(dev_rx));

	free(fp_result);
	free(fp_win);
	free(fp_rx);


		

}


void FFTTest(int fft_row, int fft_col， float* run_time)
{
	cudaEvent_t start;
	cudaEvent_t stop;
	cudaStream_t stream0;
	float ElapsedTime;

	LARGE_INTEGER lstart, lstop, ltotal;

	int buffer_size = fft_row * fft_col;
	int fft_size = fft_col;
	// Allocate host memory for the signal
	Complex *h_signal = (Complex *)malloc(sizeof(Complex) * buffer_size);
	Complex *h_fft = (Complex *)malloc(sizeof(Complex) * buffer_size);

	// Initialize the memory for the signal
	for (unsigned int i = 0; i < fft_row; ++i)
		for (unsigned int j = 0; j < fft_col; ++j)
	{
		h_signal[i*fft_col + j].x = (float)i+1.0f;
		h_signal[i*fft_col + j].y = 0;
	}



	int mem_size = sizeof(Complex) * buffer_size;

	// Allocate device memory for signal
	Complex *d_signal;
	checkCudaErrors(cudaMalloc((void **)&d_signal, mem_size));



	// CUFFT plan advanced API
	cufftHandle plan_adv;
	size_t workSize;
	int new_size_long = fft_size;
	int batch_size = fft_row;

	int nembed[2] = { fft_row , fft_col };
	int stride = 1;
	int dist = fft_col;

	QueryPerformanceCounter( &lstart);

	checkCudaErrors(cudaEventCreate(&start));
	checkCudaErrors(cudaEventCreate(&stop));
	checkCudaErrors(cudaEventRecord(start, 0));

	// Copy host memory to device
	checkCudaErrors(cudaMemcpy(d_signal, h_signal, mem_size,
		cudaMemcpyHostToDevice));

	checkCudaErrors(cufftCreate(&plan_adv));
	checkCudaErrors(cufftMakePlanMany(plan_adv, 1, &new_size_long, nembed, stride, dist, nembed, stride, dist, CUFFT_C2C, batch_size, &workSize));

	// Transform signal and kernel
	checkCudaErrors(cufftExecC2C(plan_adv, (cufftComplex *)d_signal, (cufftComplex *)d_signal, CUFFT_FORWARD));

	// Copy device memory to host
	checkCudaErrors(cudaMemcpy(h_fft, d_signal, mem_size,
		cudaMemcpyDeviceToHost));

	checkCudaErrors(cudaEventRecord(stop, 0));
	checkCudaErrors(cudaEventSynchronize(stop));

	QueryPerformanceCounter(&lstop);
	checkCudaErrors(cudaEventElapsedTime(&ElapsedTime, start, stop));

	ltotal.QuadPart = (lstop.QuadPart - lstart.QuadPart) / batch_size;
	ElapsedTime = ElapsedTime / batch_size;

	*run_time = ElapsedTime;
	// Transform signal back
	//checkCudaErrors(cufftExecC2C(plan_adv, (cufftComplex *)d_signal, (cufftComplex *)d_signal, CUFFT_INVERSE));

	// Copy device memory to host
	checkCudaErrors(cudaMemcpy(h_signal, d_signal, mem_size,
		cudaMemcpyDeviceToHost));


	//Destroy CUFFT context
	checkCudaErrors(cufftDestroy(plan_adv));

	// cleanup memory
	free(h_signal);
	free(h_fft);
	checkCudaErrors(cudaFree(d_signal));

	
}

void ColFFTTest(int fft_row, int fft_col)
{
	cudaEvent_t start;
	cudaEvent_t stop;
	cudaStream_t stream0;
	float ElapsedTime;

	LARGE_INTEGER lstart, lstop, ltotal;
	Complex *h_signal;
	Complex *h_fft;
	Complex *d_signal;
	Complex *d_fft;

	int buffer_size = fft_row * fft_col;
	int fft_size = fft_row;
	// Allocate host memory for the signal
	//Complex *h_signal = (Complex *)malloc(sizeof(Complex) * buffer_size);
	//Complex *h_fft = (Complex *)malloc(sizeof(Complex) * buffer_size);

	checkCudaErrors(cudaHostAlloc((void**)&h_signal, sizeof(Complex) * buffer_size, cudaHostAllocDefault));
	checkCudaErrors(cudaHostAlloc((void**)&h_fft, sizeof(Complex) * buffer_size, cudaHostAllocDefault));


	// Initialize the memory for the signal
	for (unsigned int i = 0; i < fft_row; ++i)
		for (unsigned int j = 0; j < fft_col; ++j)
		{
			h_signal[i*fft_col + j].x = (float)j + 1.0f;
			h_signal[i*fft_col + j].y = 0;
		}



	int mem_size = sizeof(Complex) * buffer_size;

	// Allocate device memory for signal
	
	checkCudaErrors(cudaMalloc((void **)&d_signal, mem_size));
	checkCudaErrors(cudaMalloc((void **)&d_fft, mem_size));



	// CUFFT plan advanced API
	cufftHandle plan_adv;
	size_t workSize;
	int new_size_long = fft_size;
	int batch_size = fft_col;

	int nembed[2] = { fft_row , fft_col };
	int stride = fft_col;
	int dist = 1;

	checkCudaErrors(cudaStreamCreate(&stream0));


	QueryPerformanceCounter(&lstart);

	checkCudaErrors(cudaEventCreate(&start));
	checkCudaErrors(cudaEventCreate(&stop));
	checkCudaErrors(cudaEventRecord(start, 0));

	// Copy host memory to device
	//checkCudaErrors(cudaMemcpy(d_signal, h_signal, mem_size, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpyAsync(d_signal, h_signal, mem_size, cudaMemcpyHostToDevice, stream0));

	checkCudaErrors(cufftCreate(&plan_adv));
	checkCudaErrors(cufftMakePlanMany(plan_adv, 1, &new_size_long, nembed, stride, dist, nembed, stride, dist, CUFFT_C2C, batch_size, &workSize));

	// Transform signal and kernel
	checkCudaErrors(cufftExecC2C(plan_adv, (cufftComplex *)d_signal, (cufftComplex *)d_fft, CUFFT_FORWARD));

	// Copy device memory to host
	checkCudaErrors(cudaMemcpyAsync(h_fft, d_fft, mem_size, cudaMemcpyDeviceToHost, stream0));

	// Transform signal back
	checkCudaErrors(cufftExecC2C(plan_adv, (cufftComplex *)d_fft, (cufftComplex *)d_signal, CUFFT_INVERSE));

	// Copy device memory to host
	checkCudaErrors(cudaMemcpyAsync(h_signal, d_signal, mem_size, cudaMemcpyDeviceToHost, stream0));
	checkCudaErrors(cudaStreamSynchronize(stream0));

	// Stop Time
	//checkCudaErrors(cudaStreamSynchronize(stream0));

	QueryPerformanceCounter(&lstop);
	checkCudaErrors(cudaEventRecord(stop, 0));
	checkCudaErrors(cudaEventSynchronize(stop));

	ltotal.QuadPart = (lstop.QuadPart - lstart.QuadPart) / batch_size;

	checkCudaErrors(cudaEventElapsedTime(&ElapsedTime, start, stop));

	ElapsedTime = ElapsedTime / batch_size;

	//Destroy CUFFT context
	checkCudaErrors(cufftDestroy(plan_adv));

	checkCudaErrors(cudaStreamDestroy(stream0));

	// cleanup memory
	checkCudaErrors(cudaFreeHost(h_signal));
	checkCudaErrors(cudaFreeHost(h_fft));
	checkCudaErrors(cudaFree(d_signal));
	checkCudaErrors(cudaFree(d_fft));


}

void rsp_Fft1d(Complex* pSrc, Complex* pDst, int nFftSize)
{
	cufftHandle plan;
	Complex* d_fftbuf;

	checkCudaErrors(cudaMalloc((void **)&d_fftbuf, nFftSize));
	checkCudaErrors(cufftPlan1d(&plan, nFftSize, CUFFT_C2C, 1));

	checkCudaErrors(cudaMemcpy(d_fftbuf, pSrc, nFftSize, cudaMemcpyHostToDevice));

	checkCudaErrors(cufftExecC2C(plan, (cufftComplex *)d_fftbuf, (cufftComplex *)d_fftbuf, CUFFT_FORWARD));

	checkCudaErrors(cudaMemcpy(pDst, d_fftbuf, nFftSize, cudaMemcpyDeviceToHost));

	checkCudaErrors(cufftDestroy(plan));
	checkCudaErrors(cudaFree(d_fftbuf));
}

void rsp_MemcpyToDevice(Complex* pDevBuf, void* pHost, int size, cudaStream_t stream)
{
	checkCudaErrors(cudaMemcpyAsync(pDevBuf, pHost, size, cudaMemcpyHostToDevice, stream));
}

void rsp_MemcpyToHost(void* pHost, Complex* pDevBuf, int size, cudaStream_t stream)
{
	checkCudaErrors(cudaMemcpyAsync(pHost, pDevBuf, size, cudaMemcpyDeviceToHost, stream));
}

void rsp_MatchFilter_Generator(RSP_CUDE_ENV* pCudaEnv, int nIdx,  int nCode, float fBw, float fPw, float fFs, UINT32 uOffset, int nLen)
{
	pCudaEnv->pMatchFilter[nIdx].nCode = nCode;
	pCudaEnv->pMatchFilter[nIdx].fBw = fBw;
	pCudaEnv->pMatchFilter[nIdx].fPw = fPw;
	pCudaEnv->pMatchFilter[nIdx].fFs = fFs;
	pCudaEnv->pMatchFilter[nIdx].uOffset = uOffset;
	pCudaEnv->pMatchFilter[nIdx].nLen = nLen;

	Complex* pBuf = (Complex*)(&pCudaEnv->pHostMatchFilter[uOffset]);

	int nPw = floor(fPw * fFs);
	float k = fBw / fPw;
	float ts = 1 / fFs;

	for (int i = nPw - 1; i >= 0; i--)
	{
		float t = -fPw / 2 + i * ts;
		pBuf[i].x = cos(-pi*k*t*t);
		pBuf[i].y = sin(-pi*k*t*t);
	}

	rsp_Fft1d(pBuf, pBuf, nLen);
}

void rsp_MatchFilter_Init(RSP_CUDE_ENV* pCudaEnv)
{
	pCudaEnv->MF_NUM = CODE_NUM;
	pCudaEnv->pMatchFilter = (RSP_MATCHFILTER*)malloc(pCudaEnv->MF_NUM * sizeof(RSP_MATCHFILTER));

	rsp_MatchFilter_Generator(pCudaEnv, 0, 1, 5e6, 6.4e-6, 10e6, 0, 65536);
	rsp_MatchFilter_Generator(pCudaEnv, 1, 1, 5e6, 6.4e-6, 10e6, pCudaEnv->pMatchFilter[0].uOffset + pCudaEnv->pMatchFilter[0].nLen, 32768);
	rsp_MatchFilter_Generator(pCudaEnv, 2, 1, 5e6, 6.4e-6, 10e6, pCudaEnv->pMatchFilter[1].uOffset + pCudaEnv->pMatchFilter[1].nLen, 16384);
	rsp_MatchFilter_Generator(pCudaEnv, 3, 1, 5e6, 6.4e-6, 10e6, pCudaEnv->pMatchFilter[2].uOffset + pCudaEnv->pMatchFilter[2].nLen, 8192);
	rsp_MatchFilter_Generator(pCudaEnv, 4, 1, 5e6, 6.4e-6, 10e6, pCudaEnv->pMatchFilter[3].uOffset + pCudaEnv->pMatchFilter[3].nLen, 4096);
	rsp_MatchFilter_Generator(pCudaEnv, 5, 1, 5e6, 6.4e-6, 10e6, pCudaEnv->pMatchFilter[4].uOffset + pCudaEnv->pMatchFilter[4].nLen, 2048);
	rsp_MatchFilter_Generator(pCudaEnv, 6, 1, 5e6, 6.4e-6, 10e6, pCudaEnv->pMatchFilter[5].uOffset + pCudaEnv->pMatchFilter[5].nLen, 1024);
	rsp_MatchFilter_Generator(pCudaEnv, 7, 1, 5e6, 6.4e-6, 10e6, pCudaEnv->pMatchFilter[6].uOffset + pCudaEnv->pMatchFilter[6].nLen, 512);
	rsp_MatchFilter_Generator(pCudaEnv, 8, 1, 5e6, 6.4e-6, 10e6, pCudaEnv->pMatchFilter[7].uOffset + pCudaEnv->pMatchFilter[7].nLen, 256);

	rsp_MemcpyToDevice(pCudaEnv->pDevMatchFilter, pCudaEnv->pHostMatchFilter, pCudaEnv->MF_NUM * sizeof(RSP_MATCHFILTER), pCudaEnv->stream0);
}


void rsp_Win_Init(RSP_CUDE_ENV* pCudaEnv)
{
	float* pHostWin;

	int total_size = 0;
	for (total_size = 16; total_size <= 65536; total_size *= 2);

	pHostWin = (float*)malloc(sizeof(float)*total_size);

	for (int win_size = 16; win_size <= 65536; win_size *= 2)
	{
		rsp_blackman(win_size, &pHostWin[total_size]);
		total_size += win_size;
	}

	checkCudaErrors(cudaMemcpy(pCudaEnv->pBlackmanWin, pHostWin, total_size, cudaMemcpyHostToDevice));

	free(pHostWin);
}
	

void rsp_Init(RSP_CUDE_ENV* pCudaEnv)
{
	checkCudaErrors(cudaHostAlloc((void**)&pCudaEnv->pHostMatchFilter, sizeof(Complex) * MATCHFILTER_BUFFER_SIZE, cudaHostAllocDefault));
	checkCudaErrors(cudaHostAlloc((void**)&pCudaEnv->pHostRxData, sizeof(Complex) * DATA_BUFFER_SIZE, cudaHostAllocDefault));
	checkCudaErrors(cudaHostAlloc((void**)&pCudaEnv->pHostPcData, sizeof(Complex) * DATA_BUFFER_SIZE, cudaHostAllocDefault));
	checkCudaErrors(cudaHostAlloc((void**)&pCudaEnv->pHostMTDData, sizeof(Complex) * DATA_BUFFER_SIZE, cudaHostAllocDefault));

	checkCudaErrors(cudaMalloc((void**)&pCudaEnv->pDevMatchFilter, sizeof(Complex) * MATCHFILTER_BUFFER_SIZE));
	checkCudaErrors(cudaMalloc((void**)&pCudaEnv->pDevRxData, sizeof(Complex) * DATA_BUFFER_SIZE));
	checkCudaErrors(cudaMalloc((void**)&pCudaEnv->pDevPcData, sizeof(Complex) * DATA_BUFFER_SIZE));
	checkCudaErrors(cudaMalloc((void**)&pCudaEnv->pDevMTDData, sizeof(Complex) * DATA_BUFFER_SIZE));
	checkCudaErrors(cudaMalloc((void**)&pCudaEnv->pDevCfarData, sizeof(float) * DATA_BUFFER_SIZE));
	

	int total_size = 0;
	for (int win_size = 16; win_size <= 65536; win_size *= 2)
		total_size += win_size;

	checkCudaErrors(cudaMalloc((void**)&pCudaEnv->pBlackmanWin, sizeof(float) * total_size));


	checkCudaErrors(cudaStreamCreate(&pCudaEnv->stream0));
	checkCudaErrors(cudaStreamCreate(&pCudaEnv->stream1));

	rsp_MatchFilter_Init(pCudaEnv);



}

void rsp_UnInit(RSP_CUDE_ENV* pCudaEnv)
{
	checkCudaErrors(cudaFreeHost(pCudaEnv->pHostMatchFilter));
	checkCudaErrors(cudaFreeHost(pCudaEnv->pHostRxData));
	checkCudaErrors(cudaFreeHost(pCudaEnv->pHostPcData));
	checkCudaErrors(cudaFreeHost(pCudaEnv->pHostMTDData));

	checkCudaErrors(cudaFree(pCudaEnv->pDevMatchFilter));
	checkCudaErrors(cudaFree(pCudaEnv->pDevRxData));
	checkCudaErrors(cudaFree(pCudaEnv->pDevPcData));
	checkCudaErrors(cudaFree(pCudaEnv->pDevMTDData));
	checkCudaErrors(cudaFree(pCudaEnv->pDevCfarData));
	checkCudaErrors(cudaFree(pCudaEnv->pBlackmanWin));

	checkCudaErrors(cudaStreamDestroy(pCudaEnv->stream0));
	checkCudaErrors(cudaStreamDestroy(pCudaEnv->stream0));


}


//
////最小CPI为8，而线程簇(wrap)为32，因此线程采用8x4的块
////wrap的大小可以通过常量wrapSize获得
void rsp_MatchFilter(Complex* pDevBuf, Complex* pMatchFilter, int nRow, int nCol)
{
	//MatrixWiseMulVec(Complex* rx, const float *win, Complex* result, int row, int col, float scale)
	dim3 threadsPerBlock = {8,4};
	dim3 blocksPerGrid;

	blocksPerGrid.y = (nRow + 7) / 8;
	blocksPerGrid.x = (nCol + 3) / 4;
	MatrixWiseMulVecRow  <<< blocksPerGrid, threadsPerBlock >>> (pDevBuf, pMatchFilter, pDevBuf, nRow, nCol, 1.0f);

}
//
void rsp_PulseCompress(RSP_CUDE_ENV* pCudaEnv, Complex* pDevBuf, int nCode, int nRow, int nCol, cudaStream_t stream)
{
	Complex *devMatchFilter;


	cufftHandle plan_pc;
	size_t workSize;
	int new_size_long = nCol;
	int batch_size = nRow;

	int nembed[2] = { nRow , nCol };
	int stride = 1;
	int dist = nCol;


	for (int i = 0; i < pCudaEnv->MF_NUM; i++)
	{
		if (pCudaEnv->pMatchFilter[i].nCode == nCode)
		{
			UINT32 uMFOffset = pCudaEnv->pMatchFilter[i].uOffset;
			devMatchFilter = &pCudaEnv->pDevMatchFilter[uMFOffset];
		}
	}

	checkCudaErrors(cufftCreate(&plan_pc));
	checkCudaErrors(cufftMakePlanMany(plan_pc, 1, &new_size_long, nembed, stride, dist, nembed, stride, dist, CUFFT_C2C, batch_size, &workSize));

	// Transform signal and kernel
	checkCudaErrors(cufftExecC2C(plan_pc, (cufftComplex *)pDevBuf, (cufftComplex *)pDevBuf, CUFFT_FORWARD));

	rsp_MatchFilter(pDevBuf, devMatchFilter, nRow, nCol);

	checkCudaErrors(cufftExecC2C(plan_pc, (cufftComplex *)pDevBuf, (cufftComplex *)pDevBuf, CUFFT_INVERSE));


}

void rsp_Mtd(RSP_CUDE_ENV* pCudaEnv, Complex* pDevBuf, int nRow, int nCol, cudaStream_t stream)
{
	
	dim3 threadsPerBlock = { 8,4 };
	dim3 blocksPerGrid;

	blocksPerGrid.y = (nRow + 7) / 8;
	blocksPerGrid.x = (nCol + 3) / 4;

	int total_size = 0;
	for (int win_size = 16; win_size <= nRow; win_size *= 2)
	{
		total_size += win_size;
	}

	float* pWin = &pCudaEnv->pBlackmanWin[total_size];

	MatrixWiseRealMulVecCol <<< blocksPerGrid, threadsPerBlock >>> (pDevBuf, pWin, pCudaEnv->pDevMTDData, nRow, nCol);

	cufftHandle plan_adv;
	size_t workSize;
	int new_size_long = nRow;
	int batch_size = nCol;

	int nembed[2] = { nRow , nCol };
	int stride = nCol;
	int dist = 1;


	checkCudaErrors(cufftCreate(&plan_adv));
	checkCudaErrors(cufftMakePlanMany(plan_adv, 1, &new_size_long, nembed, stride, dist, nembed, stride, dist, CUFFT_C2C, batch_size, &workSize));

	// Transform signal and kernel
	checkCudaErrors(cufftExecC2C(plan_adv, (cufftComplex *)pCudaEnv->pDevMTDData, (cufftComplex *)pCudaEnv->pDevMTDData, CUFFT_FORWARD));
	checkCudaErrors(cufftDestroy(plan_adv));
	
}

//void __global__ rsp_Cfar(Complex* cfar_in, float* cfar_out, float* cfar_buf,  int nRow, int nCol, BOOL bAbsEn)
//{
//	int crow, ccol;
//	//int cstride;
//	int offset;
//	int cfar_stag;
//
//	int step[4];
//	int last[4];
//
//	crow = threadIdx.x;
//	ccol = blockIdx.x;
//	offset = crow*nCol + ccol;
//
//	crow = blockIdx.y * blockDim.y + threadIdx.y;
//	ccol = blockIdx.x * blockDim.x + threadIdx.x;
//	//cstride = blockDim.x * gridDim.x;
//	offset = crow * nCol + ccol;
//	cfar_stag = nRow * nCol;
//	step[0] = 1;
//	step[1] = 2;
//	step[2] = 4;
//	step[3] = 8;
//	last[0] = nCol - 1;
//	last[1] = nCol - 2;
//	last[2] = nCol - 4;
//	last[3] = nCol - 8;
//
//	if ((crow < nRow) && (ccol < nCol) && bAbsEn)
//	{
//		cfar_buf[offset] = ComplexAbs(cfar_in[offset]);
//		__syncthreads();
//	}
//	
//
//	if (ccol >= last[0])
//		cfar_buf[cfar_stag + offset] = 0.0f;
//	else
//		cfar_buf[cfar_stag + offset] = cfar_buf[offset] + cfar_buf[offset + step[0]];
//	__syncthreads();
//
//	cfar_stag += nRow * nCol;
//
//	if (ccol >= last[1])
//	cfar_buf[cfar_stag + offset] = 0.0f;
//	else
//		cfar_buf[cfar_stag + offset] = cfar_buf[offset] + cfar_buf[offset + step[1]];
//	__syncthreads();
//
//	cfar_stag += nRow * nCol;
//
//	if (ccol >= last[2])
//	cfar_buf[cfar_stag + offset] = 0.0f;
//	else
//		cfar_buf[cfar_stag + offset] = cfar_buf[offset] + cfar_buf[offset + step[2]];
//	__syncthreads();
//
//	cfar_stag += nRow * nCol;
//
//	if (ccol >= last[3])
//	cfar_buf[cfar_stag + offset] = 0.0f;
//	else
//		cfar_buf[cfar_stag + offset] = cfar_buf[offset] + cfar_buf[offset + step[3]];
//	__syncthreads();
//
//}

//
////脉冲雷达脉冲组处理函数，用于按CPI分组处理的GPU程序，通过参数控制处理流程
////如果 pMatchFilter为NULL，表示不作脉冲压缩
void rsp_pd_cpi(RSP_CUDE_ENV* pCudaEnv, void* pRadarRx, int nCode, int nRow, int nCol,  RSP_MTD_MODE rspMode)
{
		
	rsp_MemcpyToDevice(pCudaEnv->pDevRxData, pRadarRx,  nRow*nCol, pCudaEnv->stream0);
	

	rsp_PulseCompress(pCudaEnv, pCudaEnv->pDevRxData, nCode, nRow, nCol, pCudaEnv->stream0);

	rsp_Mtd(pCudaEnv, pCudaEnv->pDevRxData, nRow, nCol, pCudaEnv->stream0);

	switch (rspMode)
	{
		case RSP_MTD_MTI: break;
		case RSP_MTD_NCA: break;
		case RSP_MTD_MTD: break;
		case RSP_MTD_MTIMTD: break;
		case RSP_MTD_MTINCA:break;

		default:
			break;
	} 

	

}