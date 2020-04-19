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
	_PdwDiscernDebug(_DualRamType 	&DualRam_,													//使用该类实例化对象，需绑定一个_DualRamType类型的对象引用
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
	virtual void 	run(size_t& CtrlSign, size_t& TaskDeleteSign)																		//主函数
	{
		DEBUG_RUN(FuncStackOn,
				DEBUG_RUN(FuncStatusOn,
						COUTFUNCEND("运行"),
							COUTFUNC("运行")));
		DEBUG_RUN(FuncStatusOn,
				COUTSECBEG("开始运行"));
		_PdwDiscernType::run(CtrlSign,TaskDeleteSign);
		DEBUG_RUN(FuncStatusOn,
				COUTSECEND("停止运行"));
	}
	virtual int  	CollectPulseToAnalyChannel()												//将Ram缓冲区分析通道Rf在AnalyChanRfTol范围内的脉冲输入到临时通道
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("单通道收集脉冲"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::CollectPulseToAnalyChannel();
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVALEND(GetAnalyPulseNums()));
		return Rtn;
	}
	virtual int  	CollectPulseToAnalyChannel(int ChannelNo)									//将Ram缓冲区ChannelNo通道Rf在（Cmptn[i].Rfl,Cmptn[i].Rfh）范围内的脉冲输入到临时通道
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("捷变单通道收集脉冲"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::CollectPulseToAnalyChannel(ChannelNo);
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVAL(ChannelNo);
				COUTMSGVALEND(GetAnalyPulseNums());
				ShowAnalyChannel());
		return Rtn;
	}
	virtual int 	ClctAgilePulseToAnalChan(int *AgiChanNo, int ValidAgiChLens)				//多通道捷变脉冲输入到临时通道, ToaTy Restrict
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("捷变多通道收集脉冲"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::ClctAgilePulseToAnalChan(AgiChanNo,ValidAgiChLens);
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVALEND(GetAnalyPulseNums()));
		return Rtn;
	}
	virtual int		AnalChanDeductPrcs()														//去除分析通道中已分选脉冲
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("临时通道已分析脉冲扣除"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AnalChanDeductPrcs();
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVAL(GetAnalyPulseNums());
				ShowAnalyChannel());
		return Rtn;
	}
	virtual void 	CancelDeduct()																//取消Ram缓冲区分析通道脉冲扣除标志并置脉冲已分析标志
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("临时通道取消扣除标志"));
		_PdwDiscernType::CancelDeduct();
	}
	virtual bool	AllChanAnalysisedJudge()													//Ram缓冲区有效通道完成分选判断
	{
		bool Rtn = _PdwDiscernType::AllChanAnalysisedJudge();
		if(Rtn == true)
		{
			DEBUG_RUN(FuncStackOn,
					COUTMSGSTR("单通道常规信号分选完毕!"));
		}
		return Rtn;
	}
	virtual int  	SingleChanAnalysis()														//预设阈值普通分选
	{
		DEBUG_RUN(FuncStackOn,
				DEBUG_RUN(FuncStatusOn,
						COUTFUNCEND("单通道信号分选");
						COUTSECBEG("SingleChanAnalysis"),
								COUTFUNC("单通道信号分选")));
		DEBUG_RUN(FuncStatusOn,
				COUTMSGVAL(ChanNo));
		int Rtn = 0;
		Rtn = _PdwDiscernType::SingleChanAnalysis();
		DEBUG_RUN(FuncStatusOn,
				COUTSECEND("SingleChanAnalysis"));
		return Rtn;
	}
	virtual int 	HistogramDiscern()															//直方图分选，分选顺序：参差 -> 固定 -> 抖动
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("直方图分选"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::HistogramDiscern();
		return Rtn;
	}
	virtual int  	JaggDiscern()																//参差信号分选
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("参差信号分选"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::JaggDiscern();
		return Rtn;
	}
	virtual int  	FixDiscern()																//固定信号分选 
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("固定信号分选"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::FixDiscern();
		return Rtn;
	}
	virtual int  	DitherDiscern(int A1, int A2)												//抖动信号分选
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("抖动信号分选"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::DitherDiscern(A1, A2);
		return Rtn;
	}
	virtual int 	MutiSignalDiscern()															//多部信号分选
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("序列检索法信号分选"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::MutiSignalDiscern();
		return Rtn;
	}
	virtual void 	SingChanAgileDiscern()														//单通道捷变信号分选
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("单通道捷变信号分选"));
		_PdwDiscernType::SingChanAgileDiscern();
	}
	virtual int 	AgainDiscern()																//捷变变参数再次信号分选
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("捷变信号分选"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AgainDiscern();
		return Rtn;
	}
	virtual int 	AgileDitherDiscern()														//捷变抖动信号分选
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("捷变抖动信号分选"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AgileDitherDiscern();
		return Rtn;
	}
	virtual void 	DitherParaAnaly(ToaTy *PriArray, ToaTy PriRef, int PriInitSt)				//设置捷变抖动参数
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("抖动信号参数分析"));
		_PdwDiscernType::DitherParaAnaly(PriArray, PriRef, PriInitSt);
	}
	virtual void 	MutiChanAgileDiscern()           											//多通道捷变信号分选
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("多通道捷变信号分选"));
		_PdwDiscernType::MutiChanAgileDiscern();
	}
	virtual int 	T50ClctAgilePulseToAnalChan(int *AgiChanNo, int ValidAgiChLens)				//多通道捷变脉冲输入到临时通道, ToaTy Restrict
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("捷变信号收集脉冲"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::T50ClctAgilePulseToAnalChan(AgiChanNo,ValidAgiChLens);
		return Rtn;
	}
	virtual void 	T50MutiChanAgileDiscern()           										//多通道捷变信号分选
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("多通道捷变信号分选"));
		_PdwDiscernType::T50MutiChanAgileDiscern();
	}
	virtual void 	ParaUpdate()        														//参数更新
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("参数更新"));
		_PdwDiscernType::ParaUpdate();
	}
	virtual void 	DoaUpdata()        															//方位更新
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("方位更新"));
		_PdwDiscernType::DoaUpdata();
		DEBUG_RUN(FuncStackOn,
				COUTEND);
	}
	virtual void 	DisappearJudge()        													//判消失
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("信号消失判断"));
		_PdwDiscernType::DisappearJudge();
	}
	virtual void 	ScanAnalysis()																//扫描分析
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("扫描分析"));
		_PdwDiscernType::ScanAnalysis();
	}
	virtual void 	SetScanPara(int ResultNo)													//单通道扫描分析
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("扫描参数设置"));
		_PdwDiscernType::SetScanPara(ResultNo);
	}
	virtual int 	ResultTmpRfPwPaSet(_AddressPairType *AddrPartArray,int Lens)				//设置ResultTmp的Rf、Pw、Pa参数并返回PaAnalArray有效长度,输入数据为AddressPartArray
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表Rf、Pw、Pa设置（地址对）"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::ResultTmpRfPwPaSet(AddrPartArray, Lens);
		return Rtn;
	}
	virtual int 	ResultTmpRfPwPaSet(size_t *BarLocArray,int Lens)							//设置ResultTmp的Rf、Pw、Pa参数并返回PaAnalArray有效长度,输入数据为BarLocArray
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表Rf、Pw、Pa设置（直方图）"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::ResultTmpRfPwPaSet(BarLocArray, Lens);
		return Rtn;
	}
	virtual float 	MostPwSet(int n)															//Pw的合适值设置
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("Pw又选了个合适一点的值"));
		float Rtn = 0;
		Rtn = _PdwDiscernType::MostPwSet(n);
		return Rtn;
	}
	virtual int 	CalculateDoa(int PaNums)													//获取PaAnalArray中有效脉冲使用比幅测向法求Doa
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("比幅测向Doa设置"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::CalculateDoa(PaNums);
		return Rtn;
	}
	virtual int 	SeekPaToDoa(int *PaArray, int PaArrayLens, int *OutArray, int Gate)			//标志位收集PaArray中正负有效脉冲数OutArray，返回值为OutArray有效数，若不超过Gate，返回0
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("左右舷脉冲脉冲数过值"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::SeekPaToDoa(PaArray, PaArrayLens, OutArray, Gate);
		return Rtn;
	}
	virtual int 	FindLorR(int AntenaNo, int AnalyNums)										//左右舷有效脉冲幅度最大值
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("比幅测向Doa设置"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::FindLorR(AntenaNo, AnalyNums);
		return Rtn;
	}
	virtual int 	FindValidPa(int PaNums, int *PaArray = NULL)								//取最大值，个数超过门限，则返回该值位置,否则取次大值，如果个数超过门限，则返回该值位置,如果两都均不过门限，则返回个数最多的值位置
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("查找有效Pa"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::FindValidPa(PaNums, PaArray);
		return Rtn;
	}
	virtual void 	ResultOutput()																//结果表输出
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表入口函数"));
		_PdwDiscernType::ResultOutput();
	}
	virtual int 	CompPara(int i)																//分选雷达临时结果表同结果表参数对比
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表参数对比"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::CompPara(i);
		return Rtn;
	}
	virtual int 	DoubleModJudge(int Prf1, int Prf2, int DeltaPrf)							//求余判断是否丢脉冲仍在容差范围内
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表Prf余数判断"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::DoubleModJudge(Prf1, Prf2, DeltaPrf);
		return Rtn;
	}
	virtual int 	AnalyIdenticalDoa(int i)													//相同Doa累加替换
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表相同Doa处理"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AnalyIdenticalDoa(i);
		return Rtn;
	}
	virtual int 	AnalyDifferentDoa(int i,int DeltaDoa = 10)									//不同Doa累加替换
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表不同Doa处理"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::AnalyDifferentDoa(i, DeltaDoa);
		return Rtn;
	}
	virtual void 	FoundResult(int ResultNo, int CompParaRtn)									//当前分选型号在结果表中发现
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("查找到结果表"));
		_PdwDiscernType::FoundResult(ResultNo, CompParaRtn);
	}
	virtual int 	NotFoundResult()															//当前分选型号在结果表中未发现
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("未查找到结果表"));
		int Rtn = 0;
		Rtn = _PdwDiscernType::NotFoundResult();
		ResultAnalyChannel[ResultNums - 1] = AnalyChannel;
		return Rtn;
	}
	virtual void 	AnalyDiffTypeAndPri(int i)													//不同类型和不同Pri的累加替换
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表不同类型和Pri处理"));
		_PdwDiscernType::AnalyDiffTypeAndPri(i);
	}
	virtual void 	SetResultOtherPara(int i)													//设置结果表其它参数
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表其它参数设置"));
		_PdwDiscernType::SetResultOtherPara(i);
	}
	virtual void 	MsgExport()																	//直接输出Msg
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表输出报文"));
		_PdwDiscernType::MsgExport();
	}
	virtual void 	MsgExport(int ResultNo)														//Q告警
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表输出报文"));
		_PdwDiscernType::MsgExport(ResultNo);
	}
	virtual void 	SetMsg(int ResultNo)														//根据结果表设置输出报文
	{
		if(Result[ResultNo].ResultData.Type == 3)
		{
			if(Result[ResultNo].ResultData.JaggNums == 0)
			{
				COUTMSGVAL("Track it!");
			}
		}
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("报文设置参数"));
		_PdwDiscernType::SetMsg(ResultNo);
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd)														//手动设置输出报文  All Type Not reply edit
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("报文设置参数"));
		_PdwDiscernType::SetMsg(MsgCmd);
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd,_MsgPdwType &Ty,_MsgPdwPara &PdwPara,USHORT BatchNo)	//手动设置输出报文 LongMsgType ParaUpdataMsgType reply edit
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("报文设置参数"));
		_PdwDiscernType::SetMsg(MsgCmd, Ty, PdwPara, BatchNo);
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd,_MsgScanPara &MsgScanPara,USHORT BatchNo)				//手动设置输出报文 ShortMsgType reply edit
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("报文设置参数"));
		_PdwDiscernType::SetMsg(MsgCmd, MsgScanPara, BatchNo);
	}
	virtual void 	SetMsg(_MsgCmd MsgCmd,USHORT BatchNo)										//手动设置输出报文 MissingMsgType reply edit
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("报文设置参数"));
		_PdwDiscernType::SetMsg(MsgCmd, BatchNo);
	}
	virtual void 	Clear()																		//全清除
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNCEND("参数和数据清空"));
		_PdwDiscernType::Clear();
	}
	virtual void 	ClearTmp()																	//临时通道缓存清除
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("缓冲数据清空"));
		_PdwDiscernType::ClearTmp();
	}
	virtual void 	ClearResultDataDoa(int i)
	{
		DEBUG_RUN(FuncStackOn,
				COUTFUNC("结果表Doa数据清空"));
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
