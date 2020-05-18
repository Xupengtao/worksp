/*
 * PreProcess.hpp
 *
 *  Created on: Jun 27, 2019
 *      Author: xupengtao
 */

#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include<iostream>
#include<iomanip>
using namespace std;
#include"Struct.h"
#include".\Tools\Macro.h"
#include"RecvIter.hpp"
#include"DualRam.hpp"

/*-------------------------------------- DEBUG Sec --------------------------------------*/
#define DEBUG_PREPROCESS						0

#if DEBUG_PREPROCESS
#define DEBUG_PREPROCESS_COUT_FUNCSTACK 		1					//函数调用堆栈
#define DEBUG_PREPROCESS_COUT_STATUS	 		1					//各状态标志
#define DEBUG_PREPROCESS_COUT_TIMER		 		0					//定时器标志
#define DEBUG_PREPROCESS_COUT_RTNCATCH			0					//监视单次循环返回值
#endif
/*-------------------------------------- DEBUG Sec --------------------------------------*/

template<typename _RecvIterType,typename _DualRamType,
		 size_t MainChannelNum,														//预分选主通道的通道数
		 size_t ParaNum,															//每个PDW数据通道的最大脉冲数
		 size_t	ThresholdChgeTimeOut,												//通道可分选阈值修改TimeOut
		 size_t	AnalayLeastThreshold,												//最小可分选脉冲数
		 size_t DualRamNormalStart,													//Ram缓冲区标准有效通道起始值
		 size_t ContinuousPulseChannel,												//Ram缓冲区连续波通道号
		 typename TagTy>
class _PreProcess
{
public:
	enum _BitSetMode
	{
		AddTag  = 0x01,
		Reset 	= 0x02
	};
	enum _BitTag
	{
		HavePulse 	  = 0x01,
		HaveChannelNo = 0x02,
		Have10MsDiff  = 0x04
	};
	enum _rtnSign
	{
		NewPulse 		 	= 0x01,
		In10MsDiff		 	= 0x02,
		Out10MsDiff		 	= 0x03,
		SubCellAllCheck  	= 0x04,
		WritePulse		 	= 0x06,
		PulseInUDChannel 	= 0x07,
		CreatChannel	 	= 0x08,
		ContinuousPulse	 	= 0x09,
		NotContinuousPulse 	= 0x0a
	};	
	enum CheckRtn
	{
		RfAndPwErr 	= 0x6000,
		RfErr		= 0x4000,
		PwErr		= 0x5000,
		RfAndPwCrct	= 0x2000,
		Pa0Err		= 0x0400,
		PalErr		= 0x0500,		// H
		ParErr		= 0x0600,
		PaCrct		= 0x0200,
		HavePulseH	= 0x0082,		// H
		HavePulseL	= 0x0062,		// L
		NoPulseH	= 0x1181,		// H
		NoPulseL	= 0x1161		// L
	};
	enum
	{
		NotValidChannelNo 	= -1		
	};
	struct _LogType
	{
		int SubChannel[4];
	};
	typedef typename _RecvIterType::RecPDWType 		_RecPdwType;
	typedef typename _DualRamType::_PDWType 		_PdwType;
	typedef typename _DualRamType::RfTy 			RfTy;
	typedef typename _DualRamType::PwTy 			PwTy;
	typedef typename _DualRamType::PaTy 			PaTy;
	typedef typename _DualRamType::ToaTy 			ToaTy;
	typedef typename _DualRamType::TonTy 			TonTy;
	typedef typename _DualRamType::PDWChannelType 	_PdwChannelType;
	typedef _MainChannel<TagTy,RfTy,PwTy,TonTy>		_MainChType;
private:
	_RecPdwType 	RecPdw;
	_MainChType 	*MainChannel;
	_RecvIterType	&RecPDWIter;
	_DualRamType	&DualRam;
	_TimerCount		&TimerCount;									//毫秒  -> 秒定时器
	size_t			&TimeBegin;										//一次分选起始时间值，由预处理写值，预处理和主处理读值
	UINT			BandNo;											//波段号
	RfTy	 		RfLeftInterval;									//Rf最小值
	RfTy 			RfRightInterval;								//Rf最大值, BandL: , BandH:0x01C4
	RfTy			RfTorlerance;
	size_t 			NewChannelNo;
	_LogType		*RfChannelLog;									//Rf索引通道号
	TagTy  			ChannelTag;
	size_t 			MainChannelAddr;
	size_t 			SubChannelNo;
	bool			NotValidPulseTag;								//无效脉冲标志
public:
	_PreProcess()
				:BandNo(0),
				 RfLeftInterval(0),
				 RfRightInterval(0),
				 RfTorlerance(0)
	{
		COUTFILE;
		COUTLINE;
		COUTSTREND(":  Using class _PreProcess error!");
		MainChannel = new _MainChType[MainChannelNum];
		RfChannelLog = new _LogType[MainChannelNum];
		Clear();
	}
	_PreProcess(_RecvIterType &RecPDWIter_, _DualRamType &DualRam_, _TimerCount &TimerCount_, size_t &TimeBegin_)
				:RecPDWIter(RecPDWIter_),
				 DualRam(DualRam_),
				 TimerCount(TimerCount_),
				 TimeBegin(TimeBegin_),
				 BandNo(0),
				 RfLeftInterval(0),
				 RfRightInterval(0),
				 RfTorlerance(0)
	{
		MainChannel = new _MainChType[MainChannelNum];
		RfChannelLog = new _LogType[MainChannelNum];
		Clear();
	}
	virtual ~_PreProcess()
	{
		delete[] MainChannel;
		delete[] RfChannelLog;
	}
	void 	RstBand(int BandNo_)
	{
		BandNo = BandNo_;
		if(BandNo_ == 0)
		{
			RfLeftInterval 	= 0x03E7;
			RfRightInterval = 0x0EB0;
			RfTorlerance 	= 6;
		}
		else
		{
			RfLeftInterval 	= 0x11C4;
			RfRightInterval = 0x1F70;
			RfTorlerance 	= 4;
		}
	}
	void	Clear()
	{
		NewChannelNo 		= DualRamNormalStart*0x100;
		ChannelTag 			= 0;
		MainChannelAddr 	= 0;
		SubChannelNo 		= 0;
		NotValidPulseTag 	= false;
		memset(MainChannel,0,sizeof(_MainChType)*MainChannelNum);
		memset(RfChannelLog,0,sizeof(_LogType)*MainChannelNum);
		for(int i = 0; i < MainChannelNum; i++)
		{
			for(int j = 0; j < 4; j++)
			{
				MainChannel[i].SubChannel[j].cH = NotValidChannelNo;
			}
		}
	}
	void	ChanClearJudge()
	{
		if(DualRam.GetClearTag() == true)
		{
			if(DualRam[ContinuousPulseChannel].ContinuousTag == 1)
			{
				if(DualRam[ContinuousPulseChannel].DiscernReadable == false)
				{
					DualRam[ContinuousPulseChannel].ContinuousTag = 0;
					DualRam.ClearChannel(ContinuousPulseChannel);
				}
			}
			for(int i = 0; i < NewChannelNo/0x100; i++)
			{
				if(DualRam[i].ScanReadable == true)
				{
					DualRam.ClearChannel(i);
				}
				else
				{
					if(DualRam[i].DiscernReadable == true)
					{
						DualRam.ContinueWrite(i);
					}
					DualRam.ResetThreshold(i);				//无论该通道是否可分选，此时的阈值都重设
				}
			}
			TimeBegin = TimerCount.Ms;
			DualRam.SetClearTag(false);
		}
	}
	int 	GetPdw()
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("GetPdw");
#endif
		NotValidPulseTag = false;
		while((((*RecPDWIter).Rf & 0x1FFF) < RfLeftInterval)||(((*RecPDWIter).Rf & 0x1FFF) > RfRightInterval))
		{
			++RecPDWIter;
			if(TimerCount.Ms - TimeBegin > ThresholdChgeTimeOut)
			{
				NotValidPulseTag = true;
#if DEBUG_PREPROCESS_COUT_STATUS
				COUTMSGSTR("ChgeTimeOut!");
#endif
				return 0;
			}
		}
		RecPdw.Rf  = (*RecPDWIter).Rf;
		RecPdw.Pw  = (*RecPDWIter).Pw;
		RecPdw.Pa  = (*RecPDWIter).Pa;
		RecPdw.Toa = (*RecPDWIter).Toa;
		RecPdw.Ton = (*RecPDWIter).Ton;
		NotValidPulseTag = (RecPdw.Rf == 0)?true:false;
		++RecPDWIter;
#if DEBUG_PREPROCESS_COUT_STATUS
		COUTMSGSTREND("Get New PDW!");
		COUTSBYHEXWIDTH(4,"RF",RecPdw.Rf,"Pw",RecPdw.Pw,"Pa",RecPdw.Pa,"Toa",RecPdw.Toa,"Ton",RecPdw.Ton);
		COUTSBYHEXWIDTH(4,"Txn",(RecPdw.Pa)>>13,"PaLorR",((RecPdw.Pa)&0x1FFF)>>6,"PaO",(RecPdw.Pa)&0x3F);
#endif
#if DEBUG_PREPROCESS_COUT_TIMER
		NLOGK(TimerCount.Ms);
#endif
		return 1;
	}
	void 	SetMainChannelAddr()
	{
		MainChannelAddr = (RecPdw.Rf & 0x1FFF)>>4;
		SubChannelNo = 0;
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("SetMainChannelAddr");
#endif
#if DEBUG_PREPROCESS_COUT_STATUS
		COUTMSGVAL(MainChannelAddr);
#endif
	}
	void 	GetChannelTag()
	{
		ChannelTag = MainChannel[MainChannelAddr].SubChannel[SubChannelNo].Tag;
	}
	void 	SetChannelTag(_BitTag BitTag,_BitSetMode BitSetMode = AddTag)
	{
		if(BitSetMode == AddTag)
		{
			ChannelTag = ChannelTag|BitTag;
		}
		else if(BitSetMode == Reset)
		{
			ChannelTag = ChannelTag&BitTag;
		}
		MainChannel[MainChannelAddr].SubChannel[SubChannelNo].Tag = ChannelTag;
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("SetChannelTag");
#endif
#if DEBUG_PREPROCESS_COUT_STATUS
		COUTMSGVAL(BitSetMode);
#endif
	}
	void 	SaveChannelPDW()
	{
		MainChannel[MainChannelAddr].SubChannel[SubChannelNo].Rf  = RecPdw.Rf&0x1FFF;
		MainChannel[MainChannelAddr].SubChannel[SubChannelNo].Pw  = RecPdw.Pw&0x0FFF;
		MainChannel[MainChannelAddr].SubChannel[SubChannelNo].Ton = RecPdw.Ton&0xFFFF;
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("SaveChannelPDW");
#endif
	}
	bool 	CheckTag(_BitTag BitTag)
	{
		return (ChannelTag&BitTag) != 0 ? true : false;
	}
	void 	CreateNewChannel()
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("CreateNewChannel");
#endif
#if DEBUG_PREPROCESS_COUT_STATUS
		COUTMSGVAL(NewChannelNo);
#endif
		MainChannel[MainChannelAddr].SubChannel[SubChannelNo].cH = NewChannelNo;
		RfChannelLog[MainChannelAddr].SubChannel[SubChannelNo] = NewChannelNo;
		WritePDWToCircleQueue(NewChannelNo);
		NewChannelNo = NewChannelNo+0x100;
		SetChannelTag(HaveChannelNo);
		DualRam.SetValidChannelNum(NewChannelNo/0x100-DualRamNormalStart);
	}
	bool 	PDWFitChannel()
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("PDWFitChannel");
#endif
#if DEBUG_PREPROCESS_COUT_STATUS
		COUTMSGVAL(MainChannelAddr);
		COUTMSGVAL(SubChannelNo);
#endif
		GetChannelTag();
		if(CheckTag(HavePulse) == true)
		{
			if(RfPwTlrcJudge(0x04) == true)
			{
				if(CheckTag(HaveChannelNo) == true)											//SCHNJP
				{
#if DEBUG_PREPROCESS_COUT_STATUS
					COUTMSGSTR("HavePulse && HaveChannelNo!");
#endif
					int cH = MainChannel[MainChannelAddr].SubChannel[SubChannelNo].cH;
					cH = cH|0xAA;
					MainChannel[MainChannelAddr].SubChannel[SubChannelNo].cH = cH;
					RfChannelLog[MainChannelAddr].SubChannel[SubChannelNo] = cH;
					WritePDWToCircleQueue(cH);
					SetChannelTag(HaveChannelNo);
					return true;
				}
				else
				{
#if DEBUG_PREPROCESS_COUT_STATUS
					COUTMSGSTR("HavePulse && Not HaveChannelNo!");
#endif
					SubChannelNo++;
					if(SubChannelNo < 0x04)
					{
						return PDWFitChannel();
					}
					else
					{
						return false;
					}
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	bool 	PDWFitUpDownChannel()
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("PDWFitUpDownChannel");
#endif
		size_t MainChannelAddrTmp = MainChannelAddr;
		size_t SubChannelNoTmp = SubChannelNo;
		if(MainChannelAddr != MainChannelNum)												//CUPN
		{
			SubChannelNo = 0;
			MainChannelAddr = MainChannelAddrTmp + 1;
			if(PDWFitChannel() == true)
			{
				int cH = MainChannel[MainChannelAddr].SubChannel[SubChannelNo].cH;
				MainChannelAddr = MainChannelAddrTmp;
				SubChannelNo = SubChannelNoTmp;
				GetChannelTag();
				SetChannelTag(HaveChannelNo);
				MainChannel[MainChannelAddrTmp].SubChannel[SubChannelNoTmp].cH = cH;
				RfChannelLog[MainChannelAddr].SubChannel[SubChannelNo] = cH;
				return true;
			}
		}
		if(MainChannelAddr != 0)															//CDOWN
		{
			SubChannelNo = 0;
			MainChannelAddr = MainChannelAddrTmp - 1;
			if(PDWFitChannel() == true)
			{
				int cH = MainChannel[MainChannelAddr].SubChannel[SubChannelNo].cH;
				MainChannelAddr = MainChannelAddrTmp;
				SubChannelNo = SubChannelNoTmp;
				GetChannelTag();
				SetChannelTag(HaveChannelNo);
				MainChannel[MainChannelAddrTmp].SubChannel[SubChannelNoTmp].cH = cH;
				RfChannelLog[MainChannelAddr].SubChannel[SubChannelNo] = cH;
				return true;
			}
		}
		MainChannelAddr = MainChannelAddrTmp;
		SubChannelNo = SubChannelNoTmp;
		GetChannelTag();
		return false;
	}
	int 	WritePDWToCircleQueue(int cH)										//当DualRam为Default时使用此函数
	{
		_PdwChannelType &PdwChannelTmp = DualRam[cH>>8];
		size_t PulseLoc = PdwChannelTmp.PulseNum;
		if(PulseLoc < ParaNum)
		{
			if(PdwChannelTmp.CrossChannelTag != 0xAA)
			{
				PdwChannelTmp.CrossChannelTag = cH&0xFF;
			}
			if((RecPdw.Rf&0x8000) == 0)														//ASE
			{
				return 0;
			}
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
			COUTFUNC("WritePDWToCircleQueue");
#endif
#if DEBUG_PREPROCESS_COUT_STATUS
			COUTMSGVAL(cH);
			COUTMSGVAL(PdwChannelTmp.CrossChannelTag);
			COUTMSGVAL(PulseLoc);
#endif
			PdwChannelTmp.PulseWord.Rf[PulseLoc]  = RecPdw.Rf;
			PdwChannelTmp.PulseWord.Pw[PulseLoc]  = RecPdw.Pw;
			PdwChannelTmp.PulseWord.Pa[PulseLoc]  = RecPdw.Pa;
			PdwChannelTmp.PulseWord.Toa[PulseLoc] = RecPdw.Toa;
			PdwChannelTmp.PulseWord.Ton[PulseLoc] = RecPdw.Ton;
			PdwChannelTmp.PulseNum = ++PulseLoc;											//GPDW
			if(PdwChannelTmp.DiscernThreshold != ParaNum + 0x100)
			{
				if(PulseLoc - PdwChannelTmp.DiscernStLoc >= PdwChannelTmp.DiscernThreshold)
				{
					PdwChannelTmp.DiscernPulseNums = PdwChannelTmp.DiscernThreshold;
					PdwChannelTmp.DiscernReadable = true;
					PdwChannelTmp.DiscernThreshold = ParaNum + 0x100;
				}
				else
				{
					if(TimerCount.Ms - TimeBegin > ThresholdChgeTimeOut)
					{
						if(PulseLoc - PdwChannelTmp.DiscernStLoc >= AnalayLeastThreshold)
						{
							PdwChannelTmp.DiscernThreshold = AnalayLeastThreshold;
							PdwChannelTmp.DiscernPulseNums = PdwChannelTmp.DiscernThreshold;
							PdwChannelTmp.DiscernReadable = true;
							PdwChannelTmp.DiscernThreshold = ParaNum + 0x100;
						}
					}
				}
			}
			if(PulseLoc >= PdwChannelTmp.ScanThreshold)
			{
				PdwChannelTmp.ScanPulseNums = PdwChannelTmp.ScanThreshold;
				PdwChannelTmp.ScanReadable = true;
				PdwChannelTmp.ScanThreshold = ParaNum + 0x100;
			}
			return 1;
		}
		else
		{
			static size_t NoUsedPulse = 0;
			NoUsedPulse++;
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
			COUTFUNC("WritePDWToCircleQueue");
#endif
#if DEBUG_PREPROCESS_COUT_STATUS
			COUTMSGVAL(NoUsedPulse);
#endif
			return 0;
		}
	}
	bool 	PwRfTlrcJudge(RfTy RfTlrc,PwTy PwTlrc = 0x05,int ChannelAddr = -1)
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("PwRfTlrcJudge");
#endif
		RfTy OldRf,NewRf;
		PwTy OldPw,NewPw;
		if(ChannelAddr == -1)
		{
			ChannelAddr = MainChannelAddr;
		}
		OldPw = MainChannel[ChannelAddr].SubChannel[SubChannelNo].Pw;
		NewPw = RecPdw.Pw&0x0FFF;
		if(((NewPw > 0x28) && (abs(NewPw-OldPw) < (OldPw>>3)))
		 ||((NewPw <= 0x28) && (abs(NewPw-OldPw) < PwTlrc)))
		{
#if DEBUG_PREPROCESS_COUT_STATUS
			COUTMSGSTR("true!");
#endif
			return true;
		}
		else
		{
			OldRf = MainChannel[ChannelAddr].SubChannel[SubChannelNo].Rf;
			NewRf = RecPdw.Rf&0x1FFF;
			if(abs(NewRf-OldRf) < RfTlrc)
			{
#if DEBUG_PREPROCESS_COUT_STATUS
				COUTMSGSTR("true!");
#endif
				return true;
			}
			else
			{
#if DEBUG_PREPROCESS_COUT_STATUS
				COUTMSGSTR("false!");
#endif
				return false;
			}
		}
	}
	bool 	RfPwTlrcJudge(RfTy RfTlrc,PwTy PwTlrc = 0x05,int ChannelAddr = -1)
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("RfPwTlrcJudge");
#endif
		RfTy OldRf,NewRf;
		PwTy OldPw,NewPw;
		if(ChannelAddr == -1)
		{
			ChannelAddr = MainChannelAddr;
		}
		OldRf = MainChannel[ChannelAddr].SubChannel[SubChannelNo].Rf;
		NewRf = RecPdw.Rf&0x1FFF;
		if(abs(NewRf-OldRf) < RfTlrc)									//JRFN
		{
#if DEBUG_PREPROCESS_COUT_STATUS
			COUTMSGSTR("true!");
#endif
			return true;
		}
		else
		{
			OldPw = MainChannel[ChannelAddr].SubChannel[SubChannelNo].Pw;
			NewPw = RecPdw.Pw&0x0FFF;
			if(((NewPw > 0x28) && (abs(NewPw-OldPw) < (OldPw>>3)))		//PW>=1us,ΔPW >= oldPW*12.5%
			 ||((NewPw <= 0x28) && (abs(NewPw-OldPw) < PwTlrc)))  		//PW<1us JPWLN,ΔPW >= 125ns
			{
#if DEBUG_PREPROCESS_COUT_STATUS
				COUTMSGSTR("true!");
#endif
				return true;
			}
			else
			{
#if DEBUG_PREPROCESS_COUT_STATUS
				COUTMSGSTR("false!");
#endif
				return false;
			}
		}
	}
	_rtnSign JudgeContinuousPulse()
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("JudgeContinuousPulse");
#endif
		if((RecPdw.Rf&0x4000) == 0x4000)
		{
			DualRam[ContinuousPulseChannel].ContinuousTag = 1;
			WritePDWToCircleQueue(ContinuousPulseChannel);
			return ContinuousPulse;
		}
		else
		{
			return NotContinuousPulse;
		}
	}
	_rtnSign PDWEncase()
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("PDWEncase");
#endif
#if DEBUG_PREPROCESS_COUT_STATUS
		COUTMSGVAL(SubChannelNo);
#endif
		GetChannelTag();
		if(CheckTag(HavePulse) == true)											//GCHF
		{
			if(CheckTag(HaveChannelNo) == true)									//CHFP
			{
#if DEBUG_PREPROCESS_COUT_STATUS
				COUTMSGSTR("HavePulse && HaveChannelNo");
#endif
				if(PwRfTlrcJudge(RfTorlerance) == true)							//SCHN
				{
					WritePDWToCircleQueue(MainChannel[MainChannelAddr].SubChannel[SubChannelNo].cH);
					return WritePulse;
				}
				else															//TSUB
				{
					SubChannelNo++;
					if(SubChannelNo >= 0x04)
					{
						SubChannelNo = 0;
						return SubCellAllCheck;
					}
					else
					{
						return PDWEncase();
					}
				}
			}
			else																//NCHF
			{
#if DEBUG_PREPROCESS_COUT_STATUS
				COUTMSGSTR("HavePulse && Not HaveChannelNo");
#endif
				if(PwRfTlrcJudge(RfTorlerance) == true)							//JTON
				{
					TonTy OldTon,NewTon;
					OldTon = MainChannel[MainChannelAddr].SubChannel[SubChannelNo].Ton;
					NewTon = RecPdw.Ton&0xFFFF;
					if(NewTon-OldTon >= 10)										//WTON1
					{
#if DEBUG_PREPROCESS_COUT_STATUS
						COUTMSGSTR("NewTon-OldTon >= 10");
#endif
						SetChannelTag(HavePulse,Reset);
						SaveChannelPDW();
						return Out10MsDiff;
					}
					else
					{
#if DEBUG_PREPROCESS_COUT_STATUS
						COUTMSGSTR("NewTon-OldTon < 10");
#endif
						if(CheckTag(Have10MsDiff) == true)
						{
#if DEBUG_PREPROCESS_COUT_STATUS
							COUTMSGSTR("HavePulse && Not HaveChannelNo && Have10MsDiff");
#endif
							if(PDWFitUpDownChannel() == true)					//CUPN
							{
								return PulseInUDChannel;
							}
							else												//SCHNEW
							{
								CreateNewChannel();
								return CreatChannel;
							}
						}
						else
						{
#if DEBUG_PREPROCESS_COUT_STATUS
							COUTMSGSTR("HavePulse && Not HaveChannelNo && Not Have10MsDiff");
#endif
							SetChannelTag(Have10MsDiff);
							SaveChannelPDW();									//WTON2
							return In10MsDiff;
						}
					}
				}
				else															//TSUB
				{
					SubChannelNo++;
					if(SubChannelNo >= 0x04)
					{
						SubChannelNo = 0;
						return SubCellAllCheck;
					}
					else
					{
						return PDWEncase();
					}
				}
			}
		}
		else																	//SPDW
		{
#if DEBUG_PREPROCESS_COUT_STATUS
			COUTMSGSTR("Not HavePulse");
#endif
			SaveChannelPDW();
			SetChannelTag(HavePulse);
			return NewPulse;
		}
	}
	int 	Process(size_t& CtrlSign, size_t& TaskDeleteSign)
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC("Process");
		COUTHEX;
		COUTUPPER;
#endif
#if DEBUG_PREPROCESS_COUT_RTNCATCH
		_rtnSign rtnSign;
#endif
		RecPDWIter.IsReady();
		while(CtrlSign)
		{
			ChanClearJudge();
			GetPdw();
			if(NotValidPulseTag == true)
			{
				continue;
			}
			SetMainChannelAddr();
#if !DEBUG_PREPROCESS_COUT_RTNCATCH
			if(JudgeContinuousPulse() != ContinuousPulse)
			{
				PDWEncase();
			}
#else
			rtnSign = JudgeContinuousPulse();
			if(rtnSign != ContinuousPulse)
			{
				rtnSign = PDWEncase();
			}
			cout<<endl<<"|********************************************************|"<<endl;
			cout<<"@rtnSign: "<<endl;
			switch(rtnSign)
			{
			case NewPulse:
				cout<<"          NewPulse... "<<endl;
				break;
			case In10MsDiff:
				cout<<"          In10MsDiff... "<<endl;
				break;
			case Out10MsDiff:
				cout<<"          Out10MsDiff... "<<endl;
				break;
			case SubCellAllCheck:
				cout<<"          SubCellAllCheck... "<<endl;
				break;
			case WritePulse:
				cout<<"          WritePulse... "<<endl;
				break;
			case PulseInUDChannel:
				cout<<"          PulseInUDChannel... "<<endl;
				break;
			case CreatChannel:
				cout<<"          CreatChannel... "<<endl;
				break;
			default:
				break;
			}
#endif
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
			COUTEND;
#endif
		}
		TaskDeleteSign = 1;
		return 1;
	}

	int 	PulseTlrcCheck()
	{
		int ChkRtn = 0;
		int PulseNum = 0, ChannelNo = 0;
		int RfTlrc = 0, PwTlrc = 0, PaoTlrc = 0, PalTlrc = 0, ParTlrc = 0;
		int RfRef = 0, PwRef = 0, PaoRef = 0, PalRef = 0, ParRef = 0;
		int RfErrCounts = 0, PwErrCounts = 0, PaoErrCounts = 0, PalErrCounts = 0, ParErrCounts = 0;
		ChkRtn 	= (BandNo == 0)?HavePulseL:HavePulseH;
		RfRef 	= (BandNo == 0)?0x0EA9:0x11C7;
		RfTlrc 	= (BandNo == 0)?0x0A:0x0A;
		PwRef 	= (BandNo == 0)?0x28:0x27;
		PwTlrc 	= (BandNo == 0)?0x0A:0x0A;
		PaoRef 	= (BandNo == 0)?0x0D:0x0b;
		PaoTlrc = (BandNo == 0)?0x0A:0x0A;
		PalRef 	= (BandNo == 0)?0:0x1f;
		PalTlrc = (BandNo == 0)?0:0x0A;
		ParRef 	= (BandNo == 0)?0x30:0x21;
		ParTlrc = (BandNo == 0)?0x0A:0x0A;
		for(int i = 0; i < 3; i++)
		{
			for(int SubChNo = 0; SubChNo < 4; SubChNo ++)
			{
				ChannelNo = (BandNo == 0)?(MainChannel[0x00E9 + i].SubChannel[SubChNo].cH>>8)
										 :(MainChannel[0x011B + i].SubChannel[SubChNo].cH>>8);
				if(ChannelNo != NotValidChannelNo)
				{
					PulseNum = DualRam[ChannelNo].PulseNum;
					for(int i = 0; i < PulseNum; i++)
					{
						if(abs((DualRam[ChannelNo].PulseWord.Rf[i]&0x1FFF) - RfRef) > RfTlrc)
						{
							RfErrCounts++;
						}
						if(abs((DualRam[ChannelNo].PulseWord.Pw[i]&0x0FFF) - PwRef) > PwTlrc)
						{
							PwErrCounts++;
						}
						if(abs((DualRam[ChannelNo].PulseWord.Pa[i]&0x3F) - PaoRef) > PaoTlrc)
						{
							PaoErrCounts++;
						}
						if(BandNo == 0)
						{
							if(abs(((DualRam[ChannelNo].PulseWord.Pa[i]&0x0FC0)>>6) - ParRef) > ParTlrc)
							{
								ParErrCounts++;
							}
						}
						else
						{
							if(((DualRam[ChannelNo].PulseWord.Pa[i])&0xF000) > 0x3000)	//L
							{
								if(abs(((DualRam[ChannelNo].PulseWord.Pa[i]&0x0FC0)>>6) - PalRef) > PalTlrc)
								{
									PalErrCounts++;
								}
							}
							else														//R
							{
								if(abs(((DualRam[ChannelNo].PulseWord.Pa[i]&0x0FC0)>>6) - ParRef) > ParTlrc)
								{
									ParErrCounts++;
								}
							}
						}
					}
				}
			}
		}
		if((RfErrCounts < 10) && (PwErrCounts < 10))
		{
			ChkRtn = ChkRtn|RfAndPwCrct;
		}
		else if((RfErrCounts >= 10) && (PwErrCounts < 10))
		{
			ChkRtn = ChkRtn|RfErr;
		}
		else if((RfErrCounts < 10) && (PwErrCounts >= 10))
		{
			ChkRtn = ChkRtn|PwErr;
		}
		else
		{
			ChkRtn = ChkRtn|RfAndPwCrct;
		}
		if(PaoErrCounts >= 10)
		{
			ChkRtn = ChkRtn|Pa0Err;
		}
		else if(PalErrCounts >= 10)
		{
			ChkRtn = ChkRtn|PalErr;
		}
		else if(ParErrCounts >= 10)
		{
			ChkRtn = ChkRtn|ParErr;
		}
		else
		{
			ChkRtn = ChkRtn|PaCrct;
		}
		return ChkRtn;
	}
	int		SelfCheck(size_t& CtrlSign)
	{
#if DEBUG_PREPROCESS_COUT_FUNCSTACK
		COUTFUNC(SelfCheck);
#endif
		int  Rtn = 0;
		UINT Sum = 0;
		int  ChannelNo = 0;
		RecPDWIter.Reset(1,1);
		RecPDWIter.IsReady();
		TimeBegin = TimerCount.Ms;
		while(1)
		{
			GetPdw();
			RecPdw.Rf = RecPdw.Rf|0x8000;
			if(NotValidPulseTag == true)
			{
				Rtn = (BandNo == 0)?NoPulseL:NoPulseH;
				break;
			}
			SetMainChannelAddr();
			if(JudgeContinuousPulse() != ContinuousPulse)
			{
				PDWEncase();
			}
			Sum = 0;
			for(int i = 0; i < 3; i++)
			{
				for(int SubChNo = 0; SubChNo < 4; SubChNo ++)
				{
					if(BandNo == 0)
					{
						ChannelNo = MainChannel[0x00E9 + i].SubChannel[SubChNo].cH;
					}
					else
					{
						ChannelNo = MainChannel[0x011C + i].SubChannel[SubChNo].cH;
					}
					if(ChannelNo != NotValidChannelNo)
					{
						Sum = Sum + DualRam[ChannelNo>>8].PulseNum;
					}
				}
			}
			if(Sum >= 100)
			{
				break;
			}
			if(TimerCount.Ms - TimeBegin > 1000)
			{
				break;
			}
		}
		if((Rtn != NoPulseL) && (Rtn != NoPulseH))
		{
			if(Sum >= 100)
			{
				Rtn = PulseTlrcCheck();
			}
			else
			{
				Rtn = (BandNo == 0)?NoPulseL:NoPulseH;
			}
		}
		RecPDWIter.Reset();
		CtrlSign = Rtn;
		return Rtn;
	}
	
	void	ShowWhichChannel(int SearchRf) const
	{
		RfTy SearchChan;
		if(BandNo == 0)
		{
			SearchChan = ((SearchRf/2) & 0x0FFF)>>4;
		}
		else
		{
			SearchChan = ((((SearchRf - 6144)/3) | 0x1000) & 0x1FFF)>>4;
		}
		for(int SubChNo = 0; SubChNo < 4; SubChNo ++)
		{
			NLOGKSBYHEXWIDTH(4,"主通道号",SearchChan,"子通道号",SubChNo,
							 "检索到的通道号",RfChannelLog[SearchChan].SubChannel[SubChNo]>>8,
							 "捷变标志",RfChannelLog[SearchChan].SubChannel[SubChNo] & 0xFF);
		}
	}
	void	ShowStatus() const
	{
		NLOGKSSECBEG("PreProcess Status");
		NLOGKSWIDTH(4,"预分选通道数(MainChann...)	",MainChannelNum);				//预分选主通道的通道数
		NLOGKSWIDTH(4,"通道脉冲数(ParaNum)		"	 ,ParaNum);						//每个PDW数据通道的最大脉冲数
		NLOGKSWIDTH(4,"阈值重置超时(Threshold...)	",ThresholdChgeTimeOut);		//通道可分选阈值修改TimeOut
		NLOGKSWIDTH(4,"最小分选脉冲数(AnalayLe...)	",AnalayLeastThreshold);		//最小可分选脉冲数
		NLOGKSWIDTH(4,"标准通道起始(DualRamNo...)	",DualRamNormalStart);			//Ram缓冲区标准有效通道起始值
		NLOGKSWIDTH(4,"连续波通道号(Continuou...)	",ContinuousPulseChannel);		//Ram缓冲区连续波通道号
		NLOGKSWIDTH(4,"新通道号(NewChannelN...)	"	 ,NewChannelNo);
		NLOGKSWIDTH(4,"波段号(BandNo)			"	 ,BandNo);						//波段号
		NLOGKSWIDTH(4,"通道标志(ChannelTag)		"	 ,ChannelTag);
		NLOGKSWIDTH(4,"Rf左阈值(RfLeftInter...)	",RfLeftInterval);				//Rf最小值
		NLOGKSWIDTH(4,"Rf右阈值(RfRightInte...)	",RfRightInterval);				//Rf最大值, BandL: , BandH:0x01C4
		NLOGKSWIDTH(4,"Rf容差范围(RfTorleran...)	",RfTorlerance);				//Rf容差范围
		NLOGKSWIDTH(4,"主通道号(MainChannelA...)	",MainChannelAddr);
		NLOGKSWIDTH(4,"子通道号(SubChannelNo)	"	 ,SubChannelNo);
		NLOGKSWIDTH(4,"脉冲无效标志(NotValidPu...)	",NotValidPulseTag);
		NLOGKSSECEND("PreProcess Status");
	}
	void	ShowChannel(size_t ChanNo) const
	{
		_PdwChannelType &PdwChannelTmp = DualRam[ChanNo];
		size_t PulseNum = PdwChannelTmp.PulseNum;
		ToaTy PreToa = 0, Pri = 0;
		RfTy  RfTmp = 0,RfMinVal = 0,RfMaxVal = 0;
		if(PulseNum != 0)
		{
			NLOGKSBYHEXWIDTH(4,"|···HEX Channel Begin> ChannelNo",ChanNo,",  CrossChannelTag",PdwChannelTmp.CrossChannelTag,", PulseNum",PulseNum);
		}
		for(int j = 0; j < PulseNum; j++)
		{
			Pri 	= PdwChannelTmp.PulseWord.Toa[j] - PreToa;
			RfTmp 	 = PdwChannelTmp.PulseWord.Rf[j] & 0x0FFF;
			RfMinVal = (RfMinVal == 0)?RfTmp:RfMinVal;
			RfMinVal = (RfTmp < RfMinVal)?RfTmp:RfMinVal;
			RfMaxVal = (RfTmp > RfMaxVal)?RfTmp:RfMaxVal;
			NLOGKSBYHEXWIDTH(4,"Rf",PdwChannelTmp.PulseWord.Rf[j],
							   "Pri",Pri,
							   "Pw",PdwChannelTmp.PulseWord.Pw[j],
							   "Pa",PdwChannelTmp.PulseWord.Pa[j]&0x3F,
							   "Toa",PdwChannelTmp.PulseWord.Toa[j],
							   "Ton",PdwChannelTmp.PulseWord.Ton[j]
							 );
			PreToa = PdwChannelTmp.PulseWord.Toa[j];
		}
		if(PulseNum != 0)
		{
			if(BandNo == 0)
			{
				RfMinVal = RfMinVal * 2;
				RfMaxVal = RfMaxVal * 2;
			}
			else
			{
				RfMinVal = RfMinVal * 3 + 6144;
				RfMaxVal = RfMaxVal * 3 + 6144;
			}
			NLOGKSWIDTH		(5,"|···DEC Channel End>   RfMinVal ",RfMinVal,", RfMaxVal",RfMaxVal);
		}
	}
	void	ShowChannel(size_t ChanNoSt, size_t ChanNoEnd) const
	{
		size_t ChannelNum = DualRam.GetValidChannelNum();
		if(ChanNoSt >= ChannelNum + DualRamNormalStart)
		{
			NLOGKS("Error(ShowChannel): ChannelNo ",ChanNoSt," , ValidChannelNum",ChannelNum);
			return;
		}
		if(ChanNoEnd >= ChannelNum + DualRamNormalStart)
		{
			ChanNoEnd = ChannelNum + DualRamNormalStart;
		}
		if((ChanNoSt == 0) && (ChanNoEnd == 0))
		{
			NLOGKSWIDTH(5,"|*********DEC   ChannelNums",ChannelNum);
			for(int i = 0; i < ChannelNum + DualRamNormalStart; i++)
			{
				ShowChannel(i);
			}
		}
		else if(ChanNoEnd == 0)
		{
			ShowChannel(ChanNoSt);
		}
		else
		{
			for(int i = ChanNoSt; i < ChanNoEnd; i++)
			{
				ShowChannel(i);
			}
		}
	}
	void	ShowTimeBegin()
	{
		NLOGKWIDTH(5,TimeBegin);
	}
};

#endif /* PREPROCESS_H_ */
