/*
 * Histogram.h
 *
 *  Created on: Jun 27, 2019
 *      Author: xupengtao
 */

#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_

#include<vector>
#include<memory>
#include<algorithm>
#include<numeric>
#include<set>
#include<xutility>
#include<iostream>
#include<iomanip>
using namespace std;
#include"Macro.h"

#define DIFF_HISTOGRAM_MAX 65536

enum
{
	Filter_DiffLens = 0,						//直方图柱按长度筛选，需将SortArray按Lens排序
	Filter_DiffKey 	= 1,						//直方图柱按键值筛选，需将SortArray按Key排序
	Filter_FixLens 	= 2							//直方图柱按固定值筛选，将筛选出高于阈值的未标记的直方图柱
};

TYPETRAIT_PRINT(char)
TYPETRAIT_PRINT(unsigned char)
TYPETRAIT_PRINT(short)
TYPETRAIT_PRINT(unsigned short)
TYPETRAIT_PRINT(int)
TYPETRAIT_PRINT(unsigned int)
TYPETRAIT_PRINT(float)
TYPETRAIT_PRINT(double)
template<class Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator &)
{
	return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
}
template<class Type>
inline void Typetrait(Type* type,char* strval = "point ")			//作用于指针，如: int* a = NULL;Typetrait(a);
{
	std::cout<<strval;
	iterator<random_access_iterator_tag,Type> Iter;
	Typetrait_print(value_type(Iter));
}
template<class Iter>
inline void Typetrait(Iter iter,char* strval = "iter ")				//作用于迭代器，如: BarIterator<Bar<AddressPart,1>> BarIter;Typetrait(BarIter);
{
	std::cout<<strval;
	Typetrait_print(value_type(iter));
}

/************************************ CompareFunc Zoom Begin ************************************/
typedef bool (*_CmpFunc)(size_t,size_t);
template<class T>
inline bool TestMax(T a,T b)
{
	return a > b;
}
template<class T>
inline bool TestMin(T a,T b)
{
	return a < b;
}
template<class _AddrPart>
inline bool AddrMin(_AddrPart AddrPart_a,_AddrPart AddrPart_b)
{
	return AddrPart_a.Key < AddrPart_b.Key;
}
template<class _AddrPart>
inline bool AddrMax(_AddrPart AddrPart_a,_AddrPart AddrPart_b)
{
	return AddrPart_a.Key > AddrPart_b.Key;
}
/************************************ CompareFunc Zoom End **************************************/

union AddressPart														//地址对联合
{
	size_t			Key;													//定义的任何地址对结构必须使用Key存储排序规则
	unsigned short	Addr[2];												//定义的任何地址对结构必须使用Addr存储一对地址对
};

template<class _AddrPart,class _KeyTx, size_t _AddrPartSize>
struct Bar
{
	typedef _AddrPart 		value_type;
	typedef value_type* 	pointer;
	typedef value_type* 	iterator;
	typedef value_type& 	reference;										//存在BUG 2019.04.10
	typedef size_t		 	size_type;
	typedef	ptrdiff_t		difference_type;
	typedef Bar<_AddrPart,_KeyTx,_AddrPartSize>	This_type;						

	size_t Lens;															//当前地址对长度
	size_t Index;															//数组索引
	size_t ClearStatus;
	_KeyTx Key;																//柱值 定义的任何直方图柱结构必须使用Key存储排序规则
	const size_t Capacity;													//地址对数组容量
	vector<_AddrPart> VecArray;												//vector<_AddrPart>
	
	
	Bar(_KeyTx _Key = 0,size_t _Index = 0):Lens(0),Capacity(_AddrPartSize),ClearStatus(1)	//Construct
	{
		VecArray.resize(_AddrPartSize);
		Key =  _Key;
		Index = _Index;
	}
	Bar(const This_type & BarTmp):Capacity(BarTmp.Capacity)									//Copy Construct
	{
		Lens = BarTmp.Lens;
		VecArray = BarTmp.VecArray;
		ClearStatus = BarTmp.ClearStatus;
		Key = BarTmp.Key;
		Index = BarTmp.Index;
	}
	This_type & operator = (const This_type & BarTmp)										//Assamble Construct
	{
		if(this == &BarTmp)
		{
			return *this;
		}
		Key = BarTmp.Key;
		Index = BarTmp.Index;
		Lens  = BarTmp.Lens;
		ClearStatus = BarTmp.ClearStatus;
		VecArray = BarTmp.VecArray;
		return *this;
	}
	reference operator [] (size_type n){return VecArray[n];}					//operator []

	iterator begin(){return &VecArray[0];}
	size_type size(){return Lens;}
	bool empty() const {return Lens == 0;}

	size_t GetSortKey()												//Key为short, Index为char、short、int
	{
		union
		{
			size_t n;
			unsigned short ush[2];
		} ReSortKey;
#if _MSC_VER
		ReSortKey.ush[0] = Index;
		ReSortKey.ush[1] = Key;
#elif PPC
		ReSortKey.ush[0] = Key;
		ReSortKey.ush[1] = Index;
#endif
		return ReSortKey.n;
	}
	size_t GetSortLens()											//Key为short, Index为char、short、int
	{
		union
		{
			size_t n;
			unsigned short ush[2];
		} ReSortKey;
#if _MSC_VER
		ReSortKey.ush[0] = Index;
		ReSortKey.ush[1] = Lens;
#elif PPC
		ReSortKey.ush[0] = Lens;
		ReSortKey.ush[1] = Index;
#endif
		return ReSortKey.n;
	}
	size_t SetSortInd(size_t KeyInd)
	{
		return KeyInd & 0x0000FFFF;
	}
	void SortByAddr(char* SortType = "min")
	{
		if(Lens > 0)
		{
			if(SortType == "min")
			{
				std::sort(VecArray[0],VecArray[Lens],AddrMin<value_type>);
			}
			else if(SortType == "max")
			{
				std::sort(VecArray[0],VecArray[Lens],AddrMax<value_type>);
			}
		}
	}
	void add(size_t i,size_t order)
	{
		if(Lens < _AddrPartSize)
		{
			VecArray[Lens].Addr[0] = i-order;
			VecArray[Lens].Addr[1] = i;
			Lens++;
		}
		else
		{
			std::cout<<"AddrPartSize OutofRange!"<<endl;
		}
	}
	template<class Type>
	void add(Type DiffTmp,size_t i,size_t order)
	{
		Key = DiffTmp;
		VecArray[Lens].Addr[0] = i-order;
		VecArray[Lens].Addr[1] = i;
		Lens++;
	}
	void clear()
	{
		ClearStatus = 1;
		Lens = 0;
		Key = 0;
	}

	void fill_with_continue_n(size_t n = _AddrPartSize)																//fill_with_continue_n
	{
		clear();
		if (n > Capacity)
		{
			n = Capacity;
		}
		for(size_t i = 0; i < n;i++)
		{
			VecArray[i].Addr[0] = i;
			VecArray[i].Addr[1] = i+1;
		}
		Lens = n;
	}
};

template<class Bar,class _KeyTx,size_t _BarSize>							//直方图结构类模板
struct Histogram
{
	typedef Bar 			value_type;
	typedef value_type* 	pointer;
	typedef value_type* 	iterator;
	typedef value_type& 	reference;										//存在BUG 2019.04.10
	typedef size_t		 	size_type;
	typedef	ptrdiff_t		difference_type;
	typedef Histogram<Bar,_KeyTx,_BarSize>	This_type;			
	
	struct	_BarKeyDict
	{
		typedef _KeyTx value_type;
		_KeyTx Key;
		_KeyTx Lens;
	};			

	size_t Lens;															//当前地址对长度
	size_t ClearStatus;
	const size_t Capacity;													//地址对数组容量
	vector<Bar> VecArray;													//vector<_AddrPart>

	vector<size_t> SortArray;												//排序数组
	size_t SortLens;														//排序长度
	vector<_BarKeyDict> BarKeyDict;

	Histogram():Lens(0),Capacity(_BarSize),ClearStatus(1)							//Construct
	{

		SortLens = 0;
		VecArray.resize(_BarSize);
		BarKeyDict.resize(_BarSize);
		SortArray.resize(_BarSize);
		for(size_t i = 0;i < _BarSize;i++)
		{
			VecArray[i].Index = i;
		}
	}
	Histogram(const This_type & HistTmp):Capacity(HistTmp.Capacity)						//Copy Construct
	{
		Lens = HistTmp.Lens;
		ClearStatus = HistTmp.ClearStatus;
		VecArray = HistTmp.VecArray;
		SortLens = HistTmp.SortLens;
		BarKeyDict = HistTmp.BarKeyDict;
		SortArray = HistTmp.SortArray;
	}
	This_type & operator = (const This_type & HistTmp)									//Assamble Construct
	{
		if(this == &HistTmp)
		{
			return *this;
		}
		Lens = HistTmp.Lens;
		ClearStatus = HistTmp.ClearStatus;
		VecArray = HistTmp.VecArray;
		SortLens = HistTmp.SortLens;
		BarKeyDict = HistTmp.BarKeyDict;
		SortArray = HistTmp.SortArray;
		return *this;
	}
	
	reference operator [] (size_type n){return VecArray[n];}					//operator []

	iterator begin(){return &VecArray[0];}
	size_type size(){return Lens;}
	bool empty() const {return Lens == 0;}
	
	int  FindKey(_KeyTx Key)
	{
		for(int count = 0; count < Lens; count++)
		{
			if(VecArray[i].Key == Key)
			{
				return i;
			}
		}
		return ERROR;
	}
	void Sort(_CmpFunc CmpFunc = NULL)
	{
		vector<size_t>::iterator FirstIter(SortArray.begin());
		vector<size_t>::iterator LastIter(FirstIter + SortLens);
		if(CmpFunc == NULL)
		{
			std::sort(FirstIter,LastIter);
		}
		else
		{
			std::sort(FirstIter,LastIter,CmpFunc);
		}
	}
	void SortByKey(_CmpFunc CmpFunc = NULL)
	{
		if(Lens > 0)
		{
			refresh();
			SortLens = Lens;
			for(size_t i = 0; i < SortLens;i++)
			{
				SortArray[i] = VecArray[i].GetSortKey();
			}
			Sort(CmpFunc);
			for(size_t i = 0; i < SortLens;i++)
			{
				SortArray[i] = VecArray[i].SetSortInd(SortArray[i]);
			}
		}
	}
	void SortByLens(_CmpFunc CmpFunc = NULL)
	{
		if(Lens > 0)
		{
			refresh();
			SortLens = Lens;
			for(size_t i = 0; i < SortLens;i++)
			{
				SortArray[i] = VecArray[i].GetSortLens();
			}
			Sort(CmpFunc);
			for(size_t i = 0; i < SortLens;i++)
			{
				SortArray[i] = VecArray[i].SetSortInd(SortArray[i]);
			}
		}
	}
	int  Filter(size_t FilterType,									//过滤类型
			   size_t *output,										//输出数组,输出数组请确保开辟足够大的空间
			   size_t Tolerance 		= 0,						//容差,在过滤类型为Filter_DiffKey时，当将该值置为0,则使用Key值容差
			   size_t FilterMinNums 	= 0,						//最小输出数据长度
			   int	  MinBarKey 		= 0,						//最小Key值
			   int    MaxBarKey 		= 0,						//最大Key值
			   float  BarCoefficient  	= 0.0						//按Key值比较时，Key值的容差系数,该值大于1.0时，Key值的容差直接使用该值
			   )
	{
		int ValTmp = 0;
		size_t Num = 1;
		int DiffVal = 0;
		float CmpValue = 0.0;
		if(FilterType == Filter_DiffKey)
		{
			ValTmp = BarKeyDict[SortArray[0]].Key;
			if(BarCoefficient > 1.0)							//SortArray为递增数组
			{
				BarCoefficient = 2*(BarCoefficient/(1-BarCoefficient));
			}
			for(size_t i = 1; i < SortLens;i++)
			{
				if(Tolerance == 0)
				{
					if(CmpValue == 0.0)
					{
						if((ValTmp > MinBarKey) && (ValTmp < MaxBarKey))
						{
							if(BarCoefficient > 1.0)
							{
								CmpValue = float(int(BarCoefficient));
							}
							else
							{
								CmpValue = ValTmp * BarCoefficient;
							}
						}
					}
					DiffVal = BarKeyDict[SortArray[i]].Key - ValTmp;
					if(DiffVal > CmpValue)
					{
						if(Num > FilterMinNums)
						{
							for(size_t j = 0 ;j < Num; j++)
							{
								output[j] = SortArray[i+(j-Num)];
							}
							return Num;
						}
						else
						{
							Num = 1;
							CmpValue = 0.0;
							ValTmp = BarKeyDict[SortArray[i]].Key;
						}
					}
					else
					{
						Num++;
					}
				}
				else
				{
					if(size_t(abs(ValTmp - BarKeyDict[SortArray[i]].Key)) > Tolerance)
					{
						if(Num > FilterMinNums)
						{
							for(size_t j = 0 ;j < Num; j++)
							{
								output[j] = SortArray[i+(j-Num)];
							}
							return Num;
						}
						else
						{
							Num = 1;
							ValTmp = BarKeyDict[SortArray[i]].Key;
						}
					}
					else
					{
						Num++;
					}
				}
			}
			if(Num > FilterMinNums)
			{
				for(size_t j = 0 ;j < Num; j++)
				{
					output[j] = SortArray[SortLens+(j-Num)];
				}
				return Num;
			}
		}
		else if(FilterType == Filter_DiffLens)
		{
			ValTmp = BarKeyDict[SortArray[0]].Lens;
			for(size_t i = 1; i < SortLens; i++)
			{
				if(size_t(abs(ValTmp - BarKeyDict[SortArray[i]].Lens)) > Tolerance)
				{
					if(Num > FilterMinNums)
					{
						for(size_t j = 0 ;j < Num; j++)
						{
							output[j] = SortArray[i+(j-Num)];
						}
						return Num;
					}
					else
					{
						Num = 1;
						ValTmp = BarKeyDict[SortArray[i]].Lens;
					}
				}
				else
				{
					Num++;
				}
			}
			if(Num > FilterMinNums)
			{
				for(size_t j = 0 ;j < Num; j++)
				{
					output[j] = SortArray[SortLens+(j-Num)];
				}
				return Num;
			}
		}
		else if(FilterType == Filter_FixLens)
		{
			Num = 0;
			for(size_t i = 0; i < SortLens;i++)
			{
				if(BarKeyDict[SortArray[i]].Lens >= Tolerance)
				{
					output[Num] = SortArray[i];
					Num++;
				}
				else
				{
					if(Num < FilterMinNums)
					{
						Num = 0;
					}
					break;
				}
			}
			return Num;
		}
		return 0;
	}
	int  CountBarPulseSum(size_t *BarLocArray,size_t size)
	{
		int Sum = 0;
		for(size_t i = 0; i < size; i++)
		{
			Sum += BarKeyDict[BarLocArray[i]].Lens;
		}
		return Sum;
	}
	void clear()
	{
		if((SortLens != 0)||(Lens != 0)||(BarKeyDict[0].Key != 0))
		{
			for(size_t i = 0; i < Lens;i++)
			{
				VecArray[i].clear();
				BarKeyDict[i].Key = 0;
				BarKeyDict[i].Lens = 0;
			}
			SortLens = 0;
			ClearStatus = 1;
			Lens = 0;
		}
	}
	void refresh()
	{
		if(ClearStatus == 1)
		{
			for(size_t i = 0; i < Lens;i++)
			{
				BarKeyDict[i].Key = VecArray[i].Key;
				BarKeyDict[i].Lens = VecArray[i].Lens;
			}
			ClearStatus = 0;
		}
		
	}
	template<class Type>
	void OrderDiff(const Type* InArray,Type * DiffArray,size_t len,size_t order)
	{
		for(size_t i = 0;i<order;i++)
		{
			DiffArray[i] = 0;
		}
		for(size_t i = order;i<len;i++)
		{
			DiffArray[i] = InArray[i]-InArray[i-order];
		}
	}
	template<class Type>
	void CreatHistogram(const Type* InArray,Type* DiffArray,int* KeyHashAddr,size_t len,size_t BarMin = 0,size_t tolerance = 0, size_t order = 1)
	{
		clear();
		if(order == 1)
		{
			std::adjacent_difference(InArray,&(InArray[len]),DiffArray);
		}
		else
		{
			OrderDiff(InArray,DiffArray,len,order);
		}
		Type DiffTmp,BarMinTmp = (Type)BarMin;
		
		size_t HashAddrTmp = 0;
		bool HaveBarTag;
		for(size_t i = order ;i < len;i++)								//前order个数无效
		{
			DiffTmp = DiffArray[i];
			if(DiffTmp < BarMinTmp)										//小于BarMin,continue
			{
				continue;
			}
			HaveBarTag = false;
			for(HashAddrTmp = DiffTmp-tolerance; HashAddrTmp <= DiffTmp+tolerance; HashAddrTmp++)
			{
				if(KeyHashAddr[HashAddrTmp] != -1)								
				{
					HaveBarTag = true;
					break;
				}
			}
			if(HaveBarTag == false)								
			{
				if(Lens < _BarSize)
				{
					KeyHashAddr[DiffTmp] = Lens;
					VecArray[Lens].add(DiffTmp,i,order);
					Lens++;
				}
				else
				{
					std::cout<<"BarSize OutofRange!"<<endl;
				}
			}
			else
			{
				VecArray[KeyHashAddr[HashAddrTmp]].add(i,order);
				DiffArray[i] = HashAddrTmp;
			}
		}
		refresh();
	}

	void fill_with_continue_n(size_t n = _BarSize)										//fill_with_continue_n
	{
		clear();
		if (n > Capacity)
		{
			n = Capacity;
		}
		for(size_t i = 0; i < n;i++)
		{
			VecArray[i].Key = i;
			VecArray[i].Index = i;
			VecArray[i].fill_with_continue_n();
		}
		Lens = n;
		refresh();
	}
};

#define ADDR_VAL(x,y) AddrVal[(x)*InArrayLen+(y)]
template<size_t _OrderSize,size_t _BinSize,size_t _AddrPartSize>				
class _MutiHistogram											//orders:阶数,bins:一阶直方图柱数
{
	typedef unsigned short KeyType;								//Using short for Function Sort
	typedef AddressPart _AddressPart;
	typedef Bar<_AddressPart,KeyType,_AddrPartSize> _Bar;
	typedef Histogram<_Bar,KeyType,_BinSize> _Hist;
public:
	size_t CurrentOrders;										//当前直方图阶数
	size_t BarMin;												//直方图柱门限
	size_t BarTolerance;										//直方图柱容差
	vector<_Hist> Hist;											//直方图数据属性 0:全阶、1:1阶、2:2阶...
	vector<size_t> AddrVal;										//阶值-地址数组

	size_t InArrayLen;
	size_t ClearStatus;

	struct _AllOrtersBarKeyDict
	{
		typename _Hist::_BarKeyDict::value_type Key; 
		typename _Hist::_BarKeyDict::value_type Lens;
		int Addrs[_OrderSize];										//键值对应各阶直方图位置
	};
	vector<_AllOrtersBarKeyDict> AllOrtersBarKeyDict;				//定义全阶混合直方图
	vector<vector<typename _Hist::_BarKeyDict> > BarKeyDict;		//定义全阶直方图

	_MutiHistogram(int _BarMin, int _BarTorlerance)
				  :CurrentOrders(0), BarMin(_BarMin), BarTolerance(_BarTorlerance),InArrayLen(0),ClearStatus(0)
	{
		Hist.resize(_OrderSize);
		BarKeyDict.resize(_OrderSize);
		for(size_t i = 0; i < _OrderSize; i++)
		{
			BarKeyDict[i].resize(_BinSize);
		}
		AllOrtersBarKeyDict.resize(_OrderSize*_BinSize);
		clear();
	}
	virtual ~_MutiHistogram(){}
	
	_Hist& operator [](size_t n){return Hist[n];}

	template<class Type>
	void CreatHistogram(const Type* InArray, size_t len, size_t orders = 1)
	{
		clear(CurrentOrders);
		CurrentOrders = orders;
		AddrVal.resize(len*_OrderSize);
		InArrayLen = len;
		if(orders > _OrderSize)
		{
			orders = _OrderSize;
		}
		else if(orders < 1)
		{
			orders = 1;
		}
		Type * DiffArray = NULL;													//梯度数组
		DiffArray = new Type[len];
		int* KeyHashAddr = new int[DIFF_HISTOGRAM_MAX];								//HashTable
		for(size_t i = 0;i < orders ;i++)
		{
			memset(KeyHashAddr,-1,(DIFF_HISTOGRAM_MAX)*sizeof(int));				//-1为Hash初始标志位
			Hist[i].CreatHistogram(InArray,DiffArray,KeyHashAddr,len,BarMin,BarTolerance,i+1);
			for(size_t j = 0;j<len-i-1;j++)
			{
				ADDR_VAL(i,j) = DiffArray[j+i+1];
			}
			for(size_t j = len-i-1;j<len;j++)
			{
				ADDR_VAL(i,j) = 10000000;
			}
		}
		refresh(orders);
		delete[] DiffArray;
		delete[] KeyHashAddr;
	}
	void HistogramSort(size_t order,char * Type = "Key", _CmpFunc CmpFunc = NULL)
	{
		if((order > _OrderSize)||(order <= 0))
		{
			order = _OrderSize;
		}
		else
		{
			order = order - 1;
		}
		if(order == _OrderSize)
		{
			for(int i = 0; i < _OrderSize; i++)
			{
				if(Type == "Key")
				{
					Hist[i].SortByKey(CmpFunc);
				}
				else
				{
					Hist[i].SortByLens(CmpFunc);
				}
			}
		}
		else
		{
			if(Type == "Key")
			{
				Hist[order].SortByKey(CmpFunc);
			}
			else
			{
				Hist[order].SortByLens(CmpFunc);
			}
		}
	}
	int  Filter(size_t orders,										//阶值
				size_t FilterType,									//过滤类型
			    size_t *output,										//输出数组,输出数组请确保开辟足够大的空间
			    size_t Tolerance 		= 0,						//容差,在过滤类型为Filter_DiffKey时，当将该值置为0,则使用Key值容差
			    size_t FilterMinNums 	= 0,						//最小输出数据长度
			    size_t MinBarKey 		= 0,						//最小Key值
			    size_t MaxBarKey 		= 0,						//最大Key值
			    float  BarCoefficient  	= 0.0						//按Key值比较时，Key值的容差系数,该值大于1.0时，Key值的容差直接使用该值
			    )
	{
		if((orders > _OrderSize)||(orders == 0))
		{
			orders = _OrderSize;
		}
		else
		{
			orders = orders - 1;
		}
		if(orders != _OrderSize)
		{
			return Hist[orders].Filter(FilterType,output,Tolerance,FilterMinNums,MinBarKey,MaxBarKey,BarCoefficient);
		}
		else
		{
			return 0;
		}
	}
	int  CountBarPulseSum(size_t orders, size_t *BarLocArray,size_t size)
	{
		if((orders > _OrderSize)||(orders == 0))
		{
			orders = _OrderSize;
		}
		else
		{
			orders = orders - 1;
		}
		if(orders != _OrderSize)
		{
			return Hist[orders].CountBarPulseSum(BarLocArray,size);
		}
		else
		{
			return 0;
		}
	}
	template<class _Tx>
	int  AddrValVerify(size_t orders,								//阶值
					   _Tx* BarLocArray,							//输入数组
					   _Tx* AddrOutArray,							//输出数组
					   size_t BarLocArrayLens,						//输入数组长度
					   size_t findLens,								//连续地址对长度
					   size_t type									//type: 0: not drop, 1:drop
					   )
	{
		if((orders > _OrderSize)||(orders <= 0))
		{
			orders = _OrderSize;
		}
		else
		{
			orders = orders - 1;
		}
		size_t *KeyArray = new size_t[BarLocArrayLens];
		size_t *LensArray = new size_t[BarLocArrayLens];
		memset(KeyArray, 0, sizeof(size_t) * BarLocArrayLens);
		memset(LensArray, 0, sizeof(size_t) * BarLocArrayLens);
		size_t LenTmp = 0;													//全部地址对长度
		for(size_t i = 0; i < BarLocArrayLens; i++)
		{
			if(orders != _OrderSize)
			{
				KeyArray[i] = Hist[orders].BarKeyDict[BarLocArray[i]].Key;
				LensArray[i] = Hist[orders].BarKeyDict[BarLocArray[i]].Lens;
				LenTmp += LensArray[i];
			}
			else
			{
				KeyArray[i] = AllOrtersBarKeyDict[BarLocArray[i]].Key;
				LensArray[i] = AllOrtersBarKeyDict[BarLocArray[i]].Lens;
				LenTmp += LensArray[i];
			}
		}
		set<size_t> KeySet(KeyArray,KeyArray+BarLocArrayLens);			//KeySet用于Type1查找Key值
		size_t Address1 = 0,Address2 = 0,AddrPartLenTmp = 0;				//连续地址对长度;
		if(orders != _OrderSize)
		{
			for(size_t i = 0; i < BarLocArrayLens; i++)
			{
				for(size_t j = 0; j < LensArray[i]; j++)
				{
					Address1 = Hist[orders][BarLocArray[i]][j].Addr[0];
					Address2 = Hist[orders][BarLocArray[i]][j].Addr[1];
					AddrPartLenTmp = 0;
					if(type == 1)
					{
						set<size_t> KeySetTmp = KeySet;
						while(KeySetTmp.find(ADDR_VAL(orders,Address2)) != KeySetTmp.end())
						{
//							//Debug
//							NLOGK(Address1,Address2,AddrPartLenTmp);
//							NLOGK(ADDR_VAL(orders,Address1));
//							NLOGK(ADDR_VAL(orders,Address2));
							KeySetTmp.erase(ADDR_VAL(orders,Address2));
							AddrOutArray[AddrPartLenTmp] = Address1;
							Address1 = Address2;
							Address2 = Address1+orders+1;
							AddrPartLenTmp++;
							if(AddrPartLenTmp >= findLens)
							{
								break;
							}
						}
					}
					else
					{
//						//Debug
//						NLOGK(Address1,Address2,AddrPartLenTmp);
//						NLOGK(ADDR_VAL(orders,Address1));
//						NLOGK(ADDR_VAL(orders,Address2));
						while(KeySet.find(ADDR_VAL(orders,Address2)) != KeySet.end())
						{
							AddrOutArray[AddrPartLenTmp] = Address1;
							Address1 = Address2;
							Address2 = Address1+orders+1;
							AddrPartLenTmp++;
							if(AddrPartLenTmp >= findLens)
							{
								break;
							}
						}
					}
					if(AddrPartLenTmp >= findLens)
					{
						break;
					}
				}
				if(AddrPartLenTmp >= findLens)
				{
					break;
				}
				else
				{
					KeySet.erase(KeyArray[i]);
					AddrPartLenTmp = 0;
				}
			}
		}
		else
		{
			size_t j = 0,nAddrPartFind = 1,AddrPartOrder;				//地址对数组取址,多阶地址对连续标志,地址对阶循环
			size_t LenSum,OrderInd;										//Key值全部阶总长度,遍历阶变量
			int BarInd;
			for(size_t i = 0; i < BarLocArrayLens; i++)
			{
				j = 0;
				LenSum = 0,OrderInd = 0,BarInd = 0;
				while((LenSum < LensArray[i])||(OrderInd >= _OrderSize))
				{
					BarInd = AllOrtersBarKeyDict[BarLocArray[i]].Addrs[OrderInd];
					if(BarInd == -1)
					{
						j = 0;
						OrderInd++;
						continue;
					}
					if(Hist[OrderInd][BarInd].Lens == 0)
					{
						j = 0;
						OrderInd++;
						continue;
					}
					else
					{
						Address1 = Hist[OrderInd][BarInd][j].Addr[0];
						Address2 = Address1+OrderInd+1;
						AddrPartLenTmp = 1;
						nAddrPartFind = 1;
						if(type == 1)
						{
							set<size_t> KeySetTmp = KeySet;
							while(nAddrPartFind == 1)
							{
								nAddrPartFind = 0;
								for(AddrPartOrder = 0; AddrPartOrder < _OrderSize; AddrPartOrder++)
								{
									if(KeySetTmp.find(ADDR_VAL(AddrPartOrder,Address2+AddrPartOrder)) != KeySetTmp.end())
									{
										KeySetTmp.erase(ADDR_VAL(AddrPartOrder,Address2+AddrPartOrder));
										nAddrPartFind = 1;
										break;
									}
								}
								if(nAddrPartFind == 1)
								{
									////Debug
									//NLOGK(Address1,Address2,AddrPartLenTmp);
									//NLOGK(ADDR_VAL(OrderInd,Address1));
									//NLOGK(ADDR_VAL(OrderInd,Address2));
									AddrOutArray[AddrPartLenTmp - 1] = Address1;
									Address1 = Address2; 
									AddrPartLenTmp++;
									if(AddrPartLenTmp >= findLens)
									{
										break;
									}	
								}
							}
						}
						else
						{
							while(nAddrPartFind == 1)
							{
								nAddrPartFind = 0;
								for(AddrPartOrder = 0; AddrPartOrder < _OrderSize; AddrPartOrder++)
								{
									if(KeySet.find(ADDR_VAL(AddrPartOrder,Address2+AddrPartOrder)) != KeySet.end())
									{
										nAddrPartFind = 1;
										break;
									}
								}
								if(nAddrPartFind == 1)
								{
									////Debug
									//NLOGK(Address1,Address2,AddrPartLenTmp);
									//NLOGK(ADDR_VAL(OrderInd,Address1));
									//NLOGK(ADDR_VAL(OrderInd,Address2));
									AddrOutArray[AddrPartLenTmp - 1] = Address1;
									Address1 = Address2; 
									AddrPartLenTmp++;
									if(AddrPartLenTmp >= findLens)
									{
										break;
									}	
								}
							}
						}
						if(AddrPartLenTmp >= findLens)
						{
							break;
						}
						j++;
						LenSum++;
					}
				}
				if(AddrPartLenTmp >= findLens)
				{
					break;
				}
				else
				{
					KeySet.erase(KeyArray[i]);
					AddrPartLenTmp = 0;
				}
			}
		}
		delete[] KeyArray;
		delete[] LensArray;
		return AddrPartLenTmp;
	}
	void merge()
	{
		KeyType KeyArray[_OrderSize*_BinSize] = {0};
		KeyType LenArray[_OrderSize*_BinSize] = {0};
		for(int order = 0;order < _OrderSize;order++)
		{
			for(int bin = 0;bin < _BinSize;bin++)
			{
				KeyArray[order*_BinSize + bin] = BarKeyDict[order][bin].Key;
				LenArray[order*_BinSize + bin] = BarKeyDict[order][bin].Lens;
			}
		}
		set<KeyType> KeySet(KeyArray,KeyArray+_OrderSize*_BinSize);
		set<KeyType>::iterator KeySetIter = KeySet.begin();
		int bin = 0,key,lens;
		for(size_t i = 0; i < KeySet.size();i++)
		{
			if(*KeySetIter != 0)
			{
				AllOrtersBarKeyDict[bin].Key = *KeySetIter;
				key = *KeySetIter,lens = 0;
				for(int j = 0; j < _OrderSize; j++)
				{
					for(int l = 0; l < _BinSize; l++)
					{
						if(KeyArray[j*_BinSize+l] == key)
						{
							AllOrtersBarKeyDict[bin].Addrs[j] = l;
							lens += LenArray[j*_OrderSize+l];
							break;
						}
					}
				}
				AllOrtersBarKeyDict[bin].Lens = lens;
				bin++;
			}
			KeySetIter++;
		}
	}
	void clear(size_t Orders = _OrderSize)
	{
		for(size_t order = 0;order < Orders;order++)
		{
			if(Hist[order][0].Key != 0)
			{
				Hist[order].clear();
				BarKeyDict[order] = Hist[order].BarKeyDict;
			}
		}
		for(size_t bin = 0; bin < _OrderSize*_BinSize; bin++)
		{
			AllOrtersBarKeyDict[bin].Key = 0;
			AllOrtersBarKeyDict[bin].Lens = 0;
			for(size_t order = 0; order < _OrderSize; order++)
			{
				AllOrtersBarKeyDict[bin].Addrs[order] = -1;
			}
		}
		ClearStatus = 1;
	}
	void refresh(size_t Orders = _OrderSize)
	{
		if(ClearStatus == 1)
		{
			for(size_t order = 0;order < Orders;order++)
			{
				if(Hist[order][0].Key != 0)
				{
					Hist[order].refresh();
					BarKeyDict[order] = Hist[order].BarKeyDict;
				}
			}
			merge();
			ClearStatus = 0;
		}
	}

	void fill_with_continue_n(int order = 0,size_t n = 1)
	{
		clear();
		if(order >= _OrderSize)							//fill_with_continue_n全阶
		{
			for(order = 0;order < _OrderSize;order++)
			{
				Hist[order].fill_with_continue_n(n);
			}
		}
		else											//fill_with_continue_n单阶
		{
			Hist[order].fill_with_continue_n(n);
		}
		refresh();
	}
	/****************************** Print Functions Begin******************************/
	void AddrVarPrint() const
	{
		if(AddrVal.size() != 0)
		{
			NLOGKSSECBEG("AddrVarPrint");
			for(size_t i = 0;i<AddrVal.size();i++)
			{
				NLOGKSBYHEXWIDTH(4,"No",i,"地址对的值",AddrVal[i]);
			}
			NLOGKSSECEND("AddrVarPrint");
		}
	}
	void BarPrint(_Bar & BarTmp) const
	{
		for(int Ind = 0; Ind < BarTmp.Lens;Ind++)
		{
			NLOGK(Ind,BarTmp[Ind].Key,BarTmp[Ind].Addr[0],BarTmp[Ind].Addr[1]);
		}
	}
	void HistPrint() const								//BarDict Print
	{
		NLOGKSSECBEG("HistPrint");
		for(int order = 0;order < _OrderSize;order++)
		{
			NLOGKSBYHEXWIDTH(4,"阶数",order);
			for(size_t bin = 0; bin < Hist[order].Lens; bin++)
			{
				NLOGKSBYHEXWIDTH(4,"阶数",order,
								   "键值",Hist[order].BarKeyDict[bin].Key,
								   "长度",Hist[order].BarKeyDict[bin].Lens);
			}
		}
		NLOGKSSECEND("HistPrint");
	}
	void HistPrint(int order,int layer = 1) const		//Bar Print
	{
		if((layer < 0) || (layer >= 3))
		{
			cout<<"HistPrint Para error!"<<endl;
			return;
		}
		if(order >= _OrderSize)							//print全阶
		{
			for(order = 0;order < _OrderSize;order++)
			{
				NLOGK(order,Hist[order].Lens);
				for(size_t bin = 0; bin < Hist[order].Lens; bin++)
				{
					if(layer >= 1)
					{
						NLOGK(order,bin,Hist[order][bin].Index);
						for(size_t addrpart = 0;addrpart < Hist[order][bin].Lens; addrpart++)
						{
							if(layer >= 2)
							{
								NLOGK(order,bin,Hist[order][bin].Key,Hist[order][bin][addrpart].Addr[0],Hist[order][bin][addrpart].Addr[1]);
							}
						}
					}
				}
			}
		}
		else											//print单阶
		{
			NLOGK(order,Hist[order].Lens);
			int BarInd = 0;
			for(size_t bin = 0; bin < Hist[order].Lens; bin++)
			{
				if(layer >= 1)
				{
					NLOGK(order,bin,Hist[order][BarInd].Index);
					int AddrPartInd = 0;
					for(size_t addrpart = 0;addrpart < Hist[BarInd].Lens; addrpart++)
					{
						if(layer >= 2)
						{
							NLOGK(order,bin,Hist[order][BarInd].Key,Hist[order][BarInd][AddrPartInd].Addr[0],Hist[order][BarInd][AddrPartInd].Addr[1]);
							AddrPartInd++;
						}
					}
					BarInd++;
				}
			}
		}
	}
	void BarKeyDictPrint() const
	{
		short Key = 0, Lens = 0;
		NLOGKSSECBEG("BarKeyDictPrint");
		for(int i = 0; i < _OrderSize; i++)
		{
			for(int j = 0; j < _BinSize; j++)
			{
				Key = BarKeyDict[i][j].Key;
				Lens = BarKeyDict[i][j].Lens;
				if(Key != 0)
				{
					NLOGKSBYHEXWIDTH(4,"阶数",i,"柱数",j,"键值",Key,"长度",Lens);
				}
			}
		}
		NLOGKSSECEND("BarKeyDictPrint");
	}
 	void AllOrtersBarKeyDictPrint() const
	{
		int Addr = 0;
		short Key = 0, Lens = 0;
		NLOGKSSECBEG("AllOrtersBarKeyDictPrint");
		for(int i = 0; i < _OrderSize*_BinSize; i++)
		{
			if(AllOrtersBarKeyDict[i].Key != 0)
			{
				Key = AllOrtersBarKeyDict[i].Key;
				Lens = AllOrtersBarKeyDict[i].Lens;
				for(int j = 0; j < _OrderSize; j++)
				{
					Addr = AllOrtersBarKeyDict[i].Addrs[j];
					NLOGKSBYHEXWIDTH(4,"No",i,"阶数",j,"键值",Key,"长度",Lens,"柱地址",Addr);
				}
			}
		}
		NLOGKSSECEND("AllOrtersBarKeyDictPrint");
	}
	void SortPrint(int order,char *Mode = "Key") const
	{
		if((order > _OrderSize)||(order <= 0))
		{
			order = _OrderSize;
		}
		else
		{
			order = order - 1;
		}
		NLOGKSSECBEG("SortPrint");
		if(order == _OrderSize)							//print全阶
		{
			for(order = 0;order < _OrderSize;order++)
			{
				NLOGKSBYHEXWIDTH(4,"阶数",order,"序数组长度",Hist[order].SortLens);
				for(size_t i = 0; i < Hist[order].SortLens; i++)
				{
					NLOGKSBYHEXWIDTH(4,"No",i,"序数组索引",Hist[order].SortArray[i]);
				}
			}
		}
		else											//print单阶
		{
			NLOGKSBYHEXWIDTH(4,"阶数",order,"序数组长度",Hist[order].SortLens);
			for(size_t i = 0; i < Hist[order].SortLens; i++)
			{
				NLOGKSBYHEXWIDTH(4,"No",i,"序数组索引",Hist[order].SortArray[i]);
			}
		}
		NLOGKSSECEND("SortPrint");
	}
	/****************************** Print Functions End ********************************/
};

#endif /* HISTOGRAM_H_ */
