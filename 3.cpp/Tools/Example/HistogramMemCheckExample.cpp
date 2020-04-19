#include"../MemCheck.hpp"
#include"../Macro.h"
#include"../Histogram.hpp"
#if _MSC_VER
#elif PPC
#include <taskLibCommon.h>
#endif

#define ARRAY_SIZE 200
typedef int Array[ARRAY_SIZE];
enum{fix = 0, random = 1, diff = 2};
int CreatArray(Array & Arr,size_t len,int Type = fix)
{
	switch(Type)
	{
	case fix:
		for(size_t i = 0; i < len; i++)
		{
			Arr[i] = i*100;
		}
		break;
	case random:
		for(size_t i = 0; i < len; i++)
		{
			Arr[i] = i*100+(i*12673)%71;
		}
		break;
	case diff:
		for(size_t i = 0; i < len; i++)
		{
			Arr[i] = i==0?0:Arr[i-1] + 100 + (i*10)%100;
		}
		break;
	default:
		break;
	}
	return 0;
}

template<typename Tx>
int PrintArray(Tx & Arr,size_t len)
{
	if(len > 0)
	{
		for(size_t i = 0; i < len; i++)
		{
			NLOGK(i,Arr[i]);
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

#if _MSC_VER
int MSCMain()
#elif PPC
int MemCheckAndHistogramTest()
#endif
{
//	//Magic Macro����
//	int a = COUNT_MACRO_VAR_ARGS(1,2,3,4,5,6);
//	cout<<a<<endl;
	//��ʼ�����ԣ�����ע��
	_MutiHistogram<3,100,200> MutiHist(9,2);
	Array Arr = {0};
//	//����������� 
//	MutiHist.fill_with_continue_n(3,100);
//	//���ɹ̶��������
//	CreatArray(Arr,ARRAY_SIZE,fix);
//	//��������������
//	CreatArray(Arr,ARRAY_SIZE,random);
	//���ɲβ�������
	CreatArray(Arr,ARRAY_SIZE,diff);
	//��ӡ��������
	PrintArray(Arr,ARRAY_SIZE);
	//����n��ֱ��ͼ
	MutiHist.CreatHistogram(Arr,ARRAY_SIZE,3);
//	//��ӡȫ������-��ֵ����
//	MutiHist.AddrVarPrint();
//	//��ӡͼ����ַ��
//	MutiHist.BarPrint(MutiHist[0][10]);
//	//��ӡ����ֱ��ͼBarDict
//	MutiHist.HistPrint();
//	//��ӡ����ֱ��ͼͼ������ֵ
//	MutiHist.HistPrint(3,1);
//	//��ӡ����ֱ��ͼ��ַ��
//	MutiHist.HistPrint(3,2);
//	//��ӡȫ�׵�ַ��
//	MutiHist.BarKeyDictPrint();
//	//��ӡȫ�׹鲢��ַ��
//	MutiHist.AllOrtersBarKeyDictPrint();
	//ȫ�װ���ֵ����
	//0:ȫ�� 1-OrderSize:���� OrderSize+ȫ�� 
	//��Key��:����ֵ ��Lens��:������
	MutiHist.HistogramSort(0,"Key");
	//��ӡֱ��ͼ��������
	//0:ȫ�� 1-OrderSize:���� OrderSize+ȫ�� 
	MutiHist.SortPrint(0,"Key");
	//ѡȡ��������Tolerance�ڳ���Ϊsize�����ж�
	int Tolerance = 60,size = 6,Min = 5;
	size_t OutArray[10] = {0};
	MutiHist.Filter(1,1,OutArray,size,Tolerance,Min);
	PrintArray(OutArray,size);
	//������ַ������
	//type: 0: not drop, 1:drop
	//findlens��ʾ��ַ�Ը���
	//����size��ʾֱ��ͼ����
	//AddrArray�����п�������������ýϴ�ֵ
	size_t AddrArray[10] = {0};
	int findLens = 5, orders = 1, type = 0;
	MutiHist.AddrValVerify(orders,OutArray,AddrArray,size,findLens,type);
	PrintArray(AddrArray,findLens);
	//��ջ��ַ���
	MEMCHECK_INIT;
	MEMCHECK_ADDVAR(Arr);
	MEMCHECK_ADDVAR(Tolerance);
	MEMCHECK_ADDVAR(size);
	MEMCHECK_ADDVAR(Min);
	MEMCHECK_ADDVAR(OutArray);
	MEMCHECK_ADDVAR(AddrArray);
	MEMCHECK_ADDVAR(findLens);
	MEMCHECK_ADDVAR(orders);
	MEMCHECK_ADDVAR(MutiHist.BarMin);
	MEMCHECK_ADDVAR(MutiHist.BarTolerance);
	MEMCHECK_ADDVAR(MutiHist.Hist);
	MEMCHECK_ADDVAR(MutiHist.AddrVal);
	MEMCHECK_ADDVAR(MutiHist.InArrayLen);
	MEMCHECK_ADDVAR(MutiHist.ClearStatus);
	MEMCHECK_ADDVAR(MutiHist.AllOrtersBarKeyDict);
	MEMCHECK_ADDVAR(MutiHist.BarKeyDict);
	MEMCHECK_OVERFLOWCHECK;
	cin.get();
	return 1;
}

#if PPC
int Vxmain()
{
	int taskld;
	taskld = taskSpawn("MemCheckAndHistogramTest", 100, VX_FP_TASK, 0x40000,(FUNCPTR)MemCheckAndHistogramTest,
					   0,0,0,0,0,0,0,0,0,0);
	if(taskld == ERROR)
	{
		return _ERROR;
	}
	return _OK;
}
#endif
