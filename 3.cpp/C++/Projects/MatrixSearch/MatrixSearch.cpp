#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "MatrixSearch.h"

#ifdef __WINDOWS__
#include "thr/threads.h"
#include <Windows.h>
#else
#include<pthread.h>
#endif


#include <omp.h>
#define num_threads 4


int del_matrix(MatrixStruct* pmat)
{
	if (pmat->dtoa)
		free(pmat->dtoa);
	if (pmat->drf)
		free(pmat->drf);
	if (pmat->ddoa)
		free(pmat->ddoa);
	if (pmat->flag)
		free(pmat->flag);

	if (pmat->cont)
		free(pmat->cont);

	if (pmat->doa)
		free(pmat->doa);
	if (pmat->toa)
		free(pmat->toa);
	if (pmat->rf)
		free(pmat->rf);	
	if (pmat->pw)
		free(pmat->pw);
	if (pmat->pa)
		free(pmat->pa);
	if (pmat)
		free(pmat);
	return 1;
}

QMatrixStruct * q_matrix_open(unsigned int n, unsigned int m)
{
	QMatrixStruct *pMatrix;

	pMatrix = (QMatrixStruct *)calloc(1,sizeof(QMatrixStruct));
	if (!pMatrix) return NULL;
	pMatrix->dtoa = (unsigned int *)calloc(n*m,sizeof(unsigned int));
	if (!pMatrix->dtoa) return NULL;
	pMatrix->drf = (unsigned int *)calloc(n*m,sizeof(unsigned int));
	if (!pMatrix->drf) return NULL;
	pMatrix->ddoa = (unsigned int *)calloc(n*m,sizeof(unsigned int));
	if (!pMatrix->ddoa) return NULL;
	pMatrix->flag = (unsigned int *)calloc(n*m,sizeof(unsigned int));
	if (!pMatrix->flag) return NULL;

	pMatrix->cont = (unsigned int *)calloc(n,sizeof(unsigned int));
	if (!pMatrix->cont) return NULL;

	pMatrix->doa = (unsigned int *)calloc(n, sizeof(unsigned int));
	if (!pMatrix->doa) return NULL;
	pMatrix->toa = (unsigned int *)calloc(n, sizeof(unsigned int));
	if (!pMatrix->toa) return NULL;
	pMatrix->rf = (unsigned int *)calloc(n,sizeof(unsigned int));
	if (!pMatrix->rf) return NULL;
	pMatrix->pw = (unsigned int *)calloc(n, sizeof(unsigned int));
	if (!pMatrix->pw) return NULL;
	pMatrix->pa = (unsigned int *)calloc(n, sizeof(unsigned int));
	if (!pMatrix->pa) return NULL;

	pMatrix->n = n;
	pMatrix->m = m;
	return pMatrix;
};

void q_matrix_calc(QMatrixStruct *pMatrix, unsigned int *toa, unsigned int *rf, unsigned int *doa, unsigned int *pa, unsigned int *pw, int n)
{
	int i, j, tmp;

	for (i = 0; i < n; i++){

		pMatrix->rf[i] = rf[i];
		pMatrix->pw[i] = pw[i];
		pMatrix->doa[i] = doa[i];
		pMatrix->pa[i] = pa[i];
		pMatrix->toa[i] = toa[i];

		for (j = 0; j < pMatrix->m; j++){
			if (pMatrix->m > 100000) printf("err\n");
			if (i + j + 1 < n){
				pMatrix->dtoa[i*(pMatrix->m) + j] = toa[i + j + 1] - toa[i];
				tmp = rf[i + j + 1] - rf[i];
				pMatrix->drf[i*(pMatrix->m) + j] = abs(tmp);
				tmp = doa[i + j + 1] - doa[i];
				pMatrix->ddoa[i*(pMatrix->m) + j] = abs(tmp);
			}
		}
	}
}


int q_matrix_find(QMatrixStruct *pMatrix, QMatrixTrgtStruct *trgt)
{
	unsigned i, j, z;
	unsigned int tmp;

	for (i = 0; i < pMatrix->n; i++)
	{
		if ((pMatrix->rf[i]  < trgt->rf_min)
			|| (pMatrix->rf[i]  > trgt->rf_max)
			|| (pMatrix->doa[i] > trgt->doa_max)
			|| (pMatrix->doa[i] < trgt->doa_min)
			|| (pMatrix->pw[i]  < trgt->pw_min)
			|| (pMatrix->pw[i]  > trgt->pw_max)) continue;

		for (j = 0; j < pMatrix->m; j++){
			if (pMatrix->m > 100000) printf("err\n");
			if (i + j + 1 < pMatrix->n){
				if ((pMatrix->ddoa[i*pMatrix->m + j] > trgt->ddoaThr)
					|| (pMatrix->drf[i*pMatrix->m + j] > trgt->drfThr))continue;
//#pragma omp parallel for
				for (z = 1; z <= trgt->multi; z++)
				{
					if ((pMatrix->dtoa[i*pMatrix->m + j] >= trgt->pri_min * z)
						&& (pMatrix->dtoa[i*pMatrix->m + j] <= trgt->pri_max * z))
					{
						pMatrix->flag[i*pMatrix->m + j] = z;
					}
				}
			}
		}
	}

	for (i = 0; i < pMatrix->n; i++){
		for (j = 0; j < pMatrix->m; j++){
			if (pMatrix->m > 100000) printf("err\n");
			if (pMatrix->flag[i*pMatrix->m + j] > 0){

				if (pMatrix->cont[i] == 0) pMatrix->cont[i] = 1;
				tmp = pMatrix->cont[i] + 1;
				if (tmp > pMatrix->cont[j + 1 + i]) pMatrix->cont[j + 1 + i] = tmp;
			}
		}
	}

	return 1;
}

/*
void main1(PDW_STRUCT *pPDW, unsigned int pdw_n)
{
	QMatrixStruct *pMatrix;
	int n = 10000, m = 100;
	int i;

	unsigned int pdw_toa[10000], pdw_rf[10000], pdw_doa[10000], pdw_flag[10000], pdw_pa[10000], pdw_pw[10000];

	for (i = 0; i < n; i++)
	{
		pdw_toa[i]  = (unsigned int)pPDW[i].toa;
		pdw_rf[i]   = (unsigned int)pPDW[i].rf;
		pdw_doa[i]  = (unsigned int)pPDW[i].doa;
		pdw_flag[i] = (unsigned int)pPDW[i].flag;
		pdw_pw[i] = (unsigned int)pPDW[i].pw;
		pdw_pa[i] = (unsigned int)pPDW[i].pa;
	}


	MatrixTrgtStruct trgt = { 9410, 9430, 3951, 4166, 0, 1024, 0, 3000, 0, 3000, 10, 200, 2 };
	pMatrix = q_matrix_open(n, m);

	q_matrix_calc(pMatrix, pdw_toa, pdw_rf, pdw_doa, pdw_pa, pdw_pw, pdw_n);
	q_matrix_find(pMatrix, &trgt);
}*/

ProcessArgs* init_process(PDW *pPDW, unsigned int pdw_n)
{
	QMatrixStruct *pMatrix;
	int n = pdw_n, m = 100;
	int i;

	unsigned int *pdw_toa = (unsigned*)malloc(sizeof(unsigned)*n);
	if (!pdw_toa) return NULL;
	unsigned int *pdw_rf = (unsigned*)malloc(sizeof(unsigned)*n);
	if (!pdw_rf) return NULL;
	unsigned int *pdw_doa = (unsigned*)malloc(sizeof(unsigned)*n);
	if (!pdw_doa) return NULL;
	unsigned int *pdw_flag = (unsigned*)malloc(sizeof(unsigned)*n);
	if (!pdw_flag) return NULL;
	unsigned int *pdw_pa = (unsigned*)malloc(sizeof(unsigned)*n);
	if (!pdw_pa) return NULL;
	unsigned int *pdw_pw = (unsigned*)malloc(sizeof(unsigned)*n);
	if (!pdw_pw) return NULL;


	for (i = 0; i < n; i++)
	{
		pdw_toa[i] = (unsigned int)pPDW[i].toa;
		pdw_rf[i] = (unsigned int)pPDW[i].rf;
		pdw_doa[i] = (unsigned int)pPDW[i].doa;
		pdw_flag[i] = 0;
		pdw_pw[i] = (unsigned int)pPDW[i].pw;
		pdw_pa[i] = (unsigned int)pPDW[i].pa;
	}

	MatrixTrgtStruct trgt = { 9410, 9430, 3951, 4166, 0, 1024, 0, 3000, 0, 3000, 10, 200, 2 };
	

	pMatrix = q_matrix_open(n, m);
	//del_matrix(pMatrix);
	if (!pMatrix) return NULL;
	ProcessArgs* pargs = (ProcessArgs*)malloc(sizeof(ProcessArgs));
	if (!pargs) return NULL;
	pargs->mat = pMatrix;
	pargs->pdw_doa = pdw_doa;
	pargs->pdw_flag = pdw_flag;
	pargs->pdw_n = pdw_n;
	pargs->pdw_pa = pdw_pa;
	pargs->pdw_rf = pdw_rf;
	pargs->pdw_pw = pdw_pw;
	pargs->pdw_toa = pdw_toa;
	pargs->trgt = &trgt;
	return pargs;
}
#ifdef __WINDOWS__
int process(void *proargs)
{
	ProcessArgs* args = (ProcessArgs*)proargs;
	
	q_matrix_calc(args->mat, args->pdw_toa, args->pdw_rf, args->pdw_doa,
		args->pdw_pa, args->pdw_pw, args->pdw_n);
	printf("args: %d\n", (unsigned)args);
	printf("matrix: %d\n", (unsigned)args->mat);
	q_matrix_find(args->mat, args->trgt);
	return 1;
}
#else
void* process(void *proargs)
{
	ProcessArgs* args = (ProcessArgs*)proargs;
	
	q_matrix_calc(args->mat, args->pdw_toa, args->pdw_rf, args->pdw_doa,
		args->pdw_pa, args->pdw_pw, args->pdw_n);
	q_matrix_find(args->mat, args->trgt);
	return NULL;
}
#endif

int setTrgt(ProcessArgs* args,MatrixTrgtStruct trgt)
{
	args->trgt = &trgt;
	return 1;
}



/*
int main1()
{
	#ifdef __WINDWOS__
	char filename[] = "D:\\�ļ�32_11.csv";
	#else
	char filename[] = "/home/admin/qwt/kepan/文件32_11.csv";
	#endif
	char line[1024];
	unsigned *data;
	int row, col;
	PDW_STRUCT *pPDW;

	row = get_row(filename);
	col = get_col(filename);
	printf("row=%d\n", row);
	if (row < 0 || col < 0)
		return -1;
	row = 600000;
	data = (unsigned *)malloc(sizeof(float)*row*col);
	if (!data) return -1;
	get_two_dimension(line, data, filename, row, col);
	printf("row=%d\n", row);
	printf("col=%d\n", col);
	//print_two_dimension(data, row, col);
	//return 1;
	printf("read ok!\n");
	#ifdef __WINDWOS__
	pPDW = (PDW_STRUCT *)(data + 8);
	#else
	pPDW = (PDW_STRUCT *)(data + 8);
	#endif
	clock_t tm[10];
	MatrixTrgtStruct trgt = { 9410, 9430, 3951, 4166, 0, 1024, 0, 3000, 0, 3000, 10, 200, 2 };

	///***************************���߳�*************************
	int pdw_num = row;
	ProcessArgs * pargs_all = init_process(pPDW, pdw_num);
	if (!pargs_all) return -1;
	pargs_all->trgt = &trgt;
	tm[0] = clock();

	process(pargs_all);

	tm[1] = clock();
	/////////////////////////��ӡ������ļ�
	FILE* fp1;
	#ifdef __WINDWOS__
	fopen_s(&fp1, "flag_res00.txt", "w+");
	#else
	fp1 = fopen("flag_res00.txt", "w+");
	#endif
	for (int i = 0; i < row; i++)
	{
			//printf("%d\t", ps[i]->mat->flag[j]);
			fprintf(fp1, "%d\t%u\n", pargs_all->mat->cont[i], pargs_all->pdw_toa[i]);
	}
	fclose(fp1);
	/////////////////////////
	printf("process1 ok!\n");

	///***************************���߳�*************************
	#ifdef __WINDWOS__
	thrd_t thread[num_threads];
	#else
	pthread_t thread[num_threads];
	pthread_attr_t attr[num_threads];
	for (int i =0;i<num_threads;i++)
	{
		pthread_attr_init(&attr[i]);
    	pthread_attr_setscope(&attr[i],PTHREAD_SCOPE_SYSTEM);
	}
    
	#endif

	int ret[num_threads];
	////Ϊ���̷߳����ڴ棬����
	ProcessArgs* ps[num_threads];
	for (int i = 0; i < num_threads; i++)
	{
		ps[i] = (ProcessArgs*)malloc(sizeof(ProcessArgs));
		if (!ps[i]) return -1;
		ps[i]->mat = q_matrix_open(pdw_num / num_threads, 100);
		if (!ps[i]->mat) return -1;
		ps[i]->pdw_doa = pargs_all->pdw_doa + i * pdw_num / num_threads;
		ps[i]->pdw_flag = pargs_all->pdw_flag + i * pdw_num / num_threads;
		ps[i]->pdw_n = pargs_all->pdw_n / num_threads;
		ps[i]->pdw_pa = pargs_all->pdw_pa + i * pdw_num / num_threads;
		ps[i]->pdw_pw = pargs_all->pdw_pw + i * pdw_num / num_threads;
		ps[i]->pdw_rf = pargs_all->pdw_rf + i * pdw_num / num_threads;
		ps[i]->pdw_toa = pargs_all->pdw_toa + i * pdw_num / num_threads;
		ps[i]->trgt = &trgt;
	}

	tm[2] = clock();
	for (int i = 0; i < num_threads; i++)
	{
		////����߳�
		#ifdef __WINDWOS__
		ret[i] = thrd_create(&thread[i], process, (void*)ps[i]);
		if (ret[i] != thrd_success)
			return -1;
		#else
		ret[i] = pthread_create(&thread[i],&attr[i],process,(void*)ps[i]);
		if (ret[i] != 0)
			return -1;
		#endif
		
	}
	printf("ok\n");
	for (int i = 0; i < num_threads; i++)
	{
		////�ȴ����߳��������
		#ifdef __WINDWOS__
		thrd_join(thread[i], &ret[i]);
		#else
		pthread_join(thread[i],NULL);
		#endif
	}

	tm[3] = clock();
	printf("process2 ok!\n");
	printf("time1(single thread): %fl s\ntime2(multi threads): %fl s\n", (double)(tm[1] - tm[0]) / CLOCKS_PER_SEC,
		(double)(tm[3] - tm[2]) / CLOCKS_PER_SEC);
	
	///////////////////��ӡ������ļ�
/*
	FILE* fp;
	#ifdef __WINDWOS__
	fopen_s(&fp, "flag_res11.txt", "w+");
	#else
	fp = fopen("flag_res11.txt", "w+");
	#endif
	for (int i = 0; i < num_threads; i++)
	{
		for (int j = 0; j < row / num_threads; j++)
		{
			if (j % 10 == 0) 
			{ 
				//printf("%d:", j / 10 + 1); 
				//fprintf(fp, "%d:", j / 10 + 1);
			}
			//printf("%d\t", ps[i]->mat->flag[j]);
			fprintf(fp,"%d\t%u\n", ps[i]->mat->cont[j],ps[i]->pdw_toa[j]);
			if (j % 10 == 9)
			{
				//printf("\n");
				//fprintf(fp,"\n");
			}
		}
		//printf("\n");
	}
	fclose(fp);*
	///////////////////////////////
	free(data);
	free(pargs_all->pdw_doa);
	free(pargs_all->pdw_flag);
	free(pargs_all->pdw_pa);
	free(pargs_all->pdw_pw);
	free(pargs_all->pdw_rf);
	free(pargs_all->pdw_toa);
	//getchar();
	/*for (int i = 0; i < num_threads; i++)
	{
		del_matrix(ps[i]->mat);
		free(ps[i]);
	}*
	getchar();
}*/
