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

template<typename RfTy,typename PwTy,typename PaTy,typename ToaTy,typename TonTy,	//PDW������������������
		 size_t 	MainChannelNum,													//Ԥ��ѡ��ͨ����ͨ����
		 size_t 	ParaNum,														//ÿ��PDW����ͨ�������������
		 size_t		ThresholdChgeTimeOut,											//ͨ���ɷ�ѡ��ֵ�޸�TimeOut
		 size_t		AnalayLeastThreshold,											//��С�ɷ�ѡ������
		 size_t 	DualRamNormalStart,												//Ram��������׼��Чͨ����ʼֵ
		 size_t 	DualRamChannelNum,												//PDW����ͨ����ͨ����
		 size_t 	ChannelRadarMaxNo,												//ÿ��PDW����ͨ���Ŀɷ�ѡ�״�������
         int		AnalysisNum,													//��ʱͨ�������������
         int 		SingleChanAnalyTimeOut,											//ȫ����Ч��ͨ����ѡ��ʱ
         int 		NormalAnalyNum,													//��ѡ��ʼ��ֵ�����Դ���ֵ���з�ѡʱ����־Ԥ����δ�Ը�ͨ��������ֵ
         int 		MutiSignalAnalyMinNum,											//�μӶ��źŷ�ѡ����С������
         int 		HistAnalyMinNum,												//�μ�ֱ��ͼ��ѡ����С������
         int 		SingAgileRfRgMin,												//�μӵ�ͨ���ݱ��ѡ����СRF�仯��Χ
         int 		SingChanAgileTlrc,												//�μӵ�ͨ���ݱ��ѡ�����RF�仯��Χ
         int 		MutiChanAgileTlrc,												//�μӶ�ͨ���ݱ��ѡ�����RF�仯��Χ
         int 		AgileAnaMinNum,													//�μӽݱ��ѡ����С������
		 size_t 	ContinuousPulseChannel,											//Ram������������ͨ����
		 size_t 	DiscernThreshold,												//ÿ��PDW����ͨ���Ŀɷ�ѡ��ֵ�ĳ�ʼֵ
		 int 		ScanThreshold,													//�μ�ɨ���������С������
		 int 		ParaUpdataTimeOut,												//�������¼��ʱ�䳤��
		 int 		DoaUpdataTimeOut>												//��λ���¼��ʱ�䳤��	 
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
	_RecvIterType 					RecIter;										//signal����ӿڵ�������,���ö�ʱ��1
	_Timer							Timer;											//��ʱ��0
	_TimerCount						TimerCount;
	int 							TimerMsInterival;								//��ʱ�������жϼ��
	_DualRamType 					DualRam;
	_PreProcessType 				*PreProcess;
	_PdwDiscernType 				*PdwDiscern;
	_SysCmd							SystemCmd;
	size_t							SysRecvCmd;
	size_t							CtrlSign;										//���Ʊ�־
	size_t							DebugType;
	size_t							TimeBegin;										//һ�η�ѡ��ʼʱ�䣬��Ԥ����дֵ��Ԥ������������ֵ
	size_t 							RunType;										//����ʱ������ķ�ʽ��0�����ڣ�1:���ַ���2���ַ���
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
					iRadarSimPro.Init();											/* ��ͨ�� 3 ��Ƶ�̶���Ƶ�̶�  */
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
			COUTSTREND("��������ģʽ�л��� ---> ��������");
			COUTSTREND("Command type changed to ---> SerialPort");
			break;
		case CinGet:
			COUTSTREND("��������ģʽ�л��� ---> ��������");
			COUTSTREND("Command type changed to ---> CinGet");
			break;
		case StrCmdGet:
			COUTSTREND("��������ģʽ�л��� ---> �ַ�������");
			COUTSTREND("Command type changed to ---> StrCmdGet");
			break;
		default:
			COUTSTREND("�����ʽ���󣬲ο�:   0->��������   1->��������   2->�ַ�������");
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
		COUTSTREND("�������״�����:");
		cin>>s;
		StringToInt<<s;
		StringToInt>>MsgPdwType;
		COUT(MsgPdwType);
		COUTSTREND("�������״����:");
		cin>>RadarSn;
		COUT(RadarSn);
		COUTSTREND("��������ƵRf:");
		cin>>Rf;
		COUT(Rf);
		COUTSTREND("��������ƵPri:");
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
		NLOGKSWIDTH(4,"���κ�(BandNo)			",BandNo);
		NLOGKSWIDTH(4,"���Ʊ�־(CtrlSign)		",CtrlSign);
		NLOGKSWIDTH(4,"��ѡ��ʼʱ��(TimeBegin)	",TimeBegin);
		NLOGKSWIDTH(4,"Ԥ��ѡͨ����(MainChann...)	",MainChannelNum);				//Ԥ��ѡ��ͨ����ͨ����
		NLOGKSWIDTH(4,"ͨ��������(ParaNum)		"	 ,ParaNum);						//ÿ��PDW����ͨ�������������
		NLOGKSWIDTH(4,"��ֵ��ʼֵ(DiscernThr...)	",DiscernThreshold);			//ÿ��PDW����ͨ���Ŀɷ�ѡ��ֵ�ĳ�ʼֵ
		NLOGKSWIDTH(4,"��ֵ���ó�ʱ(Threshold...)	",ThresholdChgeTimeOut);		//ͨ���ɷ�ѡ��ֵ�޸�TimeOut
		NLOGKSWIDTH(4,"��С��ѡ������(AnalayLe...)	",AnalayLeastThreshold);		//��С�ɷ�ѡ������
		NLOGKSWIDTH(4,"������ͨ����(Continuou...)	",ContinuousPulseChannel);		//Ram������������ͨ����
		NLOGKSWIDTH(4,"��׼ͨ����ʼ(DualRamNo...)	",DualRamNormalStart);			//Ram��������׼��Чͨ����ʼֵ
		NLOGKSWIDTH(4,"PDWͨ����(DualRamCha...)	",DualRamChannelNum);			//PDW����ͨ����ͨ����
		NLOGKSWIDTH(4,"ÿͨ���״���(ChannelRa...)	",ChannelRadarMaxNo);			//ÿ��PDW����ͨ���Ŀɷ�ѡ�״�������
        NLOGKSWIDTH(4,"��ʱͨ��������(Analysis...)	",AnalysisNum);					//��ʱͨ�������������
        NLOGKSWIDTH(4,"��ͨ����ѡ��ʱ(SingleCh...)	",SingleChanAnalyTimeOut);		//ȫ����Ч��ͨ����ѡ��ʱ
        NLOGKSWIDTH(4,"��ѡ��ʼ��ֵ(NormalAna...)	",NormalAnalyNum);				//��ѡ��ʼ��ֵ�����Դ���ֵ���з�ѡʱ����־Ԥ����δ�Ը�ͨ��������ֵ
        NLOGKSWIDTH(4,"���źŷ�ѡ������(MutiSig...)	",MutiSignalAnalyMinNum);		//�μӶ��źŷ�ѡ����С������
        NLOGKSWIDTH(4,"ֱ��ͼ��ѡ������(HistAna...)	",HistAnalyMinNum);				//�μ�ֱ��ͼ��ѡ����С������
        NLOGKSWIDTH(4,"��Ch�ݱ�RF��С��Χ(Sing...)"	 ,SingAgileRfRgMin);			//�μӵ�ͨ���ݱ��ѡ����СRF�仯��Χ
        NLOGKSWIDTH(4,"��Ch�ݱ�RF���Χ(Sing...)"	 ,SingChanAgileTlrc);			//�μӵ�ͨ���ݱ��ѡ�����RF�仯��Χ
        NLOGKSWIDTH(4,"��Ch�ݱ�RF���Χ(Muti...)"	 ,MutiChanAgileTlrc);			//�μӶ�ͨ���ݱ��ѡ�����RF�仯��Χ
        NLOGKSWIDTH(4,"�ݱ���С������(AgileAna...)	",AgileAnaMinNum);				//�μӽݱ��ѡ����С������
		NLOGKSWIDTH(4,"ɨ�����������(ScanThre...)	",ScanThreshold);				//�μ�ɨ���������С������
		NLOGKSWIDTH(4,"��������TimeOut(ParaU...)	",ParaUpdataTimeOut);			//��������TimeOut
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
		iRadarSimPro.Init();																	/* ��ͨ�� 3 ��Ƶ�̶���Ƶ�̶�  */
		iRadarSimPro.RadarSimGen(FixRfFixPri, 0, Rf, 1000);
		iRadarSimPro.RadarSimGen(FixRfFixPri, 1, Rf, 3223);
		iRadarSimPro.RadarSimGen(FixRfFixPri, 2, Rf, 5215);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* ��ͨ�� 1 ��Ƶ�̶���Ƶ�β�  */
		iRadarSimPro.RadarSimGen(FixRfJaggPri, 0, Rf, (UINT)PriJaggArr, 0, 8);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* ��ͨ�� 1 ��Ƶ�̶���Ƶ����  */
		iRadarSimPro.RadarSimGen(FixRfDitherPri, 0, Rf, Pri, 0, PriDitherRange);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* ��ͨ�� 1 ��Ƶ�ݱ���Ƶ�̶�  */
		iRadarSimPro.RadarSimGen(AgileRfFixPri, 0, (UINT)RfAgileArr, Pri, 8);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* ��ͨ�� 1 ��Ƶ�ݱ���Ƶ����  */
		iRadarSimPro.RadarSimGen(AgileRfDitherPri, 0, (UINT)RfAgileArr, Pri, 8, PriDitherRange);
		iRadarSimPro.run_onetimes();
		iRadarSimPro.ShowOutBuffer(0,100);
		iRadarSimPro.Init();																	/* ��ͨ�� 9 ��Ƶ�̶���Ƶ�̶�  */
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
			iRadarSimPro.Init();										/* ��ͨ�� 3 ��Ƶ�̶���Ƶ�̶�  */
			iRadarSimPro.RadarSimGen(FixRfFixPri, 0, 1000, 1000);
			iRadarSimPro.run_onetimes();
			iRadarSimPro.ShowOutBuffer(0,100);
		}
		RecIter.IsReady();												//ȷ��FIFO����
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
				NLOGKSBYHEX("�������",SysRecvCmd);
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
