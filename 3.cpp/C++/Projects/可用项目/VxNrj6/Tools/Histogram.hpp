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
	Filter_DiffLens = 0,						//ֱ��ͼ��������ɸѡ���轫SortArray��Lens����
	Filter_DiffKey 	= 1,						//ֱ��ͼ������ֵɸѡ���轫SortArray��Key����
	Filter_FixLens 	= 2							//ֱ��ͼ�����̶�ֵɸѡ����ɸѡ��������ֵ��δ��ǵ�ֱ��ͼ��
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
inline void Typetrait(Type* type,char* strval = "point ")			//������ָ�룬��: int* a = NULL;Typetrait(a);
{
	std::cout<<strval;
	iterator<random_access_iterator_tag,Type> Iter;
	Typetrait_print(value_type(Iter));
}
template<class Iter>
inline void Typetrait(Iter iter,char* strval = "iter ")				//�����ڵ���������: BarIterator<Bar<AddressPart,1>> BarIter;Typetrait(BarIter);
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

union AddressPart														//��ַ������
{
	size_t			Key;													//������κε�ַ�Խṹ����ʹ��Key�洢�������
	unsigned short	Addr[2];												//������κε�ַ�Խṹ����ʹ��Addr�洢һ�Ե�ַ��
};

template<class _AddrPart,class _KeyTx, size_t _AddrPartSize>
struct Bar
{
	typedef _AddrPart 		value_type;
	typedef value_type* 	pointer;
	typedef value_type* 	iterator;
	typedef value_type& 	reference;										//����BUG 2019.04.10
	typedef size_t		 	size_type;
	typedef	ptrdiff_t		difference_type;
	typedef Bar<_AddrPart,_KeyTx,_AddrPartSize>	This_type;						

	size_t Lens;															//��ǰ��ַ�Գ���
	size_t Index;															//��������
	size_t ClearStatus;
	_KeyTx Key;																//��ֵ ������κ�ֱ��ͼ���ṹ����ʹ��Key�洢�������
	const size_t Capacity;													//��ַ����������
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

	size_t GetSortKey()												//KeyΪshort, IndexΪchar��short��int
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
	size_t GetSortLens()											//KeyΪshort, IndexΪchar��short��int
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

template<class Bar,class _KeyTx,size_t _BarSize>							//ֱ��ͼ�ṹ��ģ��
struct Histogram
{
	typedef Bar 			value_type;
	typedef value_type* 	pointer;
	typedef value_type* 	iterator;
	typedef value_type& 	reference;										//����BUG 2019.04.10
	typedef size_t		 	size_type;
	typedef	ptrdiff_t		difference_type;
	typedef Histogram<Bar,_KeyTx,_BarSize>	This_type;			
	
	struct	_BarKeyDict
	{
		typedef _KeyTx value_type;
		_KeyTx Key;
		_KeyTx Lens;
	};			

	size_t Lens;															//��ǰ��ַ�Գ���
	size_t ClearStatus;
	const size_t Capacity;													//��ַ����������
	vector<Bar> VecArray;													//vector<_AddrPart>

	vector<size_t> SortArray;												//��������
	size_t SortLens;														//���򳤶�
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
	int  Filter(size_t FilterType,									//��������
			   size_t *output,										//�������,���������ȷ�������㹻��Ŀռ�
			   size_t Tolerance 		= 0,						//�ݲ�,�ڹ�������ΪFilter_DiffKeyʱ��������ֵ��Ϊ0,��ʹ��Keyֵ�ݲ�
			   size_t FilterMinNums 	= 0,						//��С������ݳ���
			   int	  MinBarKey 		= 0,						//��СKeyֵ
			   int    MaxBarKey 		= 0,						//���Keyֵ
			   float  BarCoefficient  	= 0.0						//��Keyֵ�Ƚ�ʱ��Keyֵ���ݲ�ϵ��,��ֵ����1.0ʱ��Keyֵ���ݲ�ֱ��ʹ�ø�ֵ
			   )
	{
		int ValTmp = 0;
		size_t Num = 1;
		int DiffVal = 0;
		float CmpValue = 0.0;
		if(FilterType == Filter_DiffKey)
		{
			ValTmp = BarKeyDict[SortArray[0]].Key;
			if(BarCoefficient > 1.0)							//SortArrayΪ��������
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
		for(size_t i = order ;i < len;i++)								//ǰorder������Ч
		{
			DiffTmp = DiffArray[i];
			if(DiffTmp < BarMinTmp)										//С��BarMin,continue
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
class _MutiHistogram											//orders:����,bins:һ��ֱ��ͼ����
{
	typedef unsigned short KeyType;								//Using short for Function Sort
	typedef AddressPart _AddressPart;
	typedef Bar<_AddressPart,KeyType,_AddrPartSize> _Bar;
	typedef Histogram<_Bar,KeyType,_BinSize> _Hist;
public:
	size_t CurrentOrders;										//��ǰֱ��ͼ����
	size_t BarMin;												//ֱ��ͼ������
	size_t BarTolerance;										//ֱ��ͼ���ݲ�
	vector<_Hist> Hist;											//ֱ��ͼ�������� 0:ȫ�ס�1:1�ס�2:2��...
	vector<size_t> AddrVal;										//��ֵ-��ַ����

	size_t InArrayLen;
	size_t ClearStatus;

	struct _AllOrtersBarKeyDict
	{
		typename _Hist::_BarKeyDict::value_type Key; 
		typename _Hist::_BarKeyDict::value_type Lens;
		int Addrs[_OrderSize];										//��ֵ��Ӧ����ֱ��ͼλ��
	};
	vector<_AllOrtersBarKeyDict> AllOrtersBarKeyDict;				//����ȫ�׻��ֱ��ͼ
	vector<vector<typename _Hist::_BarKeyDict> > BarKeyDict;		//����ȫ��ֱ��ͼ

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
		Type * DiffArray = NULL;													//�ݶ�����
		DiffArray = new Type[len];
		int* KeyHashAddr = new int[DIFF_HISTOGRAM_MAX];								//HashTable
		for(size_t i = 0;i < orders ;i++)
		{
			memset(KeyHashAddr,-1,(DIFF_HISTOGRAM_MAX)*sizeof(int));				//-1ΪHash��ʼ��־λ
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
	int  Filter(size_t orders,										//��ֵ
				size_t FilterType,									//��������
			    size_t *output,										//�������,���������ȷ�������㹻��Ŀռ�
			    size_t Tolerance 		= 0,						//�ݲ�,�ڹ�������ΪFilter_DiffKeyʱ��������ֵ��Ϊ0,��ʹ��Keyֵ�ݲ�
			    size_t FilterMinNums 	= 0,						//��С������ݳ���
			    size_t MinBarKey 		= 0,						//��СKeyֵ
			    size_t MaxBarKey 		= 0,						//���Keyֵ
			    float  BarCoefficient  	= 0.0						//��Keyֵ�Ƚ�ʱ��Keyֵ���ݲ�ϵ��,��ֵ����1.0ʱ��Keyֵ���ݲ�ֱ��ʹ�ø�ֵ
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
	int  AddrValVerify(size_t orders,								//��ֵ
					   _Tx* BarLocArray,							//��������
					   _Tx* AddrOutArray,							//�������
					   size_t BarLocArrayLens,						//�������鳤��
					   size_t findLens,								//������ַ�Գ���
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
		size_t LenTmp = 0;													//ȫ����ַ�Գ���
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
		set<size_t> KeySet(KeyArray,KeyArray+BarLocArrayLens);			//KeySet����Type1����Keyֵ
		size_t Address1 = 0,Address2 = 0,AddrPartLenTmp = 0;				//������ַ�Գ���;
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
			size_t j = 0,nAddrPartFind = 1,AddrPartOrder;				//��ַ������ȡַ,��׵�ַ��������־,��ַ�Խ�ѭ��
			size_t LenSum,OrderInd;										//Keyֵȫ�����ܳ���,�����ױ���
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
		if(order >= _OrderSize)							//fill_with_continue_nȫ��
		{
			for(order = 0;order < _OrderSize;order++)
			{
				Hist[order].fill_with_continue_n(n);
			}
		}
		else											//fill_with_continue_n����
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
				NLOGKSBYHEXWIDTH(4,"No",i,"��ַ�Ե�ֵ",AddrVal[i]);
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
			NLOGKSBYHEXWIDTH(4,"����",order);
			for(size_t bin = 0; bin < Hist[order].Lens; bin++)
			{
				NLOGKSBYHEXWIDTH(4,"����",order,
								   "��ֵ",Hist[order].BarKeyDict[bin].Key,
								   "����",Hist[order].BarKeyDict[bin].Lens);
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
		if(order >= _OrderSize)							//printȫ��
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
		else											//print����
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
					NLOGKSBYHEXWIDTH(4,"����",i,"����",j,"��ֵ",Key,"����",Lens);
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
					NLOGKSBYHEXWIDTH(4,"No",i,"����",j,"��ֵ",Key,"����",Lens,"����ַ",Addr);
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
		if(order == _OrderSize)							//printȫ��
		{
			for(order = 0;order < _OrderSize;order++)
			{
				NLOGKSBYHEXWIDTH(4,"����",order,"�����鳤��",Hist[order].SortLens);
				for(size_t i = 0; i < Hist[order].SortLens; i++)
				{
					NLOGKSBYHEXWIDTH(4,"No",i,"����������",Hist[order].SortArray[i]);
				}
			}
		}
		else											//print����
		{
			NLOGKSBYHEXWIDTH(4,"����",order,"�����鳤��",Hist[order].SortLens);
			for(size_t i = 0; i < Hist[order].SortLens; i++)
			{
				NLOGKSBYHEXWIDTH(4,"No",i,"����������",Hist[order].SortArray[i]);
			}
		}
		NLOGKSSECEND("SortPrint");
	}
	/****************************** Print Functions End ********************************/
};

#endif /* HISTOGRAM_H_ */
