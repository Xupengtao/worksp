/*
 * Nrj6_T50
 *
 *  Created on: 9, 27, 2019
 *      Author: xupengtao
 */
#ifndef NRJX_MAIN_H_
#define NRJX_MAIN_H_

#include<iostream>
#include<sstream>
#include<iomanip>
using namespace std;
#include"Struct.h"
#include"PreProcess.hpp"
#include".\Tools\Timer.h"
#include".\Tools\Error.h"
#include"PdwDiscern.hpp"
#include"PdwDiscernDebug.hpp"
#if _MSC_VER
#include<windows.h>
#include<stdlib.h>
#elif PPC
#endif

#if _MSC_VER
template<typename _PreProcessType>
struct _lpPreProcess
{
	size_t *CtrlSign;
	size_t *TaskDeleteSign;
	_PreProcessType *PreProcess;
};
template<typename _PdwDiscernType>
struct _lpPdwDiscern
{
	size_t *CtrlSign;
	size_t *TaskDeleteSign;
	_PdwDiscernType *PdwDiscern;
};
template<typename _PreProcessType>
DWORD WINAPI task_SelfCheck(LPVOID lpParameter)
{
	_lpPreProcess<_PreProcessType> *lpPreProcess = (_lpPreProcess<_PreProcessType> *)lpParameter;
	(lpPreProcess->PreProcess)->SelfCheck(*(lpPreProcess->CtrlSign));
	return 1;
}
template<typename _PreProcessType>
DWORD WINAPI task_PreProcess(LPVOID lpParameter)
{
	_lpPreProcess<_PreProcessType> *lpPreProcess = (_lpPreProcess<_PreProcessType> *)lpParameter;
	(lpPreProcess->PreProcess)->Process(*(lpPreProcess->CtrlSign),*(lpPreProcess->TaskDeleteSign));
	return 1;
}
template<typename _PdwDiscernType>
DWORD WINAPI task_PdwDiscern(LPVOID lpParameter)
{
	_lpPdwDiscern<_PdwDiscernType> *lpPdwDiscern = (_lpPdwDiscern<_PdwDiscernType> *)lpParameter;
	(lpPdwDiscern->PdwDiscern)->run(*(lpPdwDiscern->CtrlSign),*(lpPreProcess->TaskDeleteSign));
	return 1;
}
#elif PPC
template<typename _PreProcessType>
int task_SelfCheck(_PreProcessType *PreProcess,size_t *CtrlSign)
{
	PreProcess->SelfCheck(*CtrlSign);
	return 1;
}
template<typename _PreProcessType>
int task_PreProcess(_PreProcessType *PreProcess,size_t *CtrlSign,size_t *TaskDeleteSign)
{
	PreProcess->Process(*CtrlSign,*TaskDeleteSign);
	return 1;
}
template<typename _PdwDiscernType>
int task_PdwDiscern(_PdwDiscernType *PdwDiscern,size_t *CtrlSign,size_t *TaskDeleteSign)
{
	PdwDiscern->run(*CtrlSign,*TaskDeleteSign);
	return 1;
}
#endif

template<typename RfTy,typename PwTy,typename PaTy,typename ToaTy,typename TonTy,	//PDW各个参数的数据类型
		 size_t 	MainChannelNum,													//预分选主通道的通道数
		 size_t 	ParaNum,														//每个PDW数据通道的最大脉冲数
		 size_t		ThresholdChgeTimeOut,											//通道可分选阈值修改TimeOut
		 size_t		AnalayLeastThreshold,											//最小可分选脉冲数
		 size_t 	DualRamNormalStart,												//Ram缓冲区标准有效通道起始值
		 size_t 	DualRamChannelNum,												//PDW数据通道的通道数
		 size_t 	ChannelRadarMaxNo,												//每个PDW数据通道的可分选雷达的最大数
         int		AnalysisNum,													//临时通道的最大脉冲数
         int 		SingleChanAnalyTimeOut,											//全部有效单通道分选超时
         int 		NormalAnalyNum,													//分选初始阈值，当以次阈值进行分选时，标志预处理并未对该通道调整阈值
         int 		MutiSignalAnalyMinNum,											//参加多信号分选的最小脉冲数
         int 		HistAnalyMinNum,												//参加直方图分选的最小脉冲数
         int 		SingAgileRfRgMin,												//参加单通道捷变分选的最小RF变化范围
         int 		SingChanAgileTlrc,												//参加单通道捷变分选的最大RF变化范围
         int 		MutiChanAgileTlrc,												//参加多通道捷变分选的最大RF变化范围
         int 		AgileAnaMinNum,													//参加捷变分选的最小脉冲数
		 size_t 	ContinuousPulseChannel,											//Ram缓冲区连续波通道号
		 size_t 	DiscernThreshold,												//每个PDW数据通道的可分选阈值的初始值
		 int 		ScanThreshold,													//参加扫描分析的最小脉冲数
		 int 		ParaUpdataTimeOut,												//参数更新间隔时间长度
		 int 		DoaUpdataTimeOut>												//方位更新间隔时间长度	 
class _NrjxMain
{
private:
	typedef _RecPDW<RfTy,PwTy,PaTy,ToaTy,TonTy>		_RecPDWType;
	typedef _Pdw			<_RecPDWType,
							ParaNum>				_PdwType;
	typedef _iRadarSimPro	<_RecPDWType>			_iRadarSimProType;
	typedef _RecvIter  		<_RecPDWType>			_RecvIterType;
	typedef _PDWChannel		<_PdwType,
							ChannelRadarMaxNo> 		_PDWChannelType;
	typedef _DualRam   		<_PDWChannelType,
							DualRamChannelNum,
							ChannelRadarMaxNo,
							DiscernThreshold,
							ScanThreshold>   		_DualRamType;
	typedef _PreProcess		<_RecvIterType,
							_DualRamType,
							MainChannelNum,
							ParaNum,
							ThresholdChgeTimeOut,
							AnalayLeastThreshold,
							DualRamNormalStart,
							ContinuousPulseChannel,
							size_t>					_PreProcessType;
	typedef _PdwDiscern		<_DualRamType,
						    DualRamChannelNum,
							ChannelRadarMaxNo,
							DualRamNormalStart,
							AnalysisNum,
							SingleChanAnalyTimeOut,
							NormalAnalyNum,
							MutiSignalAnalyMinNum,
							HistAnalyMinNum,
							SingAgileRfRgMin,
							SingChanAgileTlrc,
							MutiChanAgileTlrc,
							AgileAnaMinNum,
							ScanThreshold,
							ParaUpdataTimeOut,
							DoaUpdataTimeOut>		_PdwDiscernType;
	typedef _PdwDiscernDebug<_DualRamType,
							DualRamChannelNum,
							ChannelRadarMaxNo,
							DualRamNormalStart,
							AnalysisNum,
							SingleChanAnalyTimeOut,
							NormalAnalyNum,
							MutiSignalAnalyMinNum,
							HistAnalyMinNum,
							SingAgileRfRgMin,
							SingChanAgileTlrc,
							MutiChanAgileTlrc,
							AgileAnaMinNum,
							ScanThreshold,
							ParaUpdataTimeOut,
							DoaUpdataTimeOut>		_PdwDiscernDebugType;
public:
	int								BandNo;
	_iRadarSimProType 				iRadarSimPro;
	_RecvIterType 					RecIter;										//signal输入接口迭代器类,内置定时器1
	_Timer							Timer;											//定时器0
	_TimerCount						TimerCount;
	int 							TimerMsInterival;								//定时器触发中断间隔
	_DualRamType 					DualRam;
	_PreProcessType 				*PreProcess;
	_PdwDiscernType 				*PdwDiscern;
	_SysCmd							SystemCmd;
	size_t							SysRecvCmd;
	size_t							CtrlSign;										//控制标志
	size_t							DebugType;
	size_t							TimeBegin;										//一次分选起始时间，由预处理写值，预处理和主处理读值
	size_t 							RunType;										//运行时读命令的方式：0：串口，1:单字符，2：字符串
#if _MSC_VER
	_lpPreProcess<_PreProcessType>	lpPreProcess;
	_lpPdwDiscern<_PdwDiscernType>	lpPdwDiscern;
	HANDLE 							TaskIdOrHandle[10];
#elif PPC
	int 							TaskIdOrHandle[10];
#endif
	size_t							TaskDelSign[10];
	enum 	_RunType
	{
		SerialPort 	= 0,
		CinGet		= 1,
		StrCmdGet	= 2
	};
	enum 	_SysCmd
	{
		WorkStartCmd			= 0x88,
		WorkStopCmd				= 0x89,
		ChangeDebug0Cmd			= 0x8A,
		ChangeDebug1Cmd			= 0x8B,
		SelfCheckCmd			= 0x85,
		DoaUpdateOnCmd			= 0x68,
		DoaUpdateOffCmd 		= 0x64,
		SetPc0To0Cmd			= 0xAA,
		SetPc0To1Cmd			= 0x55,
		iRadarGenCmd			= 0x45,
		RadarSetCmd				= 0x46,
		ReadTestCmd				= 0x31,
		OutMsgTestCmd			= 0x32,
		ShowStatusCmd			= 0x33,
		iRadarSimProTestCmd		= 0x34,
		RecIterTestCmd			= 0x35,
		TimerTestCmd			= 0x36,
		TimeBeginTestCmd		= 0x37,
		RecvIterTimerTestCmd	= 0x38
	};
	
	_NrjxMain(int RecIterType, int TimerMsInterival_)
			 :iRadarSimPro(),
			  RecIter(),
			  Timer(),
			  DualRam(),
			  PreProcess(NULL),
			  PdwDiscern(NULL),
			  TimerMsInterival(TimerMsInterival_),
			  SysRecvCmd(0),
			  CtrlSign(0),
			  DebugType(0),
			  TimeBegin(0),
			  RunType(0)
	{
		RecIter.ReBinding(iRadarSimPro.GetPdwBufAddr1(),iRadarSimPro.GetPdwStatusAddr1(),iRadarSimPro.GetSortNumAddr1(),
						  iRadarSimPro.GetPdwBufAddr2(),iRadarSimPro.GetPdwStatusAddr2(),iRadarSimPro.GetSortNumAddr2());
		RecIter.RstRecIterType(RecIterType);
		PreProcess = new _PreProcessType(RecIter,DualRam,TimerCount,TimeBegin);
		if(DebugType == 0)
		{
			PdwDiscern = new _PdwDiscernType(DualRam,TimerCount,TimeBegin,SystemCmd);
		}
		else
		{
			PdwDiscern = new _PdwDiscernDebugType(DualRam,TimerCount,TimeBegin,SystemCmd);
		}
		memset(&SystemCmd,0,sizeof(_SysCmd));
		SystemCmd.DoaUpdata = 1;
		Init(TimerMsInterival);
	}
	virtual ~_NrjxMain()
	{
		delete PreProcess;
		delete PdwDiscern;
	}
	/*-------------------------------------- Load Section --------------------------------------*/
	void	Init(int TimerMsInterival)
	{
		SetTimer(TimerMsInterival);
		SetBandNo();
		SetTaskIdOrHandle();
	}
	void 	Clear()
	{
		Timer.Stop();
		Init(TimerMsInterival);
		PreProcess->Clear();
		DualRam.Clear();
		PdwDiscern->Clear();
	}
	void	SetTimer(int TimerMsInterival)
	{
		Timer.Stop();
		memset(&TimerCount, 0, sizeof(_TimerCount));
		Timer.SetTimerNo(0);
		Timer.SetTimerInterival(TimerMsInterival);
		Timer.SetTimerSecondsFunc((UINT)(&TimerCount));
	}
	void	SetBandNo()
	{
#if _MSC_VER
		USHORT BandSel = 0;
#elif PPC
		USHORT BandSel = PdwDiscern->RdInPort((USHORT *)0xEC00004c);
#endif
		BandNo = ((0x04&BandSel) == 0)?0:1;
		PreProcess->RstBand(BandNo);
		PdwDiscern->RstBand(BandNo);
	}
	void	SetTaskIdOrHandle()
	{
		CtrlSign = 0;
#if _MSC_VER
		lpPreProcess.PreProcess 	= PreProcess;
		lpPreProcess.CtrlSign 		= &CtrlSign;
		lpPreProcess.TaskDeleteSign	= &TaskDelSign[0];
		lpPdwDiscern.PdwDiscern 	= PdwDiscern;
		lpPdwDiscern.CtrlSign 		= &CtrlSign;
		lpPreProcess.TaskDeleteSign	= &TaskDelSign[1];
		memset(TaskIdOrHandle, 0, sizeof(HANDLE)*10);
#elif PPC
		memset(TaskIdOrHandle, 0, sizeof(int)*10);
#endif
		memset(TaskDelSign, 0, sizeof(size_t)*10);
	}
	int		TaskStatusCheck() const
	{
		if((TaskIdOrHandle[0] == 0) && 
		   (TaskIdOrHandle[1] == 0) && 
		   (TaskDelSign[0] 	  == 0) && 
		   (TaskDelSign[1]	  == 0))
		{
			return _OK;
		}
		else
		{
			NLOGKS("Cmd Invalid(WorkStop):	Checked TaskIdOrHandle Value!");
			return _ERROR;
		}
	}
	/*-------------------------------------- Load Section --------------------------------------*/
	
	/*-------------------------------------- Mode Section --------------------------------------*/
	int 	Run(size_t _RunType = SerialPort)
	{
		RunType = _RunType;
		while(1)
		{
			if(RunType == SerialPort)
			{
				SysRecvCmd = PdwDiscern->ReadInPort();
			}
			else if(RunType == CinGet)
			{
				SysRecvCmd = cin.get();
			}
			else if(RunType == StrCmdGet)
			{
				SysRecvCmd = GetStrCmd();
			}
			if(SysRecvCmd != 0)
			{
				switch(SysRecvCmd)
				{
				case WorkStartCmd:
					WorkStart();
					break;
				case WorkStopCmd:
					WorkStop();
					break;
				case ChangeDebug0Cmd:
					ChangeDebugMode(0);
					break;
				case ChangeDebug1Cmd:
					ChangeDebugMode(1);
					break;
				case SelfCheckCmd:
					SelfCheck();
					break;
				case DoaUpdateOnCmd:
					SystemCmd.DoaUpdata = 1;
					break;
				case DoaUpdateOffCmd:
					SystemCmd.DoaUpdata = 0;
					break;
				case SetPc0To0Cmd:
					break;
				case SetPc0To1Cmd:
					break;		
				case ReadTestCmd:
					ReadCmdTest();
					break;
				case OutMsgTestCmd:
					OutMsgTest();
					break;
				case ShowStatusCmd:
					ShowStatus();
					break;
				case iRadarSimProTestCmd:
					iRadarSimProTest();
					break;
				case RecIterTestCmd:
					RecIterTest(0,0x1FFF,100);
					break;
				case TimerTestCmd:
					TimerTest();
					break;
				case TimeBeginTestCmd:
					TimeBeginTest();
					break;
				case RecvIterTimerTestCmd:
					RecvIterTimerTest();
					break;
				case iRadarGenCmd:
					iRadarSimPro.Init();											/* 单通道 3 载频固定重频固定  */
					iRadarSimPro.RadarSimGen(FixRfFixPri, 0, 1000, 1000);
					iRadarSimPro.RadarSimGen(FixRfFixPri, 1, 1000, 3223);
					iRadarSimPro.RadarSimGen(FixRfFixPri, 2, 1000, 5215);
					iRadarSimPro.run_onetimes();
					iRadarSimPro.ShowOutBuffer(0, 100);
					break;
				case RadarSetCmd:
					iRadarSet();
					break;
				default:
					break;
				}
			}
			taskDelay(1);
		}
	}
	int 	WorkStart()
	{
		if(CtrlSign == 0)
		{
			if(TaskStatusCheck() != _OK)
			{
				return _ERROR;
			}
			Timer.Start();
			CtrlSign = 1;
			TaskDelSign[0] = 0;
			TaskDelSign[1] = 0;
#if _MSC_VER
			TaskIdOrHandle[0] = CreateThread(NULL,0,task_PreProcess<_PreProcessType>,LPVOID(&lpPreProcess),0,NULL);
			TaskIdOrHandle[1] = CreateThread(NULL,0,task_PdwDiscern<_PdwDiscernType>,LPVOID(&lpPdwDiscern),0,NULL);
#elif PPC
			TaskIdOrHandle[0] = taskSpawn("PreProcess", 100, VX_FP_TASK, 0x40000,(FUNCPTR)task_PreProcess<_PreProcessType>,
							   (int)(PreProcess),(int)(&CtrlSign),(int)(&TaskDelSign[0]),0,0,0,0,0,0,0);
			TaskIdOrHandle[1] = taskSpawn("PdwDiscern", 100, VX_FP_TASK, 0x40000,(FUNCPTR)task_PdwDiscern<_PdwDiscernType>,
							   (int)(PdwDiscern),(int)(&CtrlSign),(int)(&TaskDelSign[1]),0,0,0,0,0,0,0);
#endif
			if(TaskIdOrHandle[0] == ERROR)
			{
				return _ERROR;
			}
			if(TaskIdOrHandle[1] == ERROR)
			{
				return _ERROR;
			}
			return _OK;
		}
		else
		{
			NLOGKS("Cmd Invalid(WorkStart):	Already Working!");
			return _OK;
		}
	}
	int		WorkStop()
	{
		if(CtrlSign == 0)
		{
			if(TaskStatusCheck() != _OK)
			{
				return _ERROR;
			}
			else
			{
				NLOGKS("Cmd Invalid(WorkStop):	Already WorkStop!");
				return _OK;
			}
		}
		CtrlSign = 0;
		bool AllTaskReady = false;
		while(AllTaskReady != true)
		{
			if((TaskDelSign[0] == 1) && (TaskDelSign[1] == 1))
			{
				AllTaskReady = true;
			}
			taskDelay(0);
		}
		TaskDelSign[0] = 0;
		TaskDelSign[1] = 0;
		TaskIdOrHandle[0] = 0;
		TaskIdOrHandle[1] = 0;
		Clear();
		return _OK;
	}
	int		ChangeDebugMode(size_t _DebugType)
	{
		if(DebugType == _DebugType)
		{
			NLOGKS("Cmd Invalid(ChangeDebugMode):	Tap Equ DebugType!");
			return _OK;
		}
		DebugType = _DebugType;
		size_t CtrlSignTmp = CtrlSign;
		if(CtrlSignTmp == 1)
		{
			WorkStop();
		}
		delete PdwDiscern;
		if(DebugType == 0)
		{
			PdwDiscern = new _PdwDiscernType(DualRam,TimerCount,TimeBegin,SystemCmd);
		}
		else
		{
			PdwDiscern = new _PdwDiscernDebugType(DualRam,TimerCount,TimeBegin,SystemCmd);
		}
		SetBandNo();
		if(CtrlSignTmp == 1)
		{
			WorkStart();
		}
		return _OK;
	}
	void	SetDebugMode(int Status)
	{
		if(DebugType == 1)
		{
			PdwDiscern->SetDebugStatus(Status);
		}
		else
		{
			NLOGKS("Cmd Invalid(DelDebugMode):	DebugType Not Allow Such Cmd!");
		}
	}
	void	DelDebugMode(int Status)
	{
		if(DebugType == 1)
		{
			PdwDiscern->DelDebugStatus(Status);
		}
		else
		{
			NLOGKS("Cmd Invalid(DelDebugMode):	DebugType Not Allow Such Cmd!");
		}
	}
	int 	SelfCheck()
	{
		WorkStop();
		Timer.Start();
#if _MSC_VER
		TaskIdOrHandle[2] = CreateThread(NULL,0,task_SelfCheck<_PreProcessType>,LPVOID(&lpPreProcess),0,NULL);
#elif PPC
		TaskIdOrHandle[2] = taskSpawn("PreProcess", 100, VX_FP_TASK, 0x40000,(FUNCPTR)task_SelfCheck<_PreProcessType>,
						   (int)(PreProcess),(int)(&CtrlSign),0,0,0,0,0,0,0,0);
#endif
		if(TaskIdOrHandle[2] == ERROR)
		{
			return _ERROR;
		}
		while(CtrlSign == 0)
		{
			;
		}
		NLOGKBYHEXWIDTH(4,CtrlSign);
		PdwDiscern->WriteOutPort(CtrlSign & 0x00FF);
		PdwDiscern->WriteOutPort((CtrlSign & 0xFF00) >> 8);
		CtrlSign = 0;
		taskDelay(0);
		TaskIdOrHandle[2] = 0;
		Clear();
		return _OK;
	}
	void 	SetRunType(size_t _RunType)
	{
		RunType = _RunType;
		switch(RunType)
		{
		case SerialPort:
			COUTSTREND("命令输入模式切换至 ---> 串口输入");
			COUTSTREND("Command type changed to ---> SerialPort");
			break;
		case CinGet:
			COUTSTREND("命令输入模式切换至 ---> 按键输入");
			COUTSTREND("Command type changed to ---> CinGet");
			break;
		case StrCmdGet:
			COUTSTREND("命令输入模式切换至 ---> 字符串输入");
			COUTSTREND("Command type changed to ---> StrCmdGet");
			break;
		default:
			COUTSTREND("输入格式错误，参考:   0->串口输入   1->按键输入   2->字符串输入");
			COUTSTREND("Command type error, referance:  0->SerialPort  1->CinGet   2->StrCmdGet");
			break;
		}
	}
	UINT 	GetStrCmd()
	{
		UINT StrCmd = 0;
		char GetStr[20];
		cin>>GetStr;
		if(strcmp(GetStr,"WorkStart") == 0)
		{
			StrCmd = WorkStartCmd;
		}
		else if(strcmp(GetStr,"WorkStop") == 0)
		{
			StrCmd = WorkStopCmd;
		}
		else if(strcmp(GetStr,"ChangeDebug0") == 0)
		{
			StrCmd = ChangeDebug0Cmd;
		}
		else if(strcmp(GetStr,"ChangeDebug1") == 0)
		{
			StrCmd = ChangeDebug1Cmd;
		}
		else if(strcmp(GetStr,"SelfCheck") == 0)
		{
			StrCmd = SelfCheckCmd;
		}
		else if(strcmp(GetStr,"DoaUpdateOn") == 0)
		{
			StrCmd = DoaUpdateOnCmd;
		}
		else if(strcmp(GetStr,"DoaUpdateOff") == 0)
		{
			StrCmd = DoaUpdateOffCmd;
		}
		else if(strcmp(GetStr,"SetPc0To0") == 0)
		{
			StrCmd = SetPc0To0Cmd;
		}
		else if(strcmp(GetStr,"SetPc0To1") == 0)
		{
			StrCmd = SetPc0To1Cmd;
		}
		else if(strcmp(GetStr,"iRadarGen") == 0)
		{
			StrCmd = iRadarGenCmd;
		}
		else if(strcmp(GetStr,"RadarSet") == 0)
		{
			StrCmd = RadarSetCmd;
		}
		else if(strcmp(GetStr,"ReadTest") == 0)
		{
			StrCmd = ReadTestCmd;
		}
		else if(strcmp(GetStr,"OutMsgTest") == 0)
		{
			StrCmd = OutMsgTestCmd;
		}
		else if(strcmp(GetStr,"ShowStatus") == 0)
		{
			StrCmd = ShowStatusCmd;
		}
		else if(strcmp(GetStr,"iRadarSimProTest") == 0)
		{
			StrCmd = iRadarSimProTestCmd;
		}
		else if(strcmp(GetStr,"RecIterTest") == 0)
		{
			StrCmd = RecIterTestCmd;
		}
		else if(strcmp(GetStr,"TimerTest") == 0)
		{
			StrCmd = TimerTestCmd;
		}
		else if(strcmp(GetStr,"TimeBeginTest") == 0)
		{
			StrCmd = TimeBeginTestCmd;
		}
		else if(strcmp(GetStr,"RecvIterTimerTest") == 0)
		{
			StrCmd = RecvIterTimerTestCmd;
		}
		else
		{
			StrCmd = 0;
		}
		return StrCmd;
	}
	/*-------------------------------------- Mode Section --------------------------------------*/
	
	/*-------------------------------------- Func Section --------------------------------------*/
	void 	iRadarGen(size_t MsgPdwType, size_t RadarSn, size_t Rf, size_t Pri, size_t RfNum = 0, size_t PriNumOrRange = 0)
	{
		iRadarSimPro.Init();
		iRadarSimPro.RadarSimGen(MsgPdwType, RadarSn, Rf, Pri, RfNum, PriNumOrRange);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
	}
	void	iRadarSet()
	{
		size_t MsgPdwType = 0;
		size_t RadarSn = 0;
		size_t Rf = 0;
		size_t Pri = 0;
		stringstream StringToInt;
		string s;
		COUTSTREND("请输入雷达类型:");
		cin>>s;
		StringToInt<<s;
		StringToInt>>MsgPdwType;
		COUT(MsgPdwType);
		COUTSTREND("请输入雷达序号:");
		cin>>RadarSn;
		COUT(RadarSn);
		COUTSTREND("请输入载频Rf:");
		cin>>Rf;
		COUT(Rf);
		COUTSTREND("请输入重频Pri:");
		cin>>Pri;
		COUT(Pri);
		iRadarGen(1,RadarSn,Rf,Pri);
	}
	/*-------------------------------------- Func Section --------------------------------------*/

	/*-------------------------------------- Show Section --------------------------------------*/
	virtual void ShowHelp() const
	{
		COUTSTREND("");
		COUTSTREND("");
		COUTSTREND("");
		COUTSTREND("                      T50 help manual                      ");
		COUTSTREND("1 Shell Command:");
		COUTSTREND("	1.1 \"Task\"");
		COUTSTREND("	1.2 \"Channel\"");
		COUTSTREND("	1.3 \"Result\"");
		COUTSTREND("	1.4 \"Status\"");
		COUTSTREND("  	1.5 \"ChangeCmdType\" arg: (int CmdType)");
		COUTSTREND("  	  You can use this Func to reset your CmdType,which CmdType is GivenBy Sec 2.");
		COUTSTREND("2 Work Env Command:");
		COUTSTREND("  As Running in The Work Func, The DataProcess Band is Running in a While(1) Loop.");
		COUTSTREND("  You can Send your cmd to this Func By choosing Such 3 Types:");
		COUTSTREND("	2.1 SerialPort:");
		COUTSTREND("	  Receive cmd from pre-module");
		COUTSTREND("	2.2 CinGet:");
		COUTSTREND("	  Tap Those Ascii char as cmd:");
		COUTSTREND("	  	WorkCmd                  Tap ");
		COUTSTREND("	  	SelfCheckCmd             Tap ");
		COUTSTREND("	  	DoaUpdateOnCmd           Tap 0x68");
		COUTSTREND("	  	DoaUpdateOffCmd          Tap 0x64");
		COUTSTREND("	  	SetPc0To0Cmd             Tap ");
		COUTSTREND("	  	SetPc0To1Cmd             Tap 0x55");
		COUTSTREND("	  	iRadarGenCmd             Tap 0x45");
		COUTSTREND("	  	ReadTestCmd              Tap 1");
		COUTSTREND("	  	OutMsgTestCmd            Tap 2");
		COUTSTREND("	  	ShowStatusCmd            Tap 3");
		COUTSTREND("	  	iRadarSimProTestCmd      Tap 4");
		COUTSTREND("	  	RecIterTestCmd           Tap 5");
		COUTSTREND("	  	TimerTestCmd             Tap 6");
		COUTSTREND("	  	TimeBeginTestCmd         Tap 7");
		COUTSTREND("	  	RecvIterTimerTestCmd     Tap 8");
		COUTSTREND("	2.3 StrCmdGet:");
		COUTSTREND("	  Tap string as cmd:");
		COUTSTREND("	  	Work");
		COUTSTREND("	  	SelfCheck");
		COUTSTREND("	  	DoaUpdateOn");
		COUTSTREND("	  	DoaUpdateOff");
		COUTSTREND("	  	SetPc0To0");
		COUTSTREND("	  	SetPc0To1");
		COUTSTREND("	  	iRadarGen");
		COUTSTREND("	  	ReadTest");
		COUTSTREND("	  	OutMsgTest");
		COUTSTREND("	  	ShowStatus");
		COUTSTREND("	  	iRadarSimProTest");
		COUTSTREND("	  	RecIterTest");
		COUTSTREND("	  	TimerTest");
		COUTSTREND("	  	TimeBeginTest");
		COUTSTREND("	  	RecvIterTimerTest");
	}
	virtual void ShowStatus() const
	{
		NLOGKSSECBEG("NrjxMain ShowStatus");
		NLOGKSWIDTH(4,"波段号(BandNo)			",BandNo);
		NLOGKSWIDTH(4,"控制标志(CtrlSign)		",CtrlSign);
		NLOGKSWIDTH(4,"分选起始时间(TimeBegin)	",TimeBegin);
		NLOGKSWIDTH(4,"预分选通道数(MainChann...)	",MainChannelNum);				//预分选主通道的通道数
		NLOGKSWIDTH(4,"通道脉冲数(ParaNum)		"	 ,ParaNum);						//每个PDW数据通道的最大脉冲数
		NLOGKSWIDTH(4,"阈值初始值(DiscernThr...)	",DiscernThreshold);			//每个PDW数据通道的可分选阈值的初始值
		NLOGKSWIDTH(4,"阈值重置超时(Threshold...)	",ThresholdChgeTimeOut);		//通道可分选阈值修改TimeOut
		NLOGKSWIDTH(4,"最小分选脉冲数(AnalayLe...)	",AnalayLeastThreshold);		//最小可分选脉冲数
		NLOGKSWIDTH(4,"连续波通道号(Continuou...)	",ContinuousPulseChannel);		//Ram缓冲区连续波通道号
		NLOGKSWIDTH(4,"标准通道起始(DualRamNo...)	",DualRamNormalStart);			//Ram缓冲区标准有效通道起始值
		NLOGKSWIDTH(4,"PDW通道数(DualRamCha...)	",DualRamChannelNum);			//PDW数据通道的通道数
		NLOGKSWIDTH(4,"每通道雷达数(ChannelRa...)	",ChannelRadarMaxNo);			//每个PDW数据通道的可分选雷达的最大数
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
		NLOGKSSECEND("NrjxMain ShowStatus");
		RecIter.ShowStatus();
		DualRam.ShowStatus();
		PreProcess->ShowStatus();
		PdwDiscern->ShowStatus();
	}
	virtual void ShowTask() const
	{
		NLOGKSWIDTH(4,"Prepross 	Task",TaskIdOrHandle[0]);
		NLOGKSWIDTH(4,"Discern 	Task"	 ,TaskIdOrHandle[1]);
		NLOGKSWIDTH(4,"SelfCheck	Task",TaskIdOrHandle[2]);
		NLOGKSWIDTH(4,"None		Task"	 ,TaskIdOrHandle[3]);
		NLOGKSWIDTH(4,"None		Task"	 ,TaskIdOrHandle[4]);
		NLOGKSWIDTH(4,"None		Task"	 ,TaskIdOrHandle[5]);
		NLOGKSWIDTH(4,"None		Task"	 ,TaskIdOrHandle[6]);
		NLOGKSWIDTH(4,"None		Task"	 ,TaskIdOrHandle[7]);
		NLOGKSWIDTH(4,"None		Task"	 ,TaskIdOrHandle[8]);
		NLOGKSWIDTH(4,"None		Task"	 ,TaskIdOrHandle[9]);
	}
	virtual void ShowTaskDelete() const
	{
		NLOGKSWIDTH(4,"Prepross 	TaskDelSign" ,TaskDelSign[0]);
		NLOGKSWIDTH(4,"Discern 	TaskDelSign"	 ,TaskDelSign[1]);
		NLOGKSWIDTH(4,"SelfCheck	TaskDelSign" ,TaskDelSign[2]);
		NLOGKSWIDTH(4,"None		TaskDelSign"	 ,TaskDelSign[3]);
		NLOGKSWIDTH(4,"None		TaskDelSign"	 ,TaskDelSign[4]);
		NLOGKSWIDTH(4,"None		TaskDelSign"	 ,TaskDelSign[5]);
		NLOGKSWIDTH(4,"None		TaskDelSign"	 ,TaskDelSign[6]);
		NLOGKSWIDTH(4,"None		TaskDelSign"	 ,TaskDelSign[7]);
		NLOGKSWIDTH(4,"None		TaskDelSign"	 ,TaskDelSign[8]);
		NLOGKSWIDTH(4,"None		TaskDelSign"	 ,TaskDelSign[9]);
	}
	virtual void ShowChannel(size_t ChanNoSt = 0, size_t ChanNoEnd = 0) const
	{
		PreProcess->ShowChannel(ChanNoSt,ChanNoEnd);
	}
	virtual void ShowAnalyChannel() const
	{
		PdwDiscern->ShowAnalyChannel();
	}
	virtual void ShowResult(size_t ResultNoSt = 0, size_t ResultNoEnd = 0) const
	{
		PdwDiscern->ShowResult(ResultNoSt, ResultNoEnd);
	}
	virtual void ShowLastTimeOfResult() const
	{
		PdwDiscern->ShowLastTimeOfResult();
	}
	virtual void ShowHistogram() const
	{
		PdwDiscern->ShowHistogram();
	}
	virtual void ShowWhichChannel(int SearchRf) const
	{
		PreProcess->ShowWhichChannel(SearchRf);
	}
	/*-------------------------------------- Show Section --------------------------------------*/

	/*-------------------------------------- Test Section --------------------------------------*/
	void iRadarSimProTest()
	{
		UINT Pri = 1000, Rf = 1000, PriDitherRange = 500;
		UINT PriJaggArr[8] = {1000,1200,1400,1600,1800,2000,2200,2400};
		UINT RfAgileArr[8] = {0x1210,0x1218,0x1220,0x1228,0x1230,0x1238,0x1240,0x1249};
		iRadarSimPro.Init();																	/* 单通道 3 载频固定重频固定  */
		iRadarSimPro.RadarSimGen(FixRfFixPri, 0, Rf, 1000);
		iRadarSimPro.RadarSimGen(FixRfFixPri, 1, Rf, 3223);
		iRadarSimPro.RadarSimGen(FixRfFixPri, 2, Rf, 5215);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* 单通道 1 载频固定重频参差  */
		iRadarSimPro.RadarSimGen(FixRfJaggPri, 0, Rf, (UINT)PriJaggArr, 0, 8);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* 单通道 1 载频固定重频抖动  */
		iRadarSimPro.RadarSimGen(FixRfDitherPri, 0, Rf, Pri, 0, PriDitherRange);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* 单通道 1 载频捷变重频固定  */
		iRadarSimPro.RadarSimGen(AgileRfFixPri, 0, (UINT)RfAgileArr, Pri, 8);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* 单通道 1 载频捷变重频抖动  */
		iRadarSimPro.RadarSimGen(AgileRfDitherPri, 0, (UINT)RfAgileArr, Pri, 8, PriDitherRange);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* 单通道 9 载频固定重频固定  */
		iRadarSimPro.RadarPriTypeModify();
		iRadarSimPro.RadarRfTypeModify();
		size_t Tmp[] = {1,2,3,4,5,6,7,8,9};
		iRadarSimPro.RadarModify(Tmp,9);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
	}
	void RecIterTest(size_t RfLeftInterival,size_t RfRightInterival,size_t TestCnt)
	{
		Timer.Start();
		_RecPDWType *PdwTmp = new _RecPDWType[TestCnt];
		memset(PdwTmp,0,sizeof(_RecPDWType) * TestCnt);
		int SecondTmp = TimerCount.Seconds;
		ToaTy PreToa = 0, Pri = 0;
		if(RecIter.GetType() == 2)
		{
			iRadarSimPro.Init();										/* 单通道 3 载频固定重频固定  */
			iRadarSimPro.RadarSimGen(FixRfFixPri, 0, 1000, 1000);
			iRadarSimPro.run_onetimes();
			iRadarSimPro.ShowOutBuffer(0,100);
		}
		RecIter.IsReady();												//确定FIFO有数
		int Count = 0;
		int MsStTmp = TimerCount.Ms;
		while(Count < TestCnt)
		{
			if(((RecIter->Rf & 0xFFF) >= RfLeftInterival) && ((RecIter->Rf & 0xFFF) < RfRightInterival))
			{
				PdwTmp[Count].Rf = RecIter->Rf;
				PdwTmp[Count].Pw = RecIter->Pw;
				PdwTmp[Count].Pa = RecIter->Pa&0x3F;
				PdwTmp[Count].Toa = RecIter->Toa;
				PdwTmp[Count].Ton = RecIter->Ton;
				Count++;
			}
			++RecIter;
			if(TimerCount.Seconds - SecondTmp > 60)
			{
				break;
			}
		}
		int MsEdTmp = TimerCount.Ms;
		PreToa = PdwTmp[0].Toa;
		for(int i = 0; i < Count; i++)
		{
			Pri = PdwTmp[i].Toa - PreToa;
			NLOGKSBYHEXWIDTH(4,"Rf",PdwTmp[i].Rf,"Pri",Pri,"Pw",PdwTmp[i].Pw,"Pa",PdwTmp[i].Pa,"Toa",PdwTmp[i].Toa,"Ton",PdwTmp[i].Ton);
			PreToa = PdwTmp[i].Toa;
		}
		NLOGKS("RecIterTest End!");
		NLOGK(MsEdTmp-MsStTmp);
		Timer.Stop();
		delete[] PdwTmp;
	}
	void TimerTest()
	{
		TimerCount.Seconds = 0;
		TimerCount.Ms = 0;
		Timer.Start();
		while (TimerCount.Seconds < 60)
		{
			COUTDEC;
			NLOGKWIDTH(5, TimerCount.Seconds, TimerCount.Ms);
		}
		Timer.Stop();
	}
	void TimeBeginTest()
	{
		TimerCount.Seconds = 0;
		TimerCount.Ms = 0;
		Timer.Start();
		while (TimerCount.Seconds < 60)
		{
			TimeBegin = TimerCount.Ms;
			PreProcess->ShowTimeBegin();
			PdwDiscern->ShowTimeBegin();
		}
		Timer.Stop();
	}
	void RecvIterTimerTest()
	{
		RecIter.TimerTest();
	}
	void ReadCmdTest()
	{
		int n = 0;
		while(n < 10)
		{
			SysRecvCmd = PdwDiscern->ReadInPort();
			if(SysRecvCmd != 0)
			{
				n++;
				COUTDEC;
				NLOGKSBYHEX("接收命令：",SysRecvCmd);
			}
		}
	}
	void OutMsgTest()
	{
		TimerCount.Seconds = 0;
		TimerCount.Ms = 0;
		Timer.Start();
		const short	BatchNo = 1;
		int 	OutMsgTimeSt = 0;
		const int 	DoaSt 	= 330,DoaEnd	= 360,DoaDiff	= 1;
		_MsgPdwType  MsgPdwType  = FixRfFixPri;
		_MsgPdwPara  MsgPdwPara  = {0x1200,						//Rf
									120,						//Doa
									10000,						//Pri
									10,							//Far
									1000,						//Pw
									50,							//Pa
									3,							//JaggNums
									{1000,1200,1400,0,0,0,0,0}, //PhasePeriod[8]
									3600,						//FramePeriod
									50};						//DitherRange
		_MsgScanPara MsgScanPara = {2,							//AntenaRpm
									2,							//ScanType
									10,							//ScanPeriod
									10};						//IrradiateTime
		PdwDiscern->SetMsg(LongMsgType,MsgPdwType,MsgPdwPara,BatchNo);
		PdwDiscern->ShowMsg();
		PdwDiscern->MsgExport();
		PdwDiscern->SetMsg(ShortMsgType,MsgScanPara,BatchNo);
		PdwDiscern->ShowMsg();
		PdwDiscern->MsgExport();
		int DoaTmp = DoaSt;
		while(DoaTmp <= DoaEnd)
		{
			OutMsgTimeSt = TimerCount.Seconds;
			while(TimerCount.Seconds - OutMsgTimeSt < 1)
			{
				taskDelay(0);
			};
			PdwDiscern->SetDoaUpdataMsg(BatchNo,DoaTmp,50);
			OutMsgTimeSt = TimerCount.Seconds;
			while(TimerCount.Seconds - OutMsgTimeSt < 1)
			{
				taskDelay(0);
			};
			PdwDiscern->SetDoaUpdataMsg(BatchNo,DoaTmp-6,50-6);
			OutMsgTimeSt = TimerCount.Seconds;
			while(TimerCount.Seconds - OutMsgTimeSt < 1)
			{
				taskDelay(0);
			};
			PdwDiscern->SetDoaUpdataMsg(BatchNo,DoaTmp+6,50+5);
			DoaTmp += DoaDiff;
		}
		Timer.Stop();
	}
	/*-------------------------------------- Test Section --------------------------------------*/
};
#endif
