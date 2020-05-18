#include"MemCheck.hpp"
#include"Macro.h"
#include"Histogram.hpp"
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
//	//Magic Macro测试
//	int a = COUNT_MACRO_VAR_ARGS(1,2,3,4,5,6);
//	cout<<a<<endl;
	//初始化测试，不可注释
	_MutiHistogram<3,100,200> MutiHist(9,2);
	Array Arr = {0};
//	//测试数据填充 
//	MutiHist.fill_with_continue_n(3,100);
//	//生成固定间隔序列
//	CreatArray(Arr,ARRAY_SIZE,fix);
//	//生成随机间隔序列
//	CreatArray(Arr,ARRAY_SIZE,random);
	//生成参差间隔序列
	CreatArray(Arr,ARRAY_SIZE,diff);
	//打印生成序列
	PrintArray(Arr,ARRAY_SIZE);
	//生成n阶直方图
	MutiHist.CreatHistogram(Arr,ARRAY_SIZE,3);
//	//打印全阶柱序-键值数组
//	MutiHist.AddrVarPrint();
//	//打印图柱地址对
//	MutiHist.BarPrint(MutiHist[0][10]);
//	//打印三阶直方图BarDict
//	MutiHist.HistPrint();
//	//打印三阶直方图图柱索引值
//	MutiHist.HistPrint(3,1);
//	//打印三阶直方图地址对
//	MutiHist.HistPrint(3,2);
//	//打印全阶地址对
//	MutiHist.BarKeyDictPrint();
//	//打印全阶归并地址对
//	MutiHist.AllOrtersBarKeyDictPrint();
	//全阶按键值排序
	//0:全阶 1-OrderSize:单阶 OrderSize+全阶 
	//“Key”:按键值 ”Lens”:按长度
	MutiHist.HistogramSort(0,"Key");
	//打印直方图排序数组
	//0:全阶 1-OrderSize:单阶 OrderSize+全阶 
	MutiHist.SortPrint(0,"Key");
	//选取排序数组Tolerance内长度为size的序列队
	int Tolerance = 60,size = 6,Min = 5;
	size_t OutArray[10] = {0};
	MutiHist.Filter(1,1,OutArray,size,Tolerance,Min);
	PrintArray(OutArray,size);
	//连续地址对搜索
	//type: 0: not drop, 1:drop
	//findlens表示地址对个数
	//上述size表示直方图柱数
	//AddrArray长度有可能溢出，需设置较大值
	size_t AddrArray[10] = {0};
	int findLens = 5, orders = 1, type = 0;
	MutiHist.AddrValVerify(orders,OutArray,AddrArray,size,findLens,type);
	PrintArray(AddrArray,findLens);
	//堆栈地址检查
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
