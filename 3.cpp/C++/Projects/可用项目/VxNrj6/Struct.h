/*
 * Struct.h
 *
 *  Created on: Jun 27, 2019
 *      Author: xupengtao
 */

#ifndef STRUCT_H_
#define STRUCT_H_
#if _MSC_VER
#define taskDelay Sleep
#elif PPC
#include<taskLibCommon.h>
#endif

typedef unsigned char 	UCHAR;
typedef unsigned short 	USHORT;
typedef unsigned int 	UINT;
typedef const char 		CONCHAR;
typedef const short 	CONSHORT;
typedef const int 		CONINT;
typedef const UCHAR 	CONUCHAR;
typedef const USHORT 	CONUSHORT;
typedef const UINT 		CONUINT;
typedef volatile USHORT VtUSHORT;

template<typename _RfTy,typename _PwTy,typename _PaTy,typename _ToaTy,typename _TonTy>			//PDW各个参数的数据类型
struct _RecPDW
{
	typedef _RfTy 	RfTy;
	typedef _PwTy 	PwTy;
	typedef _PaTy 	PaTy;
	typedef _ToaTy 	ToaTy;
	typedef _TonTy 	TonTy;
	RfTy 	Rf;
	PwTy 	Pw;
	PaTy	Pa;
	ToaTy	Toa;
	TonTy 	Ton;
};

template<typename TagTy,typename RfTy,typename PwTy,typename TonTy>
struct _SubChannel
{
	TagTy 	Tag;
	RfTy 	Rf;
	PwTy 	Pw;
	TonTy 	Ton;
	int		cH;
};

template<typename TagTy,typename RfTy,typename PwTy,typename TonTy>
struct _MainChannel
{
	_SubChannel<TagTy,RfTy,PwTy,TonTy> SubChannel[4];
};

template<typename RecPDWType,
         size_t ParaNum>																		//每个PDW数据通道的最大脉冲数
struct _Pdw
{
	typedef typename RecPDWType::RfTy 	RfTy;
	typedef typename RecPDWType::PwTy 	PwTy;
	typedef typename RecPDWType::PaTy 	PaTy;
	typedef typename RecPDWType::ToaTy 	ToaTy;
	typedef typename RecPDWType::TonTy 	TonTy;
	
	size_t	Deducted[ParaNum];			//扣除标志 1
	size_t	Analyed[ParaNum];		
	RfTy	Rf[ParaNum];
	PwTy	Pw[ParaNum];
	PaTy	Pa[ParaNum];
	ToaTy	Toa[ParaNum];
	TonTy	Ton[ParaNum];
};

#if _MSC_VER
template<typename Ty>
struct	_StaticSemVal
{
	Ty Val;
	
	_StaticSemVal(Ty Val_ = 0):Val(Val_)
	{
		;
	}
	virtual ~_StaticSemVal()
	{
		;
	}
	inline Ty GetVal() const {return Val;}
	inline Ty operator = (Ty Val_)
	{
		Val = Val_;
		return Val_;
	}
	inline bool operator == (Ty Val_)
	{
		bool Rtn;
		if(Val == Val_)
		{
			Rtn = true;
		}
		else
		{
			Rtn = false;
		}
		return Rtn;
	}
	inline void operator++()
	{
		Val++;
	}
	inline void operator++(int)
	{
		Val++;
	}
	inline void operator--()
	{
		Val--;
	}
	inline void operator--(int)
	{
		Val--;
	}
	inline bool operator == (Ty Val_) const {return (Val == Val_)?true:false;}
	inline bool operator >= (Ty Val_) const {return (Val >= Val_)?true:false;}
	inline bool operator <= (Ty Val_) const {return (Val <= Val_)?true:false;}
	inline bool operator >  (Ty Val_) const {return (Val >  Val_)?true:false;}
	inline bool operator <  (Ty Val_) const {return (Val <  Val_)?true:false;}
};
#elif PPC
template<typename Ty>
struct	_StaticSemVal
{
	Ty Val;
	static SEM_ID Sem;						//创建信号量，用于同步
	
	_StaticSemVal(Ty Val_ = 0):Val(Val_)
	{
		;
	}
	virtual ~_StaticSemVal()
	{
		;
	}
	inline Ty GetVal() const {return Val;}
	inline Ty operator = (Ty Val_)
	{
		semTake(Sem,WAIT_FOREVER);
		Val = Val_;
		semGive(Sem);
		return Val_;
	}
	inline bool operator == (Ty Val_)
	{
		bool Rtn;
		semTake(Sem,WAIT_FOREVER);
		if(Val == Val_)
		{
			Rtn = true;
		}
		else
		{
			Rtn = false;
		}
		semGive(Sem);
		return Rtn;
	}
	inline void operator++()
	{
		semTake(Sem,WAIT_FOREVER);
		Val++;
		semGive(Sem);
	}
	inline void operator++(int)
	{
		semTake(Sem,WAIT_FOREVER);
		Val++;
		semGive(Sem);
	}
	inline void operator--()
	{
		semTake(Sem,WAIT_FOREVER);
		Val--;
		semGive(Sem);
	}
	inline void operator--(int)
	{
		semTake(Sem,WAIT_FOREVER);
		Val--;
		semGive(Sem);
	}
	inline bool operator == (Ty Val_) const {return (Val == Val_)?true:false;}
	inline bool operator >= (Ty Val_) const {return (Val >= Val_)?true:false;}
	inline bool operator <= (Ty Val_) const {return (Val <= Val_)?true:false;}
	inline bool operator >  (Ty Val_) const {return (Val >  Val_)?true:false;}
	inline bool operator <  (Ty Val_) const {return (Val <  Val_)?true:false;}
};
#endif

template<typename _PDWType,
		 size_t ChannelRadarMaxNo>															//每个PDW数据通道的可分选雷达的最大数
struct _PDWChannel
{
	typedef _PDWType PDWType;
	typedef typename _PDWType::RfTy 	RfTy;
	typedef typename _PDWType::PwTy 	PwTy;
	typedef typename _PDWType::PaTy 	PaTy;
	typedef typename _PDWType::ToaTy 	ToaTy;
	typedef typename _PDWType::TonTy 	TonTy;

	size_t					ContinuousTag;						//工作或自检标志
	size_t					SelfCheckResultTag;					//自检结果标志
	size_t					DiscernStLoc;						//分选脉冲开始位置
	size_t					PulseNum;							//脉冲数
	size_t					DiscernThreshold;					//分选阈值
	_StaticSemVal<bool> 	DiscernReadable;					//满足已分选条件
	_StaticSemVal<size_t> 	DiscernPulseNums;					//满足已分选条件脉冲数
	size_t					ScanThreshold;						//扫描分析阈值
	_StaticSemVal<bool> 	ScanReadable;						//满足扫描分析条件
	_StaticSemVal<size_t> 	ScanPulseNums;						//满足扫描分析条件脉冲数
	size_t					CrossChannelTag;					//跨通道标志(0xAA)
	int 					ScanAnaly;							//扫描分析标志
	int 					RadarNumber;						//一个通道分出的雷达数计数
	int 					ResultArrLoc[ChannelRadarMaxNo];	//结果表位置数组
	_PDWType 				PulseWord;							//脉冲缓冲区
};

template<typename RfTy,typename PwTy,typename PaTy,typename ToaTy,typename TonTy,size_t AnalysisNum>
struct _AnalyChannel
{
   int ChanNo;
   int Located[AnalysisNum];            						//收集到的有效脉冲在临时通道的位置
   int Deducted[AnalysisNum];									//收集到的有效脉冲已分选扣除脉冲
   RfTy  Rf[AnalysisNum];
   PwTy  Pw[AnalysisNum];
   PaTy  Pa[AnalysisNum];
   ToaTy Toa[AnalysisNum];
   TonTy Ton[AnalysisNum];
};

struct _ResultTmp 
{
   int Cmd;									//消失 0x1d、0xc9
   int Type;
   int Rfl;
   int Rfh;						
   int Pri;
   int Pw;
   int Pa;
   int Doa;
   int Aoa;
   int JaggNums;					//injd
   int PhasePeriods[8];				//hper
   int FramePeriods;				//dper
   int DitherRnge;					//dou
   int dtime;
};

struct _CompFlag
{
	int SameRf;
	int SamePri;
	int SamePw;
	int SameDoa;
	int SameType;
	int SameTime;
       
};

struct _Result
{
    _ResultTmp ResultData;
    int DifferentType;
    int DifferentTypeCount;
    int DifferentPri;
    int DifferentPriCount;
    int SameDoa;
    int AnalyDoaCounts;
    int DifferentDoaTag;
    int DifferentDoa[3];
    int DifferentDoaCounts[3];
    int Disapper;
    int ResultNo;
    int ChanNo;
    int DisappearTimes;
    USHORT ScanType;
    USHORT ScanPeriod;
    USHORT AntenaRpm;
    USHORT IrradiateTime;
    int ScanCounts;

    int Flag_Change_Prf;
    int Flag_Change_Rf;

//  int filled;
//  int dcounter;
//  int scanedflag;
//  int tempChanno;
//  int InChan;
//  int nointnum;
};
#endif /* STRUCT_H_ */
