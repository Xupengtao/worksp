 /*
 * PdwDiscern.hpp
 *
 *  Created on: Apr 8, 2019
 *      Author: xupengtao
 */

#ifndef PDWDISCERN_DEBUG_H_
#define PDWDISCERN_DEBUG_H_
#include"PdwDiscern.hpp"

/*-------------------------------------- DEBUG Sec --------------------------------------*/
#include<string>
#include".\Tools\Macro.h"

enum _DebugStatusType
{
	DebugNotesOn	= 0x0001,
	DebugNotesOff	= 0xFFFE,
	DebugPrintOn 	= 0x0002,
	DebugPrintOff	= 0xFFFD,
	DebugClearOn 	= 0x0004,
	DebugClearOff 	= 0xFFFB,
	DebugWatchOn 	= 0x0008,
	DebugWatchOff 	= 0xFFF7,
	RecvPdwOn		= 0x0010,
	RecvPdwOff		= 0xFFEF,
	FuncStackOn		= 0x0020,
	FuncStackOff	= 0xFFDF,
	FuncStatusOn	= 0x0040,
	FuncStatusOff	= 0xFFBF
};

#define DEBUG_REGISTER(Len)	int DebugStatus;pair<size_t,string> DebugNote[Len]						//private
#define DEBUG_FUNC_REGISTER	void SetDebugStatus(int Status){DebugStatus = DebugStatus | Status;} void DelDebugStatus(int Status){DebugStatus = DebugStatus & Status;} //public		
#define DEBUG_INIT			DebugStatus = 0															//Constructe 	| Init Func
#define DEBUG_STATUS		DebugStatus																//
#define DEBUG_NOTES_ON		DebugStatus = DebugStatus | DebugNotesOn
#define DEBUG_NOTES_OFF		DebugStatus = DebugStatus & DebugNotesOff
#define DEBUG_PRINT_ON		DebugStatus = DebugStatus | DebugPrintOn
#define DEBUG_PRINT_OFF		DebugStatus = DebugStatus & DebugPrintOff
#define DEBUG_CLEAR_ON		DebugStatus = DebugStatus | DebugClearOn
#define DEBUG_CLEAR_OFF		DebugStatus = DebugStatus & DebugClearOff
#define DEBUG_WATCH_ON		DebugStatus = DebugStatus | DebugWatchOn
#define DEBUG_WATCH_OFF		DebugStatus = DebugStatus & DebugWatchOff

#define DEBUG_STATUS_SET_ARGS1(a1)						DebugStatus = DebugStatus | a1
#define DEBUG_STATUS_SET_ARGS2(a1,a2)					DebugStatus = DebugStatus | a1 | a2
#define DEBUG_STATUS_SET_ARGS3(a1,a2,a3)				DebugStatus = DebugStatus | a1 | a2 | a3
#define DEBUG_STATUS_SET_ARGS4(a1,a2,a3,a4)				DebugStatus = DebugStatus | a1 | a2 | a3 | a4
#define DEBUG_STATUS_SET_ARGS5(a1,a2,a3,a4,a5)			DebugStatus = DebugStatus | a1 | a2 | a3 | a4 | a5
#define DEBUG_STATUS_SET_ARGS6(a1,a2,a3,a4,a5,a6)		DebugStatus = DebugStatus | a1 | a2 | a3 | a4 | a5 | a6
#define DEBUG_STATUS_SET_ARGS7(a1,a2,a3,a4,a5,a6,a7)	DebugStatus = DebugStatus | a1 | a2 | a3 | a4 | a5 | a6 | a7
#define DEBUG_STATUS_SET_ARGS8(a1,a2,a3,a4,a5,a6,a7,a8)	DebugStatus = DebugStatus | a1 | a2 | a3 | a4 | a5 | a6 | a7 | a8
#define DEBUG_STATUS_SET(...)		INVOKE_VAR_MACRO(DEBUG_STATUS_SET_ARGS,__VA_ARGS__)		//Constructe 	| Init Func
#define DEBUG_STATUS_CLEAR			DebugStatus = 0											//Destructor	| Rear Func
#define DEBUG_STATUS_GET(Status)	(DebugStatus & Status)									//Destructor	| Rear Func
#define DEBUG_WATCH								\
//		COUTMSGVALEND(Result[0].ResultData.DitherRnge);
	
#define DEBUG_RUN_ARGS2(Status,Brch1)			\
		if(DEBUG_STATUS_GET(Status))			\
		{										\
			Brch1;								\
			if(DEBUG_STATUS_GET(DebugWatchOn))	\
			{									\
				DEBUG_WATCH;					\
			}									\
		}
#define DEBUG_RUN_ARGS3(Status,Brch1,Brch2)		\
		if(DEBUG_STATUS_GET(Status))			\
		{										\
			Brch1;								\
			if(DEBUG_STATUS_GET(DebugWatchOn))	\
			{									\
				DEBUG_WATCH;					\
			}									\
		}										\
		else									\
		{										\
			Brch2;								\
		}
#define DEBUG_RUN(...)					INVOKE_VAR_MACRO(DEBUG_RUN_ARGS,__VA_ARGS__)
#define DEBUG_SECSTACK_REGISTER			int SecLevel
#define DEBUG_SECSTACK_INIT				SecLevel = 0;
#define DEBUG_SECSTACK_ADD				SecLevel++
#define DEBUG_SECSTACK_DEC				SecLevel--
#define DEBUG_SECRUN(...)				INVOKE_VAR_MACRO(DEBUG_RUN_ARGS,__VA_ARGS__)
/*-------------------------------------- DEBUG Sec --------------------------------------*/

template<typename _DualRamType,
         int DualRamChannelNum,										//PDW����ͨ����ͨ����
         int ChannelRadarMaxNo,										//ÿ��PDW����ͨ���Ŀɷ�ѡ�״�������
         int DualRamNormalStart,									//Ram��������׼��Чͨ����ʼֵ
         int AnalysisNum,											//��ʱͨ�������������
         int SingleChanAnalyTimeOut,								//ȫ����Ч��ͨ����ѡ��ʱ
         int NormalAnalyNum,										//��ѡ��ʼ��ֵ�����Դ���ֵ���з�ѡʱ����־Ԥ����δ�Ը�ͨ��������ֵ
         int MutiSignalAnalyMinNum,									//�μӶ��źŷ�ѡ����С������
         int HistAnalyMinNum,										//�μ�ֱ��ͼ��ѡ����С������
         int SingAgileRfRgMin,										//�μӵ�ͨ���ݱ��ѡ����СRF�仯��Χ
         int SingChanAgileTlrc,										//�μӵ�ͨ���ݱ��ѡ�����RF�仯��Χ
         int MutiChanAgileTlrc,										//�μӶ�ͨ���ݱ��ѡ�����RF�仯��Χ
         int AgileAnaMinNum,										//�μӽݱ��ѡ����С������
         int ScanThreshold,											//�μ�ɨ���������С������
		 int ParaUpdataTimeOut,										//�������¼��ʱ�䳤��
		 int DoaUpdataTimeOut>										//��λ���¼��ʱ�䳤��
class _PdwDiscernDebug: public _PdwDiscern<_DualRamType,
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
                                           DoaUpdataTimeOut>
{
	typedef _PdwDiscern<_DualRamType,
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
					    DoaUpdataTimeOut>	_PdwDiscernType;
	DEBUG_REGISTER(20);
	_AnalyChannelType 	*ResultAnalyChannel;
public:
	DEBUG_FUNC_REGISTER
	_PdwDiscernDebug()
	{
		DEBUG_INIT;
		DEBUG_STATUS_SET(
				DebugNotesOn,
				DebugPrintOn,
				DebugClearOn,
				DebugWatchOn,
				RecvPdwOn,
				FuncStackOn,
				FuncStatusOn
				);
		ResultAnalyChannel = new _AnalyChannelType[900];
	}
	_PdwDiscernDebug(_DualRamType 	&DualRam_,													//ʹ�ø���ʵ�����������һ��_DualRamType���͵Ķ�������
					 _TimerCount  	&TimerCount_,
					 size_t 		&TimeBegin_,
					 _SysCmd 		&SystemCmd_)
										:_PdwDiscernType(DualRam_,TimerCount_,TimeBegin_,SystemCmd_)
	{
		DEBUG_INIT;
		DEBUG_STATUS_SET(
				DebugNotesOn,
				DebugPrintOn,
				DebugClearOn,
				DebugWatchOn,
				RecvPdwOn,
				FuncStackOn,
				FuncStatusOn
				);
		ResultAnalyChannel = new _AnalyChannelType[900];
	}
	_PdwDiscernDebug(_PdwDiscernType &PdwDiscern_)
										:_PdwDiscernType(PdwDiscern_)
	{
		ResultAnalyChannel = new _AnalyChannelType[900];
	}
	virtual ~_PdwDiscernDebug()
	{
		DEBUG_STATUS_CLEAR;
		delete[] ResultAnalyChannel;
	}
	virtual void 	RstBand(int BandNo)
	{
		_PdwDiscernType::RstBand(BandNo);
	}
	virtual void 	run(size_t& CtrlSign, size_t& TaskDeleteSign)																		//������
	{
		DEBUG_RUN(FuncStackOn,
				DEBUG_RUN(FuncStatusOn,
						COUTFUNCEND("����"),
							COUTFUNC("����")));
		DEBUG_RUN(FuncStatusOn,
				COUTSECBEG("��ʼ����"));
		_PdwDiscernType::run(CtrlSign,TaskDeleteSign);
		DEBUG_RUN(FuncStatusOn,
				COUTSECEND("ֹͣ����"));
	}
	virtual int  	CollectPulseToAnalyChannel()												//��Ram����������ͨ��Rf��AnalyChanRfTol��Χ�ڵ��������뵽��ʱͨ��
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("��ͨ���ռ�����"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::CollectPulseToAnalyChannel();
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVALEND(GetAnalyPulseNums()));
		return Rtn;
	}
	virtual int  	CollectPulseToAnalyChannel(int ChannelNo)									//��Ram������ChannelNoͨ��Rf�ڣ�Cmptn[i].Rfl,Cmptn[i].Rfh����Χ�ڵ��������뵽��ʱͨ��
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�ݱ䵥ͨ���ռ�����"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::CollectPulseToAnalyChannel(ChannelNo);
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVAL(ChannelNo);
				COUTMSGVALEND(GetAnalyPulseNums());
				ShowAnalyChannel());
		return Rtn;
	}
	virtual int 	ClctAgilePulseToAnalChan(int *AgiChanNo, int ValidAgiChLens)				//��ͨ���ݱ��������뵽��ʱͨ��, ToaTy Restrict
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�ݱ��ͨ���ռ�����"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::ClctAgilePulseToAnalChan(AgiChanNo,ValidAgiChLens);
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVALEND(GetAnalyPulseNums()));
		return Rtn;
	}
	virtual int		AnalChanDeductPrcs()														//ȥ������ͨ�����ѷ�ѡ����
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("��ʱͨ���ѷ�������۳�"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AnalChanDeductPrcs();
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVAL(GetAnalyPulseNums());
				ShowAnalyChannel());
		return Rtn;
	}
	virtual void 	CancelDeduct()																//ȡ��Ram����������ͨ������۳���־���������ѷ�����־
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("��ʱͨ��ȡ���۳���־"));
		_PdwDiscernType::CancelDeduct();
	}
	virtual bool	AllChanAnalysisedJudge()													//Ram��������Чͨ����ɷ�ѡ�ж�
	{
		bool Rtn = _PdwDiscernType::AllChanAnalysisedJudge();
		if(Rtn == true)
		{
			DEBUG_RUN(FuncStackOn,
					COUTMSGSTR("��ͨ�������źŷ�ѡ���!"));
		}
		return Rtn;
	}
	virtual int  	SingleChanAnalysis()														//Ԥ����ֵ��ͨ��ѡ
	{
		DEBUG_RUN(FuncStackOn,
				DEBUG_RUN(FuncStatusOn,
						COUTFUNCEND("��ͨ���źŷ�ѡ");
						COUTSECBEG("SingleChanAnalysis"),
								COUTFUNC("��ͨ���źŷ�ѡ")));
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVAL(ChanNo));
		int Rtn = 0;
		Rtn = _PdwDiscernType::SingleChanAnalysis();
		DEBUG_RUN(FuncStatusOn,
				COUTSECEND("SingleChanAnalysis"));
		return Rtn;
	}
	virtual int 	HistogramDiscern()															//ֱ��ͼ��ѡ����ѡ˳�򣺲β� -> �̶� -> ����
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("ֱ��ͼ��ѡ"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::HistogramDiscern();
		return Rtn;
	}
	virtual int  	JaggDiscern()																//�β��źŷ�ѡ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�β��źŷ�ѡ"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::JaggDiscern();
		return Rtn;
	}
	virtual int  	FixDiscern()																//�̶��źŷ�ѡ 
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�̶��źŷ�ѡ"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::FixDiscern();
		return Rtn;
	}
	virtual int  	DitherDiscern(int A1, int A2)												//�����źŷ�ѡ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����źŷ�ѡ"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::DitherDiscern(A1, A2);
		return Rtn;
	}
	virtual int 	MutiSignalDiscern()															//�ಿ�źŷ�ѡ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("���м������źŷ�ѡ"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::MutiSignalDiscern();
		return Rtn;
	}
	virtual void 	SingChanAgileDiscern()														//��ͨ���ݱ��źŷ�ѡ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("��ͨ���ݱ��źŷ�ѡ"));
		_PdwDiscernType::SingChanAgileDiscern();
	}
	virtual int 	AgainDiscern()																//�ݱ������ٴ��źŷ�ѡ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�ݱ��źŷ�ѡ"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AgainDiscern();
		return Rtn;
	}
	virtual int 	AgileDitherDiscern()														//�ݱ䶶���źŷ�ѡ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�ݱ䶶���źŷ�ѡ"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AgileDitherDiscern();
		return Rtn;
	}
	virtual void 	DitherParaAnaly(ToaTy *PriArray, ToaTy PriRef, int PriInitSt)				//���ýݱ䶶������
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����źŲ�������"));
		_PdwDiscernType::DitherParaAnaly(PriArray, PriRef, PriInitSt);
	}
	virtual void 	MutiChanAgileDiscern()           											//��ͨ���ݱ��źŷ�ѡ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("��ͨ���ݱ��źŷ�ѡ"));
		_PdwDiscernType::MutiChanAgileDiscern();
	}
	virtual int 	T50ClctAgilePulseToAnalChan(int *AgiChanNo, int ValidAgiChLens)				//��ͨ���ݱ��������뵽��ʱͨ��, ToaTy Restrict
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�ݱ��ź��ռ�����"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::T50ClctAgilePulseToAnalChan(AgiChanNo,ValidAgiChLens);
		return Rtn;
	}
	virtual void 	T50MutiChanAgileDiscern()           										//��ͨ���ݱ��źŷ�ѡ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("��ͨ���ݱ��źŷ�ѡ"));
		_PdwDiscernType::T50MutiChanAgileDiscern();
	}
	virtual void 	ParaUpdate()        														//��������
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("��������"));
		_PdwDiscernType::ParaUpdate();
	}
	virtual void 	DoaUpdata()        															//��λ����
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("��λ����"));
		_PdwDiscernType::DoaUpdata();
		DEBUG_RUN(FuncStackOn,
				COUTEND);
	}
	virtual void 	DisappearJudge()        													//����ʧ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�ź���ʧ�ж�"));
		_PdwDiscernType::DisappearJudge();
	}
	virtual void 	ScanAnalysis()																//ɨ�����
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("ɨ�����"));
		_PdwDiscernType::ScanAnalysis();
	}
	virtual void 	SetScanPara(int ResultNo)													//��ͨ��ɨ�����
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("ɨ���������"));
		_PdwDiscernType::SetScanPara(ResultNo);
	}
	virtual int 	ResultTmpRfPwPaSet(_AddressPairType *AddrPartArray,int Lens)				//����ResultTmp��Rf��Pw��Pa����������PaAnalArray��Ч����,��������ΪAddressPartArray
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����Rf��Pw��Pa���ã���ַ�ԣ�"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::ResultTmpRfPwPaSet(AddrPartArray, Lens);
		return Rtn;
	}
	virtual int 	ResultTmpRfPwPaSet(size_t *BarLocArray,int Lens)							//����ResultTmp��Rf��Pw��Pa����������PaAnalArray��Ч����,��������ΪBarLocArray
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����Rf��Pw��Pa���ã�ֱ��ͼ��"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::ResultTmpRfPwPaSet(BarLocArray, Lens);
		return Rtn;
	}
	virtual float 	MostPwSet(int n)															//Pw�ĺ���ֵ����
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("Pw��ѡ�˸�����һ���ֵ"));
		float Rtn = 0;
		Rtn = _PdwDiscernType::MostPwSet(n);
		return Rtn;
	}
	virtual int 	CalculateDoa(int PaNums)													//��ȡPaAnalArray����Ч����ʹ�ñȷ�������Doa
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�ȷ�����Doa����"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::CalculateDoa(PaNums);
		return Rtn;
	}
	virtual int 	SeekPaToDoa(int *PaArray, int PaArrayLens, int *OutArray, int Gate)			//��־λ�ռ�PaArray��������Ч������OutArray������ֵΪOutArray��Ч������������Gate������0
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("������������������ֵ"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::SeekPaToDoa(PaArray, PaArrayLens, OutArray, Gate);
		return Rtn;
	}
	virtual int 	FindLorR(int AntenaNo, int AnalyNums)										//��������Ч����������ֵ
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�ȷ�����Doa����"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::FindLorR(AntenaNo, AnalyNums);
		return Rtn;
	}
	virtual int 	FindValidPa(int PaNums, int *PaArray = NULL)								//ȡ���ֵ�������������ޣ��򷵻ظ�ֵλ��,����ȡ�δ�ֵ����������������ޣ��򷵻ظ�ֵλ��,����������������ޣ��򷵻ظ�������ֵλ��
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("������ЧPa"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::FindValidPa(PaNums, PaArray);
		return Rtn;
	}
	virtual void 	ResultOutput()																//��������
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�������ں���"));
		_PdwDiscernType::ResultOutput();
	}
	virtual int 	CompPara(int i)																//��ѡ�״���ʱ�����ͬ���������Ա�
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("���������Ա�"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::CompPara(i);
		return Rtn;
	}
	virtual int 	DoubleModJudge(int Prf1, int Prf2, int DeltaPrf)							//�����ж��Ƿ����������ݲΧ��
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����Prf�����ж�"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::DoubleModJudge(Prf1, Prf2, DeltaPrf);
		return Rtn;
	}
	virtual int 	AnalyIdenticalDoa(int i)													//��ͬDoa�ۼ��滻
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�������ͬDoa����"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AnalyIdenticalDoa(i);
		return Rtn;
	}
	virtual int 	AnalyDifferentDoa(int i,int DeltaDoa = 10)									//��ͬDoa�ۼ��滻
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����ͬDoa����"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AnalyDifferentDoa(i, DeltaDoa);
		return Rtn;
	}
	virtual void 	FoundResult(int ResultNo, int CompParaRtn)									//��ǰ��ѡ�ͺ��ڽ�����з���
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("���ҵ������"));
		_PdwDiscernType::FoundResult(ResultNo, CompParaRtn);
	}
	virtual int 	NotFoundResult()															//��ǰ��ѡ�ͺ��ڽ������δ����
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("δ���ҵ������"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::NotFoundResult();
		ResultAnalyChannel[ResultNums - 1] = AnalyChannel;
		return Rtn;
	}
	virtual void 	AnalyDiffTypeAndPri(int i)													//��ͬ���ͺͲ�ͬPri���ۼ��滻
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����ͬ���ͺ�Pri����"));
		_PdwDiscernType::AnalyDiffTypeAndPri(i);
	}
	virtual void 	SetResultOtherPara(int i)													//���ý������������
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����������������"));
		_PdwDiscernType::SetResultOtherPara(i);
	}
	virtual void 	MsgExport()																	//ֱ�����Msg
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("������������"));
		_PdwDiscernType::MsgExport();
	}
	virtual void 	MsgExport(int ResultNo)														//Q�澯
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("������������"));
		_PdwDiscernType::MsgExport(ResultNo);
	}
	virtual void 	SetMsg(int ResultNo)														//���ݽ���������������
	{
		if(Result[ResultNo].ResultData.Type == 3)
		{
			if(Result[ResultNo].ResultData.JaggNums == 0)
			{
				COUTMSGVAL("Track it!");
			}
		}
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�������ò���"));
		_PdwDiscernType::SetMsg(ResultNo);
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd)														//�ֶ������������  All Type Not reply edit
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�������ò���"));
		_PdwDiscernType::SetMsg(MsgCmd);
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd,_MsgPdwType &Ty,_MsgPdwPara &PdwPara,USHORT BatchNo)	//�ֶ������������ LongMsgType ParaUpdataMsgType reply edit
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�������ò���"));
		_PdwDiscernType::SetMsg(MsgCmd, Ty, PdwPara, BatchNo);
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd,_MsgScanPara &MsgScanPara,USHORT BatchNo)				//�ֶ������������ ShortMsgType reply edit
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�������ò���"));
		_PdwDiscernType::SetMsg(MsgCmd, MsgScanPara, BatchNo);
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd,USHORT BatchNo)										//�ֶ������������ MissingMsgType reply edit
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�������ò���"));
		_PdwDiscernType::SetMsg(MsgCmd, BatchNo);
	}
	virtual void 	Clear()																		//ȫ���
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNCEND("�������������"));
		_PdwDiscernType::Clear();
	}
	virtual void 	ClearTmp()																	//��ʱͨ���������
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����������"));
		_PdwDiscernType::ClearTmp();
	}
	virtual void 	ClearResultDataDoa(int i)
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("�����Doa�������"));
		_PdwDiscernType::ClearResultDataDoa(i);
	}
	virtual void	ShowResult(size_t ResultNo) const
	{
		ToaTy PreToa = 0, Pri = 0;
		for(int i = 0; i < NormalAnalyNum; i++)
		{
			Pri = ResultAnalyChannel[ResultNo].Toa[i] - PreToa;
			NLOGKSBYHEXWIDTH(4,"No",i,
							   "Rf",ResultAnalyChannel[ResultNo].Rf[i],
							   "Pw",ResultAnalyChannel[ResultNo].Pw[i],
							   "Pri",Pri,
							   "Toa",ResultAnalyChannel[ResultNo].Toa[i],
							   "Ton",ResultAnalyChannel[ResultNo].Ton[i]
							 );
			PreToa = ResultAnalyChannel[ResultNo].Toa[i];
		}
		_PdwDiscernType::ShowResult(ResultNo);
	}
	virtual void	ShowResult(size_t ResultNoSt, size_t ResultNoEnd) const
	{
		_PdwDiscernType::ShowResult(ResultNoSt,ResultNoEnd);
	}
};

#endif /* PDWDISCERN_DEBUG_H_ */
