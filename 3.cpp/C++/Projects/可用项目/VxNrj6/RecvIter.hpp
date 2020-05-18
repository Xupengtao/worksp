/*
 * RecvIter.hpp
 *
 *  Created on: Jun 27, 2019
 *      Author: xupengtao
 */

#ifndef RECVITER_H_
#define RECVITER_H_

#include<iostream>
#include<iomanip>
using namespace std;
#include"Struct.h"
#include".\iRadarSimPro\iRadarSimPro.h"
#include".\Tools\Macro.h"
#include".\Tools\Timer.h"

#define DEBUG_RECVITER 1

uint_t 	_RadarBuffer::GlobalToaH 	= 0;
uint_t 	_RadarBuffer::GlobalToaL 	= 0;
uint_t 	_RadarBuffer::RadarRCCLNum 	= 0;
uint_t	_RadarBuffer::SortTimes 	= 0;

#define PDWBUFFER_WRITETAG 	RecOutPDW_IDLE					//Writable
#define PDWBUFFER_READYTAG 	RecOutPDW_READY					//Readable

template<typename _RecPDWType>
class _RecvIter
{
public:
	typedef _RecPDWType 			RecPDWType;
	typedef volatile RecPDWType 	VtRecPDWType;
	typedef volatile unsigned short	VtSize_t;

private:
	VtRecPDWType		*RecvPtr;
	RecPDWType			RecvPtrTmp;							//超时返回
	size_t				RecIterType;						//0: 串口, 1: 单Buffer, 2: 双Buffer
	_Timer				Timer;
	_TimerCount			TimerCount;
	CONUINT				TimeOut;							//超时设置,单位ms
	size_t				GetPdwTimeBegin;					//一次读脉冲起始时间值，由预处理写值，预处理读值
	RecPDWType 			*PDWBuffer1;
	RecPDWType 			*PDWBuffer2;
	size_t				*Buffer1ReadyTag;
	size_t				*Buffer2ReadyTag;
	size_t 				*Buffer1Size;
	size_t 				*Buffer2Size;
	size_t 				Buffer1Loc;
	size_t 				Buffer2Loc;
	size_t 				RecentBuffer;
	
	VtSize_t *const		FIFOReg;
	VtSize_t *const		FIFOStatus;
	VtSize_t *const		FIFOSet;
	size_t 				FIFOCount;
	
	
public:
	_RecvIter(size_t TimerNo = 1, size_t TimerMsInterival = 100, int TimeOut_ = 100,
			  size_t *PDWBuffer1_ = NULL,size_t *Buffer1ReadyTag_ = NULL,size_t *Buffer1Size_ = NULL,
			  size_t *PDWBuffer2_ = NULL,size_t *Buffer2ReadyTag_ = NULL,size_t *Buffer2Size_ = NULL)
			 :TimeOut(TimeOut_),
			  FIFOReg((VtSize_t *)0xec000100),FIFOStatus((VtSize_t *)0xec000120),FIFOSet((VtSize_t *)0xec000140),FIFOCount(0),
			  PDWBuffer1((RecPDWType*)PDWBuffer1_),Buffer1ReadyTag(Buffer1ReadyTag_),Buffer1Size(Buffer1Size_),
			  PDWBuffer2((RecPDWType*)PDWBuffer2_),Buffer2ReadyTag(Buffer2ReadyTag_),Buffer2Size(Buffer2Size_)
	{
		RecvPtr = NULL;
		RecvPtrTmp.Rf 	= 0;
		RecvPtrTmp.Pw 	= 0;
		RecvPtrTmp.Pa 	= 0;
		RecvPtrTmp.Toa 	= 0;
		RecvPtrTmp.Ton 	= 0;
		if(PDWBuffer1 == NULL)
		{
			RecIterType = 0;
		}
		else if(PDWBuffer2 == NULL)
		{
			RecIterType = 1;
		}
		else
		{
			RecIterType = 2;
		}
		Reset(TimerNo,TimerMsInterival);
	}
	~_RecvIter(){Timer.Stop();}
	//no need copy ctor
	//no need operator=
	inline VtRecPDWType& operator*()const{return *RecvPtr;}
	inline VtRecPDWType* operator->()const{return RecvPtr;}
	inline VtRecPDWType& operator++()
	{
		bool PDWValidTag = true;
		if(RecIterType == 0)										//串口接收方式
		{
			PDWValidTag = FIFORead();
			if(PDWValidTag == true)
			{
				RecvPtr = (VtRecPDWType *)FIFOReg;
			}
		}
		else if(RecIterType == 1)									//单一BUffer
		{
			Buffer1Loc++;
			if(EndTagJudge(Buffer1Loc) == false)
			{
				RecvPtr = &(PDWBuffer1[Buffer1Loc]);
			}
			else
			{
				*Buffer1ReadyTag = PDWBUFFER_WRITETAG;
				PDWValidTag = WaitBufferReady(1);
			}
		}
		else if(RecIterType == 2)									//双Buffer切换
		{
			if(RecentBuffer == 1)
			{
				Buffer1Loc++;
				if(EndTagJudge(Buffer1Loc) == false)
				{
					RecvPtr = &(PDWBuffer1[Buffer1Loc]);
				}
				else
				{
#if DEBUG_RECVITER
					cout<<"EndTagJudge(Buffer1Loc) true!"<<"  Buffer1Loc = "<<Buffer1Loc<<endl;
#endif
					*Buffer1ReadyTag = PDWBUFFER_WRITETAG;
					PDWValidTag = WaitBufferReady(2);
				}
			}
			else
			{
				Buffer2Loc++;
				if(EndTagJudge(Buffer2Loc) == false)
				{
					RecvPtr = &(PDWBuffer2[Buffer2Loc]);
				}
				else
				{
#if DEBUG_RECVITER
					cout<<"EndTagJudge(Buffer2Loc) true!"<<"  Buffer2Loc = "<<Buffer2Loc<<endl;
#endif
					*Buffer2ReadyTag = PDWBUFFER_WRITETAG;
					PDWValidTag = WaitBufferReady(1);
				}
			}
		}
		if(PDWValidTag == false)
		{
//			COUTSTREND("Not Valid Pdw!");
			RecvPtr = &RecvPtrTmp;
		}
		return *RecvPtr;
	}
	size_t 	GetType() const {return RecIterType;}
	bool 	ReBinding(size_t *PDWBuffer1_ = NULL,size_t *Buffer1ReadyTag_ = NULL,size_t *Buffer1Size_ = NULL,
					  size_t *PDWBuffer2_ = NULL,size_t *Buffer2ReadyTag_ = NULL,size_t *Buffer2Size_ = NULL,
					  size_t TimerNo = 1, size_t TimerMsInterival = 100)
	{
		PDWBuffer1 		= (RecPDWType*)PDWBuffer1_;
		Buffer1ReadyTag = Buffer1ReadyTag_;
		Buffer1Size 	= Buffer1Size_;
		PDWBuffer2 		= (RecPDWType*)PDWBuffer2_;
		Buffer2ReadyTag = Buffer2ReadyTag_;
		Buffer2Size 	= Buffer2Size_;
		RecvPtr = NULL;
		if(PDWBuffer1 == NULL)
		{
			RecIterType = 0;
		}
		else if(PDWBuffer2 == NULL)
		{
			RecIterType = 1;
		}
		else
		{
			RecIterType = 2;
		}
		Reset(TimerNo,TimerMsInterival);
		return true;
	}
	bool 	RstRecIterType(size_t RecIterType_)
	{
		RecvPtr = NULL;
		if(PDWBuffer1 == NULL)
		{
			RecIterType = 0;
		}
		else if(PDWBuffer2 == NULL)
		{
			RecIterType = 1;
		}
		else
		{
			RecIterType = RecIterType_;
		}
		Reset();
		return true;
	}
	bool 	Reset(size_t TimerNo = 1, size_t TimerMsInterival = 100)
	{
		bool Rtn = true;
		Timer.Stop();
		GetPdwTimeBegin = 0;
		memset(&TimerCount, 0, sizeof(_TimerCount));
		Timer.SetTimerNo(TimerNo);
		Timer.SetTimerInterival(TimerMsInterival);
		Timer.SetTimerSecondsFunc((UINT)(&TimerCount));
		Timer.Start();
		Buffer1Loc 		= 0;
		Buffer2Loc 		= 0;
		RecentBuffer 	= 1;
		if(RecIterType == 0)										//串口接收方式
		{
#if _MSC_VER
#elif PPC
			*FIFOSet 	= 0x8000;
			*FIFOSet	= 0;
			RecvPtr 	= (VtRecPDWType *)FIFOReg;
#endif
			Rtn = true;
		}
		else if(RecIterType == 1)
		{
			Rtn = false;
		}
		else
		{
			RecvPtr = &(PDWBuffer1[Buffer1Loc]);
			Rtn = true;
		}
		return Rtn;
	}
	inline bool IsReady()
	{
		bool PDWValidTag = true;
		if(RecIterType == 0)										//串口接收方式
		{
#if _MSC_VER
#elif PPC
			*FIFOSet 	= 0x8000;
			*FIFOSet	= 0;
			NLOGKS("FIFO Reset!");
#endif
			PDWValidTag = FIFOWait();
			if(PDWValidTag == true)
			{
				RecvPtr = (VtRecPDWType *)FIFOReg;
			}
		}
		else if(RecIterType == 1)
		{
			PDWValidTag = false;
		}
		else
		{
			PDWValidTag = WaitBufferReady(1);
		}
		if(PDWValidTag == false)
		{
//			COUTSTREND("Not Valid Pdw!");
			RecvPtr = &RecvPtrTmp;
		}
		return PDWValidTag;
	}
	inline bool FIFOWait()
	{
		bool Rtn = true;
#if _MSC_VER
#elif PPC
		GetPdwTimeBegin = TimerCount.Ms;
		if(((*FIFOStatus)&0x4000) == 1)
		{
			NLOGKS("接收数据缓冲区已满，自动清空!");
#if _MSC_VER
#elif PPC
			*FIFOSet 	= 0x8000;
			*FIFOSet	= 0;
#endif
		}
		while(((*FIFOStatus)&0x0FFF) <= 1)
		{
			if((TimerCount.Ms - GetPdwTimeBegin) > TimeOut)
			{
				Rtn = false;
				break;
			}
		}
#endif
		return Rtn;
	}
	inline bool FIFORead()
	{
		bool Rtn = true;
#if _MSC_VER
#elif PPC
		Rtn = FIFOWait();
		if(Rtn == true)
		{
			*FIFOSet = 0x0800;										//ReadNextPDW
		}
#endif
		return Rtn;
	}
	inline bool WaitBufferReady(size_t BufferNo)
	{
		bool Rtn = true;
		GetPdwTimeBegin = TimerCount.Ms;
		switch(BufferNo)
		{
		case 1:
			Buffer2Loc = *Buffer2Size;
			while(*Buffer1ReadyTag != PDWBUFFER_READYTAG)
			{
				if((TimerCount.Ms - GetPdwTimeBegin) > TimeOut)
				{
					Rtn = false;
					break;
				}
			}
			if(Rtn == true)
			{
				Buffer1Loc = 0;
				RecentBuffer = 1;
			}
			RecvPtr = &(PDWBuffer1[Buffer1Loc]);
			break;
		case 2:
			Buffer1Loc = *Buffer1Size;
			while(*Buffer2ReadyTag != PDWBUFFER_READYTAG)
			{
				if((TimerCount.Ms - GetPdwTimeBegin) > TimeOut)
				{
					Rtn = false;
					break;
				}
			}
			if(Rtn == true)
			{
				Buffer2Loc = 0;
				RecentBuffer = 2;
			}
			RecvPtr = &(PDWBuffer2[Buffer2Loc]);
			break;
		default:
			break;
		}
		return Rtn;
	}
	inline bool EndTagJudge(size_t Loc)
	{
		if(RecentBuffer == 1)
		{
			if(Loc >= *Buffer1Size)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(Loc >= *Buffer2Size)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	void 	TimerTest()
	{
		TimerCount.Seconds = 0;
		TimerCount.Ms = 0;
		Timer.Start();
		while(TimerCount.Seconds < 60)
		{
			COUTDEC;
			NLOGKWIDTH(4, TimerCount.Seconds, TimerCount.Ms);
		}
		Timer.Stop();
	}
	void	ShowStatus() const
	{
		NLOGKSSECBEG("RecIter Status");
		NLOGKSWIDTH(4,"Iter类型(RecIte...)		",RecIterType);
		NLOGKSWIDTH(4,"读取超时(TimeOut)		"	 ,TimeOut);
		NLOGKSWIDTH(4,"读取时间(GetPdwTi...)		",GetPdwTimeBegin);
		NLOGKSWIDTH(4,"缓冲区1读取位置(Buf...)	"	 ,Buffer1Loc);
		NLOGKSWIDTH(4,"缓冲区2读取位置(Buf...)	"	 ,Buffer2Loc);
		NLOGKSWIDTH(4,"读取缓冲区(Recent...)		",RecentBuffer);
		NLOGKSSECEND("RecIter Status");
	}
};

#endif /* RECVITER_H_ */
