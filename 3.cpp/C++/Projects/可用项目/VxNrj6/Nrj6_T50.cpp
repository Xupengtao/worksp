/*
 * Nrj6_T50
 *
 *  Created on: Jun 27, 2019
 *      Author: xupengtao
 */
#include"NrjxMain.hpp"
#if PPC
/*------------------------------- StaticInit Section ----------------------------*/
template<typename Ty>
SEM_ID _StaticSemVal<Ty>::Sem 	= semBCreate(SEM_Q_FIFO,SEM_FULL);
/*------------------------------- StaticInit Section ----------------------------*/
#endif

typedef USHORT RfTy,PwTy,PaTy,ToaTy,TonTy;
const size_t 	MainChannelNum 			= 0x200;				//预分选主通道的通道数
const size_t 	ParaNum 				= 0x100;				//每个PDW数据通道的最大脉冲数
const size_t	ThresholdChgeTimeOut	= 400;					//通道可分选阈值修改TimeOut
const size_t	AnalayLeastThreshold	= 6;					//最小可分选脉冲数
const size_t 	DualRamNormalStart 		= 1;					//Ram缓冲区标准有效通道起始值
const size_t 	DualRamChannelNum 		= 0x200;				//PDW数据通道的通道数
const size_t 	ChannelRadarMaxNo 		= 9;					//每个PDW数据通道的可分选雷达的最大数
const int		AnalysisNum	 			= 0x100;				//临时通道的最大脉冲数
const int 		SingleChanAnalyTimeOut	= 500;					//全部有效单通道分选超时
const int 		NormalAnalyNum 			= 32;					//分选初始阈值，当以次阈值进行分选时，标志预处理并未对该通道调整阈值
const int 		MutiSignalAnalyMinNum 	= 6;					//参加多信号分选的最小脉冲数
const int 		HistAnalyMinNum 		= 16;					//参加直方图分选的最小脉冲数
const int 		SingAgileRfRgMin		= 20;					//参加单通道捷变分选的最小RF变化范围
const int 		SingChanAgileTlrc		= 100;					//参加单通道捷变分选的最大RF变化范围
const int 		MutiChanAgileTlrc		= 200;					//参加多通道捷变分选的最大RF变化范围
const int 		AgileAnaMinNum 			= 15;					//参加捷变分选的最小脉冲数
const size_t 	ContinuousPulseChannel 	= 0;					//Ram缓冲区连续波通道号
const size_t 	DiscernThreshold 		= 32;					//每个PDW数据通道的可分选阈值的初始值
const int 		ScanThreshold			= 156;					//参加扫描分析的最小脉冲数
const int		ParaUpdataTimeOut		= 3000;					//参数更新间隔时间长度
const int		DoaUpdataTimeOut		= 1000;					//方位更新间隔时间长度

typedef   _NrjxMain<RfTy, PwTy, PaTy, ToaTy, TonTy,
					MainChannelNum,
					ParaNum,
					ThresholdChgeTimeOut,
					AnalayLeastThreshold,
					DualRamNormalStart,
					DualRamChannelNum,
					ChannelRadarMaxNo,
					AnalysisNum,
					SingleChanAnalyTimeOut,
					NormalAnalyNum,
					MutiSignalAnalyMinNum,
					HistAnalyMinNum,
					SingAgileRfRgMin,
					SingChanAgileTlrc,
					MutiChanAgileTlrc,
					AgileAnaMinNum,
					ContinuousPulseChannel,
					DiscernThreshold,
					ScanThreshold,
					ParaUpdataTimeOut,
					DoaUpdataTimeOut> 	_NrjxMainType;

_NrjxMainType NrjxMain(0,100);

void Help()
{
	NrjxMain.ShowHelp();
}

void Task()
{
	NrjxMain.ShowTask();
}

void TaskDelete()
{
	NrjxMain.ShowTaskDelete();
}

void AnalyChannel()
{
	NrjxMain.ShowAnalyChannel();
}

void Channel(size_t ChanNoSt = 0, size_t ChanNoEnd = 0)
{
	NrjxMain.ShowChannel(ChanNoSt, ChanNoEnd);
}

void LastTimeOfResult()
{
	NrjxMain.ShowLastTimeOfResult();
}

void Result(size_t ResultNoSt = 0, size_t ResultNoEnd = 0)
{
	NrjxMain.ShowResult(ResultNoSt, ResultNoEnd);
}

void HistogramPrint()
{
	NrjxMain.ShowHistogram();
}

void WhichChannel(int SearchRf)
{
	NrjxMain.ShowWhichChannel(SearchRf);
}

void Status()
{
	NrjxMain.ShowStatus();
}

void RecTest(size_t RfLeftInterival, size_t RfRightInterival, size_t TestCnt)
{
	NrjxMain.RecIterTest(RfLeftInterival,RfRightInterival,TestCnt);
}

void OutTest()
{
	NrjxMain.OutMsgTest();
}

void ChangeCmdType(int CmdType)
{
	NrjxMain.SetRunType(CmdType);
}

void TaskStart()
{
	NrjxMain.WorkStart();
}

void TaskStop()
{
	NrjxMain.WorkStop();
}

void ChangeDbgType(int DbgType)
{
	NrjxMain.ChangeDebugMode(DbgType);
}

void SetDbgMode(int Status)
{
	NrjxMain.SetDebugMode(Status);
}

void DelDbgMode(int Status)
{
	NrjxMain.DelDebugMode(Status);
}

int main()
{
	NrjxMain.ShowStatus();
	NrjxMain.Run();
	return 1;
}
