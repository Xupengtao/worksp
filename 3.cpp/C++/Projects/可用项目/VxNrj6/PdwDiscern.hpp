 /*
 * PdwDiscern.hpp
 *
 *  Created on: Apr 8, 2019
 *      Author: xupengtao
 */

#ifndef PDWDISCERN_H_
#define PDWDISCERN_H_

#include<map>
#include"Struct.h"
#include"DualRam.hpp"
#include".\Tools\Histogram.hpp"
#include".\Tools\Timer.h"
#include"Nrj6Msg.h"

#define DEBUG_NLOGKS NLOGKS

#define PDWDISCERN_COUT_STUTAS 			0					//分选各状态标志和分选数

#define ABS(x) (((x) > 0) ? (x) : (-(x)))

template<typename PairX,typename PairY>
inline bool PwCountCmp(const pair<PairX,PairY> p1,const pair<PairX,PairY> p2)
{
	return p1.second < p2.second;
}
template<typename PaTy>
inline bool PaCmp(PaTy &FstPa,PaTy &LstPa)
{
	return (FstPa & 0x3F) < (LstPa & 0x3F);
}
template<typename PaTy>
inline bool PaSort(PaTy &FstPa,PaTy &LstPa)
{
	return ((FstPa & 0x1fc0) < (LstPa & 0x1fc0));
}
template<class _InIt, class _CmpTy, class _Pr>
inline typename iterator_traits<_InIt>::difference_type CountCmp2(_InIt _First, _InIt _Last, _CmpTy CmpVal, _Pr _Pred)
{
	_DEBUG_RANGE(_First, _Last);
	_DEBUG_POINTER(_Pred);
	typename iterator_traits<_InIt>::difference_type _Count = 0;
	for (; _First+1 != _Last; ++_First)
	{
		if (_Pred(*_First,*(_First+1),CmpVal))
		{
			++_Count;
		}
		else
		{
			break;
		}
	}
	return (_Count);
}
template<typename FreTy>
inline bool RfCmp2(FreTy Fst, FreTy Sec, FreTy CmpVal)
{
	return ((CmpVal > Fst)&&(CmpVal > Sec))?1:0;
}

template<typename _DualRamType,
         int DualRamChannelNum,										//PDW数据通道的通道数
         int ChannelRadarMaxNo,										//每个PDW数据通道的可分选雷达的最大数
         int DualRamNormalStart,									//Ram缓冲区标准有效通道起始值
         int AnalysisNum,											//临时通道的最大脉冲数
         int SingleChanAnalyTimeOut,								//全部有效单通道分选超时
         int NormalAnalyNum,										//分选初始阈值，当以次阈值进行分选时，标志预处理并未对该通道调整阈值
         int MutiSignalAnalyMinNum,									//参加多信号分选的最小脉冲数
         int HistAnalyMinNum,										//参加直方图分选的最小脉冲数
         int SingAgileRfRgMin,										//参加单通道捷变分选的最小RF变化范围
         int SingChanAgileTlrc,										//参加单通道捷变分选的最大RF变化范围
         int MutiChanAgileTlrc,										//参加多通道捷变分选的最大RF变化范围
         int AgileAnaMinNum,										//参加捷变分选的最小脉冲数
         int ScanThreshold,											//参加扫描分析的最小脉冲数
		 int ParaUpdataTimeOut,										//参数更新间隔时间长度
		 int DoaUpdataTimeOut>										//方位更新间隔时间长度
class _PdwDiscern
{
public:
	typedef typename _DualRamType::_PDWType	_PDWType;
	typedef typename _DualRamType::RfTy		RfTy;
	typedef typename _DualRamType::PwTy		PwTy;
	typedef typename _DualRamType::PaTy 	PaTy;
	typedef typename _DualRamType::ToaTy	ToaTy;
	typedef typename _DualRamType::TonTy 	TonTy;
	typedef typename _DualRamType::PDWChannelType 					_PDWChannelType;
	typedef _AnalyChannel<RfTy,PwTy,PaTy,ToaTy,TonTy,AnalysisNum> 	_AnalyChannelType;
	typedef _MutiHistogram<3,64,0x200> 								_MutiHistogramType;
	typedef pair<UCHAR,UCHAR> 										_AddressPairType;
	struct _AgileChan
	{
		RfTy  AgileRf[0x1000];
		PwTy  AgilePw[0x1000];
		PaTy  AgilePa[0x1000];
		ToaTy AgileToa[0x1000];
		TonTy AgileTon[0x1000];
	};
	enum _AnalysisedType
	{
		NormalChanNotDiscern		= 0,
		NormalChanDiscern	 		= 1,
		AgileChanNotDiscern			= 2,
		AgileChanDiscern	 		= 3,
		ChanRemaindedPulse			= 4
	};
	enum _HistogramDiscernRtnType
	{
		NoDiscernType 	= 0,
		JaggType 		= 1,
		FixType 		= 2,
		DitherType 		= 3
	};
	enum _TonaJudgeType
	{
		MoreThan 	= 0,
		LessThan 	= 1,
		Equal 	 	= 2,
		JudgeErr 	= 3
	};
	enum _MaxElemType
	{
		findit 		= 0,
		whereit 	= 1
	};
public:
	_DualRamType		&DualRam;									//Ram缓冲区对象
	_TimerCount			&TimerCount;								//毫秒  -> 秒定时器
	size_t				&TimeBegin;									//一次分选起始时间，由预处理写值，预处理和主处理读值
	_SysCmd 			&SysCmd;									//接收命令
	_MutiHistogramType 	MutiHistogram;								//多阶直方图对象
	_AnalyChannelType 	AnalyChannel;								//临时分析通道
	_CompFlag			CompFlag;									//用于标记分析通道结果表与结果表数组比较的结果
	_ResultTmp			ResultTmp;									//分析通道结果表
	_Msg				Msg;										//输出报文
	_DoaUpdataMsg		DoaUpdataMsg;								//输出方位更新报文
	RfTy				AnalyChanRfTlrc;							//Rf分选容差范围
	PaTy				PaTrackTlrc;								//Pa跟踪容差范围
	UINT 				BandNo;										//波段号
	UINT 				AntenaNums;									//天线数
	UINT 				FreqLens;									//FREQ数组长度
	UINT 				MainDeltaPaGate;							//主天线正负脉冲数最小值
	UINT 				DeltaPaRgPulseMinNums;						//Pa容差范围内最小脉冲数
	UINT 				SameAntenaNoMinNums;						//相同天线号脉冲最小值
	UINT 				SubDeltaPaGate;								//次天线正负脉冲数最小值
	UINT 				DeltaPaTolerance;							//Pa容差，初始值设为0x1c0
	int 				ChanNo;										//通道号
	int					ChanDiscernStLoc;							//通道分选脉冲开始位置
	int					ChanPulseNums;								//临时通道可读脉冲数
	int					AnalyPulseNums;								//临时通道有效脉冲数，对AnalyChannel中有效脉冲计数
	int 				DualRamValidChanNum;						//Ram缓冲区有效通道数
	int 				ResultNums;									//当前已分选出结果的结果表长度
	int 				AgileAnalyTag;								//捷变Rfh
	int 				ParaUpdateTag;								//参数更新
	int 				DoaUpdataTag;								//方位更新
	int 				ParaUpdateStTime;							//参数更新定时计数
	int 				DoaUpdataStTime;							//方位更新定时计数
	int					ContDoaUpdataStTime;						//连续波方位更新定时计数
	int 				DisapJudgeTag;								//判断消失
	UINT				PortTimeBegin;								//超时判断
	_AgileChan			*AgileChan;									//捷变累积脉冲通道
	size_t 				*BarLocArray;								//用于存储直方图柱地址的数组
	size_t 				*AddrPartArray;								//用于存储直方图柱内地址对地址的数组
	int					*AnalysisedChannel;							//已分析通道状态数组      类型如_AnalysisedType
	float 				*PwAnalArray;								//Pw缓冲分析数组
	int 				*PaAnalArray;								//Pa缓冲分析数组
	_AddressPairType	*AddressPair;								//用于序列搜索分选方法的地址对pair
	int 				*PaToDoaArray;								//比幅测向法计算Doa的Pa数组
	int 				*LastTimeOfResult;							//Ram缓冲区Result秒计数数组
	_Result				*Result;									//结果表数组
public:
	_PdwDiscern()																				//Construction func
				:AnalyChanRfTlrc(0),
				 PaTrackTlrc(0),
				 BandNo(0),
				 AntenaNums(0),
				 FreqLens(0),
				 MainDeltaPaGate(0),
				 DeltaPaRgPulseMinNums(0),
				 SameAntenaNoMinNums(0),
				 SubDeltaPaGate(0),
				 DeltaPaTolerance(0)
	{
		AgileChan		 	= new _AgileChan;
		BarLocArray   		= new size_t[NormalAnalyNum];
		AddrPartArray 		= new size_t[NormalAnalyNum];
		AnalysisedChannel 	= new int[DualRamChannelNum];
		PwAnalArray			= new float[NormalAnalyNum];
		PaAnalArray			= new int[NormalAnalyNum];
		AddressPair			= new _AddressPairType[NormalAnalyNum+1];
		PaToDoaArray		= new int[NormalAnalyNum];
		LastTimeOfResult	= new int[900];
		Result 		  		= new _Result[900];
		Clear();
		COUTFILE;
		COUTLINE;
		COUTSTREND(":  Using class _PdwDiscern error!");
	}
	_PdwDiscern(_DualRamType	&DualRam_,														//使用该类实例化对象，需绑定一个_DualRamType类型的对象引用
				_TimerCount  	&TimerCount_,
				size_t 			&TimeBegin_,
				_SysCmd 		&SysCmd_
				)
								:DualRam(DualRam_),
								 TimerCount(TimerCount_),
								 TimeBegin(TimeBegin_),
								 SysCmd(SysCmd_),
								 MutiHistogram(20,2),
								 AnalyChanRfTlrc(0),
								 PaTrackTlrc(0),
								 BandNo(0),
								 AntenaNums(0),						//g_Antena_Max_Number
								 FreqLens(0),						//g_Fres_Max_Number
								 MainDeltaPaGate(0),				//g_Dpa_Data_Number
								 DeltaPaRgPulseMinNums(0),			//g_Dpa_same_Number
								 SameAntenaNoMinNums(4),			//g_Same_Txn
								 SubDeltaPaGate(2),					//g_SubDpa_same_Number
								 DeltaPaTolerance(0x1c0)			//g_Dpa_Tolerance
	{
		AgileChan		 	= new _AgileChan;
		BarLocArray   		= new size_t[NormalAnalyNum];
		AddrPartArray 		= new size_t[NormalAnalyNum];
		AnalysisedChannel 	= new int[DualRamChannelNum];
		PwAnalArray			= new float[NormalAnalyNum];
		PaAnalArray			= new int[NormalAnalyNum];
		AddressPair			= new _AddressPairType[NormalAnalyNum+1];
		PaToDoaArray		= new int[NormalAnalyNum];
		LastTimeOfResult	= new int[900];
		Result 		  		= new _Result[900];
		Clear();
#if _MSC_VER
#elif PPC
		*((USHORT *)0xEC0000A0) = 0x7F;								//置天线扫描方式
		*((USHORT *)0xEC000060) = 0x8000;							//Reset 8255
#endif
	}
	virtual ~_PdwDiscern()																		//virtual distroy constructfunc
	{
		delete	 AgileChan;
		delete[] BarLocArray;
		delete[] AddrPartArray;
		delete[] AnalysisedChannel;
		delete[] PwAnalArray;
		delete[] PaAnalArray;
		delete[] AddressPair;
		delete[] PaToDoaArray;
		delete[] LastTimeOfResult;
		delete[] Result;
	}
	virtual void 	SetDebugStatus(int Status){};
	virtual void 	DelDebugStatus(int Status){};
	virtual void 	RstBand(int BandNo_)
	{
		BandNo = BandNo_;
		if(BandNo == 0)
		{
			AnalyChanRfTlrc 		= 5;
			PaTrackTlrc				= 1;
			AntenaNums 				= 5;
			FreqLens 				= 57;
			MainDeltaPaGate			= 4;
			DeltaPaRgPulseMinNums	= 4;
		}
		else
		{
			AnalyChanRfTlrc 		= 5;
			PaTrackTlrc				= 1;
			AntenaNums 				= 7;
			FreqLens 				= 107;
			MainDeltaPaGate			= 4;
			DeltaPaRgPulseMinNums	= 2;
		}
	}
	virtual void 	run(size_t& CtrlSign, size_t& TaskDeleteSign)																		//主函数
	{
		bool SingleChanProcedTag;
		while(CtrlSign)
		{
			DualRamValidChanNum = DualRam.GetValidChannelNum();
			if(DualRamValidChanNum != 0)
			{
				ChanNo = DualRamNormalStart-1;
				ClearAnalysisedChannel();
				SingleChanProcedTag = false;
				while(SingleChanProcedTag == false)
				{
					if(NextValidChannel())
					{
						SingleChanAnalysis();
					}
					if(AllChanAnalysisedJudge())
					{
						SingleChanProcedTag = true;
					}
					if(TimerCount.Ms - TimeBegin > SingleChanAnalyTimeOut)
					{
						break;
					}
				}
//				SingChanAgileDiscern();
				T50MutiChanAgileDiscern();
				ParaUpdate();
				DisappearJudge();
				ScanAnalysis();
				DoaUpdata();
				DualRam.SetClearTag(true);
				while((CtrlSign != 0) && (DualRam.GetClearTag() == true))
				{
					;//do someting
				}
			}
			if(DualRam[0].ContinuousTag == 1)
			{
				while(ContinuousChanDiscern() == 0)
				{
					if(TimerCount.Ms - TimeBegin > 100)
					{
						break;
					}
				}
			}
			else
			{
				ContiDisappearJudge();
			}
		}
		TaskDeleteSign = 1;
	}
	virtual int  	NextValidChannel()															//查找下一个有效通道，并返回该通道有效脉冲数
	{
		while(ChanNo < DualRamValidChanNum + DualRamNormalStart - 1)
		{
			ChanNo++;
			if((DualRam[ChanNo].DiscernReadable == true) && (AnalysisedChannel[ChanNo] == NormalChanNotDiscern))
			{
				if(DualRam[ChanNo].CrossChannelTag == 0xAA)
				{
					AnalysisedChannel[ChanNo] = AgileChanNotDiscern;
					continue;
				}
				if(DualRam[ChanNo].DiscernPulseNums.GetVal() > 0)
				{
					return 1;
				}
			}
		}
		ChanNo = DualRamNormalStart-1;
		return 0;
	}
	virtual int  	CollectPulseToAnalyChannel()												//将Ram缓冲区分析通道Rf在AnalyChanRfTol范围内的脉冲输入到临时通道
	{
		int Count = 0;
		RfTy RfRef = 0, RfTmp = 0;						//基准RF,当前脉冲RF
		_PDWType &PdwTmp = DualRam[ChanNo].PulseWord;
		ChanDiscernStLoc = DualRam[ChanNo].DiscernStLoc;
		ChanPulseNums 	 = DualRam[ChanNo].DiscernPulseNums.GetVal();
		ChanPulseNums 	 = (ChanPulseNums >= NormalAnalyNum)?NormalAnalyNum:ChanPulseNums;
		ClearAnalyChannel();
		AnalyChannel.ChanNo = ChanNo;
		for(int i = ChanDiscernStLoc; i < ChanDiscernStLoc + ChanPulseNums; i++) 
		{
			if((PdwTmp.Deducted[i] == 0)&& (PdwTmp.Analyed[i] == 0)) 
			{
				RfTmp = PdwTmp.Rf[i] & 0x1fff;
				RfRef = (RfRef == 0)?RfTmp:RfRef;
				if(ABS(RfRef - RfTmp) < AnalyChanRfTlrc)
				{
					PdwTmp.Deducted[i] 			= 1;
					AnalyChannel.Located[Count] = i;
					AnalyChannel.Rf[Count] 		= PdwTmp.Rf[i];
					AnalyChannel.Pw[Count] 		= PdwTmp.Pw[i];
					AnalyChannel.Toa[Count] 	= PdwTmp.Toa[i];
					AnalyChannel.Pa[Count] 		= PdwTmp.Pa[i];
					AnalyChannel.Ton[Count] 	= PdwTmp.Ton[i];
					Count++;
				}
			}
		}
		return Count;
	}
	virtual int  	CollectPulseToAnalyChannel(int ChannelNo)									//将Ram缓冲区ChannelNo通道Rf在（Cmptn[i].Rfl,Cmptn[i].Rfh）范围内的脉冲输入到临时通道
	{
		int Count = 0;
		RfTy RfRef = 0, RfTmp = 0;
		_PDWType &PdwTmp = DualRam[ChannelNo].PulseWord;
		ChanDiscernStLoc = DualRam[ChannelNo].DiscernStLoc;
		ChanPulseNums 	 = DualRam[ChannelNo].DiscernPulseNums.GetVal();
		ChanPulseNums 	 = (ChanPulseNums >= NormalAnalyNum)?NormalAnalyNum:ChanPulseNums;
		ClearAnalyChannel();
		AnalyChannel.ChanNo = ChannelNo;
		for(int i = ChanDiscernStLoc; i < ChanDiscernStLoc + ChanPulseNums; i++)
		{
			if((PdwTmp.Deducted[i] == 0)&& (PdwTmp.Analyed[i] == 0)) 
			{
				RfTmp = PdwTmp.Rf[i] & 0x1fff;
				RfRef = (RfRef == 0)?RfTmp:RfRef;
				if((RfTmp >= RfRef - SingChanAgileTlrc) && (RfTmp <= RfRef + SingChanAgileTlrc))
				{
					RfRef = (RfRef*Count + RfTmp)/(Count + 1);
					PdwTmp.Deducted[i] 			= 1;
					AnalyChannel.Located[Count] = i;
					AnalyChannel.Rf[Count] 		= PdwTmp.Rf[i];
					AnalyChannel.Pw[Count] 		= PdwTmp.Pw[i];
					AnalyChannel.Toa[Count] 	= PdwTmp.Toa[i];
					AnalyChannel.Pa[Count] 		= PdwTmp.Pa[i];
					AnalyChannel.Ton[Count] 	= PdwTmp.Ton[i];
					Count++;
				}
			}
		}
		return Count;
	}
	virtual int 	ClctAgilePulseToAnalChan(int *AgiChanNo, int ValidAgiChLens)				//多通道捷变脉冲输入到临时通道, ToaTy Restrict
	{
		int AgiChanBeginRecord[15] = {0};
		int RfCounts = 0, Rtn = 0;
		ToaTy ToaBegin, ToaEnd, Toa;
		TonTy TonBegin, TonEnd, Ton;
		for(int i = 0; i < ValidAgiChLens; i++)
		{
			AgiChanBeginRecord[i] = DualRam[AgiChanNo[i]].DiscernStLoc;
		}
		for(int i = 0; i < ValidAgiChLens; i++)
		{
			ChanNo = AgiChanNo[i];
			ChanDiscernStLoc = DualRam[ChanNo].DiscernStLoc;
			ChanPulseNums 	 = DualRam[ChanNo].DiscernPulseNums.GetVal();
			ChanPulseNums	 = (ChanPulseNums >= NormalAnalyNum)?NormalAnalyNum:ChanPulseNums;
			for(int j = AgiChanBeginRecord[i]; j < ChanDiscernStLoc + ChanPulseNums - 1; j++)
			{
				if(DualRam[ChanNo].PulseWord.Deducted[j] == 1)        	//从第一个通道里寻找脉冲列的toa范围toa0~toa1
				{
					continue;          									//被扣除的脉冲 continue;
				}
				for(int k = j + 1; k < ChanDiscernStLoc + ChanPulseNums; k++)
				{
					if(DualRam[ChanNo].PulseWord.Deducted[k] == 1)
					{
						continue;
					}
					ToaBegin = DualRam[ChanNo].PulseWord.Toa[j];
					TonBegin = DualRam[ChanNo].PulseWord.Ton[j];
					ToaEnd   = DualRam[ChanNo].PulseWord.Toa[k];
					TonEnd   = DualRam[ChanNo].PulseWord.Ton[k];
					break;
				}
				break;
			}
			AnalyPulseNums = 0;
			for(int j = 0; j < ValidAgiChLens; j++)
			{
				ChanNo = AgiChanNo[j];
				ChanDiscernStLoc = DualRam[ChanNo].DiscernStLoc;
				ChanPulseNums 	 = DualRam[ChanNo].DiscernPulseNums.GetVal();
				ChanPulseNums 	 = (ChanPulseNums >= NormalAnalyNum)?NormalAnalyNum:ChanPulseNums;
				for(int k = AgiChanBeginRecord[j]; k < ChanDiscernStLoc + ChanPulseNums; k++)
				{
					if(DualRam[ChanNo].PulseWord.Deducted[k] == 1)
					{
						continue;
					}
					Toa = DualRam[ChanNo].PulseWord.Toa[k];
					Ton = DualRam[ChanNo].PulseWord.Ton[k];
					if((TonaJudge(Ton,Toa,TonBegin,ToaBegin) == MoreThan) 
					 &&(TonaJudge(Ton,Toa,TonEnd,ToaEnd) == LessThan))
					{
						DualRam[ChanNo].PulseWord.Deducted[k] = 1;
						AgileChan->AgileRf[AnalyPulseNums] = DualRam[ChanNo].PulseWord.Rf[k];
						AgileChan->AgilePw[AnalyPulseNums] = DualRam[ChanNo].PulseWord.Pw[k];
						AgileChan->AgilePa[AnalyPulseNums] = DualRam[ChanNo].PulseWord.Pa[k];
						AgileChan->AgileTon[AnalyPulseNums] = Ton;
						AgileChan->AgileToa[AnalyPulseNums] = Toa;
						AnalyPulseNums++;
					}
					else if(TonaJudge(Ton,Toa,TonEnd,ToaEnd) != LessThan)
					{
						AgiChanBeginRecord[j] = k;
						break;
					}
					if(AnalyPulseNums >= AgileAnaMinNum)
					{
						break;
					}
				}
				if(AnalyPulseNums >= AgileAnaMinNum)
				{
					break;
				}
			}
			if(AnalyPulseNums < AgileAnaMinNum)
			{
				continue;
			}
			int Temp = 0;
			TonTy PreTon, NextTon;
			ToaTy PreToa, NextToa;
			for(int j = 0; j < AnalyPulseNums; j++)							//按Toa从小到大排序
			{
				for(int k = 0; k < AnalyPulseNums - j - 1; k++)
				{
					PreTon 	= AgileChan->AgileTon[k];
					PreToa 	= AgileChan->AgileToa[k];
					NextTon = AgileChan->AgileTon[k + 1];
					NextToa	= AgileChan->AgileToa[k + 1];
					if(TonaJudge(PreTon,PreToa,NextTon,NextToa) == MoreThan)
					{
						Temp = NextToa;
						AgileChan->AgileToa[k + 1] = PreToa;
						AgileChan->AgileToa[k] = Temp;
						Temp = NextTon;
						AgileChan->AgileTon[k + 1] = PreTon;
						AgileChan->AgileTon[k] = Temp;
						Temp = AgileChan->AgileRf[k + 1];
						AgileChan->AgileRf[k + 1] = AgileChan->AgileRf[k];
						AgileChan->AgileRf[k] = Temp;
						Temp = AgileChan->AgilePw[k + 1];
						AgileChan->AgilePw[k + 1] = AgileChan->AgilePw[k];
						AgileChan->AgilePw[k] = Temp;
						Temp = AgileChan->AgilePa[k + 1];
						AgileChan->AgilePa[k + 1] = AgileChan->AgilePa[k];
						AgileChan->AgilePa[k] = Temp;
					}
				}
			}
			break;
		}
		if (AnalyPulseNums < AgileAnaMinNum)
		{
			AnalyPulseNums = 0;
			Rtn = 0;
		}
		if(Rtn != 0)
		{
			for (int i = 0; i < AnalyPulseNums; i++)
			{
				if (abs(AgileChan->AgileRf[i] - AgileChan->AgileRf[0]) <= 5)
				{
					RfCounts++;
				}
			}
			if (RfCounts >= (int) (AnalyPulseNums * 85 / 100))
			{
				AnalyPulseNums = 0;
				Rtn = 0;
			}
		}
		if(Rtn != 0)
		{
			ClearAnalyChannel();
			AnalyChannel.ChanNo = AgiChanNo[0];
			for (int i = 0; i < AnalyPulseNums; i++)
			{
				AnalyChannel.Rf[i] 	= AgileChan->AgileRf[i];
				AnalyChannel.Pw[i] 	= AgileChan->AgilePw[i];
				AnalyChannel.Pa[i] 	= AgileChan->AgilePa[i];
				AnalyChannel.Toa[i] = AgileChan->AgileToa[i];
				AnalyChannel.Ton[i] = AgileChan->AgileTon[i];
			}
			Rtn = 1;
		}
		return Rtn;
	}
	virtual int		AnalChanDeductPrcs()														//去除分析通道中已分选脉冲
	{
		int i = 0, Loc = 0, Next = 0;
		for(i = 0; i < AnalyPulseNums; i++)
		{
			if(AnalyChannel.Deducted[i] == 1)
			{
				Next = (Next == 0)?(i+1):Next;
				for(Loc = Next; Loc < AnalyPulseNums; Loc++)
				{
					if(AnalyChannel.Deducted[Loc] == 0)
					{
						Next = Loc;
						break;
					}
				}
				if(Loc < AnalyPulseNums)
				{
					AnalyChannel.Deducted[Next] = 1;
					AnalyChannel.Deducted[i]	= 0;
					AnalyChannel.Located[i]		= AnalyChannel.Located[Next];
					AnalyChannel.Rf[i]			= AnalyChannel.Rf[Next];
					AnalyChannel.Pw[i] 	   		= AnalyChannel.Pw[Next];
					AnalyChannel.Toa[i]			= AnalyChannel.Toa[Next];
					AnalyChannel.Pa[i] 	   		= AnalyChannel.Pa[Next];
					AnalyChannel.Ton[i]			= AnalyChannel.Ton[Next];
				}
				else
				{
					break;
				}
			}
		}
		AnalyPulseNums = i;
		return i;
	}
	virtual void 	CancelDeduct()																//取消Ram缓冲区分析通道脉冲扣除标志并置脉冲已分析标志
	{
		if(AnalyPulseNums > 0)
		{
			int i, j;
			for (i = 0; i < AnalyPulseNums; i++)
			{
				j = AnalyChannel.Located[i];
				DualRam[ChanNo].PulseWord.Deducted[j] = 0;
				DualRam[ChanNo].PulseWord.Analyed[j]  = 1;
			}
			AnalysisedChannel[ChanNo] = ChanRemaindedPulse;
		}
		AnalyPulseNums = 0;
	}
	virtual bool	AllChanAnalysisedJudge()													//Ram缓冲区有效通道完成分选判断
	{
		bool Rtn = true;
		for(int i = DualRamNormalStart; i < DualRamNormalStart+DualRamValidChanNum; i++)
		{
			if(AnalysisedChannel[i] == 0)
			{
				Rtn = false;
				break;
			}
		}
		return Rtn;
	}
	virtual int  	SingleChanAnalysis()														//预设阈值普通分选
	{
		int AnalysisedPulseNum = 0, DiscernRtn = 0;
		while(1)
		{
			AnalyPulseNums = CollectPulseToAnalyChannel();
			if(AnalyPulseNums == 0)
			{
				break;
			}
			AnalysisedPulseNum = AnalysisedPulseNum + AnalyPulseNums;
			if(AnalyPulseNums >= MutiSignalAnalyMinNum) 
			{
				DiscernRtn = MutiSignalDiscern();
				if (DiscernRtn == AnalyPulseNums)
				{
					AnalysisedChannel[ChanNo] = NormalChanDiscern;
				}
				else
				{
					AnalysisedChannel[ChanNo] = ChanRemaindedPulse;
				}
			}
			else
			{
				AnalysisedChannel[ChanNo] = ChanRemaindedPulse;
			}
			if(AnalyPulseNums >= HistAnalyMinNum)
			{
				DiscernRtn = HistogramDiscern();
				if(DiscernRtn != NoDiscernType)
				{
					AnalysisedChannel[ChanNo] = NormalChanDiscern;
				}
				else
				{
					AnalysisedChannel[ChanNo] = ChanRemaindedPulse;
				}
			}
			if(AnalysisedChannel[ChanNo] == ChanRemaindedPulse) 
			{
				CancelDeduct();                                		//取消扣除标志
			}
		}
		return 1;
	}
	virtual int 	HistogramDiscern()															//直方图分选，分选顺序：参差 -> 固定 -> 抖动
	{
		int rtn;
		MutiHistogram.CreatHistogram(AnalyChannel.Toa,AnalyPulseNums,1);
		rtn = JaggDiscern();
		if(rtn == 1) 
		{
			return JaggType;
		}
		rtn = FixDiscern();
		if(rtn > 0)
		{
			return FixType;
		}
		rtn = DitherDiscern(2, 1);
		if(rtn == 1)
		{
			return DitherType;
		}
		return NoDiscernType;
	}
	virtual int  	JaggDiscern()																//参差信号分选
	{
		int rtn = 0, FilterBarLens = 0, AddrPartLens = 0, FstPri = 0, SecPri = 0;
		float PeriodTmp = 0;
		MutiHistogram.HistogramSort(1,"Lens",TestMax);
		FilterBarLens = MutiHistogram.Filter(1,Filter_DiffLens,BarLocArray,2,2);
		if(FilterBarLens == 0)
		{
			return 0;
		}
		else if(FilterBarLens == 2) 
		{
			FstPri = MutiHistogram[0][BarLocArray[0]].Key;
			SecPri = MutiHistogram[0][BarLocArray[1]].Key;
			PeriodTmp = (FstPri > SecPri)?float(FstPri % SecPri):float(SecPri % FstPri);
			if(PeriodTmp < 20)
			{
				return 0;
			}
			else
			{
				PeriodTmp = 0;
			}
		}
		if(MutiHistogram[0][BarLocArray[0]].Lens < 2)
		{
			return 0;
		}
		if(MutiHistogram[0][BarLocArray[0]].Lens < size_t(AnalyPulseNums/FilterBarLens)*8/10)
		{
			return 0;
		}
		FilterBarLens = (FilterBarLens > 8)?8:FilterBarLens;
		AddrPartLens = MutiHistogram.AddrValVerify(1,BarLocArray,AddrPartArray,FilterBarLens,FilterBarLens,1);
		if(AddrPartLens != 0)
		{
			ClearResultTmp();
			int temp = 0;
			for (int i = 0; i < AddrPartLens; i++) 
			{
				temp = BarLocArray[i];
				PeriodTmp = PeriodTmp + MutiHistogram[0][temp].Key;
				ResultTmp.PhasePeriods[i] = MutiHistogram[0][temp].Key;
			}
			ResultTmp.FramePeriods = (int)PeriodTmp;
			ResultTmp.Cmd 		= LongMsgType;
			ResultTmp.Type		= 3;
			ResultTmp.Pri 		= 0;
			ResultTmp.JaggNums 	= AddrPartLens;
			ResultTmp.dtime = TimerCount.Seconds/10;
			int PaNum = ResultTmpRfPwPaSet(BarLocArray,FilterBarLens);
			temp = CalculateDoa(PaNum);
			if ((temp <= 360) && (ResultTmp.PhasePeriods[0] != ResultTmp.PhasePeriods[1]))
			{
				ResultTmp.Doa = temp;
				ResultOutput();
			}
			rtn = 1;
		}
		return rtn;
	}
	virtual int  	FixDiscern()																//固定信号分选 
	{
		int temp = 0, rtn = 0,FixPulseSum = 0 ,FilterBarLens = 0, AddrPartLens = 0;
		FilterBarLens = MutiHistogram.Filter(1,Filter_FixLens,BarLocArray,16,1);
		if(FilterBarLens == 0)
		{
			return rtn;
		}
		for(int No = 0; No < FilterBarLens; No++)
		{
			AddrPartLens = MutiHistogram.AddrValVerify(1,&(BarLocArray[No]),AddrPartArray,1,3,0);
			if(AddrPartLens >= 2)
			{
				ClearResultTmp();
				ResultTmp.Type 	= 1;
				ResultTmp.Cmd 	= LongMsgType;
				ResultTmp.Pri	= MutiHistogram[0][BarLocArray[No]].Key;
				ResultTmp.dtime = TimerCount.Seconds/10;
				FixPulseSum +=  MutiHistogram[0][BarLocArray[No]].Lens;
				int PaNum = ResultTmpRfPwPaSet(&(BarLocArray[No]),1);
				temp = CalculateDoa(PaNum);
				if (temp <= 360) 
				{
					ResultTmp.Doa = temp;
					ResultOutput();
				}
			}
		}
		return FixPulseSum;
	}
	virtual int  	DitherDiscern(int PriFixDitherType, int RfFixAgileType)												//抖动信号分选
	{
		int temp = 0,rtn = 0,FilterBarLens = 0,DitherAddrLens = 0,AddrPartLens = 0,DitherPulseSum = 0;
		MutiHistogram.HistogramSort(1,"Key",TestMin);
		if(PriFixDitherType == 1)
		{
			FilterBarLens = MutiHistogram.Filter(1,Filter_DiffKey,BarLocArray,0,1,30,101000,float(0.02));
		}
		else
		{
			FilterBarLens = MutiHistogram.Filter(1,Filter_DiffKey,BarLocArray,0,3,30,101000,float(0.86));
		}
		DitherPulseSum 	= MutiHistogram.CountBarPulseSum(1,BarLocArray,FilterBarLens);
		DitherAddrLens  = (FilterBarLens >= 8)?FilterBarLens/2:3;
		AddrPartLens	= MutiHistogram.AddrValVerify(1,BarLocArray,AddrPartArray,FilterBarLens,DitherAddrLens,0);
		if((DitherPulseSum >= int(AnalyPulseNums * 0.50)) && (AddrPartLens != 0))
		{
			ClearResultTmp();
			if (PriFixDitherType == 1)
			{
				ResultTmp.Pri = MutiHistogram[0].BarKeyDict[BarLocArray[0]].Key;
				if (RfFixAgileType == 1)
				{
					ResultTmp.Type = 1;
				}
				else
				{
					ResultTmp.Type = 4;
				}
			}
			else
			{
				if (RfFixAgileType == 1)
				{
					ResultTmp.Type = 2;
				}
				else
				{
					ResultTmp.Type = 5;
				}
				ResultTmp.DitherRnge = 100 * (MutiHistogram[0].BarKeyDict[BarLocArray[FilterBarLens - 1]].Key 
						               - MutiHistogram[0].BarKeyDict[BarLocArray[0]].Key);
				ResultTmp.Pri = MutiHistogram[0].BarKeyDict[BarLocArray[FilterBarLens/2]].Key;
				ResultTmp.DitherRnge /= ResultTmp.Pri;
				if (ResultTmp.DitherRnge == 0)
				{
					if (ResultTmp.Type == 2)
					{
						ResultTmp.Type = 1;
					}
					else
					{
						ResultTmp.Type = 4;
					}
				}
			}
			ResultTmp.dtime = TimerCount.Seconds/10;
			int PaNum = ResultTmpRfPwPaSet(BarLocArray,FilterBarLens);
			temp = CalculateDoa(PaNum);
			if (temp <= 360)
			{
				ResultTmp.Doa = temp;
				ResultOutput();
			}
			rtn = 1;
		}
		return rtn;
	}
	virtual int 	MutiSignalDiscern()															//多部信号分选
	{
		const int FIX_DISCERN_MINNUM = 3,DITHER_DISCERN_MINNUM = 7, DITHER_PRI_MIN = 20;
		ToaTy PriTmp,PriRef,PriLoc,PriTolrc = 2,PriMod,PriDiv;
		int AddressLens;
		int i,j,k;
		int temp,AnalysisedNum = 0,rtn = 0;
		map<ToaTy,int> PriRefMap;
		int *PriArray 		= new int[AnalyPulseNums];
		int *PdwDeducted 	= AnalyChannel.Deducted;
		ClearAddressPair();
		int FixDiscernTimes = 0;
		while((FixDiscernTimes < 3) && (AnalyPulseNums >= FIX_DISCERN_MINNUM))
		{
			for (i = 0; i < 3; i++)
			{
				if(i < AnalyPulseNums)
				{
					for (j = 1; j < 4; j++) 
					{
						if(i+j < AnalyPulseNums)
						{
							PriTmp = AnalyChannel.Toa[i + j] - AnalyChannel.Toa[i];
							if ((PriTmp >= 10) && (PriTmp <= 10000))
							{
								PriRefMap[PriTmp] = 0;
							}
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					break;
				}
			}
			for(map<ToaTy,int>::iterator PriRefIter = PriRefMap.begin(); PriRefIter != PriRefMap.end(); PriRefIter++)
			{
				AddressLens = 0;
				PriRef = PriRefIter->first;
				for (i = 0; i < 9; i++)
				{
					if(i < AnalyPulseNums)
					{
						for (j = 1; j < 5; j++)
						{
							if(i+j < AnalyPulseNums)
							{
								PriLoc = AnalyChannel.Toa[i + j] - AnalyChannel.Toa[i];
								if (ABS(PriRef - PriLoc) > PriTolrc)
								{
									if (PriLoc > PriRef)
									{
										break;
									}
									continue;
								}
								PriRefMap[PriRef]++;
								AddressPair[AddressLens].first = i;
								AddressPair[AddressLens++].second = i + j;
								break;
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						break;
					}
				}
				if ((PriRefMap[PriRef] < 3) || ((PriRefMap[PriRef] >= 3) && ((AddressPair[0].second != AddressPair[1].first) || (AddressPair[1].second != AddressPair[2].first)))) 
				{
					for (i = 0; i < AddressLens; i++)
					{
						AddressPair[i].first = 0;
						AddressPair[i].second = 0;
					}
					AddressLens = 0;
					PriRefMap[PriRef] = 0;
					continue;
				}
				else
				{
					for (k = AddressPair[AddressLens - 1].second; k < AnalyPulseNums; k += j)
					{
						for (j = 1; j < 5; j++)
						{
							if ((k + j) >= AnalyPulseNums)
							{
								break;
							}
							PriTmp = AnalyChannel.Toa[k + j] - AnalyChannel.Toa[k];
							PriDiv = PriTmp / PriRef;
							PriMod = PriTmp % PriRef;
							if ((PriMod > PriTolrc) && ((PriRef - PriMod) > PriTolrc)) // if (abs(Toa0-temp)>Toa05)
							{
								if (PriDiv > 2)
								{
									break; //if(temp>Toa0) break;
								}
								continue;
							}
							PriRefMap[PriRef]++;
							AddressPair[AddressLens].first = k;
							AddressPair[AddressLens++].second = k + j;
							break;
						}
					}
					j = 0;
					for (k = 0; k < AddressLens; k++)
					{
						if (AddressPair[k].second != AddressPair[k + 1].first)
						{
							j = 0;
						}
						else if (++j >= 3)
						{
							break;
						}
					}
					if (j < 3)
					{
						continue;
					}
					else
					{
						ClearResultTmp();
						ResultTmp.Cmd = LongMsgType;
						ResultTmp.Pri = PriRef;
						ResultTmp.Type = 1;
						ResultTmp.dtime = TimerCount.Seconds/10;
						int PaNum = ResultTmpRfPwPaSet(AddressPair,AddressLens);
						temp = CalculateDoa(PaNum);
						if (temp <= 360)
						{
							ResultTmp.Doa = temp;
							ResultOutput();
						}
						AnalysisedNum += AddressLens;
						rtn = AnalysisedNum;
						for (j = 0; j < AddressLens; j++)
						{
							k = AddressPair[j].first;
							if (PdwDeducted[k] != 1)
							{
								PdwDeducted[k] = 1;
							}
							k = AddressPair[j].second;
							if (PdwDeducted[k] != 1)
							{
								PdwDeducted[k] = 1;
							}
						}
						AnalChanDeductPrcs();
					}
				}
			}
			PriRefMap.clear();
			if(rtn == 0)
			{
				break;
			}
			FixDiscernTimes++;
		}
//		if (AnalyPulseNums >= DITHER_DISCERN_MINNUM)
//		{
//			k = -1, PriRef = 0, AddressLens = 0;
//			for (i = 0; i < AnalyPulseNums; i++)
//			{
//				if (k == -1)
//				{
//					k = i;
//				}
//				else
//				{
//					if (PriRef == 0)
//					{
//						PriRef = AnalyChannel.Toa[i] - AnalyChannel.Toa[k];
//						if(PriRef <= 20)
//						{
//							continue;
//						}
//						PriTolrc = ToaTy(PriRef * 0.30);
//						PriArray[AddressLens] = PriRef;
//						AddressPair[AddressLens].first = k;
//						AddressPair[AddressLens++].second = i;
//						k = i;
//					}
//					else
//					{
//						temp = AnalyChannel.Toa[i] - AnalyChannel.Toa[k];
//						if ((temp > PriRef) && ((temp % PriRef) < PriTolrc))
//						{
//							PriRef = temp;
//						}
//						if (temp == 0)
//						{
//							continue;
//						}
//						PriMod = PriRef % temp;
//						if (PriMod < PriTolrc)
//						{
//							PriArray[AddressLens] = temp;
//							AddressPair[AddressLens].first = k;
//							AddressPair[AddressLens++].second = i;
//						}
//						k = i;
//					}
//				}
//			}
//			if (AddressLens >= DITHER_DISCERN_MINNUM)
//			{
//				k = 0;
//				for (i = 0; i < AddressLens; i++)
//				{
//					if (AddressPair[i].second != AddressPair[i + 1].first)
//					{
//						k = 0;
//					}
//					else if (++k >= 3)
//					{
//						break;
//					}
//				}
//				if (k >= 3)
//				{
//					PriTolrc = 2;
//					k = 0;
//					for (i = 0; i < AddressLens; i++)
//					{
//						PriMod = PriRef % PriArray[i];
//						if (PriMod < PriTolrc)
//						{
//							++k;
//						}
//					}
//					if (k >= AddressLens * 6 / 10)
//					{
//						ClearResultTmp();
//						ResultTmp.Cmd = LongMsgType;
//						ResultTmp.Type = 1;
//						std::sort(PriArray,&PriArray[AddressLens]);
//						ResultTmp.Pri = PriArray[1];
//						ResultTmp.dtime = TimerCount.Seconds/10;
//						int PaNum = ResultTmpRfPwPaSet(AddressPair,AddressLens);
//						temp = CalculateDoa(PaNum);
//						if (temp <= 360)
//						{
//							ResultTmp.Doa = temp;
//							ResultOutput();
//						}
//					}
//					else
//					{
//						ClearResultTmp();
//						ResultTmp.Cmd = LongMsgType;
//						ResultTmp.Type = 2;
//						std::sort(PriArray,&PriArray[AddressLens]);
//						for (i = 0; i < AddressLens; i++)
//						{
//							if (PriArray[i] < DITHER_PRI_MIN)
//							{
//								continue;
//							}
////							if (PriArray[i + 1] - PriArray[i] > PriTolrc)   //@Message bug
////							{
////								continue;
////							}
//							for (k = AddressLens - 1; k > 0; k--)
//							{
//								if (PriArray[k] / PriArray[i] >= 2)
//								{
//									continue;
//								}
//								else
//								{
//									break;
//								}
//							}
//							break;
//						}
//						PriRef = (PriArray[k] + PriArray[i]) / 2;
//						ResultTmp.DitherRnge = ((PriArray[k] - PriArray[i]) * 100)/ PriRef;
//						ResultTmp.DitherRnge = (ResultTmp.DitherRnge == 0)?1:ResultTmp.DitherRnge;
//						ResultTmp.Pri = PriRef;
//						ResultTmp.dtime = TimerCount.Seconds/10;
//						int PaNum = ResultTmpRfPwPaSet(AddressPair,AddressLens);
//						temp = CalculateDoa(PaNum);
//						if (temp <= 360)
//						{
//							ResultTmp.Doa = temp;
//							ResultOutput();
//						}
//					}
//					AnalysisedNum += AddressLens;
//					rtn = AnalysisedNum;
//					for (j = 0; j < AddressLens; j++)
//					{
//						k = AddressPair[j].first;
//						if (PdwDeducted[k] != 1)
//						{
//							PdwDeducted[k] = 1;
//						}
//						k = AddressPair[j].second;
//						if (PdwDeducted[k] != 1)
//						{
//							PdwDeducted[k] = 1;
//						}
//					}
//					AnalChanDeductPrcs();
//				}
//			}
//		}
		delete[] PriArray;
		return rtn;
	}
	virtual void 	SingChanAgileDiscern()														//单通道捷变信号分选
	{
		int RfCounts = 0, SingleChanAgileRtn = 0;
		for (ChanNo = DualRamNormalStart; ChanNo < DualRamNormalStart + DualRamValidChanNum; ChanNo++)
		{
			if(AnalysisedChannel[ChanNo] == AgileChanNotDiscern)
			{
				while(1)
				{
					AnalyPulseNums = CollectPulseToAnalyChannel(ChanNo);
					if(AnalyPulseNums == 0)
					{
						break;
					}
					if(AnalyPulseNums < AgileAnaMinNum)
					{
						CancelDeduct();
						continue;
					}
					for(int j = 0; j < AnalyPulseNums; j++)
					{
						if (abs(AnalyChannel.Rf[j] - AnalyChannel.Rf[0]) > SingAgileRfRgMin)
						{
							RfCounts++;
						}
					}
					if(RfCounts < AgileAnaMinNum)
					{
						CancelDeduct();
						continue;
					}
					SingleChanAgileRtn = AgainDiscern();
					if(SingleChanAgileRtn == 1)
					{
						AnalysisedChannel[ChanNo] = AgileChanDiscern;
					}
					else
					{
						CancelDeduct();
					}
				}
			}
		}
	}
	virtual int 	AgainDiscern()																//捷变变参数再次信号分选
	{
		MutiHistogram.CreatHistogram(AnalyChannel.Toa,AnalyPulseNums,1);
		AgileAnalyTag = 0xffff;
//		if(JaggDiscern() != 1)
//		{
			if(DitherDiscern(1, 2) != 1)
			{
				if(DitherDiscern(2, 2) != 1)
				{
//					if(AgileDitherDiscern() != 1)
//					{
						return 0;
//					}
				}
			}
//		}
		AgileAnalyTag = 0;
		return 1;
	}
	virtual int 	AgileDitherDiscern()														//捷变抖动信号分选
	{
		ToaTy PriRef, PriDitherTlrc, PriFixTlrc;
		int Rtn = 0, temp, PriFixCounts = 0, PriDitherCounts = 0;
		ToaTy *ToaArray = AnalyChannel.Toa;
		ToaTy *PriArray = new ToaTy[AnalyPulseNums];
		ClearAddressPair();
		for(int i = 0; i < AnalyPulseNums - 1; i++)
		{
			PriArray[i] = ToaArray[i + 1] - ToaArray[i];
			AddressPair[i].first = i;
			AddressPair[i].second = i + 1;
		}
		std::sort(PriArray,&(PriArray[AnalyPulseNums]));
		for(int i = 0; i < 10; i++)
		{
			PriRef = PriArray[i];
			if (PriRef < 20)
			{
				continue;
			}
			PriFixTlrc = PriRef * 15 / 10000;
			if(PriFixTlrc == 0)
			{
				PriFixTlrc = 2;
			}
			PriDitherTlrc = PriRef * 8 / 10;
			if(PriDitherTlrc == 0)
			{
				PriDitherTlrc = 100;
			}
			for(int j = 0; j < AnalyPulseNums - 1; j++)
			{
				if ((PriArray[j] % PriRef) < PriFixTlrc)
				{
					PriFixCounts++;
				}
				else if ((PriArray[j] % PriRef) < PriDitherTlrc)
				{
					PriDitherCounts++;
				}
			}
			if(PriFixCounts > AnalyPulseNums * 50 / 100)
			{
				ClearResultTmp();
				ResultTmp.Type = 4;
				Rtn = 1;
				break;
			}
			else if(PriDitherCounts > AnalyPulseNums * 50 / 100)
			{
				ClearResultTmp();
				ResultTmp.Type = 5;
				DitherParaAnaly(PriArray, PriRef, i);
				Rtn = 1;
				break;
			}
		}
		if(Rtn != 0)
		{
			ResultTmp.Cmd = LongMsgType;
			ResultTmp.Pri = PriRef;
			ResultTmp.dtime = TimerCount.Seconds/10;
			int PaNum = ResultTmpRfPwPaSet(AddressPair, AnalyPulseNums - 1);
			temp = CalculateDoa(PaNum);
			if (temp <= 360)
			{
				ResultTmp.Doa = temp;
				ResultOutput();
			}
		}
		delete[] PriArray;
		return Rtn;
	}
	virtual void 	DitherParaAnaly(ToaTy *PriArray, ToaTy PriRef, int PriInitSt)				//设置捷变抖动参数
	{
		int DitherPri = 0, DitherPriSum = 0, DitherPriLens = 0, DitherPriRange = 0;
		ToaTy ToaTlrc;
		int PriSt = 0, PriEnd = 0;
		ToaTlrc = int(PriRef * 0.2);
		ToaTlrc++;
		for(PriSt = PriInitSt; PriSt < AnalyPulseNums - 1; PriSt++)
		{
			if((PriArray[PriSt + 1] - PriArray[PriSt]) > ToaTlrc)
			{
				continue;
			}
			for (PriEnd = AnalyPulseNums - 2; PriEnd >= PriSt; PriEnd--)
			{
				if((PriArray[PriSt] == 0)?0:(PriArray[PriEnd] / PriArray[PriSt] >= 2))
				{
					continue;
				}
				else
				{
					break;
				}
			}
			if (PriEnd != PriSt)
			{
				break;
			}
		}
		if (PriEnd != PriSt)
		{
			DitherPriRange = (PriArray[PriEnd] - PriArray[PriSt]) * 100;
			DitherPriLens = PriEnd - PriSt + 1;
			for (int i = PriSt; i <= PriEnd; i++)
			{
				DitherPriSum = DitherPriSum + PriArray[i];
			}
			DitherPri = int(DitherPriSum / DitherPriLens);
			if (DitherPri > 0)
			{
				DitherPriRange = DitherPriRange / DitherPri;
				ResultTmp.DitherRnge = DitherPriRange;
				ResultTmp.Pri = DitherPri;
			}
		}
		else
		{
			ResultTmp.DitherRnge = 0;
			ResultTmp.Pri = 0;
		}
	}
	virtual void 	MutiChanAgileDiscern()           											//多通道捷变信号分选
	{
		int *AgiChanNo = new int[DualRamNormalStart + DualRamValidChanNum];
		int ValidAgiChLens = 0;
		int Rtn = 0;
		RfTy RfRef = 0, RfTlrc = MutiChanAgileTlrc, RfTmp;
		PwTy PwRef = 0, PwTlrc, PwTmp;
		int ChanlNoTmp, ChanStart = DualRamNormalStart;
		while(1)
		{
			RfRef = 0;
			memset(AgiChanNo, 0, sizeof(int)*(DualRamNormalStart + DualRamValidChanNum));
			for(int i = ChanStart; i < DualRamNormalStart + DualRamValidChanNum; i++)
			{
				if(AnalysisedChannel[i] == ChanRemaindedPulse)
				{
					ChanlNoTmp = i;
					ChanDiscernStLoc = DualRam[i].DiscernStLoc;
					ChanPulseNums 	 = DualRam[i].DiscernPulseNums.GetVal();
					ChanPulseNums 	 = (ChanPulseNums >= NormalAnalyNum)?NormalAnalyNum:ChanPulseNums;
					for(int j = ChanDiscernStLoc; j < ChanDiscernStLoc + ChanPulseNums; j++) 
					{
						if(DualRam[ChanlNoTmp].PulseWord.Deducted[j] == 0) //找一没被扣除过的有效载频
						{
							RfRef = DualRam[ChanlNoTmp].PulseWord.Rf[j];
							PwRef = DualRam[ChanlNoTmp].PulseWord.Pw[j];
							break;
						}
					}
				break;
				}
			}
			if(RfRef == 0)
			{
				break;
			}
			ValidAgiChLens = 0;
			for(int i = DualRamNormalStart; i < DualRamNormalStart + DualRamValidChanNum; i++)
			{
				if(AnalysisedChannel[i] == ChanRemaindedPulse)
				{
					RfTmp = DualRam[i].PulseWord.Rf[DualRam[i].DiscernStLoc];
					PwTmp = DualRam[i].PulseWord.Pw[DualRam[i].DiscernStLoc];
					if(PwTmp < 25)
					{
						PwTlrc = 4;
					}
					else
					{
						PwTlrc = (int) (PwTmp * 0.12);
					}
					if(((abs(RfRef - RfTmp)) < RfTlrc) && (abs(PwRef - PwTmp) < PwTlrc))
					{
						AgiChanNo[ValidAgiChLens++] = i;
					}
				}
			}
			if((ValidAgiChLens > 0) && (ValidAgiChLens < 15))    //!=0
			{
				Rtn = ClctAgilePulseToAnalChan(AgiChanNo, ValidAgiChLens);
				if(Rtn == 0)
				{
					break;
				}
				Rtn = AgainDiscern();
				if(Rtn == 1)
				{
					for(int i = 0; i < ValidAgiChLens; i++)
					{
						AnalysisedChannel[AgiChanNo[i]] = AgileChanDiscern;
					}
				}
			}
			ChanStart = ChanlNoTmp + 1;
			if(ChanStart >= DualRamNormalStart + DualRamValidChanNum)
			{
				break;
			}
		}
		delete[] AgiChanNo;
	}
	virtual int 	T50ClctAgilePulseToAnalChan(int *AgiChanNo, int ValidAgiChLens)				//多通道捷变脉冲输入到临时通道, ToaTy Restrict
	{
		int AgiChanBeginRecord[15] = {0};
		int RfCounts = 0, Rtn = 0;
		AnalyPulseNums = 0;
		for(int i = 0; i < ValidAgiChLens; i++)
		{
			ChanNo = AgiChanNo[i];
			ChanDiscernStLoc = DualRam[ChanNo].DiscernStLoc;
			ChanPulseNums 	 = DualRam[ChanNo].DiscernPulseNums.GetVal() + ChanDiscernStLoc;
			for(int j = AgiChanBeginRecord[i]; j < ChanPulseNums; j++)
			{
				if(DualRam[ChanNo].PulseWord.Deducted[j] == 1)        	//从第一个通道里寻找脉冲列的toa范围toa0~toa1
				{
					continue;          									//被扣除的脉冲 continue;
				}
				AgileChan->AgileRf[AnalyPulseNums] 	= DualRam[ChanNo].PulseWord.Rf[j];
				AgileChan->AgilePw[AnalyPulseNums] 	= DualRam[ChanNo].PulseWord.Pw[j];
				AgileChan->AgilePa[AnalyPulseNums] 	= DualRam[ChanNo].PulseWord.Pa[j];
				AgileChan->AgileTon[AnalyPulseNums] = DualRam[ChanNo].PulseWord.Ton[j];
				AgileChan->AgileToa[AnalyPulseNums] = DualRam[ChanNo].PulseWord.Toa[j];
				AnalyPulseNums++;
			}
		}
		if (AnalyPulseNums < AgileAnaMinNum)
		{
			AnalyPulseNums = 0;
			Rtn = 0;
		}
		else
		{
			int Temp = 0;
			TonTy PreTon, NextTon;
			ToaTy PreToa, NextToa;
			for(int j = 0; j < AnalyPulseNums; j++)							//按Toa从小到大排序
			{
				for(int k = 0; k < AnalyPulseNums - j - 1; k++)
				{
					PreTon 	= AgileChan->AgileTon[k];
					PreToa 	= AgileChan->AgileToa[k];
					NextTon = AgileChan->AgileTon[k + 1];
					NextToa	= AgileChan->AgileToa[k + 1];
					if(TonaJudge(PreTon,PreToa,NextTon,NextToa) == MoreThan)
					{
						Temp = NextToa;
						AgileChan->AgileToa[k + 1] = PreToa;
						AgileChan->AgileToa[k] = Temp;
						Temp = NextTon;
						AgileChan->AgileTon[k + 1] = PreTon;
						AgileChan->AgileTon[k] = Temp;
						Temp = AgileChan->AgileRf[k + 1];
						AgileChan->AgileRf[k + 1] = AgileChan->AgileRf[k];
						AgileChan->AgileRf[k] = Temp;
						Temp = AgileChan->AgilePw[k + 1];
						AgileChan->AgilePw[k + 1] = AgileChan->AgilePw[k];
						AgileChan->AgilePw[k] = Temp;
						Temp = AgileChan->AgilePa[k + 1];
						AgileChan->AgilePa[k + 1] = AgileChan->AgilePa[k];
						AgileChan->AgilePa[k] = Temp;
					}
				}
			}
			Rtn = 1;
		}
		if(Rtn != 0)
		{
			int AgileChAnalyLocSt = 0, AgileChAnalyLocEd = 0;
			if(AnalyPulseNums >= NormalAnalyNum)
			{
				{
					TonTy TonBegin, TonEnd;
					TonBegin = AgileChan->AgileTon[0];
					TonEnd = AgileChan->AgileTon[AnalyPulseNums - 1];
					if(TonEnd == TonBegin)
					{
						AgileChAnalyLocSt = (AnalyPulseNums-NormalAnalyNum)/2;
						AgileChAnalyLocEd = (AnalyPulseNums+NormalAnalyNum)/2;
					}
					else
					{
						int TonDiffSum = 0, TonDiffMinSum = 0;
						int TonDiffMinSumSt = 0,TonDiffMinSumEd = 0;
						TonDiffMinSum = AgileChan->AgileTon[NormalAnalyNum-1] - AgileChan->AgileTon[0];
						for(int i = 0; i <= AnalyPulseNums - NormalAnalyNum; i++)
						{
							TonDiffSum = AgileChan->AgileTon[i+NormalAnalyNum-1] - AgileChan->AgileTon[i];
							if(TonDiffMinSum > TonDiffSum)
							{
								TonDiffMinSum = TonDiffSum;
								TonDiffMinSumSt = i;
							}
							else if(TonDiffMinSum == TonDiffSum)
							{
								TonDiffMinSumEd = i;
							}
						}
						if(TonDiffMinSumEd > TonDiffMinSumSt)
						{
							AgileChAnalyLocSt = TonDiffMinSumSt + (TonDiffMinSumEd - TonDiffMinSumSt)/2;
							AgileChAnalyLocEd = AgileChAnalyLocSt + NormalAnalyNum;
						}
						else
						{
							AgileChAnalyLocSt = TonDiffMinSumSt;
							AgileChAnalyLocEd = AgileChAnalyLocSt + NormalAnalyNum;
						}
					}
				}
				AnalyPulseNums = NormalAnalyNum;
			}
			else
			{
				AgileChAnalyLocSt = 0;
				AgileChAnalyLocEd = AnalyPulseNums;
			}
			ClearAnalyChannel();
			AnalyChannel.ChanNo = AgiChanNo[0];
			for (int i = AgileChAnalyLocSt; i < AgileChAnalyLocEd; i++)
			{
				AnalyChannel.Rf[i - AgileChAnalyLocSt] 	= AgileChan->AgileRf[i];
				AnalyChannel.Pw[i - AgileChAnalyLocSt] 	= AgileChan->AgilePw[i];
				AnalyChannel.Pa[i - AgileChAnalyLocSt] 	= AgileChan->AgilePa[i];
				AnalyChannel.Toa[i - AgileChAnalyLocSt] = AgileChan->AgileToa[i];
				AnalyChannel.Ton[i - AgileChAnalyLocSt] = AgileChan->AgileTon[i];
			}
			Rtn = 1;
			for (int i = 0; i < AnalyPulseNums; i++)
			{
				if (abs(AnalyChannel.Rf[i] - AnalyChannel.Rf[0]) <= 5)
				{
					RfCounts++;
				}
			}
			if (RfCounts >= (int) (AnalyPulseNums * 85 / 100))
			{
				AnalyPulseNums = 0;
				Rtn = 0;
			}
		}
		return Rtn;
	}
	virtual void 	T50MutiChanAgileDiscern()           											//多通道捷变信号分选
	{
		int *AgiChanNo = new int[DualRamNormalStart + DualRamValidChanNum];
		int ValidAgiChLens = 0;
		int Rtn = 0;
		RfTy RfRef = 0, RfTlrc = MutiChanAgileTlrc, RfTmp;
		PwTy PwRef = 0, PwTlrc, PwTmp;
		int ChanStart = DualRamNormalStart;
		while(1)
		{
			if(ChanStart >= DualRamNormalStart + DualRamValidChanNum)
			{
				break;
			}
			RfRef = 0;
			memset(AgiChanNo, 0, sizeof(int)*(DualRamNormalStart + DualRamValidChanNum));
			for(int i = ChanStart; i < DualRamNormalStart + DualRamValidChanNum; i++)
			{
				if((AnalysisedChannel[i] == AgileChanNotDiscern) ||
				   (AnalysisedChannel[i] == ChanRemaindedPulse))
				{
					ChanDiscernStLoc = DualRam[i].DiscernStLoc;
					ChanPulseNums 	 = ChanDiscernStLoc + DualRam[i].DiscernPulseNums.GetVal();
					for(int j = 0; j < ChanPulseNums; j++) 
					{
						if(DualRam[i].PulseWord.Deducted[j] == 0) //找一没被扣除过的有效载频
						{
							RfRef = DualRam[i].PulseWord.Rf[j];
							PwRef = DualRam[i].PulseWord.Pw[j];
							break;
						}
					}
				}
				ChanStart = i + 1;
				if(RfRef == 0)
				{
					continue;
				}
				else
				{
					break;
				}
			}
			if(RfRef == 0)
			{
				break;
			}
			ValidAgiChLens = 0;
			for(int i = DualRamNormalStart; i < DualRamNormalStart + DualRamValidChanNum; i++)
			{
				if((AnalysisedChannel[i] == AgileChanNotDiscern) ||
				   (AnalysisedChannel[i] == ChanRemaindedPulse))
				{
					RfTmp = DualRam[i].PulseWord.Rf[DualRam[i].DiscernStLoc];
					PwTmp = DualRam[i].PulseWord.Pw[DualRam[i].DiscernStLoc];
					if(PwTmp < 25)
					{
						PwTlrc = 4;
					}
					else
					{
						PwTlrc = (int) (PwTmp * 0.12);
					}
					if(((abs(RfRef - RfTmp)) < RfTlrc) && (abs(PwRef - PwTmp) < PwTlrc))
					{
						AgiChanNo[ValidAgiChLens++] = i;
					}
				}
			}
			if((ValidAgiChLens > 0) && (ValidAgiChLens < 15))    //!=0
			{
				Rtn = T50ClctAgilePulseToAnalChan(AgiChanNo, ValidAgiChLens);
				if(Rtn == 0)
				{
					continue;
				}
				Rtn = AgainDiscern();
				if(Rtn == 1)
				{
					for(int i = 0; i < ValidAgiChLens; i++)
					{
						AnalysisedChannel[AgiChanNo[i]] = AgileChanDiscern;
					}
				}
			}
		}
		delete[] AgiChanNo;
	}
	virtual int 	ContiSignalDiscern()
	{
		const int FIX_DISCERN_MINNUM = 3;
		ToaTy PriTmp,PriRef,PriLoc,PriTolrc = 2,PriMod,PriDiv;
		int AddressLens;
		int i,j,k;
		int temp,AnalysisedNum = 0,rtn = 0;
		map<ToaTy,int> PriRefMap;
		int *PriArray 		= new int[AnalyPulseNums];
		int *PdwDeducted 	= AnalyChannel.Deducted;
		ClearAddressPair();
		int FixDiscernTimes = 0;
		while((FixDiscernTimes < 3) && (AnalyPulseNums >= FIX_DISCERN_MINNUM))
		{
			for (i = 0; i < 3; i++)
			{
				if(i < AnalyPulseNums)
				{
					for (j = 1; j < 4; j++) 
					{
						if(i+j < AnalyPulseNums)
						{
							PriTmp = AnalyChannel.Toa[i + j] - AnalyChannel.Toa[i];
							if ((PriTmp >= 10) && (PriTmp <= 10000))
							{
								PriRefMap[PriTmp] = 0;
							}
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					break;
				}
			}
			for(map<ToaTy,int>::iterator PriRefIter = PriRefMap.begin(); PriRefIter != PriRefMap.end(); PriRefIter++)
			{
				AddressLens = 0;
				PriRef = PriRefIter->first;
				for (i = 0; i < 9; i++)
				{
					if(i < AnalyPulseNums)
					{
						for (j = 1; j < 5; j++)
						{
							if(i+j < AnalyPulseNums)
							{
								PriLoc = AnalyChannel.Toa[i + j] - AnalyChannel.Toa[i];
								if (ABS(PriRef - PriLoc) > PriTolrc)
								{
									if (PriLoc > PriRef)
									{
										break;
									}
									continue;
								}
								PriRefMap[PriRef]++;
								AddressPair[AddressLens].first = i;
								AddressPair[AddressLens++].second = i + j;
								break;
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						break;
					}
				}
				if ((PriRefMap[PriRef] < 3) || ((PriRefMap[PriRef] >= 3) && ((AddressPair[0].second != AddressPair[1].first) || (AddressPair[1].second != AddressPair[2].first)))) 
				{
					for (i = 0; i < AddressLens; i++)
					{
						AddressPair[i].first = 0;
						AddressPair[i].second = 0;
					}
					AddressLens = 0;
					PriRefMap[PriRef] = 0;
					continue;
				}
				else
				{
					for (k = AddressPair[AddressLens - 1].second; k < AnalyPulseNums; k += j)
					{
						for (j = 1; j < 5; j++)
						{
							if ((k + j) >= AnalyPulseNums)
							{
								break;
							}
							PriTmp = AnalyChannel.Toa[k + j] - AnalyChannel.Toa[k];
							PriDiv = PriTmp / PriRef;
							PriMod = PriTmp % PriRef;
							if ((PriMod > PriTolrc) && ((PriRef - PriMod) > PriTolrc)) // if (abs(Toa0-temp)>Toa05)
							{
								if (PriDiv > 2)
								{
									break; //if(temp>Toa0) break;
								}
								continue;
							}
							PriRefMap[PriRef]++;
							AddressPair[AddressLens].first = k;
							AddressPair[AddressLens++].second = k + j;
							break;
						}
					}
					j = 0;
					for (k = 0; k < AddressLens; k++)
					{
						if (AddressPair[k].second != AddressPair[k + 1].first)
						{
							j = 0;
						}
						else if (++j >= 3)
						{
							break;
						}
					}
					if (j < 3)
					{
						continue;
					}
					else
					{
						ClearResultTmp();
						ResultTmp.Cmd 	= LongMsgType;
						ResultTmp.Type 	= 6;
						ResultTmp.Pri 	= PriRef;
						ResultTmp.Rfl 	= AnalyChannel.Rf[AddressPair[0].first];
						ResultTmp.Rfh 	= 0;
						ResultTmp.Pw	= AnalyChannel.Pw[AddressPair[0].first];
						ResultTmp.Pa	= AnalyChannel.Pa[AddressPair[0].first];
						ResultTmp.dtime = TimerCount.Seconds/10;
						int PaNum = ResultTmpRfPwPaSet(AddressPair,AddressLens);
						temp = CalculateDoa(PaNum);
						if (temp <= 360)
						{
							ResultTmp.Doa = temp;
							ContiResultOutput();
						}
						AnalysisedNum += AddressLens;
						rtn = AnalysisedNum;
						for (j = 0; j < AddressLens; j++)
						{
							k = AddressPair[j].first;
							if (PdwDeducted[k] != 1)
							{
								PdwDeducted[k] = 1;
							}
							k = AddressPair[j].second;
							if (PdwDeducted[k] != 1)
							{
								PdwDeducted[k] = 1;
							}
						}
						AnalChanDeductPrcs();
					}
				}
			}
			PriRefMap.clear();
			if(rtn == 0)
			{
				break;
			}
			FixDiscernTimes++;
		}
		delete[] PriArray;
		return rtn;
	}
	virtual int 	ContiSumSignalDiscern()
	{
		int rtn = 0;
		if(AnalyPulseNums > 5)
		{
			for(int i = 0; i < AnalyPulseNums - 1; i++)
			{
				AddressPair[i].first = i;
				AddressPair[i].first = i + 1;
			}
			ClearResultTmp();
			int PaNum = ResultTmpRfPwPaSet(AddressPair,AnalyPulseNums);
			ResultTmp.Cmd 	= LongMsgType;
			ResultTmp.Type 	= 6;
			ResultTmp.Pri 	= 0;
			ResultTmp.Rfl 	= AnalyChannel.Rf[0];
			ResultTmp.Rfh 	= 0;
			ResultTmp.Pw	= 4096;
			ResultTmp.Pa	= AnalyChannel.Pa[0];
			ResultTmp.dtime = TimerCount.Seconds/10;
			int temp = CalculateDoa(PaNum);
			if (temp <= 360)
			{
				ResultTmp.Doa = temp;
				ContiResultOutput();
			}
			rtn = AnalyPulseNums;
			for (int j = 0; j < AnalyPulseNums - 1; j++)
			{
				int k = AddressPair[j].first;
				if (AnalyChannel.Deducted[k] != 1)
				{
					AnalyChannel.Deducted[k] = 1;
				}
				k = AddressPair[j].second;
				if (AnalyChannel.Deducted[k] != 1)
				{
					AnalyChannel.Deducted[k] = 1;
				}
			}
			AnalChanDeductPrcs();
		}
		return rtn;
	}
	virtual void 	ContiResultOutput()
	{
		int ContiResultNo = 0;
		for(int i = 897; i < 900; i++)
		{
			if(Result[i].ResultData.Cmd == 0)
			{
				Result[i].ResultData.Cmd = LongMsgType;
				ContiResultNo = i;
				break;
			}
			else if(Result[i].ResultData.Cmd != MissingMsgType)
			{
				if(ABS(Result[i].ResultData.Rfl - ResultTmp.Rfl) < 5)
				{
					Result[i].ResultData.Cmd = ParaUpdataMsgType;
					ContiResultNo = i;
					break;
				}
			}
			else if(Result[i].ResultData.Cmd == MissingMsgType)
			{
				Result[i].ResultData.Cmd = LongMsgType;
				ContiResultNo = i;
				break;
			}
		}
		if(ContiResultNo != 0)
		{
			int CmdTmp = Result[ContiResultNo].ResultData.Cmd;
			Result[ContiResultNo].ResultData = ResultTmp;
			Result[ContiResultNo].ResultData.Cmd = CmdTmp;
			if (Result[ContiResultNo].ResultData.Cmd == LongMsgType)
			{
				Result[ContiResultNo].ResultNo	= ContiResultNo;
				MsgExport(ContiResultNo);
			}
			LastTimeOfResult[ContiResultNo] = TimerCount.Seconds;
		}
		else
		{
			COUTSTREND("连续波通道已满！");
		}
	}
	virtual void 	ContiDoaUpdata()        															//方位更新
	{
		if(SysCmd.DoaUpdata == 1)
		{
			if(TimerCount.Ms - ContDoaUpdataStTime > DoaUpdataTimeOut)
			{
				DoaUpdataTag = 1;
			}
			if (DoaUpdataTag == 1)
			{
				for (int i = 897; i < 900; i++)
				{
					if (Result[i].ResultData.Cmd != MissingMsgType)
					{
						DoaUpdataMsg.SetDoaPaMsg(Result[i].ResultNo,
												 Result[i].ResultData.Doa,
												 Result[i].ResultData.Pa&0x3F);
					}
				}
				DoaUpdataMsg.SetEndTag();
				UCHAR *OutData = (UCHAR *)(&DoaUpdataMsg);
				for(int i = 0; i < DoaUpdataMsg.CharLens; i++)
				{
					WriteOutPort(USHORT(OutData[i]));
				}
				DoaUpdataMsg.Clear();
				DoaUpdataTag = 0;
				ContDoaUpdataStTime = TimerCount.Ms;
			}
		}
	}
	virtual void 	ContiDisappearJudge()        													//判消失
	{
		UINT DisappearTimeOut = 5;
		if(BandNo == 1)														// BandNo 1
		{
#if _MSC_VER
			USHORT DisapJudgeTmp = 0;
#elif PPC
			USHORT DisapJudgeTmp = RdInPort((USHORT *)0xEC00004C);
#endif /* _MSC_VER / PPC */
			DisapJudgeTag = ((0x02&DisapJudgeTmp) == 0)?0:1;
			if(DisapJudgeTag == 1)
			{
				return;
			}
		}
		for(int i = 897; i < 900; i++)
		{
			if(TimerCount.Seconds - LastTimeOfResult[i] >= DisappearTimeOut)
			{
				if(Result[i].ResultData.Cmd != MissingMsgType)
				{
					Result[i].ResultData.Cmd = MissingMsgType;
					Result[i].ResultNo = i;
					MsgExport(i);
				}
			}
		}
	}
	virtual int 	ContinuousChanDiscern()
	{
		int rtn = 0;
		int DiscernRtn = 0;
		while(1)
		{
			ChanNo = 0;
			AnalyPulseNums = CollectPulseToAnalyChannel();
			if(AnalyPulseNums == 0)
			{
				break;
			}
			if(AnalyPulseNums >= MutiSignalAnalyMinNum) 
			{
				rtn = 1;
				DiscernRtn = ContiSumSignalDiscern();
				if (DiscernRtn > 0)
				{
					AnalysisedChannel[ChanNo] = NormalChanDiscern;
				}
				else
				{
					AnalysisedChannel[ChanNo] = ChanRemaindedPulse;
				}
			}
			else
			{
				AnalysisedChannel[ChanNo] = ChanRemaindedPulse;
			}
			if(AnalysisedChannel[ChanNo] == ChanRemaindedPulse) 
			{
				CancelDeduct();                                		//取消扣除标志
			}
		}
		ContiDoaUpdata();
		ContiDisappearJudge();
		return rtn;
	}
	virtual void 	ParaUpdate()        														//参数更新
	{
		if(TimerCount.Ms - ParaUpdateStTime > ParaUpdataTimeOut)
		{
			ParaUpdateTag = 1;
		}
		if (ParaUpdateTag == 1)
		{
			for (int i = 0; i < ResultNums; i++)
			{
				if (Result[i].ResultData.Cmd == ParaUpdataMsgType)
				{
					MsgExport(i);
				}
			}
			ParaUpdateTag = 0;
			ParaUpdateStTime = TimerCount.Ms;
		}
	}
	virtual void 	DoaUpdata()        															//方位更新
	{
		if(SysCmd.DoaUpdata == 1)
		{
			if(TimerCount.Ms - DoaUpdataStTime > DoaUpdataTimeOut)
			{
				DoaUpdataTag = 1;
			}
			if (DoaUpdataTag == 1)
			{
				for (int i = 0; i < ResultNums; i++)
				{
					if (Result[i].ResultData.Cmd != MissingMsgType)
					{
						DoaUpdataMsg.SetDoaPaMsg(Result[i].ResultNo,
												 Result[i].ResultData.Doa,
												 Result[i].ResultData.Pa&0x3F);
					}
				}
				DoaUpdataMsg.SetEndTag();
				UCHAR *OutData = (UCHAR *)(&DoaUpdataMsg);
				for(int i = 0; i < DoaUpdataMsg.CharLens; i++)
				{
					WriteOutPort(USHORT(OutData[i]));
				}
				DoaUpdataMsg.Clear();
				DoaUpdataTag = 0;
				DoaUpdataStTime = TimerCount.Ms;
			}
		}
	}
	virtual void 	DisappearJudge()        													//判消失
	{
		UINT DisappearTimeOut = 60;
		if(BandNo == 1)														// BandNo 1
		{
#if _MSC_VER
			USHORT DisapJudgeTmp = 0;
#elif PPC
			USHORT DisapJudgeTmp = RdInPort((USHORT *)0xEC00004C);
#endif /* _MSC_VER / PPC */
			DisapJudgeTag = ((0x02&DisapJudgeTmp) == 0)?0:1;
			if(DisapJudgeTag == 1)
			{
				return;
			}
		}
		for(int i = 0; i < ResultNums; i++)
		{
//			if((Result[i].ScanType >= 2) && (Result[i].AntenaRpm <= 30))
//			{
//				DisappearTimeOut = 60;
//			}
//			if((Result[i].ScanType == 1) && (Result[i].ResultData.Type < 3))
//			{
//				DisappearTimeOut = 5;
//			}
//			else if((Result[i].ScanType == 1) && (Result[i].ResultData.Type >= 3))
//			{
//				DisappearTimeOut = 30;
//			}
//			else
//			{
//				DisappearTimeOut = 40;
//			}
//			if(Result[i].ResultData.Type == 0)
//			{
//				DisappearTimeOut = 60;
//			}
			if(TimerCount.Seconds - LastTimeOfResult[i] >= DisappearTimeOut)
			{
				if(Result[i].ResultData.Cmd != MissingMsgType)
				{
					Result[i].ResultData.Cmd = MissingMsgType;
					Result[i].ResultNo = i;
					COUT(i,TimerCount.Seconds,LastTimeOfResult[i]);
					MsgExport(i);
				}
			}
		}
	}
	virtual void 	ScanAnalysis()																//扫描分析
	{
		for (int i = 0; i < ResultNums; i++)
		{
			if((Result[i].ScanType == 0) || (Result[i].ResultData.Type >= 4))
			{
				continue;
			}
			if(Result[i].ResultData.Cmd == MissingMsgType)
			{
				continue;
			}
			ChanNo = Result[i].ChanNo;
			if(DualRam[ChanNo].ScanReadable == true)
			{
				if(Result[i].ScanType == 1)
				{
					;
				}
				else
				{
					SetScanPara(i);
				}
				DualRam[ChanNo].ScanAnaly = 1;
			}
		}
	}
	virtual void 	SetScanPara(int ResultNo)													//单通道扫描分析
	{
		_PDWChannelType &PDWChannelTmp = DualRam[ChanNo];
		_PDWType &PdwTmp = DualRam[ChanNo].PulseWord;
		ChanPulseNums = PDWChannelTmp.ScanPulseNums.GetVal();
		int ToaCounts = 0, ToaLocTmp = -1, GapCounts = 0;
		ToaTy PriRef = 0, ToaPre = 0, ToaTail = 0, PriTlrc = 0, PriTmp = 0, PriIntervaLeft, PriIntervaRight;
		TonTy DiffTonTmp = 0;
		ToaTy *ToaArray = new ToaTy[ChanPulseNums];
		TonTy *TonArray = new TonTy[ChanPulseNums];
		PaTy  *PaArray  = new PaTy[ChanPulseNums];
		TonTy TonPaMax[3] = {0};
		PriRef = Result[ResultNo].ResultData.Pri;
		if((PriRef != 0) && (Result[ResultNo].ResultData.DitherRnge == 0))			//ScanFixPri
		{
			PriTlrc = 0;
			PriIntervaLeft 	= PriRef - 2;
			PriIntervaRight = PriRef + 2;
		}
		else if(PriRef == 0)														//ScanJaggPri
		{
			PriTlrc = 0;
			PriIntervaLeft = 0;
			PriIntervaRight = Result[ResultNo].ResultData.FramePeriods;
		}
		else if(Result[ResultNo].ResultData.DitherRnge != 0) 						//ScanDitherPri
		{
			PriTlrc = PriRef * (Result[ResultNo].ResultData.DitherRnge) / 100;
			PriIntervaLeft = PriRef - PriTlrc;
			PriIntervaRight = PriRef + PriTlrc;
		}
		for(int i = 0; i < ChanPulseNums; i++)
		{
			ToaPre = PdwTmp.Toa[i];
			for(int j = i + 1; j < ChanPulseNums; j++)
			{
				ToaTail = PdwTmp.Toa[j];
				PriTmp = ToaTail - ToaPre;
				if((PriTmp > PriIntervaLeft) && (PriTmp < PriIntervaRight))
				{
					if(ToaLocTmp != i)
					{
						ToaArray[ToaCounts] = ToaPre;
						TonArray[ToaCounts] = PdwTmp.Ton[i];
						PaArray[ToaCounts]  = PdwTmp.Pa[i] & 0x3f;
						ToaCounts++;
					}
					ToaArray[ToaCounts] = ToaTail;
					TonArray[ToaCounts] = PdwTmp.Ton[j];
					PaArray[ToaCounts]  = PdwTmp.Pa[j] & 0x3f;
					ToaCounts++;
					i = j - 1;
					ToaLocTmp = j;
					break;
				}
			}
		}
		int TonRegionLeft = 0, TonRegionRight = ToaCounts;
		for(int i = 1; i < ToaCounts; i++)
		{
			DiffTonTmp = TonArray[i] - TonArray[i - 1];
			if(DiffTonTmp > 20)
			{
				TonRegionRight 		= i;
				ToaLocTmp 		  	= MaxElement(PaArray, TonRegionLeft, TonRegionRight, whereit);
				TonPaMax[GapCounts]	= TonArray[ToaLocTmp];
				TonRegionLeft 		= TonRegionRight;
				TonRegionRight 		= ToaCounts;
				if(GapCounts == 1)
				{
					Result[ResultNo].IrradiateTime = TonArray[TonRegionRight-1] - TonArray[TonRegionLeft];
				}
				GapCounts++;
				if(GapCounts >= 3)
				{
					break;
				}
			}
		}
		if(GapCounts == 0)
		{
			Result[ResultNo].ScanType = 1; //跟踪
			Result[ResultNo].ScanPeriod = 0;
			Result[ResultNo].AntenaRpm 	= 0;
			Result[ResultNo].IrradiateTime = 0;
		}
		else if(GapCounts == 1)
		{
			ToaLocTmp	= MaxElement(PaArray, TonRegionLeft, TonRegionRight, whereit);
			TonPaMax[1]	= TonArray[ToaLocTmp];
			if(TonPaMax[1] - TonPaMax[0] >= 10000)
			{
				Result[ResultNo].ScanType 	= 4;
				Result[ResultNo].ScanPeriod = 10000;
				Result[ResultNo].AntenaRpm 	= 6;
				Result[ResultNo].IrradiateTime = 2*(TonPaMax[1] - TonArray[TonRegionLeft]);
			}
			else
			{
				Result[ResultNo].ScanType 	= 4;
				Result[ResultNo].ScanPeriod = TonPaMax[1] - TonPaMax[0];
				if(Result[ResultNo].ScanPeriod == 0)
				{
					Result[ResultNo].ScanType 	= 1;
					Result[ResultNo].AntenaRpm 	= 0;
					Result[ResultNo].IrradiateTime = 0;
				}
				else
				{
					Result[ResultNo].AntenaRpm 	= 60000/Result[ResultNo].ScanPeriod;
					Result[ResultNo].IrradiateTime = 2*(TonPaMax[1] - TonArray[TonRegionLeft]);
				}
			}
		}
		else if(GapCounts >= 2)
		{
			if(GapCounts == 2)
			{
				ToaLocTmp	= MaxElement(PaArray, TonRegionLeft, TonRegionRight, whereit);
				TonPaMax[2]	= TonArray[ToaLocTmp];
			}
			if(abs((TonPaMax[2] - TonPaMax[1]) - (TonPaMax[1] - TonPaMax[0])) >= 20)
			{
				Result[ResultNo].ScanType 	= 2;
				Result[ResultNo].ScanPeriod = abs((TonPaMax[2] - TonPaMax[0]));
				if(Result[ResultNo].ScanPeriod == 0)
				{
					Result[ResultNo].ScanType 	= 1;
					Result[ResultNo].AntenaRpm 	= 0;
					Result[ResultNo].IrradiateTime = 0;
				}
				else
				{
					Result[ResultNo].AntenaRpm 	= 60000/Result[ResultNo].ScanPeriod;
				}
			}
			else
			{
				Result[ResultNo].ScanType 	= 4;
				if(TonPaMax[2] - TonPaMax[0] >= 10000)
				{
					Result[ResultNo].ScanPeriod = 10000;
					Result[ResultNo].AntenaRpm 	= 6;
				}
				else
				{
					Result[ResultNo].ScanPeriod = TonPaMax[2] - TonPaMax[0];
					if(Result[ResultNo].ScanPeriod == 0)
					{
						Result[ResultNo].ScanType 	= 1;
						Result[ResultNo].AntenaRpm 	= 0;
						Result[ResultNo].IrradiateTime = 0;
					}
					else
					{
						Result[ResultNo].AntenaRpm 	= 60000/Result[ResultNo].ScanPeriod;
					}
				}
			}
			Result[ResultNo].IrradiateTime = Result[ResultNo].IrradiateTime;
		}
		int CmdTmp = Result[ResultNo].ResultData.Cmd;
		Result[ResultNo].ResultData.Cmd = ShortMsgType;
		MsgExport(ResultNo);
		Result[ResultNo].ResultData.Cmd = CmdTmp;
		delete[] ToaArray;
		delete[] TonArray;
		delete[] PaArray;
		return;
	}
	virtual int 	ResultTmpRfPwPaSet(_AddressPairType *AddrPartArray,int Lens)				//设置ResultTmp的Rf、Pw、Pa参数并返回PaAnalArray有效长度,输入数据为AddressPartArray
	{
		int 	PdwNums = 0,PaNums = 0;
		float 	RfTemp = 0.0, PwTemp = 0, PaTemp = 0;
		int 	cwr = 0;
		UINT temp = 0, AddrTemp = 0,RfLTemp = AnalyChannel.Rf[AddrPartArray[0].first] & 0x1fff,RfHTemp = RfLTemp;
		for (int i = 0; i < Lens; i++)
		{
			AddrTemp = AddrPartArray[i].first;
			if ((AnalyChannel.Pw[AddrTemp] & 0x4000) == 0x4000)
			{
				cwr++;
			}
			temp = AnalyChannel.Rf[AddrTemp] & 0x1fff;
			RfLTemp = (RfLTemp > temp)?temp:RfLTemp;
			RfHTemp = (RfHTemp < temp)?temp:RfHTemp;
			RfTemp = RfTemp + temp;
			temp = AnalyChannel.Pw[AddrTemp] & 0x3fff;
			PwAnalArray[PdwNums++] = float(temp);
			temp = AnalyChannel.Pa[AddrTemp] & 0x3f;
			PaTemp = PaTemp + (float) temp;
			temp = AnalyChannel.Rf[AddrTemp] & 0x8000;
			if (temp != 0)
			{
				temp = AnalyChannel.Pa[AddrTemp];
				if ((temp == 0xffff) || ((temp & 0xfff) == 0) || ((temp & 0x1ff0) == 0x0ff0))
				{
					continue;
				}
				PaAnalArray[PaNums++] = AnalyChannel.Pa[AddrTemp];
			}
		}
		if(cwr >= (int) (PdwNums / 2))
		{
			ResultTmp.Type = 0;
		}
		RfTemp = RfTemp / (float) PdwNums;
		PaTemp = PaTemp / (float) PdwNums;
		PwTemp = MostPwSet(PdwNums);
		ResultTmp.Pa = (int) PaTemp;
		ResultTmp.Pw = (int) PwTemp;
		ResultTmp.Rfl = RfLTemp;							//%findbug%
		ResultTmp.Rfh = (AgileAnalyTag != 0xffff)?0:RfHTemp;
		return PaNums;
	}
	virtual int 	ResultTmpRfPwPaSet(size_t *BarLocArray,int Lens)							//设置ResultTmp的Rf、Pw、Pa参数并返回PaAnalArray有效长度,输入数据为BarLocArray
	{
		int 	BarLocLens = 0, PdwNums = 0,PaNums = 0;
		float 	RfTemp = 0.0, PwTemp = 0, PaTemp = 0;
		int 	cwr = 0;
		UINT temp = 0, AddrTemp = 0,RfLTemp = AnalyChannel.Rf[MutiHistogram[0][BarLocArray[0]][0].Addr[0]] & 0x1fff,RfHTemp = RfLTemp;
		for (int i = 0; i < Lens; i++)
		{
			BarLocLens = MutiHistogram[0][BarLocArray[i]].Lens;
			for(int j = 0; j < BarLocLens;j++)
			{
				AddrTemp = MutiHistogram[0][BarLocArray[i]][j].Addr[0];
				if ((AnalyChannel.Pw[AddrTemp] & 0x4000) == 0x4000)
				{
					cwr++;
				}
				temp = AnalyChannel.Rf[AddrTemp] & 0x1fff;
				RfLTemp = (RfLTemp > temp)?temp:RfLTemp;
				RfHTemp = (RfHTemp < temp)?temp:RfHTemp;
				RfTemp = RfTemp + temp;
				temp = AnalyChannel.Pw[AddrTemp] & 0x3fff;
				PwAnalArray[PdwNums++] = float(temp);
				temp = AnalyChannel.Pa[AddrTemp] & 0x3f;
				PaTemp = PaTemp + (float) temp;
				temp = AnalyChannel.Rf[AddrTemp] & 0x8000;
				if(temp != 0)
				{
					temp = AnalyChannel.Pa[AddrTemp];
					if ((temp == 0xffff) || ((temp & 0xfff) == 0) || ((temp & 0x1ff0) == 0x0ff0))
					{
						continue;
					}
					PaAnalArray[PaNums++] = AnalyChannel.Pa[AddrTemp];
				}
			}
		}
		if(cwr >= (int) (PdwNums / 2))
		{
			ResultTmp.Type = 0;
		}
		RfTemp = RfTemp / (float) PdwNums;
		PaTemp = PaTemp / (float) PdwNums;
		PwTemp = MostPwSet(PdwNums);
		ResultTmp.Pa = (int) PaTemp;
		ResultTmp.Pw = (int) PwTemp;
		ResultTmp.Rfl = RfLTemp;							//%findbug%
		ResultTmp.Rfh = (AgileAnalyTag != 0xffff)?0:RfHTemp;
		return PaNums;
	}
	virtual float 	MostPwSet(int n)															//Pw的合适值设置
	{
		map<float,int> PwCount;
		float rPw;
		int *PwSign = new int[n];
		memset(PwSign,0,sizeof(int)*n);
		for(int i = 0; i < n; i++)
		{
			if (PwSign[i] != 0)
			{
				continue;
			}
			rPw = (PwAnalArray[i] <= 40)?4:(PwAnalArray[i] * 12 / 100);
			for (int j = i; j < n; j++) 
			{
				if (PwSign[j] == 1)
				{
					continue;
				}
				if (abs(int(PwAnalArray[i] - PwAnalArray[j])) <= rPw) 
				{
					PwSign[j] = 1;
					PwCount[PwAnalArray[i]]++;
				}
			}
		}
		delete[] PwSign;
		map<float,int>::iterator iterPw = max_element(PwCount.begin(), PwCount.end(), PwCountCmp<float,int>);
		return iterPw->first;
	}
	virtual int 	CalculateDoa(int PaNums)													//获取PaAnalArray中有效脉冲使用比幅测向法求Doa
	{
		if(PaNums == 0)
		{
			COUTSTR("func(CalculateDoa): PaNums == 0!");
			return 361;										//大于360，可跳出后续部分
		}
		extern char XLI[][1696];//斜率
		extern int 	FRES[][107]/*频率*/, ZOUDT[][848]/*主轴*/, DOATT[][848]/*天线交叉点*/;
		pair<int,int> MainAntenaNo,LeftAntenaNo,RightAntenaNo;				//主天线号、左天线号、右天线天线号
		int FreqIndex = 0/*当前信号对应的频率表索引值*/, PaCounts = 0, AntenaValidPa = 0, AntenaDoaTag = 0;
		int Temp1, Temp2, BX = 0, SI = 0;
		ResultTmp.Pa = *(max_element(PaAnalArray,&(PaAnalArray[PaNums-1]),PaCmp<int>));							//取数据中最大PAO值为幅度值
		FreqIndex = CountCmp2(&(FRES[BandNo][0]),&(FRES[BandNo][FreqLens-1]),ResultTmp.Rfl&0xFFF,RfCmp2<int>);	//依据当前信号的 RF查表FRES，取出RF对应的索引值, 存入Fres_Index
		PaCounts = SeekPaToDoa(PaAnalArray, PaNums, PaToDoaArray, MainDeltaPaGate);																			//统计当前信号的DPA正负值，并将值存入，对就数组中,当正值和负值均不足4个脉冲时，方位计算失败，返0x200
		if(PaCounts == 0)
		{
			return 0x200;
		}
		AntenaValidPa = FindValidPa(PaCounts);								//找 出当前信号中的DPA最大值，对应天线号定义为主天线号
		MainAntenaNo.first  = PaToDoaArray[AntenaValidPa]>>13;
		MainAntenaNo.second = PaToDoaArray[AntenaValidPa];
		if(MainAntenaNo.first == AntenaNums)      	 						//计算左右邻天线号         
		{
			RightAntenaNo.first = 0;
		}
		else
		{
			RightAntenaNo.first = MainAntenaNo.first + 1;       			//txn[1] is r;
		}
		if(MainAntenaNo.first == 0)
		{
			LeftAntenaNo.first = AntenaNums;
		}
		else
		{
			LeftAntenaNo.first = MainAntenaNo.first - 1;					//tnx[2] is l;
		}
		LeftAntenaNo.second  = FindLorR(LeftAntenaNo.first,PaNums);			//脉冲数小于3个，或，正值或负值脉冲数小于2，置0xAA
		RightAntenaNo.second = FindLorR(RightAntenaNo.first,PaNums);
		if((LeftAntenaNo.second == 0xaa) && (RightAntenaNo.second == 0xaa))	//左右邻判别
		{
			AntenaDoaTag = 2;															//only master;
		}
		else if(RightAntenaNo.second == 0xaa)
		{
			AntenaDoaTag = 0;
		}
		else if(LeftAntenaNo.second == 0xaa)
		{
			AntenaDoaTag = 1;
		}
		else if((LeftAntenaNo.second != 0xaa)&&(RightAntenaNo.second != 0xaa))		//compare l r max,sure second
		{
			Temp1 = RightAntenaNo.second & 0x1fc0;
			Temp2 = LeftAntenaNo.second  & 0x1fc0;
			if(((Temp1&0x1000) == 0) && ((Temp2&0x1000) != 0))
			{
				AntenaDoaTag = 1;
			}
			if(((Temp1&0x1000) != 0) && ((Temp2&0x1000) == 0))
			{
				AntenaDoaTag = 0;
			}
			if((((Temp1&0x1000) == 0)&&((Temp2&0x1000) == 0))
			 ||(((Temp1&0x1000) != 0)&&((Temp2&0x1000) != 0)))
			{
				if(Temp1 > Temp2)
				{
					AntenaDoaTag = 1;
				}
				else if(Temp1 < Temp2)
				{
					AntenaDoaTag = 0;
				}
				if(abs(Temp1-Temp2) <= 1)
				{
					AntenaDoaTag = 2;                           //  else k=2;                //equal
				}
			}
		}
		switch(AntenaDoaTag)
		{
			case 0:                           // l
			{
				int i = 0;
				Temp1 = (MainAntenaNo.second & 0x1fc0) >> 6;
				if((Temp1 & 0x40) != 0)
				{
					Temp1 = 0x40 - (Temp1&0x3f);
					i = 0x55;
				}
				Temp2 = (LeftAntenaNo.second & 0x1fc0) >> 6;
				if((Temp2 & 0x40) != 0)
				{
					Temp2 = 0x40 - (Temp2&0x3f);
					if(i == 0x55)
					{
						Temp1 = abs(Temp1 - Temp2);
					}
					else
					{
						Temp1 = Temp1 + Temp2;
					}
				}
				else
				{
					Temp1 = abs(Temp1 - Temp2);
				}
				{
					BX = MainAntenaNo.first;             						// ana number*2  ;>>13
					PaCounts = FreqIndex*(AntenaNums+1)*2;                   	// xli start address
					SI = FreqIndex * (AntenaNums+1);                         	// doatt and zoudt start address
					Temp1 = (Temp1 * XLI[BandNo][BX*2+PaCounts]) / 10;
					if(BX==0)
					{
						Temp2=DOATT[BandNo][SI+AntenaNums];
					}
					else
					{
						Temp2=DOATT[BandNo][SI+BX-1];
					}
					Temp1 += Temp2;
				}
				if(Temp1>=360)
				{
					Temp1=Temp1%360;
				}
				if(Temp1<350)
				{
					if((BX != 0) && (BX != AntenaNums))
					{
						if(Temp1 > ZOUDT[BandNo][SI+BX])
						{
							Temp1 = ZOUDT[BandNo][SI+BX];
						}
					}
				}
				break;
			}
			case 1:                          //  r
			{
				int i = 0;
				Temp1 = (MainAntenaNo.second & 0x1fc0) >> 6;
				if((Temp1 & 0x40) != 0)
				{
					Temp1 = 0x40 - (Temp1&0x3f);
					i = 0x55;
				}
				Temp2 = (RightAntenaNo.second & 0x1fc0) >> 6;
				if((Temp2 & 0x40) != 0)
				{
					Temp2 = 0x40 - (Temp2&0x3f);
					if(i == 0x55)
					{
						Temp1 = abs(Temp1 - Temp2);
					}
					else
					{
						Temp1 = Temp1 + Temp2;
					}
				}
				else
				{
					Temp1=abs(Temp1-Temp2);
				}
				{
					BX = MainAntenaNo.first;          // ana number*2  ; >>13
					PaCounts = FreqIndex * (AntenaNums + 1) * 2;                        // xli start address
					SI = FreqIndex * (AntenaNums + 1);                         // doatt and zoudt start address
					Temp1 = (Temp1 * XLI[BandNo][(BX*2+1)+PaCounts])/10;
					Temp2 = DOATT[BandNo][SI+BX];
					Temp1 = abs(Temp2-Temp1);
				}
				if(Temp1 >= 360)
				{
					Temp1 = Temp1 % 360;
				}
				if(Temp1 < 350)
				{
					if((BX != 0) && (BX != AntenaNums))
					{
						if(Temp1 < ZOUDT[BandNo][SI+BX])
						{
							Temp1 = ZOUDT[BandNo][SI+BX];
						}
					}
				}
				break;
			}
			case 2:                          //  e
			{
				BX = MainAntenaNo.first;
				SI = FreqIndex*(AntenaNums+1);
				Temp1 = ZOUDT[BandNo][SI+BX];
				break;
			}
		}
		if(BandNo == 0)
		{
			Temp1 = Temp1 - 120;
			if(Temp1 < 0)
			{
				Temp1 = Temp1 + 360;
			}
		}
		else
		{
			Temp1 = Temp1 + 180;
			if(Temp1 > 360)
			{
				Temp1 = Temp1 - 360;
			}
		}
		return Temp1;	  
	}
	virtual int 	SeekPaToDoa(int *PaArray, int PaArrayLens, int *OutArray, int Gate)			//标志位收集PaArray中正负有效脉冲数OutArray，返回值为OutArray有效数，若不超过Gate，返回0
	{
		int PaCounts = 0;
		for(int i = 0; i < PaArrayLens; i++)
		{
			if((PaArray[i]&0x1000)==0)
			{
				OutArray[PaCounts++]=PaArray[i];     				//+
			}
		} 
		if(PaCounts < Gate) 										//正值不够数，查负值
		{
			PaCounts=0;
			for(int i = 0; i < PaArrayLens; i++)
			{
				if((PaArray[i]&0x1000)!=0)
				{
					OutArray[PaCounts++]=PaArray[i];    			//-
				}
			}
			if(PaCounts < Gate)										//负值不够数，返回0x200
			{
				PaCounts = 0;
			}
		}
		return PaCounts;
	}
	virtual int 	FindLorR(int AntenaNo, int AnalyNums)										//左右舷有效脉冲幅度最大值
	{
	    UINT Rtn = 0, PaCounts = 0, SameAntenaNoNums=0;
	    int *Doapa = new int[AnalyNums];
	    memset(Doapa, 0, sizeof(int)*AnalyNums);
	    for(int i=0; i < AnalyNums; i++)
	    {
	    	if(AntenaNo == (PaAnalArray[i]>>13))
	    	{
	    		PaToDoaArray[SameAntenaNoNums++] = PaAnalArray[i];
	    	}
	    }
	    if(SameAntenaNoNums < SameAntenaNoMinNums)
	    {
	    	Rtn = 0xAA; 											//处理数量不够，返回
	    }
	    else
	    {
	    	PaCounts = SeekPaToDoa(PaToDoaArray, SameAntenaNoNums, Doapa, SubDeltaPaGate);
		    if(PaCounts == 0)
			{
		    	Rtn = 0xAA; 
			}
		    else
		    {
		    	Rtn = FindValidPa(PaCounts,Doapa);
		    	Rtn = Doapa[Rtn];
		    }
	    }
	    delete[] Doapa;
	    return Rtn;
	}
	virtual int 	FindValidPa(int PaNums, int *PaArray = NULL)								//取最大值，个数超过门限，则返回该值位置,否则取次大值，如果个数超过门限，则返回该值位置,如果两都均不过门限，则返回个数最多的值位置
	{
		if(PaArray == NULL)
		{
			PaArray = PaToDoaArray;
		}
		UINT PaCounts = 0, PaRef = 0;
		pair<int,int> LocCount[2];
		std::sort(PaArray,&PaArray[PaNums],PaSort<int>);	//由小到大排序
		LocCount[0].first = PaNums-1;						//找第一个最大值个数
		PaRef=PaArray[LocCount[0].first]&0xffc0;
		for(int i = LocCount[0].first; i > 0; i--)
		{
			if((PaRef-(PaArray[i]&0xffc0)) <= DeltaPaTolerance) 
			{
				PaCounts++;
				if(PaCounts >= DeltaPaRgPulseMinNums)
				{
					return LocCount[0].first;
				}
			}
			else
			{
				LocCount[1].first = i;
				break;
			}
		}
		LocCount[0].second = PaCounts;
		PaCounts=0;											//找第二个最大值个数
		PaRef=PaArray[LocCount[1].first]&0xffc0;
		for(int i = LocCount[1].first; i > 0; i--)
		{
			if((PaRef-(PaArray[i]&0xffc0)) <= DeltaPaTolerance)
			{
				PaCounts++;
				if(PaCounts >= DeltaPaRgPulseMinNums)
				{
					return LocCount[1].first;
				}
			}
			else
			{
				break;
			}
		}
		LocCount[1].second = PaCounts;						//都不过门限，取个数多的
		return (LocCount[0].second >= LocCount[1].second)?LocCount[0].first:LocCount[1].first;
	}
	virtual void 	ResultOutput()																//结果表输出
	{
		int CompParaRtn = 0, ResultNo = 0, rtnSign = 0;
		if(ResultTmp.Rfl == 0) 
		{
			return;
		}
		if(ResultTmp.Type >= 4)
		{
			if(ABS(ResultTmp.Rfh - ResultTmp.Rfl) >= 167)
			{
				ResultTmp.Rfh = ResultTmp.Rfl + 160;
			}
			else if(ABS(ResultTmp.Rfh - ResultTmp.Rfl) <= 10) 
			{
				ResultTmp.Rfh = 0;
				if (ResultTmp.Type == 4)
				{
					ResultTmp.Type = 1;
				}
				else if(ResultTmp.Type == 5)
				{
					ResultTmp.Type = 2;
				}
				else if(ResultTmp.Type == 7)
				{
					ResultTmp.Type = 3;
				}
			}
		}
		for (int i = 0; i < ResultNums; i++)
		{
			CompParaRtn = CompPara(i);
			if(CompParaRtn == 6)
			{
				if(Result[i].ResultData.Cmd != MissingMsgType)
				{
					rtnSign = 1;
					LastTimeOfResult[i] = TimerCount.Seconds;
					break;
				}
				else
				{
					ResultNo = i;
					break;
				}
			}
			else if(CompParaRtn == 1)
			{
				if(CompFlag.SamePri)
				{
					ResultNo = i;
					break;
				}
				if(ResultTmp.Type == 0)
				{
					ResultNo = i;
					break;
				}
				if(!CompFlag.SamePri)
				{
					rtnSign = 1;
					LastTimeOfResult[i] = TimerCount.Seconds;
					Result[i].Flag_Change_Prf++;
					if (Result[i].Flag_Change_Prf >= 5)
					{
						rtnSign = 0;
						ResultNo = i;
						break;
					}
				}
			}
			else if(CompParaRtn == 5)
			{
				if(CompFlag.SameRf)
				{
					ResultNo = i;
					break;
				}
				if(!CompFlag.SameRf)
				{
					rtnSign = 1;
					LastTimeOfResult[i] = TimerCount.Seconds;
					Result[i].Flag_Change_Rf++;
					if (Result[i].Flag_Change_Rf >= 5)
					{
						rtnSign = 0;
						ResultNo = i;
						break;
					}
				}
			}
			else if (CompParaRtn != 0)
			{
				ResultNo = i;
				break;
			}
		}
		if (rtnSign == 1)
		{
			return;
		}
		if (CompParaRtn != 0)  	//找到有结果
		{
			Result[ResultNo].Flag_Change_Prf = 0;
			Result[ResultNo].Flag_Change_Rf = 0;
			FoundResult(ResultNo, CompParaRtn);
		}
		else    				//未找到结果
		{
			ResultNo = NotFoundResult();
			int RadarNumber = DualRam[ChanNo].RadarNumber;
			if(RadarNumber < ChannelRadarMaxNo)
			{
				DualRam[ChanNo].ResultArrLoc[RadarNumber] = ResultNo;
				DualRam[ChanNo].RadarNumber = RadarNumber + 1;
			}
		}
		Result[ResultNo].ChanNo = AnalyChannel.ChanNo;
		int DiffPaSum = 0;
		for(int i = 0; i < AnalyPulseNums; i++)
		{
			DiffPaSum += ABS((AnalyChannel.Pa[i]&0x3F) - (AnalyChannel.Pa[AnalyPulseNums - 1 - i]&0x3F));
		}
		if(DiffPaSum < AnalyPulseNums * PaTrackTlrc)
		{
			Result[ResultNo].ScanType 	= 1;
		}
		else
		{
			Result[ResultNo].ScanType = 2;
		}
		Result[ResultNo].ScanPeriod = 0;
		Result[ResultNo].AntenaRpm 	= 0;
		Result[ResultNo].IrradiateTime = 0;
		if (Result[ResultNo].ResultData.Cmd == LongMsgType)
		{
			MsgExport(ResultNo);
		}
		LastTimeOfResult[ResultNo] = TimerCount.Seconds;
	}
	virtual int 	CompPara(int i)																//分选雷达临时结果表同结果表参数对比
	{
		int MinPri = 0, DeltaRf = 0, DeltaPri = 0, DeltaPw = 0, DeltaDoa = 5, CmpFlg1 = 0, CmpFlg2 = 0;
		ClearCompFlag();
		if((Result[i].ResultData.Type == 1) && (Result[i].ResultData.Rfl >= 0x1421) && (Result[i].ResultData.Rfl <= 0x146C))
		{
			DeltaRf = 1;
		}
		else if(Result[i].ResultData.Type >= 4)
		{
			DeltaRf = 167;
		}
		else
		{
			DeltaRf = 24;
		}
		MinPri = min(Result[i].ResultData.Pri,ResultTmp.Pri);
		if((Result[i].ResultData.Type == 2) || (Result[i].ResultData.Type == 5))
		{
			DeltaPri = (int) (MinPri * 25 / 100);
		}
		else
		{
			DeltaPri = (int) (MinPri * 15 / 1000);
		}
		if (DeltaPri == 0)
		{
			DeltaPri = 1;
		}
		DeltaPw = (Result[i].ResultData.Pw <= 0x28)?5:int(Result[i].ResultData.Pw * 12 / 100);
		CompFlag.SamePw   = (ABS(Result[i].ResultData.Pw - ResultTmp.Pw) <= DeltaPw)?1:0;		//同脉宽
		CompFlag.SameDoa  = (ABS(Result[i].ResultData.Doa - ResultTmp.Doa) <= DeltaDoa)?1:0;   	//同方位
		CompFlag.SameRf   = (ABS(Result[i].ResultData.Rfl - ResultTmp.Rfl) <= DeltaRf)?1:0;   	//同载频
		CompFlag.SameType = (Result[i].ResultData.Type == ResultTmp.Type)?1:0;					//同类型
		CompFlag.SameTime = (ABS(Result[i].ResultData.dtime - ResultTmp.dtime) 					//同dtime
				             <= (AnalyPulseNums * MinPri))?1:0;							
		if (ABS(Result[i].ResultData.Pri - ResultTmp.Pri) <= DeltaPri)
		{
			CompFlag.SamePri = 1;         														//同重频
		}
		else 
		{
			CmpFlg1 = DoubleModJudge(Result[i].ResultData.Pri, ResultTmp.Pri, DeltaPri);
			if (CmpFlg1 != 0)
			{
				CompFlag.SamePri = 1;
			}
		}
		if (((Result[i].ResultData.Type == 4) && (ResultTmp.Type == 1))
		 || ((Result[i].ResultData.Type == 5) && (ResultTmp.Type == 2))
		 || ((Result[i].ResultData.Type == 7) && (ResultTmp.Type == 3)))
		{
			CmpFlg1 = (ResultTmp.Rfl >= Result[i].ResultData.Rfl) && (ResultTmp.Rfl <= Result[i].ResultData.Rfh);
			CmpFlg2 = (abs(Result[i].ResultData.Rfl - ResultTmp.Rfl) <= DeltaRf) || (abs(ResultTmp.Rfl - Result[i].ResultData.Rfh) <= DeltaRf);
			if (CmpFlg1 || CmpFlg2)
			{
				CompFlag.SameRf = 1;
			}
		}
		if (((Result[i].ResultData.Type == 1) && (ResultTmp.Type == 4))
		 || ((Result[i].ResultData.Type == 2) && (ResultTmp.Type == 5))
		 || ((Result[i].ResultData.Type == 3) && (ResultTmp.Type == 7)))
		{
			CmpFlg1 = (Result[i].ResultData.Rfl >= ResultTmp.Rfl) && (Result[i].ResultData.Rfl <= ResultTmp.Rfh);
			CmpFlg2 = (abs(Result[i].ResultData.Rfl - ResultTmp.Rfl) <= DeltaRf) || (abs(ResultTmp.Rfh - Result[i].ResultData.Rfl) <= DeltaRf);
			if (CmpFlg1 || CmpFlg2)
			{
				CompFlag.SameRf = 1;
			}
		}
		if (((Result[i].ResultData.Type == 4) && (ResultTmp.Type == 4))
		 || ((Result[i].ResultData.Type == 5) && (ResultTmp.Type == 5))
		 || ((Result[i].ResultData.Type == 7) && (ResultTmp.Type == 7)))
		{
			if ((abs(ResultTmp.Rfl - Result[i].ResultData.Rfl) <= DeltaRf) 
			 || (abs(Result[i].ResultData.Rfh - ResultTmp.Rfh) <= DeltaRf))
			{
				CompFlag.SameRf = 1;
			}
		}
		if ((Result[i].ResultData.Type == 4) && (ResultTmp.Type == 7))
		{
			CompFlag.SameType = 1;
			CompFlag.SamePri = 1;
		}
		if ((CompFlag.SameRf) && ((CompFlag.SamePri) || (CompFlag.SamePw)))
		{
			CmpFlg1 = (CompFlag.SameDoa)?AnalyIdenticalDoa(i):AnalyDifferentDoa(i);
			if (CmpFlg1 == 1)
			{
				return 0; //NEW TN
			}
			else if (CmpFlg1 == 2)
			{
				return 8; //NEW DOA
			}
		}
		if (ResultTmp.Type == 0)
		{
			if ((CompFlag.SameDoa) && (CompFlag.SameRf))
			{
				return 1;
			}
		}
		if ((Result[i].ResultData.Type == 7) && (ResultTmp.Type == 5) && (CompFlag.SameRf) && (CompFlag.SameDoa) && (CompFlag.SamePw))
		{
			return 6;
		}
		if ((Result[i].ResultData.Type == 3) && (ResultTmp.Type == 2) && (CompFlag.SameRf) && (CompFlag.SameDoa) && (CompFlag.SamePw))
		{
			return 6;
		}

		if ((CompFlag.SameDoa) && (CompFlag.SameRf) && (!(CompFlag.SameTime)) && ((ResultTmp.Type < 4)))
		{
			return 1;
		}
		if ((CompFlag.SameDoa) && (CompFlag.SameRf) && ((ResultTmp.Type >= 4) && (CompFlag.SameType)))
		{
			return 7;     //同重频且同载频则认为是同一部
		}
		if ((CompFlag.SameDoa) && (CompFlag.SameRf) && (!CompFlag.SameType))
		{
			if (((Result[i].ResultData.Type == 4) && (ResultTmp.Type == 1)) || ((Result[i].ResultData.Type == 7) && (ResultTmp.Type == 3)) || ((Result[i].ResultData.Type == 5) && (ResultTmp.Type == 2)))
			{
				return 2;     //同重频且同载频则认为是同一部
			}
			if (((Result[i].ResultData.Type == 1) && (ResultTmp.Type == 4)) || ((Result[i].ResultData.Type == 3) && (ResultTmp.Type == 7)) || ((Result[i].ResultData.Type == 2) && (ResultTmp.Type == 5)))
			{
				return 3;
			}
		}
		if ((CompFlag.SameDoa) && (CompFlag.SamePri) && (CompFlag.SamePw) && (CompFlag.SameTime) && (!CompFlag.SameRf))
		{
			if ((CompFlag.SameType) && (ResultTmp.Type < 4) && (abs(Result[i].ResultData.Rfl - ResultTmp.Rfl) < 167))
			{
				return 4;
			}
		}
		if ((CompFlag.SameDoa) && (CompFlag.SamePri) && (CompFlag.SamePw) && (!CompFlag.SameTime) && (!CompFlag.SameRf))
		{
			if ((CompFlag.SameType) && (ResultTmp.Type < 4) && (abs(Result[i].ResultData.Rfl - ResultTmp.Rfl) < 167))
			{
				return 5;
			}
		}
		if ((CompFlag.SameDoa) && (CompFlag.SameRf) && (!CompFlag.SameTime))
		{
			return 1;     //同重频且同载频则认为是同一部
		}
		MinPri = 0;
		if (CompFlag.SamePri)
		{
			MinPri++;
		}
		if (CompFlag.SamePw)
		{
			MinPri++;
		}
		if ((CompFlag.SameType) && (CompFlag.SameRf) && (CompFlag.SameDoa) && (MinPri >= 1))
		{
			return 1;     //同重频且同载频则认为是同一部
		}
		return 0;
	}
	virtual int 	DoubleModJudge(int Prf1, int Prf2, int DeltaPrf)							//求余判断是否丢脉冲仍在容差范围内
	{
		int rtn = 0;
		int temp;
		if((Prf1 != 0) && (Prf2 != 0))
		{
			if((Prf1 >= Prf2) && ((Prf1 / Prf2) >= 1)) 
			{
				temp = Prf1 % Prf2;
				if(temp <= DeltaPrf)
				{
					rtn = 1;
				}
				else if((Prf2 - temp) <= DeltaPrf)
				{
					rtn = 1;
				}
			}
			else if((Prf2 / Prf1) >= 1)
			{
				temp = Prf2 % Prf1;
				if(temp <= DeltaPrf)
				{
					rtn = 2;
				}
				else if((Prf1 - temp) <= DeltaPrf)
				{
					rtn = 2;
				}
			}
		}
		return rtn;
	}
	virtual int 	AnalyIdenticalDoa(int i)													//相同Doa累加替换
	{
		if(Result[i].DifferentDoaTag == 1)
		{
			Result[i].AnalyDoaCounts++;
			int AnalyDoaCountTmp = Result[i].AnalyDoaCounts;
			if(AnalyDoaCountTmp >= 10)
			{
				int IdenticalDoaCountTmp = 0;
				for(int j = 0; j < 3; j++)
				{
					IdenticalDoaCountTmp = Result[i].DifferentDoaCounts[j];
					if(IdenticalDoaCountTmp != 0)
					{
						if(abs(AnalyDoaCountTmp - IdenticalDoaCountTmp) <= 2)
						{
							Result[i].SameDoa = 1;
							ResultTmp.Doa = Result[i].DifferentDoa[j];
							break;
						}
					}
				}
				ClearResultDataDoa(i);
			}
			return 2;
		}
		return 0;
	}
	virtual int 	AnalyDifferentDoa(int i,int DeltaDoa = 10)									//不同Doa累加替换
	{
		int AnalyDoaCountTmp = 0, IdenticalDoaCountTmp = 0;
		Result[i].DifferentDoaTag = 1;
		for (int j = 0; j < 3; j++)
		{  
			if(Result[i].DifferentDoaCounts[j] == 0)
			{
				Result[i].DifferentDoa[j] = ResultTmp.Doa;
				Result[i].DifferentDoaCounts[j]++;
				break;
			}
			else if(ABS((Result[i].DifferentDoa[j]) - ResultTmp.Doa) <= DeltaDoa) //新的超差DOA与备份单元相比较容差
			{   
				Result[i].DifferentDoaCounts[j]++;
				AnalyDoaCountTmp 	= Result[i].AnalyDoaCounts;
				IdenticalDoaCountTmp 	= Result[i].DifferentDoaCounts[j];
				if(IdenticalDoaCountTmp >= 10) 
				{ 
					if(abs(IdenticalDoaCountTmp - AnalyDoaCountTmp) >= 2) 
					{
						Result[i].ResultData.Doa = Result[i].DifferentDoa[j];
					} 
					else 
					{
						Result[i].SameDoa = 1;
					}
					ClearResultDataDoa(i);
				}
				break;
			}
		}
		ResultTmp.Doa = Result[i].ResultData.Doa;
		return 2;
	}
	virtual void 	FoundResult(int ResultNo, int CompParaRtn)									//当前分选型号在结果表中发现
	{
		int RflTmp, RfhTmp;
		if (Result[ResultNo].ResultData.Cmd == MissingMsgType)
		{
			Result[ResultNo].ResultData.Cmd 	= LongMsgType;
			Result[ResultNo].ResultData.Type 	= ResultTmp.Type;
			Result[ResultNo].ResultData.Rfl 	= ResultTmp.Rfl;
			Result[ResultNo].ResultData.Rfh 	= ResultTmp.Rfh;
			Result[ResultNo].ResultData.Pw 		= ResultTmp.Pw;
			Result[ResultNo].ResultData.Pri 	= ResultTmp.Pri;
			Result[ResultNo].ResultData.Doa 	= ResultTmp.Doa;
			Result[ResultNo].ResultData.Aoa	 	= ResultTmp.Aoa;
			Result[ResultNo].ResultData.Pa 		= ResultTmp.Pa;
		} 
		else
		{
			Result[ResultNo].ResultData.Cmd = ParaUpdataMsgType;
		}
		AnalyDiffTypeAndPri(ResultNo);               //重频计数以决定是否更新重频
		switch (CompParaRtn) 
		{
		case 1:
			break;
		case 2:
			if (ResultTmp.Rfl < Result[ResultNo].ResultData.Rfl) 
			{
				Result[ResultNo].ResultData.Rfl = ResultTmp.Rfl;
			}
			if (ResultTmp.Rfl > Result[ResultNo].ResultData.Rfh) 
			{
				Result[ResultNo].ResultData.Rfh = ResultTmp.Rfl;
			}
			break;
		case 3:
			Result[ResultNo].ResultData.Type = ResultTmp.Type;
			RflTmp = ResultTmp.Rfl;
			RfhTmp = ResultTmp.Rfh;
			if (Result[ResultNo].ResultData.Rfl < RflTmp)
			{
				RflTmp = Result[ResultNo].ResultData.Rfl;
			}
			if (Result[ResultNo].ResultData.Rfl > RfhTmp)
			{
				RfhTmp = Result[ResultNo].ResultData.Rfl;
			}
			Result[ResultNo].ResultData.Rfl = RflTmp;
			Result[ResultNo].ResultData.Rfh = RfhTmp;
			break;
		case 4:
			Result[ResultNo].ResultData.Type = 4;
			if (ResultTmp.Rfl < Result[ResultNo].ResultData.Rfl)
			{
				RflTmp = ResultTmp.Rfl;
				RfhTmp = Result[ResultNo].ResultData.Rfl;
			}
			else
			{
				RflTmp = Result[ResultNo].ResultData.Rfl;
				RfhTmp = ResultTmp.Rfl;
			}
			Result[ResultNo].ResultData.Rfl = RflTmp;
			Result[ResultNo].ResultData.Rfh = RfhTmp;
			break;
		case 5:
			Result[ResultNo].ResultData.Rfl = ResultTmp.Rfl;
			break;
		case 6:
			break;
		case 7:
			if (ResultTmp.Rfl < Result[ResultNo].ResultData.Rfl)
			{
				Result[ResultNo].ResultData.Rfl = ResultTmp.Rfl;
			}
			if (ResultTmp.Rfh > Result[ResultNo].ResultData.Rfh)
			{
				Result[ResultNo].ResultData.Rfh = ResultTmp.Rfh;
			}
			break;
		case 8:
			break;
		}
		if(Result[ResultNo].ResultData.Type == 4)
		{
			if (abs(Result[ResultNo].ResultData.Rfh - Result[ResultNo].ResultData.Rfl) >= 167)
			{
				return;
			}
			if (abs(Result[ResultNo].ResultData.Rfh - Result[ResultNo].ResultData.Rfl) <= 10)
			{
				Result[ResultNo].ResultData.Type = 1;
				Result[ResultNo].ResultData.Rfh = 0;
			}
		}
		SetResultOtherPara(ResultNo);
	}
	virtual int 	NotFoundResult()															//当前分选型号在结果表中未发现
	{
		int ResultNumTmp;
		if (ResultNums < 897) 
		{
			Result[ResultNums].ResultData.Cmd	= LongMsgType;
			Result[ResultNums].ResultData.Type 	= ResultTmp.Type;
			Result[ResultNums].ResultData.Pri 	= ResultTmp.Pri;
			Result[ResultNums].ResultData.Rfl 	= ResultTmp.Rfl;
			Result[ResultNums].ResultData.Rfh 	= ResultTmp.Rfh;
			ResultNumTmp = ResultNums;
			Result[ResultNums].ResultNo	= ResultNums;
			ResultNums++;
		}
		else
		{
			for (int i = 0; i < ResultNums; i++)
			{
				if (Result[i].ResultData.Cmd == MissingMsgType)
				{
					Result[i].ResultData.Cmd 	= LongMsgType;
					Result[i].ResultData.Type 	= ResultTmp.Type;
					Result[i].ResultData.Pri 	= ResultTmp.Pri;
					Result[i].ResultData.Rfl 	= ResultTmp.Rfl;
					Result[i].ResultData.Rfh 	= ResultTmp.Rfh;
					ResultNumTmp 				= i;
					Result[i].ResultNo 			= i;
					break;
				}
			}
		}
		SetResultOtherPara(ResultNumTmp);
		return ResultNumTmp;
	}
	virtual void 	AnalyDiffTypeAndPri(int i)													//不同类型和不同Pri的累加替换
	{
		int DeltaPri;
		if (Result[i].ResultData.Type == ResultTmp.Type)
		{
			Result[i].DifferentType = 0;
			Result[i].DifferentTypeCount = 0;
		}
		if (Result[i].ResultData.Type != ResultTmp.Type)
		{
			if (Result[i].DifferentType == 0)
			{
				Result[i].DifferentType = ResultTmp.Type;
				Result[i].DifferentTypeCount = 1;
			}
			else
			{
				if (Result[i].DifferentType == ResultTmp.Type)
				{
					Result[i].DifferentTypeCount++;
				}
				else 
				{
					Result[i].DifferentType = ResultTmp.Type;
					Result[i].DifferentTypeCount = 1;
				}
			}
			if (Result[i].DifferentTypeCount >= 4) 
			{
				Result[i].ResultData.Type = Result[i].DifferentType;
				Result[i].ResultData.Pri = ResultTmp.Pri;
				Result[i].ResultData.JaggNums = ResultTmp.JaggNums;
				for (int j = 0; j < 8; j++)
				{
					Result[i].ResultData.PhasePeriods[j] = ResultTmp.PhasePeriods[j];
				}
				Result[i].ResultData.FramePeriods = ResultTmp.FramePeriods;
				Result[i].ResultData.DitherRnge = ResultTmp.DitherRnge;
				Result[i].DifferentType = 0;
				Result[i].DifferentTypeCount = 0;
			}
		}
		DeltaPri = (Result[i].ResultData.Pri > ResultTmp.Pri)?ResultTmp.Pri:Result[i].ResultData.Pri;
		if ((Result[i].ResultData.Type == 2) || (Result[i].ResultData.Type == 5))
		{
			DeltaPri = (int) (DeltaPri * 25 / 100);
		}
		else
		{
			DeltaPri = (int) (DeltaPri * 15 / 1000);
		}
		if (Result[i].DifferentPriCount != 0)
		{
			if (ABS(ResultTmp.Pri - Result[i].DifferentPri) <= DeltaPri)
			{
				if (Result[i].DifferentPriCount >= 4)
				{
					Result[i].ResultData.Pri = Result[i].DifferentPri;
					Result[i].DifferentPriCount = 0;
				}
				else
				{
					Result[i].DifferentPriCount++;
				}
			}
			else
			{
				Result[i].DifferentPriCount = 0;
			}
		}
		else
		{
			Result[i].DifferentPri = ResultTmp.Pri;
			Result[i].DifferentPriCount = 1;
		}
	}
	virtual void 	SetResultOtherPara(int i)													//设置结果表其它参数
	{
		Result[i].ResultData.dtime = ResultTmp.dtime;
		Result[i].ResultData.Doa = ResultTmp.Doa;
		Result[i].ResultData.Aoa = ResultTmp.Aoa;
		Result[i].ResultData.Pw = ResultTmp.Pw;
		Result[i].ResultData.Pa = ResultTmp.Pa;
		if((Result[i].ResultData.Cmd != MissingMsgType) && (ResultTmp.Cmd == LongMsgType))
		{
			Result[i].ResultData.JaggNums = ResultTmp.JaggNums;
			for (int j = 0; j < 8; j++)
			{
				Result[i].ResultData.PhasePeriods[j] = ResultTmp.PhasePeriods[j];
			}
			Result[i].ResultData.FramePeriods = ResultTmp.FramePeriods;
			Result[i].ResultData.DitherRnge = ResultTmp.DitherRnge;
		}
		else if((Result[i].ResultData.Type == 3) && (ResultTmp.Type == 3))
		{
			Result[i].ResultData.JaggNums = ResultTmp.JaggNums;
			for (int j = 0; j < 8; j++)
			{
				Result[i].ResultData.PhasePeriods[j] = ResultTmp.PhasePeriods[j];
			}
			Result[i].ResultData.FramePeriods = ResultTmp.FramePeriods;
		}
		else if(((Result[i].ResultData.Type == 2) &&(ResultTmp.Type == 2)) ||
				((Result[i].ResultData.Type == 5) &&(ResultTmp.Type == 5))
				)
		{
			Result[i].ResultData.DitherRnge = ResultTmp.DitherRnge;
		}
	}
	virtual void 	MsgExport()																	//直接输出Msg
	{
		UCHAR *OutData = (UCHAR *)(&Msg);
		for(int i = 0; i < sizeof(_Msg); i++)
		{
			WriteOutPort(USHORT(OutData[i]));
		}
	}
	virtual void 	MsgExport(int ResultNo)														//Q告警
	{
		SetMsg(ResultNo);
		UCHAR *OutData = (UCHAR *)(&Msg);
		for(int i = 0; i < sizeof(_Msg); i++)
		{
			WriteOutPort(USHORT(OutData[i]));
		}
	}
	virtual void 	SetMsg(int ResultNo)														//根据结果表设置输出报文
	{
		ClearMsg();
		Msg.Cmd = Result[ResultNo].ResultData.Cmd;
		switch(Result[ResultNo].ResultData.Type)
		{
		case 0:
			Msg.Type	= Unknowed;
			break;
		case 1:
			Msg.Type	= FixRfFixPri;
			break;
		case 2:
			Msg.Type	= FixRfDitherPri;
			break;
		case 3:
			Msg.Type	= FixRfJaggPri;
			break;
		case 4:
			Msg.Type	= AgileRfFixPri;
			break;
		case 5:
			Msg.Type	= AgileRfDitherPri;
			break;
		case 6:
			Msg.Type	= CotinuePulse;
			break;
		default:
			Msg.Type	= Unknowed;
			break;
		}
		if((Msg.Type == AgileRfFixPri) || (Msg.Type == AgileRfDitherPri))
		{
			int RfRange = 0;
			RfRange = Result[ResultNo].ResultData.Rfh - Result[ResultNo].ResultData.Rfl;
			RfRange = (RfRange >= 167)?160:RfRange;
			if(BandNo == 1)
			{
				RfRange = RfRange|0x1000;
			}
			Msg.PdwPara.Rf			= Msg.BigToLittle((Result[ResultNo].ResultData.Rfl + Result[ResultNo].ResultData.Rfh)/2);
			Msg.PdwPara.Far			= Msg.BigToLittle(RfRange);
		}
		else
		{
			Msg.PdwPara.Rf			= Msg.BigToLittle(Result[ResultNo].ResultData.Rfl);
			Msg.PdwPara.Far			= Msg.BigToLittle(Result[ResultNo].ResultData.Rfh);
		}
		Msg.PdwPara.Doa				= Msg.BigToLittle(Result[ResultNo].ResultData.Doa);
		Msg.PdwPara.Pri				= Msg.BigToLittle(Result[ResultNo].ResultData.Pri);
		Msg.PdwPara.Pw				= Msg.BigToLittle(Result[ResultNo].ResultData.Pw);
		Msg.PdwPara.Pa				= (Result[ResultNo].ResultData.Pa & 0x3F);
		Msg.PdwPara.JaggNums		= Result[ResultNo].ResultData.JaggNums;
		for (int i = 0; i < 8; i++) 
		{
			Msg.PdwPara.PhasePeriods[i] = Msg.BigToLittle(Result[ResultNo].ResultData.PhasePeriods[i]);
		}
		Msg.PdwPara.FramePeriods	= Msg.BigToLittle(Result[ResultNo].ResultData.FramePeriods);
		Msg.PdwPara.DitherRange		= Result[ResultNo].ResultData.DitherRnge;
		Msg.ScanPara.AntenaRpm		= Msg.BigToLittle(Result[ResultNo].AntenaRpm);
		Msg.ScanPara.ScanType		= Result[ResultNo].ScanType;
		Msg.ScanPara.ScanPeriod		= Msg.BigToLittle(Result[ResultNo].ScanPeriod);
		Msg.ScanPara.IrradiateTime	= Msg.BigToLittle(Result[ResultNo].IrradiateTime);
		Msg.BatchNo					= Msg.BigToLittle(Result[ResultNo].ResultNo+1);
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd,_MsgPdwType &Ty,_MsgPdwPara &PdwPara,USHORT BatchNo)	//手动设置输出报文 LongMsgType ParaUpdataMsgType reply edit
	{
		ClearMsg();
		_Msg MsgTmp;
		switch(MsgCmd)
		{
		case LongMsgType:
			MsgTmp.SetLongMsg(Ty, PdwPara, BatchNo);
			break;
		case ParaUpdataMsgType:
			MsgTmp.SetLongMsg(Ty, PdwPara, BatchNo);
			break;
		default:
			COUTSTR("Func(SetMsg): MsgCmd Error!");
			break;
		}
		Msg = MsgTmp;
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd,_MsgScanPara &MsgScanPara,USHORT BatchNo)				//手动设置输出报文 ShortMsgType reply edit
	{
		ClearMsg();
		_Msg MsgTmp;
		switch(MsgCmd)
		{
		case ShortMsgType:
			MsgTmp.SetShrotMsg(MsgScanPara,BatchNo);
			break;
		default:
			COUTSTR("Func(SetMsg): MsgCmd Error!");
			break;
		}
		Msg = MsgTmp;
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd,USHORT BatchNo)										//手动设置输出报文 MissingMsgType reply edit
	{
		ClearMsg();
		_Msg MsgTmp;
		switch(MsgCmd)
		{
		case MissingMsgType:
			MsgTmp.SetMissingMsg(BatchNo);
			break;
		default:
			COUTSTR("Func(SetMsg): MsgCmd Error!");
			break;
		}
		Msg = MsgTmp;
	}
	virtual void 	SetDoaUpdataMsg(USHORT BatchNo,USHORT Doa,USHORT Pa)
	{
		DoaUpdataMsg.Clear();
		DoaUpdataMsg.SetDoaPaMsg(BatchNo,
								 Doa,
								 Pa&0x3F);
		DoaUpdataMsg.SetEndTag();
		UCHAR *OutData = (UCHAR *)(&DoaUpdataMsg);
		for(int i = 0; i < DoaUpdataMsg.CharLens; i++)
		{
			NLOGK(i,OutData[i]);
			WriteOutPort(USHORT(OutData[i]));
		}
		DoaUpdataMsg.Clear();
		
	}
	virtual void 	Clear()																		//全清除
	{
		ChanNo 				= DualRamNormalStart-1;
		ChanDiscernStLoc	= 0;
		ChanPulseNums 		= 0;
		AnalyPulseNums 		= 0;
		DualRamValidChanNum = 0;
		ResultNums 			= 0;
		AgileAnalyTag 		= 0;
		ParaUpdateTag 		= 0;
		DoaUpdataTag		= 0;
		ParaUpdateStTime    = 0;
		DoaUpdataStTime 	= 0;
		ContDoaUpdataStTime	= 0;
		DisapJudgeTag 		= 0;
		PortTimeBegin		= 0;
		ClearAnalyChannel();
		ClearCompFlag();
		ClearResultTmp();
		ClearMsg();
		ClearAgileChan();
		ClearDoaUpdataMsg();
		ClearPwAnalArray();
		ClearPaAnalArray();
		ClearBarLocArray();
		ClearAddrPartArray();
		ClearResult();
		ClearAnalysisedChannel();
		ClearAddressPair();
		ClearPaToDoaArray();
		ClearLastTimeOfResult();
	}
	virtual void 	ClearTmp()																	//临时通道缓存清除
	{
		ClearAnalyChannel();
		ClearMsg();
		ClearDoaUpdataMsg();
		ClearCompFlag();
		ClearResultTmp();
	}
	virtual void 	ClearAnalyChannel()		{memset(&AnalyChannel,0,sizeof(_AnalyChannelType));}
	virtual void 	ClearCompFlag()			{memset(&CompFlag,0,sizeof(_CompFlag));}
	virtual void 	ClearResultTmp()		{memset(&ResultTmp,0,sizeof(_ResultTmp));}
	virtual void 	ClearMsg()				{memset(&Msg,0,sizeof(_Msg));}
	virtual void 	ClearDoaUpdataMsg()		{DoaUpdataMsg.Clear();}
	virtual void 	ClearAgileChan() 		{memset(AgileChan,0,sizeof(_AgileChan));}
	virtual void 	ClearBarLocArray() 		{memset(BarLocArray,0,sizeof(size_t)*NormalAnalyNum);}
	virtual void 	ClearAddrPartArray() 	{memset(AddrPartArray,0,sizeof(size_t)*NormalAnalyNum);}
	virtual void 	ClearAnalysisedChannel(){memset(AnalysisedChannel,0,sizeof(int)*DualRamChannelNum);}
	virtual void 	ClearPwAnalArray()		{memset(PwAnalArray,0,sizeof(float)*NormalAnalyNum);} 
	virtual void 	ClearPaAnalArray() 		{memset(PaAnalArray,0,sizeof(int)*NormalAnalyNum);}
	virtual void 	ClearAddressPair()		{memset(AddressPair,0,sizeof(UCHAR)*(NormalAnalyNum+1)*2);}
	virtual void 	ClearPaToDoaArray()		{memset(PaToDoaArray,0,sizeof(int)*NormalAnalyNum);}
	virtual void 	ClearLastTimeOfResult()	{memset(LastTimeOfResult,0,sizeof(int)*900);}
	virtual void 	ClearResult()
	{
		ResultNums = 0;
		memset(Result,0,sizeof(_Result)*900);
	}
	virtual void 	ClearResultDataDoa(int i)
	{
		Result[i].AnalyDoaCounts = 0;
		for (int j = 0; j < 3; j++)
		{
			Result[i].DifferentDoa[j] = 0;
			Result[i].DifferentDoaCounts[j] = 0;
		}
	}
#if _MSC_VER
	virtual inline USHORT ReadInPort()																//PowerPC 端口输入
	{
		USHORT ReadInPortCmd = cin.get();
		return ReadInPortCmd;
	}
	virtual inline void	  WriteOutPort(USHORT sh)													//Visual Studio重定向输出
	{
		NLOGK(sh);
	}
#elif PPC
	virtual inline USHORT ReadInPort()																//PowerPC 端口输入
	{
		if((0x8000&RdInPort()) != 0x8000)
		{
			return RstThenRdInPort((VtUSHORT *)0xEC000044, (VtUSHORT *)0xEC000042);
		}
		else
		{
			return 0;
		}
	}
	virtual inline USHORT RdInPort(VtUSHORT *port = (VtUSHORT *)0xEC000040)							//读输入端口
	{
		return *port;
	}
	virtual inline USHORT RstThenRdInPort(VtUSHORT *InPort,VtUSHORT *Port = (VtUSHORT *)0xEC000042)	//写输入端口
	{
		*Port = 1;
		taskDelay(0);
		return *InPort;
	}
	virtual inline void	  WriteOutPort(USHORT sh)													//PowerPC 端口输出
	{
		PortTimeBegin = TimerCount.Ms;
		while((0x8000&RdOutPort()) == 0x8000)
		{
			if(TimerCount.Ms - PortTimeBegin > 60)
			{
				return;
			}
		}
		WrOutPort(sh);
	}
	virtual inline USHORT RdOutPort(VtUSHORT *port = (VtUSHORT *)0xEC000080)						//读输出端口
	{
		return *port;
	}
	virtual inline void   WrOutPort(VtUSHORT outWord,VtUSHORT *port = (VtUSHORT *)0xEC000082)		//写输出端口
	{
		*port = outWord;
	}
#endif
	virtual inline _TonaJudgeType TonaJudge(TonTy &PreTon, ToaTy &PreToa, TonTy &NextTon, ToaTy &NextToa)	//两个Ton、Toa判断大小, 存在设计缺陷, 以Next为准
	{
		if(PreTon > NextTon)
		{
			return MoreThan;
		}
		else if (PreTon < NextTon)
		{
			return LessThan;
		}
		else
		{
			if((NextToa > 12500) && (NextToa < 53036))
			{
				if(PreToa > NextToa)
				{
					return MoreThan;
				}
				else if (PreToa < NextToa)
				{
					return LessThan;
				}
				else
				{
					return Equal;
				}
			}
			else if(NextToa <= 12500)
			{
				if(PreToa > NextToa)
				{
					if(PreToa > 53036)
					{
						return LessThan;
					}
					else
					{
						return MoreThan;
					}
				}
				else if (PreToa < NextToa)
				{
					return LessThan;
				}
				else
				{
					return Equal;
				}
			}
			else if(NextToa >= 53036)
			{
				if(PreToa < NextToa)
				{
					if(PreToa < 12500)
					{
						return MoreThan;
					}
					else
					{
						return LessThan;
					}
				}
				else if (PreToa > NextToa)
				{
					return MoreThan;
				}
				else
				{
					return Equal;
				}
			}
		}
		return JudgeErr;
	}
	template<typename Ty>
	inline int MaxElement(Ty *Arr, int Begin, int End,_MaxElemType MaxElemType = findit) 					//查找左开右闭区间[Begin, End)最大值, findit返回其值, whereit返回其值位置
	{
		int Rtn = 0;
		Ty ArrValTmp = Arr[Begin];
		if(MaxElemType == findit)
		{
			for (int i = Begin; i < End; i++)
			{
				if (Arr[i] > ArrValTmp)
				{
					ArrValTmp = Arr[i];
					Rtn = ArrValTmp;
				}
			}
		}
		else if(MaxElemType == whereit)
		{
			for (int i = Begin; i < End; i++)
			{
				if (Arr[i] > ArrValTmp)
				{
					ArrValTmp = Arr[i];
					Rtn = i;
				}
			}
		}
		return Rtn;
	}
	
	int 	GetAnalyPulseNums() const {return AnalyPulseNums;}
	void	ShowStatus() const
	{
		NLOGKSSECBEG("PdwDiscern Status");
		NLOGKSWIDTH(4,"PDW通道数(DualRamCha...)	",DualRamChannelNum);			//PDW数据通道的通道数
		NLOGKSWIDTH(4,"每通道雷达数(ChannelRa...)	",ChannelRadarMaxNo);			//每个PDW数据通道的可分选雷达的最大数
		NLOGKSWIDTH(4,"标准通道起始(DualRamNo...)	",DualRamNormalStart);			//Ram缓冲区标准有效通道起始值
        NLOGKSWIDTH(4,"临时通道脉冲数(Analysis...)	",AnalysisNum);					//临时通道的最大脉冲数
        NLOGKSWIDTH(4,"单通道分选超时(SingleCh...)	",SingleChanAnalyTimeOut);		//全部有效单通道分选超时
        NLOGKSWIDTH(4,"分选初始阈值(NormalAna...)	",NormalAnalyNum);				//分选初始阈值，当以次阈值进行分选时，标志预处理并未对该通道调整阈值
        NLOGKSWIDTH(4,"多信号分选脉冲数(MutiSig...)	",MutiSignalAnalyMinNum);		//参加多信号分选的最小脉冲数
        NLOGKSWIDTH(4,"直方图分选脉冲数(HistAna...)	",HistAnalyMinNum);				//参加直方图分选的最小脉冲数
        NLOGKSWIDTH(4,"单Ch捷变RF最小范围(Sing...)"	 ,SingAgileRfRgMin);			//参加单通道捷变分选的最小RF变化范围
        NLOGKSWIDTH(4,"单Ch捷变RF最大范围(Sing...)"	 ,SingChanAgileTlrc);			//参加单通道捷变分选的最大RF变化范围
        NLOGKSWIDTH(4,"多Ch捷变RF最大范围(Muti...)"	 ,MutiChanAgileTlrc);			//参加多通道捷变分选的最大RF变化范围
        NLOGKSWIDTH(4,"捷变最小脉冲数(AgileAna...)	",AgileAnaMinNum);				//参加捷变分选的最小脉冲数
		NLOGKSWIDTH(4,"扫描分析脉冲数(ScanThre...)	",ScanThreshold);				//参加扫描分析的最小脉冲数
		NLOGKSWIDTH(4,"参数更新TimeOut(ParaU...)	",ParaUpdataTimeOut);			//参数更新TimeOut
		NLOGKSWIDTH(4,"Rf分选容差范围(AnalyCh...)	",AnalyChanRfTlrc);          	//Rf分选容差范围
		NLOGKSWIDTH(4,"Pa跟踪容差范围(PaTrack...)	",PaTrackTlrc);              	//Pa跟踪容差范围
		NLOGKSWIDTH(4,"波段号(BandNo)			"	 ,BandNo);						//波段号
		NLOGKSWIDTH(4,"天线数(AntenaNums)		"	 ,AntenaNums);              	//天线数
		NLOGKSWIDTH(4,"FREQ数组长度(FreqLen...)	"	 ,FreqLens);                	//FREQ数组长度
		NLOGKSWIDTH(4,"主天线脉冲最小值(Mai...)	"	 ,MainDeltaPaGate);          	//主天线正负脉冲数最小值 
		NLOGKSWIDTH(4,"Pa容差范围脉冲数(De...)    "	 ,DeltaPaRgPulseMinNums);   	//Pa容差范围内最小脉冲数
		NLOGKSWIDTH(4,"天线脉冲最小值(...)		"	 ,SameAntenaNoMinNums);      	//相同天线号脉冲最小值		CONSTANT
		NLOGKSWIDTH(4,"次天线脉冲数最小值(...) 	"		 ,SubDeltaPaGate);           	//次天线正负脉冲数最小值 		CONSTANT
		NLOGKSWIDTH(4,"Pa容差(DeltaPaToler...) 	",DeltaPaTolerance);         	//Pa容差，初始值设为0x1c0 	CONSTANT
		NLOGKSWIDTH(4,"方位更新(SystemCmd)		"	 ,SysCmd.DoaUpdata);         	//方位更新 
		NLOGKSWIDTH(4,"通道号(ChanNo)			"	 ,ChanNo);                   	//通道号
		NLOGKSWIDTH(4,"通道分选脉冲开始位置(...)	"	 ,ChanDiscernStLoc);            //通道分选脉冲开始位置
		NLOGKSWIDTH(4,"临时通道可读脉冲数(...)	"		 ,ChanPulseNums);            	//临时通道可读脉冲数
		NLOGKSWIDTH(4,"临时通道有效脉冲数(...)	"		 ,AnalyPulseNums);           	//临时通道有效脉冲数，对AnalyChannel中有效脉冲计数
		NLOGKSWIDTH(4,"Ram缓冲区有效通道数(...)	"	 ,DualRamValidChanNum);      	//Ram缓冲区有效通道数
		NLOGKSWIDTH(4,"结果表长度(ResultNums...)	",ResultNums);               	//当前已分选出结果的结果表长度
		NLOGKSWIDTH(4,"捷变Rfh(AgileAnalyT...)	",AgileAnalyTag);            	//捷变Rfh
		NLOGKSWIDTH(4,"参数更新(ParaUpdateTa...)	",ParaUpdateTag);            	//参数更新
		NLOGKSWIDTH(4,"参数更新定时计数(ParaUpd...)"	 ,ParaUpdateStTime);            //参数更新定时计数
		NLOGKSWIDTH(4,"方位更新(DoaUpdataTag...)	",DoaUpdataTag);            	//方位更新
		NLOGKSWIDTH(4,"方位更新定时计数(DoaUpda...)"	 ,DoaUpdataStTime);            	//方位更新定时计数
		NLOGKSWIDTH(4,"连续波方位更新计数(ContDo...)"	 ,ContDoaUpdataStTime);         //连续波方位更新定时计数
		NLOGKSWIDTH(4,"判断消失(DisapJudgeTa...)	",DisapJudgeTag);            	//判断消失
		NLOGKSWIDTH(4,"超时判断(PortTimeBegi...)	",PortTimeBegin);				//超时判断
		NLOGKSSECEND("PdwDiscern Status");
	}
	void 	ShowLastTimeOfResult() const
	{
		NLOGKSWIDTH(5,"当前定时器(s)",TimerCount.Seconds);
		for(int i = 0; i < ResultNums; i++)
		{
			NLOGKSWIDTH(5,"No",i,"结果表(s)",LastTimeOfResult[i]);
		}
	}
	void	ShowTimeBegin() const
	{
		NLOGKWIDTH(5,TimeBegin);
	}
	void 	ShowHistogram() const
	{
		MutiHistogram.AddrVarPrint();
		MutiHistogram.HistPrint();
		MutiHistogram.BarKeyDictPrint();
		MutiHistogram.AllOrtersBarKeyDictPrint();
	}
	virtual void	ShowResult(size_t ResultNo) const
	{
		int i = ResultNo;
		NLOGKSWIDTH		(5,"|·······> 		结果表表头 		<·······| 序号： ",i);
		NLOGKSBYHEXWIDTH(5,"|·····HEX 命令(Cmd)				"		,UINT(Result[i].ResultData.Cmd				));
		NLOGKSWIDTH		(5,"|········ 标号(ResultNo)			"		,UINT(Result[i].ResultNo					));
		NLOGKSWIDTH		(5,"|········ 通道(ChanNo)			"		,UINT(Result[i].ChanNo						));
		NLOGKSWIDTH		(5,"|········ 类型(Type)				"		,UINT(Result[i].ResultData.Type				));
		NLOGKSWIDTH		(5,"|········ 载频(Rfl)				"		,UINT((BandNo == 1)?((Result[i].ResultData.Rfl & 0x0FFF)*3 + 6144):(Result[i].ResultData.Rfl & 0x0FFF)*2));
		NLOGKSWIDTH		(5,"|········ 载频范围(Rfh)			"			,(Result[i].ResultData.Rfh == 0)?0:(UINT((BandNo == 1)?((Result[i].ResultData.Rfh & 0x0FFF)*3 + 6144):(Result[i].ResultData.Rfh & 0x0FFF)*2)));
		NLOGKSWIDTH		(5,"|········ 重频(Pri)				"		,UINT((Result[i].ResultData.Pri)			));
		if((Result[i].ResultData.Type == 2) || (Result[i].ResultData.Type == 5))
		{
			NLOGKSWIDTH	(5,"|········ 抖动范围(DitherRnge)		"		,UINT((Result[i].ResultData.DitherRnge)		));
		}
		if((Result[i].ResultData.Type == 3) || (Result[i].ResultData.Type == 7))
		{
			NLOGKSWIDTH	(5,"|········ 参差数(JaggNums)			"		,UINT(Result[i].ResultData.JaggNums			));
			NLOGKSWIDTH	(5,"|········ 帧周期(FramePeriods)		"		,UINT((Result[i].ResultData.FramePeriods)	));
			NLOGKSWIDTH	(5,"|········ 相周期①(PhasePeriods[0])		"	,UINT((Result[i].ResultData.PhasePeriods[0])));
			NLOGKSWIDTH	(5,"|········ 相周期②(PhasePeriods[1])		"	,UINT((Result[i].ResultData.PhasePeriods[1]))); 	
			NLOGKSWIDTH	(5,"|········ 相周期③(PhasePeriods[2])		"	,UINT((Result[i].ResultData.PhasePeriods[2]))); 	
			NLOGKSWIDTH	(5,"|········ 相周期④(PhasePeriods[3])		"	,UINT((Result[i].ResultData.PhasePeriods[3]))); 	
			NLOGKSWIDTH	(5,"|········ 相周期⑤(PhasePeriods[4])		"	,UINT((Result[i].ResultData.PhasePeriods[4]))); 	
			NLOGKSWIDTH	(5,"|········ 相周期⑥(PhasePeriods[5])		"	,UINT((Result[i].ResultData.PhasePeriods[5]))); 	
			NLOGKSWIDTH	(5,"|········ 相周期⑦(PhasePeriods[6])		"	,UINT((Result[i].ResultData.PhasePeriods[6]))); 	
			NLOGKSWIDTH	(5,"|········ 相周期⑧(PhasePeriods[7])		"	,UINT((Result[i].ResultData.PhasePeriods[7]))); 	
		}
		NLOGKSWIDTH		(5,"|········ 脉宽(Pw)				"		,UINT((Result[i].ResultData.Pw * 25)		));
		NLOGKSWIDTH		(5,"|········ 脉幅(Pa)				"		,UINT(Result[i].ResultData.Pa&0x3F			));
		NLOGKSWIDTH		(5,"|········ 方向(Doa)				"		,UINT(Result[i].ResultData.Doa				));
		NLOGKSWIDTH		(5,"|········ 俯仰(Aoa)				"		,UINT(Result[i].ResultData.Aoa				));
		NLOGKSWIDTH		(5,"|········ 发现时间(dtime)			"		,UINT((Result[i].ResultData.dtime) 			));
		NLOGKSWIDTH		(5,"|········ 扫描类型(ScanType)			"	,UINT(Result[i].ScanType					));
		NLOGKSWIDTH		(5,"|········ 天线转速(AntenaRpm)		"		,UINT(Result[i].AntenaRpm					));
		NLOGKSWIDTH		(5,"|········ 扫描周期(ScanPeriod)		"		,UINT(Result[i].ScanPeriod					));
		NLOGKSWIDTH		(5,"|········ 照射时间(IrradiateTime)		"	,UINT(Result[i].IrradiateTime				));
		NLOGKSWIDTH		(5,"|········ 扫描计数(ScanCounts)		"		,UINT(Result[i].ScanCounts					));
		NLOGKSWIDTH		(5,"|·······> 		结果表表尾  		<·······| 序号： ",i);
	}
	virtual void	ShowResult(size_t ResultNoSt, size_t ResultNoEnd) const
	{
		for(int i = 897; i < 900; i++)
		{
			ShowResult(i);
		}
		if(ResultNoSt >= ResultNums)
		{
			NLOGKS("Error(ShowResult): ResultNo ",ResultNoSt," , ResultNums",ResultNums);
			return;
		}
		if(ResultNoEnd >= ResultNums)
		{
			ResultNoEnd = ResultNums;
		}
		if((ResultNoSt == 0) && (ResultNoEnd == 0))
		{
			for(int i = 0; i < ResultNums; i++)
			{
				ShowResult(i);
			}
		}
		else if(ResultNoEnd == 0)
		{
			ShowResult(ResultNoSt);
		}
		else
		{
			for(int i = ResultNoSt; i < ResultNoEnd; i++)
			{
				ShowResult(i);
			}
		}
	}
	void 	ShowAnalyChannel() const
	{
		if(AnalyPulseNums != 0)
		{
			NLOGKSBYHEXWIDTH(4,"|···HEX AnalyChannel Begin>   AnalyPulseNums",AnalyPulseNums);
			for(int i = 0; i < AnalyPulseNums; i++)
			{
				NLOGKSBYHEXWIDTH(4,"No",i,
								   "Rf",AnalyChannel.Rf[i],
								   "Pw",AnalyChannel.Pw[i],
								   "Pa",AnalyChannel.Pa[i],
								   "Toa",AnalyChannel.Toa[i],
								   "Ton",AnalyChannel.Ton[i]
								 );
			}
			NLOGKSBYHEXWIDTH(4,"|···HEX AnalyChannel End  >   AnalyPulseNums",AnalyPulseNums);
		}
	}
	void 	ShowMsg() const
	{
		NLOGKSSECBEG("ShowMsg");
		int OutDataTmp;
		UCHAR *OutData = (UCHAR *)(&Msg);
		for(int i = 0; i < sizeof(_Msg); i++)
		{
			OutDataTmp = OutData[i];
			NLOGKBYHEXWIDTH(4,i,OutDataTmp);
		}
		NLOGKSSECEND("ShowMsg");
	}
	void 	ShowDoaUpdataMsg() const
	{
		NLOGKSSECBEG("ShowDoaUpdataMsg");
		int OutDataTmp;
		UCHAR *OutData = (UCHAR *)(&DoaUpdataMsg);
		for(int i = 0; i < DoaUpdataMsg.CharLens; i++)
		{
			OutDataTmp = OutData[i];
			NLOGKBYHEXWIDTH(4,i,OutDataTmp);
		}
		NLOGKSSECEND("ShowDoaUpdataMsg");
	}
};

#endif /* PDWDISCERN_H_ */
