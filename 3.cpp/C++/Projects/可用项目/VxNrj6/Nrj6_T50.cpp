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
const size_t 	MainChannelNum 			= 0x200;				//Ԥ��ѡ��ͨ����ͨ����
const size_t 	ParaNum 				= 0x100;				//ÿ��PDW����ͨ�������������
const size_t	ThresholdChgeTimeOut	= 400;					//ͨ���ɷ�ѡ��ֵ�޸�TimeOut
const size_t	AnalayLeastThreshold	= 6;					//��С�ɷ�ѡ������
const size_t 	DualRamNormalStart 		= 1;					//Ram��������׼��Чͨ����ʼֵ
const size_t 	DualRamChannelNum 		= 0x200;				//PDW����ͨ����ͨ����
const size_t 	ChannelRadarMaxNo 		= 9;					//ÿ��PDW����ͨ���Ŀɷ�ѡ�״�������
const int		AnalysisNum	 			= 0x100;				//��ʱͨ�������������
const int 		SingleChanAnalyTimeOut	= 500;					//ȫ����Ч��ͨ����ѡ��ʱ
const int 		NormalAnalyNum 			= 32;					//��ѡ��ʼ��ֵ�����Դ���ֵ���з�ѡʱ����־Ԥ����δ�Ը�ͨ��������ֵ
const int 		MutiSignalAnalyMinNum 	= 6;					//�μӶ��źŷ�ѡ����С������
const int 		HistAnalyMinNum 		= 16;					//�μ�ֱ��ͼ��ѡ����С������
const int 		SingAgileRfRgMin		= 20;					//�μӵ�ͨ���ݱ��ѡ����СRF�仯��Χ
const int 		SingChanAgileTlrc		= 100;					//�μӵ�ͨ���ݱ��ѡ�����RF�仯��Χ
const int 		MutiChanAgileTlrc		= 200;					//�μӶ�ͨ���ݱ��ѡ�����RF�仯��Χ
const int 		AgileAnaMinNum 			= 15;					//�μӽݱ��ѡ����С������
const size_t 	ContinuousPulseChannel 	= 0;					//Ram������������ͨ����
const size_t 	DiscernThreshold 		= 32;					//ÿ��PDW����ͨ���Ŀɷ�ѡ��ֵ�ĳ�ʼֵ
const int 		ScanThreshold			= 156;					//�μ�ɨ���������С������
const int		ParaUpdataTimeOut		= 3000;					//�������¼��ʱ�䳤��
const int		DoaUpdataTimeOut		= 1000;					//��λ���¼��ʱ�䳤��

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
