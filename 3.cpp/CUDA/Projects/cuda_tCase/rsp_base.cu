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

// Complex addition
static __device__ __host__ inline Complex ComplexAdd(Complex a, Complex b)
{
	Complex c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return c;
}

// Complex scale
static __device__ __host__ inline Complex ComplexScale(Complex a, float s)
{
	Complex c;
	c.x = s * a.x;
	c.y = s * a.y;
	return c;
}

// Complex multiplication
static __device__ __host__ inline Complex ComplexMul(Complex a, Complex b)
{
	Complex c;
	c.x = a.x * b.x - a.y * b.y;
	c.y = a.x * b.y + a.y * b.x;
	return c;
}

// Complex multiplication
static __device__ __host__ inline Complex ComplexMulReal(Complex a, float b)
{
	Complex c;
	c.x = a.x * b;
	c.y = a.y * b;
	return c;
}


// Complex multiplication
static __device__ __host__ inline float ComplexAbs(Complex a)
{
	float c;

	c = fabsf(a.x * a.x + a.y * a.y);

	return c;
}


// Complex pointwise multiplication
__global__ void ComplexPointwiseMulAndScale(Complex *a, const Complex *b, int size, float scale)
{
	const int numThreads = blockDim.x * gridDim.x;
	const int threadID = blockIdx.x * blockDim.x + threadIdx.x;

	for (int i = threadID; i < size; i += numThreads)
	{
		a[i] = ComplexScale(ComplexMul(a[i], b[i]), scale);
	}
}



//static __global__ void MatrixWiseMulVec(Complex* a, const Complex *b, int row, int col, float scale)
//{
//
//}

__global__ void MatrixWiseMulVecRow(Complex* m, Complex *v, Complex* result, int row, int col, float scale)
{
	int crow, ccol;
	//int cstride;
	int offset;
	Complex vv;

	crow = threadIdx.x;
	ccol = blockIdx.x;
	offset = crow*col + ccol;

	crow = blockIdx.y * blockDim.y + threadIdx.y;
	ccol = blockIdx.x * blockDim.x + threadIdx.x;
	//cstride = blockDim.x * gridDim.x;
	offset = crow * col + ccol;

	if ((crow < row) && (ccol < col))
	{
		vv = ComplexMulReal(v[ccol], scale);
		result[offset] = ComplexMul(m[offset], vv);
	}
}

__global__ void MatrixWiseRealMulVecCol(Complex* m, float *v, Complex* result, int row, int col)
{
	int crow, ccol;
	//int cstride;
	int offset;

	crow = threadIdx.x;
	ccol = blockIdx.x;
	offset = crow*col + ccol;

	crow = blockIdx.y * blockDim.y + threadIdx.y;
	ccol = blockIdx.x * blockDim.x + threadIdx.x;
	//cstride = blockDim.x * gridDim.x;
	offset = crow * col + ccol;

	if ((crow < row) && (ccol < col))
	{
		result[offset] = ComplexMulReal(m[offset], v[crow]);
	}
}


void rsp_FFTRow(cufftComplex *fft_datain, cufftComplex *fft_dataout, int fft_row, int fft_col)
{

	// CUFFT plan advanced API
	cufftHandle plan_adv;
	size_t workSize;
	int fft_size = fft_col;
	int new_size_long = fft_size;
	int batch_size = fft_row;

	int nembed[2] = { fft_row , fft_col };
	int stride = 1;
	int dist = fft_col;

	checkCudaErrors(cufftCreate(&plan_adv));
	checkCudaErrors(cufftMakePlanMany(plan_adv, 1, &new_size_long, nembed, stride, dist, nembed, stride, dist, CUFFT_C2C, batch_size, &workSize));

	// Transform signal and kernel
	checkCudaErrors(cufftExecC2C(plan_adv, (cufftComplex *)fft_datain, (cufftComplex *)fft_dataout, CUFFT_FORWARD));

	//Destroy CUFFT context
	checkCudaErrors(cufftDestroy(plan_adv));



}

void rsp_FFTCol(cufftComplex *fft_datain, cufftComplex *fft_dataout, int fft_row, int fft_col)
{

	int fft_size = fft_row;

	// CUFFT plan advanced API
	cufftHandle plan_adv;
	size_t workSize;
	int new_size_long = fft_size;
	int batch_size = fft_col;

	int nembed[2] = { fft_row , fft_col };
	int stride = fft_col;
	int dist = 1;


	checkCudaErrors(cufftCreate(&plan_adv));
	checkCudaErrors(cufftMakePlanMany(plan_adv, 1, &new_size_long, nembed, stride, dist, nembed, stride, dist, CUFFT_C2C, batch_size, &workSize));

	// Transform signal and kernel
	checkCudaErrors(cufftExecC2C(plan_adv, (cufftComplex *)fft_datain, (cufftComplex *)fft_dataout, CUFFT_FORWARD));

	//Destroy CUFFT context
	checkCudaErrors(cufftDestroy(plan_adv));
}