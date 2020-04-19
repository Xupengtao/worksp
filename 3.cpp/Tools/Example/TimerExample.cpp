#include"../Timer.h"
#include<iostream>
#include<iomanip>
using namespace std;
#if _MSC_VER

int TimerTest(void)
{
	_TimerCount TimerCount = {0,0,0,0};
	_Timer Timer;
	Timer.SetTimerSecondsFunc((int)(&TimerCount));
	Timer.Start();
	while(1)
	{
		NLOGKWIDTH(5, TimerCount.Seconds, TimerCount.Ms);
	}
	return 1;
}

#else
int g_timerTestCnt 	= 0;
void 	TimerTest_Handler(unsigned int param)
{
	NLOGK(param,g_timerTestCnt++);
}
void TimerTest(int TimerFuncSel)
{
	g_timerTestCnt = 0;
	_TimerCount TimerCount = {0,0,0,0};
	_Timer Timer;
	if(TimerFuncSel == 1)
	{
		Timer.SetFuncPoint(TimerTest_Handler,1);
		Timer.Start();
		cin.get();
	}
	else
	{
		Timer.SetTimerHoursFunc((unsigned int)(&TimerCount));
		Timer.Start();
		while(TimerCount.Seconds < 10)
		{
			;
		}
	}
}
#endif /* _MSC_VER / PPC */
