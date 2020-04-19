#ifndef __READ_CSV_H__
#define __READ_CSV_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DataFrame.h"

#pragma pack(push)
#pragma pack(1)
typedef struct  QMatrixStruct
{
	unsigned int *dtoa;
	unsigned int *drf;
	unsigned int *ddoa;

	unsigned int *toa;
	unsigned int *rf;
	unsigned int *pw;
	unsigned int *pa;
	unsigned int *doa;
	unsigned int *flag;

	unsigned int *cont;

	unsigned int n;
	unsigned int m;
}MatrixStruct;


typedef struct QMatrixTrgtStruct
{
	unsigned int rf_min;
	unsigned int rf_max;

	unsigned int pri_min;
	unsigned int pri_max;

	unsigned int doa_min;
	unsigned int doa_max;

	unsigned int pa_min;
	unsigned int pa_max;

	unsigned int pw_min;
	unsigned int pw_max;

	unsigned int ddoaThr;
	unsigned int drfThr;

	unsigned int multi;
}MatrixTrgtStruct;

typedef struct
{
	unsigned idx;
	unsigned rf;
	unsigned pw;
	unsigned doa;
	unsigned pa;
	unsigned toa;
	unsigned time;
	unsigned flag;
}PDW_STRUCT;

typedef struct ProcessArgs
{
	MatrixTrgtStruct* trgt;
	MatrixStruct *mat;
	unsigned int *pdw_toa;
	unsigned int *pdw_rf;
	unsigned int *pdw_doa;
	unsigned int *pdw_flag;
	unsigned int *pdw_pw;
	unsigned int *pdw_pa;
	int pdw_n;
}ProcessArgs;

int del_matrix(MatrixStruct* pmat);
QMatrixStruct * q_matrix_open(unsigned int n, unsigned int m);
void q_matrix_calc(QMatrixStruct *pMatrix, unsigned int *toa, unsigned int *rf,
		unsigned int *doa, unsigned int *pa, unsigned int *pw, int n);

int q_matrix_find(QMatrixStruct *pMatrix, QMatrixTrgtStruct *trgt);
ProcessArgs* init_process(PDW *pPDW, unsigned int pdw_n);
#ifdef __WINDOWS__
int process(void *proargs)
#else
void* process(void *proargs);
#endif
int setTrgt(ProcessArgs* args,MatrixTrgtStruct trgt);
#pragma pack(pop)

#endif
