#ifndef TIMER_H_
#define TIMER_H_
#include"Macro.h"

#include<iostream>
#include<iomanip>
using namespace std;
#if _MSC_VER
#include<windows.h>
typedef WAITORTIMERCALLBACK timerHandler;
#elif PPC
#if defined(__cplusplus)
extern "C"
{
#endif /* defined(__cplusplus) */
typedef void (*timerHandler)(unsigned int);
int HalTimer_Init(void);
int HalTimer_GetTimerNum(void);
int HalTimer_Alloc(int index, int time, timerHandler func, unsigned int param, int type);
int HalTimer_Release(int index);
#if defined(__cplusplus)
}
#endif /* defined(__cplusplus) */
#endif /* _MSC_VER / PPC */

struct _TimerCount
{
	unsigned int MsInterival;
	unsigned int Ms;
	unsigned int Seconds;
	unsigned int Minutes;
	unsigned int Hours;
};
template<typename Ty>
void MsInterrupt(Ty TimerCountAddr)
{
	_TimerCount *Timer = (_TimerCount *)TimerCountAddr;
	Timer->Ms = Timer->Ms + Timer->MsInterival;
}
template<typename Ty>
void SecondstoMsInterrupt(Ty TimerCountAddr)
{
	_TimerCount *Timer = (_TimerCount *)TimerCountAddr;
	Timer->Ms = Timer->Ms + Timer->MsInterival;
	if(Timer->Ms / 1000 > Timer->Seconds)
	{
		Timer->Seconds++;
	}
}
template<typename Ty>
void MinutestoMsInterrupt(Ty TimerCountAddr)
{
	_TimerCount *Timer = (_TimerCount *)TimerCountAddr;
	Timer->Ms = Timer->Ms + Timer->MsInterival;
	if(Timer->Ms / 1000 > Timer->Seconds)
	{
		Timer->Seconds++;
	}
	if(Timer->Seconds / 60 > Timer->Minutes)
	{
		Timer->Minutes++;
	}
}
template<typename Ty>
void HourstoMsInterrupt(Ty TimerCountAddr)
{
	_TimerCount *Timer = (_TimerCount *)TimerCountAddr;
	Timer->Ms = Timer->Ms + Timer->MsInterival;
	if(Timer->Ms / 1000 > Timer->Seconds)
	{
		Timer->Seconds++;
	}
	if(Timer->Seconds / 60 > Timer->Minutes)
	{
		Timer->Minutes++;
	}
	if(Timer->Minutes/60 > Timer->Hours)
	{
		Timer->Hours++;
	}
}
#if _MSC_VER
template<typename Ty>
void CALLBACK MSCMsInterrupt(PVOID lpParam, Ty TimerOrWaitFired)
{
	unsigned int Addr = (unsigned int)lpParam;
	MsInterrupt<unsigned int>(Addr);
}
template<typename Ty>
void CALLBACK MSCSecondstoMsInterrupt(PVOID lpParam, Ty TimerOrWaitFired)
{
	unsigned int Addr = (unsigned int)lpParam;
	SecondstoMsInterrupt<unsigned int>(Addr);
}
template<typename Ty>
void CALLBACK MSCMinutestoMsInterrupt(PVOID lpParam, Ty TimerOrWaitFired)
{
	unsigned int Addr = (unsigned int)lpParam;
	MinutestoMsInterrupt<unsigned int>(Addr);
}
template<typename Ty>
void CALLBACK MSCHourstoMsInterrupt(PVOID lpParam, Ty TimerOrWaitFired)
{
	unsigned int Addr = (unsigned int)lpParam;
	HourstoMsInterrupt<unsigned int>(Addr);
}
class _Timer
{
	HANDLE 			hTimer;
	HANDLE 			hTimerQueue;
	int 			TimerNums;
	timerHandler 	Func;
	int				param;
	int 			TimerNo;
	int 			MsInterival;
	const int		OneMsInterival;
	int				Status;
public:
	_Timer(timerHandler Func_ = 0, int param_ = 0, int TimerNo_ = 0, int OneMsInterival_ = 1)
		  :Func(Func_), param(param_), TimerNo(TimerNo_), OneMsInterival(OneMsInterival_)
	{
		TimerNums 	= 0;
		Status 		= 0;
		MsInterival = OneMsInterival;
	}
	virtual ~_Timer()
	{
		if(Status == 1)
		{
			Stop();
		}
	}
	int  GetTimerNums() const {return TimerNums;}
	void SetTimerNo(int TimerNo_)
	{
		TimerNo = TimerNo_;
	}
	void SetTimerInterival(int MsNums)
	{
		MsInterival = OneMsInterival * MsNums;
	}
	void SetFuncPoint(timerHandler Func_, int param_ = 0)
	{
		Func	= Func_;
		param 	= param_;
	}
	int  Start()
	{
		if(Status == 0)
		{
			hTimerQueue = CreateTimerQueue();
			if(hTimerQueue == NULL)
			{
				COUTSTREND("CreatTimerQueue failed!\n");
				NLOGK(GetLastError());
			}
			else
			{
				COUTSTREND("CreatTimerQueue success!\n");
			}
			int Rtn = CreateTimerQueueTimer(&hTimer, hTimerQueue, (WAITORTIMERCALLBACK)Func, PVOID(param), 0, MsInterival, WT_EXECUTEDEFAULT);
			if(!Rtn)
			{
				COUTSTR("CreateTimerQueueTimer failed");
				NLOGK(GetLastError());
				return 0;
			}
			Status = 1;
		}
		return 1;
	}
	int  Stop()
	{
		if(Status == 1)
		{
			DeleteTimerQueueTimer(hTimerQueue, hTimer, NULL);
			if(!DeleteTimerQueue(hTimerQueue))
			{
				COUTSTR("DeleteTimerQueue failed");
				NLOGK(GetLastError());
				return 0;
			}
			Status = 0;
		}
		return 1;
	}
	void SetTimerMsFunc(unsigned int  Addr)
	{
		Func	= MSCMsInterrupt<BOOLEAN>;
		param 	= Addr;
		_TimerCount *Timer = (_TimerCount *)Addr;
		Timer->MsInterival = MsInterival;
	}
	void SetTimerSecondsFunc(unsigned int  Addr)
	{
		Func	= MSCSecondstoMsInterrupt<BOOLEAN>;
		param 	= Addr;
		_TimerCount *Timer = (_TimerCount *)Addr;
		Timer->MsInterival = MsInterival;
	}
	void SetTimerMinutesFunc(unsigned int  Addr)
	{
		Func	= MSCMinutestoMsInterrupt<BOOLEAN>;
		param 	= Addr;
		_TimerCount *Timer = (_TimerCount *)Addr;
		Timer->MsInterival = MsInterival;
	}
	void SetTimerHoursFunc(unsigned int  Addr)
	{
		Func	= MSCHourstoMsInterrupt<BOOLEAN>;
		param 	= Addr;
		_TimerCount *Timer = (_TimerCount *)Addr;
		Timer->MsInterival = MsInterival;
	}
};
#else
class _Timer
{
	int 			TimerNums;
	timerHandler 	Func;
	unsigned int	param;
	int 			TimerNo;
	int 			MsInterival;
	const int		OneMsInterival;
	int				Status;
	int 			MsAdd;
public:
	_Timer(timerHandler Func_ = 0, unsigned int param_ = 0, int TimerNo_ = 0, int OneMsInterival_ = 1016)
		  :Func(Func_), param(param_), TimerNo(TimerNo_), OneMsInterival(OneMsInterival_)
	{
		
		Status		= 0;
		MsAdd		= 0;
		MsInterival = OneMsInterival;
		TimerNums 	= HalTimer_GetTimerNum();
		NLOGK(TimerNums);
	}
	virtual ~_Timer()
	{
		if(Status == 1)
		{
			Stop();
		}
	}
	int  GetTimerNums() const {return TimerNums;}
	void Init()
	{
		HalTimer_Init();
	}
	void SetTimerNo(int TimerNo_)
	{
		TimerNo = TimerNo_;
	}
	void SetTimerInterival(int MsNums)
	{
		MsAdd		= MsNums;
		MsInterival = OneMsInterival * MsNums;
	}
	void SetFuncPoint(timerHandler Func_, int param_ = 0)
	{
		Func	= Func_;
		param 	= param_;
	}
	int  Start()
	{
		if(Status == 0)
		{
			int Rtn;
			COUTSTR("Timer Start!");
			COUTMSGVALEND(TimerNo);
			Rtn = HalTimer_Alloc(TimerNo, MsInterival, Func, param, MsInterival);
			if(Rtn != 0)
			{
				NLogK("TimerStart():Timer %d, alloc failed.\n",TimerNo,0,0,0,0,0);
				return -1;
			}
			Status = 1;
		}
		return 1;
	}
	int  Stop()
	{
		if(Status == 1)
		{
			COUTSTR("Timer Stop! ");
			COUTMSGVALEND(TimerNo);
			HalTimer_Release(TimerNo);
			Status = 0;
		}
		return 1;
	}
	void SetTimerMsFunc(unsigned int Addr)
	{
		Func	= MsInterrupt<unsigned int>;
		param 	= Addr;
		_TimerCount *Timer = (_TimerCount *)Addr;
		Timer->MsInterival = MsAdd;
	}
	void SetTimerSecondsFunc(unsigned int Addr)
	{
		Func	= SecondstoMsInterrupt<unsigned int>;
		param 	= Addr;
		_TimerCount *Timer = (_TimerCount *)Addr;
		Timer->MsInterival = MsAdd;
	}
	void SetTimerMinutesFunc(unsigned int Addr)
	{
		Func	= MinutestoMsInterrupt<unsigned int>;
		param 	= Addr;
		_TimerCount *Timer = (_TimerCount *)Addr;
		Timer->MsInterival = MsAdd;
	}
	void SetTimerHoursFunc(unsigned int Addr)
	{
		Func	= HourstoMsInterrupt<unsigned int>;
		param 	= Addr;
		_TimerCount *Timer = (_TimerCount *)Addr;
		Timer->MsInterival = MsAdd;
	}
};
#endif /* _MSC_VER / PPC */
#endif /* TIMER_H_ */
