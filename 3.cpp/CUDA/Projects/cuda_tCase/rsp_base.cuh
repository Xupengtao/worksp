#pragma once

#define pi	3.1415926535897f

// Complex data type
typedef float2 Complex;


static __device__ __host__ inline Complex ComplexAdd(Complex, Complex);
static __device__ __host__ inline Complex ComplexScale(Complex, float);
static __device__ __host__ inline Complex ComplexMul(Complex, Complex);
__device__ __host__ inline float ComplexAbs(Complex a);
__global__ void ComplexPointwiseMulAndScale(Complex *, const Complex *, int, float);
__global__ void ComplexPointwiseMulAndScale(Complex *a, const Complex *b, int size, float scale);
__global__ void MatrixWiseMulVecRow(Complex* m, Complex *v, Complex* result, int row, int col, float scale);
__global__ void MatrixWiseRealMulVecCol(Complex* m, float *v, Complex* result, int row, int col);

void rsp_FFTRow(cufftComplex *fft_datain, cufftComplex *fft_dataout, int fft_row, int fft_col);
void rsp_FFTCol(cufftComplex *fft_datain, cufftComplex *fft_dataout, int fft_row, int fft_col);

