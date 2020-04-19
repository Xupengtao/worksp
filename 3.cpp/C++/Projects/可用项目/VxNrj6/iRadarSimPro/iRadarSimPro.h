#ifndef _RADAR_H_
#define _RADAR_H_

#include"RadarStruct.h"
#include"CircularList.hpp"

#define PRNBUFFER_SIGN 0x0A00

class _iRadarAlgorithm
{
	_iRadarAlgorithm(){}
	virtual ~_iRadarAlgorithm(){}
public:
	friend class _Radar;
	friend class _RadarBuffer;
	static void PriFix(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void PriDither(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void PriJagging(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void PriSlider(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void PriIdle(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void PwFix(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void PwDefine(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void FreContinuously(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void FreFix(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void FreAgility(_Radar & Radars,_RadarBuffer & RadarBuf);
	static void FreDiversity(_Radar & Radars,_RadarBuffer & RadarBuf);
};

class _Radar
{
private:
	ushort_t 	PlatRadar;
	ushort_t 	RadarMode;
	uint_t		RadarNum;		//同一平台下雷达个数
	uint_t 		ToaInitH;
	uint_t 		ToaInitL;
	uint_t 		FrameNum;
	_PriType 	PriType;
	_PriPara 	PriPara;
	_RfType		RFType;
	_RfPara		RFPara;
	_PwType     PwType;
	_PwPara		PwPara;
public:
	friend class _RadarBuffer;
	friend class _iRadarAlgorithm;
	typedef void (*RADAR_FUNCPTR)(_Radar & Radars,_RadarBuffer & RadarBuf);
	RADAR_FUNCPTR PriFunc;
	RADAR_FUNCPTR PwFunc;
	RADAR_FUNCPTR RFFunc;
	
	_Radar(uint_t RadarNum_ = 10)
	{
		PlatRadar = 0;
		RadarMode = 0;
		RadarNum = RadarNum_;
		ToaInitH = 0;
		ToaInitL = 0;
		FrameNum= 0;
		PriType = PRI_FIX;
		PriFunc = _iRadarAlgorithm::PriFix;
		PwType = PW_FIX;
		PwFunc = _iRadarAlgorithm::PwFix;
		RFType = RF_FIX;
		RFFunc = _iRadarAlgorithm::FreFix;
		PriPara.NumTemp = 0;
		PwPara.NumTemp = 0;
		RFPara.NumTemp = 0;
		memset(&PriPara,0,sizeof(_PriPara));
		memset(&PwPara,0,sizeof(_PwPara));
		memset(&RFPara,0,sizeof(_RfPara));
	}
	virtual ~_Radar()
	{
		;
	}
	
	void 		SetNum(uchar_t Plat_,uchar_t Radar_){PlatRadar = Plat_*RadarNum +Radar_;}
	ushort_t	GetNum(){return PlatRadar;}
	void 		SetRadarMode(_RadarMode	RadarMode_)
	{
		RadarMode = RadarMode_En;
		SetPriPara(RadarMode_);
		SetPwPara(RadarMode_);
		SetRFPara(RadarMode_);
	}
	ushort_t	GetRadarMode(){return RadarMode;}
	void 		SetToaInit(uint_t ToaInitH_,uint_t ToaInitL_){ToaInitH = ToaInitH_;ToaInitL = ToaInitL_;}
	uint_t 		GetToaInit(){return ToaInitL;}
	uint_t 		GetFrameNum(){return FrameNum;}
	void		SetPriType(_PriType PriType_){PriType = PriType_;}
	_PriType	GetPriType(){return PriType;}
	void		SetPriPara(_PriPara & PriPara_){PriPara = PriPara_;}
	void 		SetFrameNum(uint_t FrameNum_)
	{
		if(FrameNum_ != 0)
		{
			FrameNum = FrameNum_;
		}
		else
		{
			NLogF("\n|*********************** Radar Strat ***********************|\n");
			NLogF("Exception: function(SetFrameNum) error!\n");
			NLogF("|*********************** Radar End   ***********************|\n\n");
		}
	}
	void		CalculateFrameNum(uint_t RadarBufSize_)
	{
		uint_t FrameNumTemp = 0;
		switch(PriType)
		{
		case PRI_FIX:
			FrameNumTemp = (PriPara.uPri.dPri != 0)?(RadarBufSize_/PriPara.uPri.dPri+100):0;
			SetFrameNum(FrameNumTemp);
			break;
		case PRI_DITHER:
			FrameNumTemp = ((PriPara.uPri.PriDither.dPri-PriPara.uPri.PriDither.dRange) != 0)?
							(RadarBufSize_/(PriPara.uPri.PriDither.dPri-PriPara.uPri.PriDither.dRange)):0;
			SetFrameNum(FrameNumTemp);
			break;
		case PRI_JAGGING:
			FrameNumTemp = (min(PriPara.uPri.PriJagging.dpRatio,PriPara.uPri.PriJagging.nPriNum) != 0)?
						   (RadarBufSize_/min(PriPara.uPri.PriJagging.dpRatio,PriPara.uPri.PriJagging.nPriNum)):0;
			SetFrameNum(FrameNumTemp);
			break;
		case PRI_SLIDER:
			FrameNumTemp = (min(PriPara.uPri.PriSlider.dpPri,PriPara.uPri.PriSlider.nPriNum) != 0)?
						   (RadarBufSize_/min(PriPara.uPri.PriSlider.dpPri,PriPara.uPri.PriSlider.nPriNum)):0;
			SetFrameNum(FrameNumTemp);
			break;
		default:
			break;
		}
	}
	/********* 由于显控报文为1字节对齐结构体，考虑到数据处理需要频繁读取结构体参数，此函  **********
	********** 数主要是将紧凑结构体方式转换为读写速度较快的结构体方式。区别于结构体赋值。 **********/
	void		SetPriPara(_RadarMode &RadarMode)			
	{
		if(RadarMode.byPriType == PRI_FIX)
		{
			PriFunc = _iRadarAlgorithm::PriFix;
			PriType = PRI_FIX;
			PriPara.uPri.dPri = RadarMode.Pri.dPri;
		}	
		else if(RadarMode.byPriType == PRI_DITHER)
		{
			PriFunc = _iRadarAlgorithm::PriDither;
			PriType = PRI_DITHER;
			PriPara.uPri.PriDither.dPri = RadarMode.Pri.PriDither.dPri;
			PriPara.uPri.PriDither.dRange = RadarMode.Pri.PriDither.dRange;
			PriPara.uPri.PriDither.nGroupNum = RadarMode.Pri.PriDither.nGroupNum;
		}
		else if(RadarMode.byPriType == PRI_JAGGING)
		{
			PriFunc = _iRadarAlgorithm::PriJagging;
			PriType = PRI_JAGGING;
			PriPara.uPri.PriJagging.nGroupNum = RadarMode.Pri.PriJagging.nGroupNum;
			PriPara.uPri.PriJagging.nPriNum = RadarMode.Pri.PriJagging.nPriNum;
			for (uint_t i = 0; i < RadarMode.Pri.PriJagging.nPriNum; i++)
			{
				PriPara.uPri.PriJagging.dpRatio[i] = RadarMode.Pri.PriJagging.dpRatio[i];
			}
		}
		else if(RadarMode.byPriType == PRI_SLIDER)
		{
			PriFunc = _iRadarAlgorithm::PriSlider;
			PriType = PRI_SLIDER;
			PriPara.uPri.PriSlider.dPriStep = RadarMode.Pri.PriSlider.dPriStep;
			PriPara.uPri.PriSlider.nGroupNum = RadarMode.Pri.PriSlider.nGroupNum;
			PriPara.uPri.PriSlider.nPriNum = RadarMode.Pri.PriSlider.nPriNum;
			for (uint_t i = 0; i < RadarMode.Pri.PriSlider.nPriNum; i++)
			{
				PriPara.uPri.PriSlider.dpPri[i] = RadarMode.Pri.PriSlider.dpPri[i];
			}
		}
	}
	void		SetPwPara(_RadarMode &RadarMode)
	{
		if(RadarMode.byPwType == PW_FIX)
		{
			PwFunc = _iRadarAlgorithm::PwFix;
			PwType = PW_FIX;
			PwPara.sPw.dPw = RadarMode.Pw.dPw;
		}	
		else if(RadarMode.byPwType == PW_DEFINE)
		{
			PwFunc = _iRadarAlgorithm::PwDefine;
			PwType = PW_DEFINE;
			PwPara.sPw.PwDefine.nPwNum = RadarMode.Pw.PwDefine.nPwNum;
			PwPara.sPw.PwDefine.nGroupNum = RadarMode.Pw.PwDefine.nGroupNum;
			for(uint_t i=0;i<RadarMode.Pw.PwDefine.nPwNum;i++)
			{
				PwPara.sPw.PwDefine.dpPw[i] = RadarMode.Pw.PwDefine.dpPw[i];
			}
		}
	}
	void		SetRFPara(_RadarMode &RadarMode)
	{
		if(RadarMode.byFreType == RF_CONTINUOUSLY)
		{
			RFFunc = _iRadarAlgorithm::FreContinuously;
			RFType = RF_CONTINUOUSLY;
			RFPara.sFre.dFre = RadarMode.Rf.dFre;
		}
		else if(RadarMode.byFreType == RF_FIX)
		{
			RFFunc = _iRadarAlgorithm::FreFix;
			RFType = RF_FIX;
			RFPara.sFre.dFre = RadarMode.Rf.dFre;
		}	
		else if(RadarMode.byFreType == RF_AGILITY)
		{
			RFFunc = _iRadarAlgorithm::FreAgility;
			RFType = RF_AGILITY;
			RFPara.sFre.FreAgility.nFreNum = RadarMode.Rf.FreAgility.nFreNum;
			RFPara.sFre.FreAgility.nGroupNum = RadarMode.Rf.FreAgility.nGroupNum;
			for(uint_t j=0;j<RadarMode.Rf.FreAgility.nFreNum;j++)
			{
				RFPara.sFre.FreAgility.dpFre[j] = RadarMode.Rf.FreAgility.dpFre[j];
			}
		}
	}
	template <typename T>
	T 		min(T *pType,uint_t Num)
	{
		T Temp = pType[0];
		for(uint_t i=0;i<Num;i++)
		{
			if (pType[i]<Temp)
			{
				Temp = pType[i];
			}
		}
		return Temp;
	}
	void		Reset()
	{
		RadarMode = RadarMode_Null;
		PriPara.NumTemp = 0;
		PwPara.NumTemp = 0;
		RFPara.NumTemp = 0;
		memset(&PriPara,0,sizeof(_PriPara));
		memset(&PwPara,0,sizeof(_PwPara));
		memset(&RFPara,0,sizeof(_RfPara));
	}
	void		Clear()
	{
		Reset();
		PlatRadar = 0;
		ToaInitH = 0;
		ToaInitL = 0;
		FrameNum= 0;
		PriType = PRI_FIX;
		PriFunc = _iRadarAlgorithm::PriFix;
		PwType = PW_FIX;
		PwFunc = _iRadarAlgorithm::PwFix;
		RFType = RF_FIX;
		RFFunc = _iRadarAlgorithm::FreFix;
	}
	void		showStatus()
	{
		PRINT(PlatRadar);
		PRINT(RadarMode);
		PRINT(ToaInitL);
		PRINT(FrameNum);
		PRINT(PriType);
		PRINT(RFType);
		PRINT(PwType);
		if(PriType ==  PRI_FIX)
		{
			PRINT(PriPara.uPri.dPri);
		}
		else if(PriType ==  PRI_DITHER)
		{
			PRINT(PriPara.uPri.PriDither.dPri);
			PRINT(PriPara.uPri.PriDither.dRange);
			PRINT(PriPara.uPri.PriDither.nGroupNum);
		}
		else if(PriType ==  PRI_JAGGING)
		{
			PRINT(PriPara.uPri.PriJagging.nPriNum);
			PRINT(PriPara.uPri.PriJagging.nGroupNum);
			for(uint_t i = 0;i<PriPara.uPri.PriJagging.nPriNum;i++)
			{
				NLogF("PriPara.uPri.PriJagging.dpRatio[%d] = %d",i,PriPara.uPri.PriJagging.dpRatio[i]);
			}
		}
		else if(PriType ==  PRI_SLIDER)
		{
			PRINT(PriPara.uPri.PriSlider.nPriNum);
			PRINT(PriPara.uPri.PriSlider.dPriStep);
			PRINT(PriPara.uPri.PriSlider.nGroupNum);
			for(uint_t i = 0;i<PriPara.uPri.PriSlider.nPriNum;i++)
			{
				NLogF("PriPara.uPri.PriSlider.dpPri[%d] = %d",i,PriPara.uPri.PriSlider.dpPri[i]);
			}
		}
		PRINT(PriPara.ToaMod);
		PRINT(PriPara.ToaModRand);
		PRINT(PriPara.NumTemp);
		PRINT(PriPara.GroupNumTemp);
		
		if(RFType ==  RF_CONTINUOUSLY)
		{
			PRINT(RFPara.sFre.dFre);
		}
		else if(RFType ==  RF_FIX)
		{
			PRINT(RFPara.sFre.dFre);
		}
		else if(RFType ==  RF_AGILITY)
		{
			PRINT(RFPara.sFre.FreAgility.nFreNum);
			PRINT(RFPara.sFre.FreAgility.nGroupNum);
			for(uint_t i=0;i<RFPara.sFre.FreAgility.nFreNum;i++)
			{
				NLogF("RFPara.sFre.FreAgility.dpFre[%d] = %d",i,RFPara.sFre.FreAgility.dpFre[i]);
			}
		}
		else if(RFType ==  RF_DIVERSITY)
		{
			PRINT(RFPara.sFre.FreDiversity.nFreNum);
			for(uint_t i=0;i<RFPara.sFre.FreDiversity.nFreNum;i++)
			{
				NLogF("RFPara.sFre.FreDiversity.dpFre[%d] = %d",i,RFPara.sFre.FreDiversity.dpFre[i]);
			}
			for(uint_t i=0;i<RFPara.sFre.FreDiversity.nFreNum;i++)
			{
				NLogF("RFPara.sFre.FreDiversity.dInterval[%d] = %d",i,RFPara.sFre.FreDiversity.dInterval[i]);
			}
		}
		PRINT(RFPara.NumTemp);
		PRINT(RFPara.PriSumTemp);
		PRINT(RFPara.GroupNumTemp);
		
		if(PwType ==  PW_FIX)
		{
			PRINT(PwPara.sPw.dPw);
		}
		else if(PwType ==  PW_DEFINE)
		{
			PRINT(PwPara.sPw.PwDefine.nPwNum);
			PRINT(PwPara.sPw.PwDefine.nGroupNum);
			for(uint_t i=0;i<PwPara.sPw.PwDefine.nPwNum;i++)
			{
				NLogF("PwPara.sPw.PwDefine.dpPw[%d] = %d",i,PwPara.sPw.PwDefine.dpPw[i]);
			}
		}
		PRINT(PwPara.NumTemp);
		PRINT(PwPara.GroupNumTemp);
	}
	void		show()
	{
		NLogF("\n|*********************** Radar Strat ***********************|\n");
		PRINT_X(GetNum());
		PRINT_X(GetRadarMode());
		PRINT_X(GetToaInit());
		PRINT_X(GetFrameNum());
		NLogF("|*********************** Radar End   ***********************|\n\n");
	}
};

class _PlatForm
{
private:
	uchar_t 	PlatNo;
	bool 		PlatFormEn;
	ushort_t 	Az;
	ushort_t 	El;
	ushort_t 	Distance;
	ushort_t 	RadarModeList[RADAR_NUM];
	_Radar*		Radars;
public:
	_PlatForm(uchar_t PlatNo_=0,bool PlatFormEn_=false,ushort_t Az_=0,ushort_t El_=0,ushort_t Distance_=0)
			 :PlatNo(PlatNo_),PlatFormEn(PlatFormEn_),Az(Az_),El(El_),Distance(Distance_)
	{	
		for (uint_t i = 0; i < RADAR_NUM; i++)
		{
			RadarModeList[i] = false;
		}
		Radars = new _Radar[RADAR_NUM];
	}
	virtual ~_PlatForm()
	{
		delete[] Radars;
	}
	
	void 	EnablePlat(){PlatFormEn= true;}
	void 	DisenablePlat(){PlatFormEn= false;}
	bool	GetPlatFormEn() const{return PlatFormEn;}
	void 	SetPlat(const uchar_t Plat){PlatNo = Plat;}
	uchar_t GetPlat(){return PlatNo;}
	void 	SetAz(const ushort_t Az_){Az = Az_;}
	ushort_t	GetAz() const {return Az;}
	void	SetEl(const ushort_t El_){El = El_;}
	ushort_t	GetEl(){return El;}
	void	SetDistance(const ushort_t Distance_){Distance = Distance_;}
	ushort_t 	GetDistance(){return Distance;}
	void 	SetAED(const _AzElDis AzElDistance){Az = AzElDistance.Az;El = AzElDistance.El;Distance = AzElDistance.Distance;}
	_AzElDis GetAED() const { _AzElDis AED; AED.Az = Az; AED.El = El; AED.Distance = Distance; return AED;}
	void 	SetRadarEn(uchar_t RadarNo_){RadarModeList[RadarNo_] = RadarMode_En;}
	void 	SetRadarDisEn(uchar_t RadarNo_){RadarModeList[RadarNo_] = RadarMode_Null;}
	ushort_t GetRadarMode(uchar_t RadarNo_) const {return RadarModeList[RadarNo_];}
	void 	SetRadar(_RadarMsg &RadarMsg)
	{
		uchar_t PlatSn = RadarMsg.PlatSN, RadarSn = RadarMsg.RadSN;
		EnablePlat();
		SetRadarEn(RadarSn);
		GetRadar(RadarSn).SetNum(PlatSn,RadarSn);
		GetRadar(RadarSn).Reset();
		Radars[RadarSn].SetRadarMode(RadarMsg.RadarMode);
	}
	_Radar& GetRadar(uchar_t RadarNo_){return Radars[RadarNo_];}
	void	Clear()
	{
		PlatFormEn = false;
		Az = 0;
		El = 0;
		Distance = 0;
		for(uint_t i=0;i<RADAR_NUM;i++)
		{
			if(RadarModeList[i] != RadarMode_Null)
			{
				RadarModeList[i] = RadarMode_Null;
				Radars[i].Clear();
			}
		}
	}
	void	showStatus()
	{
		PRINT(GetPlat());
		PRINT(GetPlatFormEn());
		PRINT(GetAz());
		PRINT(GetEl());
		PRINT(GetDistance());
		for(uint_t i=0;i<RADAR_NUM;i++)
		{
			if(GetRadarMode(i) != 0)
			{
				NLogF("|*********************** Radar[%d] Start ***********************|\n",i);
				NLogF("RadarModeList[%d] = %d",i,GetRadarMode(i));
				GetRadar(i).showStatus();
				NLogF("|*********************** Radar[%d] End   ***********************|\n",i);
			}
		}
	}
	void 	show()
	{
		NLogF("\n|*********************** PlatForm Start ***********************|\n");
		PRINT_X(GetPlat());
		PRINT_X(GetAz());
		PRINT_X(GetEl());
		PRINT_X(GetDistance());
		NLogF("|*********************** PlatForm End   ***********************|\n\n");
	}
};

class _RadarBuffer
{
private:
	static uint_t 	GlobalToaH;
	static uint_t 	GlobalToaL;
	static uint_t 	RadarRCCLNum;
	static uint_t	SortTimes;
	
	uint_t 			RadarBufSize;
	uint_t 			SignSize;
	uint_t 			InPDWSize;
	uint_t			RadarBufferSortTime;
	
public:
	friend class 	_PlatForm;
	friend class 	_Radar;
	friend class 	_iRadarAlgorithm;
	uint_t 			SortNum;
	_PlatRadarNum 	*PRNBuffer;
	_PRNSign 	 	*ToaBufferSign;
	CircularList<_RadarPara> 	*RadarPara;
	_InPDW 			*InPDW;
	uint_t			ParaPRFrames[(PLAT_NUM)*(RADAR_NUM)];
	
	uint_t			PriStatus;
	uint_t			PwStatus;
	uint_t			RFStatus;
	uint_t			SortReadyStatus;
	uint_t 			InPDWStatus;
	
	_RadarBuffer(uint_t RadarBufSize_ = 0x1000000,uint_t SignSize_ = 8,uint_t InPDWSize_ = 1000000)
				:RadarBufSize(RadarBufSize_),SignSize(SignSize_),InPDWSize(InPDWSize_)
	{
		PriStatus = IDLE;
		PwStatus = IDLE;
		RFStatus = IDLE;
		SortReadyStatus = PARABUF_IDLE;
		InPDWStatus = InPDW_IDLE;
		SortNum = 0;
		RadarBufferSortTime = 0;
		PRNBuffer = new _PlatRadarNum[RadarBufSize];
		memset(PRNBuffer,0,RadarBufSize*sizeof(_PlatRadarNum));
		for(uint_t i=0;i<RadarBufSize;i++)
		{
			PRNBuffer[i].PlatRadar = PRNBUFFER_SIGN;
		}
		ToaBufferSign = new _PRNSign[RadarBufSize>>SignSize];
		memset(ToaBufferSign,0,(RadarBufSize>>SignSize)*sizeof(_PRNSign));
		InPDW = new _InPDW[InPDWSize];
		memset(InPDW,0,InPDWSize*sizeof(_InPDW));
		memset(ParaPRFrames,0,(PLAT_NUM)*(RADAR_NUM)*sizeof(uint_t));

		RadarPara = new CircularList<_RadarPara>[PLAT_NUM*RADAR_NUM];
	}
	~_RadarBuffer()
	{
		delete[] PRNBuffer;
		delete[] ToaBufferSign;
		delete[] InPDW;
		delete[] RadarPara;
	}

	void ShellSort(uint_t *pToa, uint_t Num)
	{
		uint_t i, j, Inc;
		uint_t Tmp;
		for (Inc = Num / 2; Inc>0; Inc /= 2)
		{
			for (i = Inc; i<Num; i++)
			{
				Tmp = pToa[i];
				for (j = i; j >= Inc; j -= Inc)
				{
					if (Tmp < pToa[j - Inc])
					{
						pToa[j] = pToa[j - Inc];
					}
					else
						break;
				}
				pToa[j] = Tmp;
			}
		}
	}

	void ToaSort()
	{
		if ((SortReadyStatus == PARABUF_READY) && (InPDWStatus == InPDW_IDLE))
		{
			InPDWStatus = InPDW_SORTBUSY;
			ParaPRReadReset();
			RadarBufferSortTime = SortTimes;
			SortTimes++;
			uint_t i, j, SortNum_ = 0, ToaLocal_, ToaLocalStep_, ToaNum_, SortNumTemp_, GlobalToaHTemp_ = this->GlobalToaH, GlobalToaLTemp_ = this->GlobalToaL;
			ushort_t PlatRadarTemp_ = 0;
			uint_t	 RadarsToaNum_ = 0;
			const uint_t BufSignSize_ = RadarBufSize >> SignSize;
			const uint_t StepMax_ = 1 << SignSize;
			for (i = 0; i<BufSignSize_; i++)
			{
				if (ToaBufferSign[i].Num != 0)
				{
					ToaNum_ = ToaBufferSign[i].Num;
					ToaBufferSign[i].Num = 0;
					if (ToaNum_ < SIGN_NUM)
					{
						ShellSort(ToaBufferSign[i].Index, ToaNum_);
						for (j = 0; j<ToaNum_; j++)
						{
							SortNumTemp_ = ToaBufferSign[i].Index[j];
							PlatRadarTemp_ = PRNBuffer[SortNumTemp_].PlatRadar;
							RadarsToaNum_ = PRNBuffer[SortNumTemp_].ToaNum;
							PRNBuffer[SortNumTemp_].PlatRadar = PRNBUFFER_SIGN;
							InPDW[SortNum_].PlatRadar = PlatRadarTemp_;
							InPDW[SortNum_].RadarPara = RadarPara[PlatRadarTemp_][RadarsToaNum_];
							InPDW[SortNum_].ToaH = GlobalToaHTemp_;
							InPDW[SortNum_].ToaL = GlobalToaLTemp_ | SortNumTemp_;
							ParaPRFrames[PlatRadarTemp_] = RadarsToaNum_;
							SortNum_++;
						}
					}
					else
					{
						ToaLocal_ = i << SignSize;
						ToaLocalStep_ = ToaLocal_ + StepMax_;
						while ((ToaNum_ > 0) && (ToaLocal_ < ToaLocalStep_))
						{
							if (PRNBuffer[ToaLocal_].PlatRadar < PRNBUFFER_SIGN)
							{
								PlatRadarTemp_ = PRNBuffer[ToaLocal_].PlatRadar;
								RadarsToaNum_ = PRNBuffer[ToaLocal_].ToaNum;
								PRNBuffer[ToaLocal_].PlatRadar = PRNBUFFER_SIGN;
								InPDW[SortNum_].PlatRadar = PlatRadarTemp_;
								InPDW[SortNum_].RadarPara = RadarPara[PlatRadarTemp_][RadarsToaNum_];
								InPDW[SortNum_].ToaH = GlobalToaHTemp_;
								InPDW[SortNum_].ToaL = GlobalToaLTemp_ | ToaLocal_;
								ParaPRFrames[PlatRadarTemp_] = RadarsToaNum_;
								ToaNum_--;
								SortNum_++;
							}
							ToaLocal_++;
						}
					}
				}
			}
			ParaPRWriteReset();
			if (SortNum_ == 0)
			{
				SortNum = SortNum_;
			}
			else
			{
				SortNum = SortNum_ - 1;
			}
			GlobalToaAdd(RadarBufSize);
			SortReadyStatus = PARABUF_IDLE;
			InPDWStatus = InPDW_READY;
		}
	}

	static void ResetGlobalToa()
	{
		GlobalToaH = 0;
		GlobalToaL = 0;
	}
	static uint_t GetRCCLNum(uint_t Num)
	{
		uint_t i;
		for(i=0;Num>1;i++)
		{
			Num = Num>>1;
		}
		return i;
	}
	static void GlobalToaAdd(uint_t RadarBufSize)
	{
		RadarRCCLNum = GetRCCLNum(RadarBufSize);
		uint_t ToaLMax;
		ToaLMax = 0xFFFFFFFF>>RadarRCCLNum;
		uint_t GlobalToaLRccl = GlobalToaL>>RadarRCCLNum;
		if(GlobalToaLRccl<ToaLMax)
		{
			GlobalToaL = GlobalToaL+RadarBufSize;
		}
		else
		{
			GlobalToaL = 0;
			GlobalToaH++;
		}
	}
	static uint_t GetGlobalToaH(){return GlobalToaH;}
	static uint_t GetGlobalToaL(){return GlobalToaL;}
	uint_t 	GetStatus(){return InPDWStatus;}
	void 	SetStatus(const uint_t InPDWStatus_){InPDWStatus = InPDWStatus_;}
	uint_t	GetSortTimes() const {return SortTimes;}
	uint_t  GetRadarBufSize() const {return RadarBufSize;}
	uint_t  GetSignSize() const {return SignSize;}
	uint_t 	GetSortNum(){return SortNum;}
	uint_t	GetRadarBufferSortTimes() const {return RadarBufferSortTime;}
	void	NoToaParaBufferLink(_RadarBuffer &RadarBuf)
	{
		for(uint_t i=0;i<(PLAT_NUM)*(RADAR_NUM);i++)
		{
			RadarPara[i].ChainLink(RadarBuf.RadarPara[i]);
		}
	}
	void	SetNoToaParaBuf(_PlatForm *PlatForm)
	{
		for (uint_t i = 0; i<PLAT_NUM; i++)
		{
			if (PlatForm[i].GetPlatFormEn() == true)
			{
				for (uint_t j = 0; j<RADAR_NUM; j++)
				{
					if (PlatForm[i].GetRadarMode(j) != RadarMode_Null)
					{
						PlatForm[i].GetRadar(j).CalculateFrameNum(RadarBufSize);
						RadarPara[PlatForm[i].GetRadar(j).PlatRadar].CapacityReset(PlatForm[i].GetRadar(j).FrameNum);
					}
				}
			}
		}
	}
	void	ParaPRReadReset()
	{
		for(uint_t i=0;i<(PLAT_NUM)*(RADAR_NUM);i++)
		{
			RadarPara[i].ReadReset();
		}
	}
	void	ParaPRWriteReset()
	{
		for(uint_t i=0;i<(PLAT_NUM)*(RADAR_NUM);i++)
		{
			if(ParaPRFrames[i] != 0)
			{
				RadarPara[i].WriteReset(ParaPRFrames[i]);
				ParaPRFrames[i] = 0;
			}
		}
	}
	void	BufferClear()
	{
		GlobalToaH = 0;
		GlobalToaL = 0;
		RadarRCCLNum = 0;
		SortNum = 0;
		SortTimes = 0;
		RadarBufferSortTime = 0;
		PriStatus = IDLE;
		PwStatus = IDLE;
		RFStatus = IDLE;
		SortReadyStatus = PARABUF_IDLE;
		InPDWStatus = InPDW_IDLE;
		for(uint_t i=0;i<RadarBufSize;i++)
		{
			PRNBuffer[i].PlatRadar = PRNBUFFER_SIGN;
		}
		memset(ToaBufferSign,0,(RadarBufSize>>SignSize)*sizeof(_PRNSign));
		memset(ParaPRFrames,0,(PLAT_NUM)*(RADAR_NUM)*sizeof(uint_t));
		memset(InPDW,0,InPDWSize*sizeof(_InPDW));
	}
	void	showStatus()
	{
		PRINT(GlobalToaH);
		PRINT(GlobalToaL);
		PRINT(SortTimes);
		PRINT(RadarBufSize);
		PRINT(SignSize);
		PRINT(SortNum);
		PRINT(RadarBufferSortTime);
		PRINT_STATUS(PriStatus);
		PRINT_STATUS(PwStatus);
		PRINT_STATUS(RFStatus);
		PRINT_STATUS(SortReadyStatus);
		PRINT_STATUS(InPDWStatus);
	}
	void 	show(uint_t Num)
	{
		PRINT(Num);
		PRINT(InPDW[Num].PlatRadar);
		PRINT(InPDW[Num].RadarPara.Flag);
		PRINT(InPDW[Num].RadarPara.Pw);
		PRINT(InPDW[Num].RadarPara.Rf);
		PRINT(InPDW[Num].RadarPara.Az);
		PRINT(InPDW[Num].RadarPara.El);
		PRINT(InPDW[Num].RadarPara.Pa);
		PRINT_X(InPDW[Num].ToaH);
		PRINT_X(InPDW[Num].ToaL);
	}
	void 	show(uint_t NumStart,uint_t NumStop)
	{
		if(NumStart<NumStop)
		{
			for(uint_t i=NumStart;i<=NumStop;i++)
			{
				show(i);
			}
		}
	}
};

template<typename _OutPdwType>
class _Receiver
{
private:
	float		Rcs;
	short 		Sensitivity;
	short 		DynamicEquil;
	uint_t 		OutPdwBufferSize;
	ushort_t 	Mode;
	uint_t 		SortNum;
	uint_t		SortTimes;
	_OutPdwType	*OutPdw;
	
public:
	uint_t 		OutPdwStatus;
	
	_Receiver(float Rcs_=0,short Sensitivity_=10,uint_t DynamicEquil_=500,uint_t OutPdwBufferSize_=1000000,short Mode_=1)
			  :Rcs(Rcs_),Sensitivity(Sensitivity_),DynamicEquil(DynamicEquil_),
			   OutPdwBufferSize(OutPdwBufferSize_),Mode(Mode_)
	{
		OutPdw = new _OutPdwType[OutPdwBufferSize];
		memset(OutPdw,0,sizeof(_OutPdwType)*OutPdwBufferSize);
		SortNum = 0;
		SortTimes = 0;
		OutPdwStatus = OutPDW_IDLE;
	}

	virtual ~_Receiver()
	{
		delete[] OutPdw;
	}

	void 	SetRcs(float Rcs_){Rcs = Rcs_;}
	float 	GetRcs(){return Rcs;}
	void 	SetSensitivity(short Sensitivity_){Sensitivity = Sensitivity_;}
	short 	GetSensitivity(){return Sensitivity;}
	void 	SetDynamicEquil(short DynamicEquil_){DynamicEquil = DynamicEquil_;}
	short 	GetDynamicEquil(){return DynamicEquil;}
	void 	SetMode(ushort_t Mode_){Mode = Mode_;}
	short 	GetMode(){return Mode;}
	uint_t	GetOutPdwBufAddr() const {return (uint_t)OutPdw;}
	uint_t	GetOutPdwStatusAddr() const {return (uint_t)(&OutPdwStatus);}
	uint_t	GetSortNumAddr() const {return (uint_t)(&SortNum);}
	uint_t	GetOutPdwBufSize() const {return OutPdwBufferSize;}
	void	ClearSortNum(){SortNum = 0;}
	uint_t	GetSortNum() const {return SortNum;}
	void	ClearSortTime(){SortTimes = 0;}
	uint_t	GetSortTime() const {return SortTimes;}
	
	uint_t 	PDWSort(_RadarBuffer & RadarBuf)
	{
		if ((RadarBuf.InPDWStatus == InPDW_READY) && (OutPdwStatus == OutPDW_IDLE))
		{
			OutPdwStatus = OutPDW_SORTBUSY;
			SortTimes++;

			uint_t SortNum_ = SortNum;
			uint_t RadarBufSortNum_ = RadarBuf.SortNum;
			uint_t PwTemp_ = 0;

			RadarBuf.SortNum = 0;
			if (SortNum_ + 2 * RadarBufSortNum_ < OutPdwBufferSize)
			{
				for (uint_t i = 0; i <= RadarBufSortNum_;)
				{
					_InPDW &InPDWTemp = RadarBuf.InPDW[i];
					if ((InPDWTemp.ToaL>PwTemp_) && (InPDWTemp.RadarPara.Pa>Sensitivity) && (InPDWTemp.RadarPara.Pa<Sensitivity + DynamicEquil))
					{
//						OutPdw[SortNum_].Flag = InPDWTemp.RadarPara.Flag;
//						OutPdw[SortNum_].ToaL = InPDWTemp.ToaL;
//						OutPdw[SortNum_].ToaH = InPDWTemp.ToaH;
//						OutPdw[SortNum_].Az = InPDWTemp.RadarPara.Az;
//						OutPdw[SortNum_].El = InPDWTemp.RadarPara.El;
						OutPdw[SortNum_].Toa = InPDWTemp.ToaL;
						OutPdw[SortNum_].Ton = InPDWTemp.ToaH;
						OutPdw[SortNum_].Pa = InPDWTemp.RadarPara.Pa;
						OutPdw[SortNum_].Pw = InPDWTemp.RadarPara.Pw;
						OutPdw[SortNum_].Rf = InPDWTemp.RadarPara.Rf;
						SortNum_++;
						PwTemp_ = InPDWTemp.ToaL + InPDWTemp.RadarPara.Pw;
					}
					i++;
				}
			}
			else
			{
				for (uint_t i = 0; i <= RadarBufSortNum_;)
				{
					_InPDW &InPDWTemp = RadarBuf.InPDW[i];
					if ((InPDWTemp.ToaL>PwTemp_) && (InPDWTemp.RadarPara.Pa>Sensitivity) && (InPDWTemp.RadarPara.Pa<Sensitivity + DynamicEquil))
					{
						if (SortNum_ < OutPdwBufferSize)
						{
//							OutPdw[SortNum_].Flag = InPDWTemp.RadarPara.Flag;
//							OutPdw[SortNum_].ToaL = InPDWTemp.ToaL;
//							OutPdw[SortNum_].ToaH = InPDWTemp.ToaH;
//							OutPdw[SortNum_].Az = InPDWTemp.RadarPara.Az;
//							OutPdw[SortNum_].El = InPDWTemp.RadarPara.El;
							OutPdw[SortNum_].Toa = InPDWTemp.ToaL;
							OutPdw[SortNum_].Ton = InPDWTemp.ToaH;
							OutPdw[SortNum_].Pa = InPDWTemp.RadarPara.Pa;
							OutPdw[SortNum_].Pw = InPDWTemp.RadarPara.Pw;
							OutPdw[SortNum_].Rf = InPDWTemp.RadarPara.Rf;
							SortNum_++;
							PwTemp_ = InPDWTemp.ToaL + InPDWTemp.RadarPara.Pw;
						}
						else
						{
							SortNum = SortNum_ - 1;
							RadarBuf.InPDWStatus = InPDW_IDLE;
							OutPdwStatus = OutPDW_READY;
							return OutPdwStatus;
						}
					}
					i++;
				}
				SortNum = SortNum_ - 1;
				RadarBuf.InPDWStatus = InPDW_IDLE;
				OutPdwStatus = OutPDW_READY;
				return OutPdwStatus;
			}
			SortNum = SortNum_;
			if ((SortTimes >= RECIVERSORTTIMES_MAX) && (SortNum >= RECIVERSORTNUM_MAX))
			{
				SortNum = SortNum - 1;
				RadarBuf.InPDWStatus = InPDW_IDLE;
				OutPdwStatus = OutPDW_READY;
			}
			else
			{
				RadarBuf.InPDWStatus = InPDW_IDLE;
				OutPdwStatus = OutPDW_IDLE;
			}
		}
		return OutPdwStatus;
	}
	void 	showStatus()
	{
		PRINT(Sensitivity);
		PRINT(DynamicEquil);
		PRINT(OutPdwBufferSize);
		PRINT(GetOutPdwBufAddr());
		PRINT(SortNum);
		PRINT(SortTimes);
		PRINT_STATUS(OutPdwStatus);
	}
	void 	show(uint_t Num) const
	{
		printf("Pdw No.%6d: RF = %5d, PW = %5d, PA = %5d, TOA = %5d, TON = %5d\n",
				Num,OutPdw[Num].Rf,OutPdw[Num].Pw,OutPdw[Num].Pa,OutPdw[Num].Toa,OutPdw[Num].Ton);
	}
	void 	show(uint_t NumStart,uint_t NumStop)
	{
		if(NumStart<NumStop)
		{
			for(uint_t i=NumStart;i<NumStop;i++)
			{
				show(i);
			}
		}
	}
	void	BufferClear()
	{
		SortNum = 0;
		SortTimes = 0;
		memset(OutPdw,0,sizeof(_OutPdwType)*OutPdwBufferSize);
		OutPdwStatus = OutPDW_IDLE;
	}
	void	StatusClear()
	{
		Rcs = 0;
		Sensitivity = 0;
		DynamicEquil = 500;
	}
};

template<typename _OutPdwType>
class _XkMsg
{
	typedef _Receiver<_OutPdwType> 	_ReceiverType;
	
	_RecMsg		RecMsg;
	_PlatMsg 	PlatMsg;
	_RadarMsg	RadarMsg;
public:
	_XkMsg(_PlatMsgType IniMsgType = SingleRadar)
	{
		memset(&RecMsg, 0, sizeof(_RecMsg));
		memset(&PlatMsg, 0, sizeof(_PlatMsg));
		memset(&RadarMsg, 0, sizeof(_RadarMsg));
		RecMsgGen();
		PlatMsgGen(IniMsgType);
		RadarMsgGen(0,0);
	}
	virtual ~_XkMsg() {}
	/******************************************参数报文生成******************************************/
	bool RecMsgGen(int dRCS_ = 100, short nLmd_ = -10, ushort_t nEsmType_ = BroadBank)
	{
		RecMsg.dRCS = dRCS_;
		RecMsg.nLmd = nLmd_;
		RecMsg.nEsmType = nEsmType_;
		return true;
	}
	bool PlatMsgGen(_PlatMsgType PlatMsgType = SingleRadar)
	{
		switch (PlatMsgType)
		{
		case SingleRadar:
			PlatMsg.nPlatNum = 1;
			break;
		case SinglePlat:
			PlatMsg.nPlatNum = 1;
			break;
		case FullNum:
			PlatMsg.nPlatNum = 60;
			break;
		default:
			break;
		}
		return true;
	}
	bool PlatMsgGen(size_t nPlatNum)
	{
		PlatMsg.nPlatNum = nPlatNum;
		return true;
	}
	bool RadarMsgGen(ushort_t PlatSn, ushort_t RadarSn, 
					 _PriType PriType = PRI_FIX,_Pri *Pri_ = NULL,
					 _PwType PwType = PW_FIX, _Pw *Pw_ = NULL,
					 _RfType FreType = RF_FIX,_Rf *Rf_ = NULL)
	{
		RadarMsg.PlatSN = uchar_t(PlatSn);
		RadarMsg.RadSN = uchar_t(RadarSn);
		RadarMsgPriParaModify(PriType,Pri_);
		RadarMsgPwParaModify(PwType,Pw_);
		RadarMsgRfParaModify(FreType,Rf_);
		return true;
	}
	bool RadarMsgPlatRadarNumModify(ushort_t PlatSn, ushort_t RadarSn)
	{
		RadarMsg.PlatSN = uchar_t(PlatSn);
		RadarMsg.RadSN = uchar_t(RadarSn);
		return true;
	}
	bool RadarMsgPriParaModify(_PriType PriType = PRI_FIX, _Pri *Pri_ = NULL)
	{
		_RadarMode &RadarMode = RadarMsg.RadarMode;
		RadarMode.byPriType = PriType;
		if(Pri_ == NULL)
		{
			if (PriType == PRI_FIX)
			{
				RadarMode.Pri.dPri = 20000;
			}
			else if (PriType == PRI_DITHER)
			{
				RadarMode.Pri.PriDither.nGroupNum = 10;
				RadarMode.Pri.PriDither.dPri	  = 20000;
				RadarMode.Pri.PriDither.dRange	  = 5000;
			}
			else if (PriType == PRI_JAGGING)
			{
				RadarMode.Pri.PriJagging.nPriNum = 5;
				RadarMode.Pri.PriJagging.nGroupNum = 10;
				for (size_t i = 0; i < RadarMode.Pri.PriJagging.nPriNum; i++)
				{
					RadarMode.Pri.PriJagging.dpRatio[i] = (i == 0) ? 20000 : RadarMode.Pri.PriJagging.dpRatio[i-1]+5000;
				}
			}
			else if (PriType == PRI_SLIDER)
			{
				RadarMode.Pri.PriSlider.nPriNum = 5;
				RadarMode.Pri.PriSlider.nGroupNum = 10;
				RadarMode.Pri.PriSlider.dPriStep = -3;
				for (size_t i = 0; i < RadarMode.Pri.PriSlider.nPriNum; i++)
				{
					RadarMode.Pri.PriSlider.dpPri[i] = (i == 0) ? 20000 : RadarMode.Pri.PriSlider.dpPri[i - 1] + 5000;
				}
			}
		}
		else
		{
			_Pri &Pri = *Pri_;
			if (PriType == PRI_FIX)
			{
				RadarMode.Pri.dPri = Pri.dPri;
			}
			else if (PriType == PRI_DITHER)
			{
				RadarMode.Pri.PriDither.nGroupNum = Pri.PriDither.nGroupNum;
				RadarMode.Pri.PriDither.dPri	  = Pri.PriDither.dPri;
				RadarMode.Pri.PriDither.dRange	  = Pri.PriDither.dRange;
			}
			else if (PriType == PRI_JAGGING)
			{
				RadarMode.Pri.PriJagging.nPriNum 	= Pri.PriJagging.nPriNum;
				RadarMode.Pri.PriJagging.nGroupNum 	= Pri.PriJagging.nGroupNum;
				for (size_t i = 0; i < RadarMode.Pri.PriJagging.nPriNum; i++)
				{
					RadarMode.Pri.PriJagging.dpRatio[i] = Pri.PriJagging.dpRatio[i];
				}
			}
			else if (PriType == PRI_SLIDER)
			{
				RadarMode.Pri.PriSlider.nPriNum = Pri.PriSlider.nPriNum;
				RadarMode.Pri.PriSlider.nGroupNum = Pri.PriSlider.nGroupNum;
				RadarMode.Pri.PriSlider.dPriStep = Pri.PriSlider.dPriStep;
				for (size_t i = 0; i < RadarMode.Pri.PriSlider.nPriNum; i++)
				{
					RadarMode.Pri.PriSlider.dpPri[i] = Pri.PriSlider.dpPri[i];
				}
			}
		}
		return true;
	}
	bool RadarMsgPwParaModify(_PwType PwType = PW_FIX, _Pw *Pw_ = NULL)
	{
		_RadarMode &RadarMode = RadarMsg.RadarMode;
		RadarMode.byPwType  = PwType;
		if(Pw_ == NULL)
		{
			if (PwType == PW_FIX)
			{
				RadarMode.Pw.dPw = 50;
			}
			else if (PwType == PW_DEFINE)
			{
				RadarMode.Pw.PwDefine.nPwNum = 10;
				RadarMode.Pw.PwDefine.nGroupNum = 5;
				for (size_t i = 0; i < RadarMode.Pw.PwDefine.nPwNum; i++)
				{
					RadarMode.Pw.PwDefine.dpPw[i] = (i == 0) ? 30 : RadarMode.Pw.PwDefine.dpPw[i - 1] + 10;
				}
			}
		}
		else
		{
			_Pw &Pw = *Pw_;
			if (PwType == PW_FIX)
			{
				RadarMode.Pw.dPw = Pw.dPw;
			}
			else if (PwType == PW_DEFINE)
			{
				RadarMode.Pw.PwDefine.nPwNum 	= Pw.PwDefine.nPwNum;
				RadarMode.Pw.PwDefine.nGroupNum = Pw.PwDefine.nGroupNum;
				for (size_t i = 0; i < RadarMode.Pw.PwDefine.nPwNum; i++)
				{
					RadarMode.Pw.PwDefine.dpPw[i] = Pw.PwDefine.dpPw[i];
				}
			}
		}
		return true;
	}
	bool RadarMsgRfParaModify(_RfType FreType = RF_FIX, _Rf *Rf_ = NULL)
	{
		_RadarMode &RadarMode = RadarMsg.RadarMode;
		RadarMode.byFreType = FreType;
		if(Rf_ == NULL)
		{
			if (FreType == RF_CONTINUOUSLY)
			{
				RadarMode.Rf.dFre = 256000;
			}
			else if (FreType == RF_FIX)
			{
				RadarMode.Rf.dFre = 256000;
			}
			else if (FreType == RF_AGILITY)
			{
				RadarMode.Rf.FreAgility.nFreNum = 10;
				RadarMode.Rf.FreAgility.nGroupNum = 5;
				for (size_t i = 0; i < RadarMode.Rf.FreAgility.nFreNum; i++)
				{
					RadarMode.Rf.FreAgility.dpFre[i] = (i == 0) ? 1000 : RadarMode.Rf.FreAgility.dpFre[i - 1] + 1000;
				}
			}
			else if (FreType == RF_DIVERSITY)
			{
				;
			}
		}
		else
		{
			_Rf &Rf = *Rf_;
			if (FreType == RF_CONTINUOUSLY)
			{
				RadarMode.Rf.dFre = Rf.dFre;
			}
			else if (FreType == RF_FIX)
			{
				RadarMode.Rf.dFre = Rf.dFre;
			}
			else if (FreType == RF_AGILITY)
			{
				RadarMode.Rf.FreAgility.nFreNum 	= Rf.FreAgility.nFreNum;
				RadarMode.Rf.FreAgility.nGroupNum 	= Rf.FreAgility.nGroupNum;
				for (size_t i = 0; i < RadarMode.Rf.FreAgility.nFreNum; i++)
				{
					RadarMode.Rf.FreAgility.dpFre[i] = Rf.FreAgility.dpFre[i];
				}
			}
			else if (FreType == RF_DIVERSITY)
			{
				;
			}
		}
		return true;
	}
	/******************************************参数报文生成******************************************/

	/******************************************参数报文处理******************************************/
	size_t 	RecMsgProcess(_ReceiverType &Rec)
	{
		Rec.SetRcs(float(RecMsg.dRCS));
		Rec.SetSensitivity(RecMsg.nLmd);
		Rec.SetMode(RecMsg.nEsmType);
		return 1;
	}
	size_t 	RecMsgProcess(_ReceiverType &Rec, _RecMsg &RecMsg_)
	{
		Rec.SetRcs(float(RecMsg_.dRCS));
		Rec.SetSensitivity(RecMsg_.nLmd);
		Rec.SetMode(RecMsg_.nEsmType);
		return 1;
	}
	size_t 	PlatMsgProcess(_PlatForm *Plat)
	{
		for (size_t i = 0; i < PlatMsg.nPlatNum; i++)
		{
			Plat[i].EnablePlat();
		}
		return 1;
	}
	size_t 	PlatMsgProcess(_PlatForm *Plat, _PlatMsg &PlatMsg_)
	{
		for (size_t i = 0; i < PlatMsg_.nPlatNum; i++)
		{
			Plat[i].EnablePlat();
		}
		return 1;
	}
	size_t 	RadarMsgProcess(_PlatForm *Plat)
	{
		uchar_t PlatSn = RadarMsg.PlatSN;
		Plat[PlatSn].SetRadar(RadarMsg);
		return 1;
	}
	size_t 	RadarMsgProcess(_PlatForm *Plat, _RadarMsg &RadarMsg_)
	{
		uchar_t PlatSn = RadarMsg_.PlatSN;
		Plat[PlatSn].SetRadar(RadarMsg_);
		return 1;
	}
	/******************************************参数报文处理******************************************/
};

void _iRadarAlgorithm::PriFix(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t RadarBufSize = RadarBuf.RadarBufSize;		//Buffer长度 0x1000000
	size_t BufSignSize = RadarBuf.SignSize;				//Buffer标记右移长度 8
	size_t ToaBufferSignLoc,Pri;						//Sign位置       Pri
	size_t ToaMod;										//ToaMod
	ushort_t PlatRadar = Radars.PlatRadar;				//平台雷达号
	size_t ToaNum = 0;									//Toa生成编号
	
	Pri = Radars.PriPara.uPri.dPri;
	ToaMod = Radars.PriPara.ToaMod;
	//
	for(;ToaMod<RadarBufSize;ToaMod+=Pri)
	{
		while((ToaMod<RadarBufSize)&&(RadarBuf.PRNBuffer[ToaMod].PlatRadar != PRNBUFFER_SIGN)){ToaMod++;}
		if(ToaMod<RadarBufSize)
		{
			RadarBuf.PRNBuffer[ToaMod].PlatRadar = PlatRadar;
			RadarBuf.PRNBuffer[ToaMod].ToaNum = ToaNum;
			ToaNum++;
			ToaBufferSignLoc = ToaMod >> BufSignSize;
			if(RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num < SIGN_NUM)
			{
				RadarBuf.ToaBufferSign[ToaBufferSignLoc].Index[RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num] = ToaMod;
			}
			RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num++;
		}
		else
		{
			break;
		}
	}
	Radars.PriPara.ToaMod = ToaMod-RadarBufSize;
}

void _iRadarAlgorithm::PriDither(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t RadarBufSize = RadarBuf.RadarBufSize;							//Buffer长度 0x1000000
	size_t BufSignSize = RadarBuf.SignSize;									//Buffer标记右移长度 8
	size_t ToaBufferSignLoc,GroupNum,GroupNumTemp,Pri,PriRand,DitherRange;	//Sign位置       Pri  GroupNum
	size_t ToaMod,ToaModRand;												//ToaMod
	size_t RandTime = (size_t)time(NULL);									//RandTime
	ushort_t PlatRadar = Radars.PlatRadar;									//平台雷达号
	size_t ToaNum = 0;														//Toa生成编号

	GroupNum = Radars.PriPara.uPri.PriDither.nGroupNum;
	GroupNumTemp = Radars.PriPara.GroupNumTemp;
	Pri = Radars.PriPara.uPri.PriDither.dPri;
	DitherRange = Radars.PriPara.uPri.PriDither.dRange;
	ToaMod = Radars.PriPara.ToaMod;
	ToaModRand = Radars.PriPara.ToaModRand;
	
	DitherRange *= 2;
	if(GroupNumTemp == 0)
	{
		RandTime = size_t(RandTime*69069+12345);
		PriRand = RandTime%DitherRange;
	}
	else
	{
		PriRand = ToaModRand - ToaMod;
	}

	while((ToaModRand<RadarBufSize)&&(RadarBuf.PRNBuffer[ToaModRand].PlatRadar != PRNBUFFER_SIGN)){ToaModRand++;}
	if(ToaModRand<RadarBufSize)
	{
		RadarBuf.PRNBuffer[ToaModRand].PlatRadar = PlatRadar;
		RadarBuf.PRNBuffer[ToaModRand].ToaNum = ToaNum;
		ToaNum++;
		ToaBufferSignLoc = ToaModRand >> BufSignSize;
		if(RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num < SIGN_NUM)
		{
			RadarBuf.ToaBufferSign[ToaBufferSignLoc].Index[RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num] = ToaModRand;
		}
		RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num++;
		GroupNumTemp++;
		if(GroupNumTemp >= GroupNum)
		{
			GroupNumTemp = 0;
			RandTime = size_t(RandTime*69069+12345);
			PriRand = RandTime%DitherRange;
		}
		
		ToaMod = ToaMod+Pri/2;
		
		for(;ToaMod<RadarBufSize;ToaMod+=Pri)
		{
			ToaModRand = ToaMod+PriRand;
			while((ToaModRand<RadarBufSize)&&(RadarBuf.PRNBuffer[ToaModRand].PlatRadar != PRNBUFFER_SIGN)){ToaModRand++;}
			if(ToaModRand < RadarBufSize)
			{
				RadarBuf.PRNBuffer[ToaModRand].PlatRadar = PlatRadar;
				RadarBuf.PRNBuffer[ToaModRand].ToaNum = ToaNum;
				ToaNum++;
				ToaBufferSignLoc = ToaModRand >> BufSignSize;
				if(RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num < SIGN_NUM)
				{
					RadarBuf.ToaBufferSign[ToaBufferSignLoc].Index[RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num] = ToaModRand;
				}
				RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num++;
				GroupNumTemp++;
				if(GroupNumTemp >= GroupNum)
				{
					GroupNumTemp = 0;
					RandTime = size_t(RandTime*69069+12345);
					PriRand = RandTime%DitherRange;
				}
#if DEBUG_PRIDITHER1
				NLOGF(ToaModRand);
#endif 
			}
			else
			{
				break;
			}
		}
	}
	if(ToaMod<RadarBufSize)
	{
		Radars.PriPara.ToaMod = 0;
		Radars.PriPara.ToaModRand = PriRand;
	}
	else
	{
		Radars.PriPara.ToaMod = ToaMod-RadarBufSize;
		Radars.PriPara.ToaModRand = ToaMod-RadarBufSize+PriRand;
	}
	Radars.PriPara.GroupNumTemp = GroupNumTemp;
#if DEBUG_PRIDITHER2
		NLOGF(Radars.PriPara.ToaMod);
#endif 
}

void _iRadarAlgorithm::PriJagging(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t RadarBufSize = RadarBuf.RadarBufSize;				//Buffer长度 0x1000000
	size_t BufSignSize = RadarBuf.SignSize;					//Buffer标记右移长度 8
	size_t ToaBufferSignLoc,GroupNum,GroupNumTemp,PriNum,NumTemp,Pri;	//Sign位置       Pri  GroupNum
	size_t ToaMod;												//ToaMod
	size_t *pRatio;
	ushort_t PlatRadar = Radars.PlatRadar;						//平台雷达号
	size_t ToaNum = 0;											//Toa生成编号

	GroupNum = Radars.PriPara.uPri.PriJagging.nGroupNum;
	PriNum = Radars.PriPara.uPri.PriJagging.nPriNum;
	ToaMod = Radars.PriPara.ToaMod;
	NumTemp = Radars.PriPara.NumTemp;
	GroupNumTemp = Radars.PriPara.GroupNumTemp;
	pRatio = Radars.PriPara.uPri.PriJagging.dpRatio;
	Pri = pRatio[NumTemp];
	NumTemp++;
	if(NumTemp >= PriNum)
	{
		NumTemp = 0;
	}
	
	for(;ToaMod<RadarBufSize;ToaMod+=Pri)
	{
		while((ToaMod<RadarBufSize)&&(RadarBuf.PRNBuffer[ToaMod].PlatRadar != PRNBUFFER_SIGN)){ToaMod++;}
		if(ToaMod<RadarBufSize)
		{
			RadarBuf.PRNBuffer[ToaMod].PlatRadar = PlatRadar;
			RadarBuf.PRNBuffer[ToaMod].ToaNum = ToaNum;
			ToaNum++;
			ToaBufferSignLoc = ToaMod >> BufSignSize;
			if(RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num < SIGN_NUM)
			{
				RadarBuf.ToaBufferSign[ToaBufferSignLoc].Index[RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num] = ToaMod;
			}
			RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num++;
			GroupNumTemp++;
			if(GroupNumTemp >= GroupNum)
			{
				GroupNumTemp = 0;
				Pri = pRatio[NumTemp];
				NumTemp++;
				if(NumTemp >= PriNum)
				{
					NumTemp = 0;
				}
			}
#if DEBUG_PRIJAGGING
			NLOGF(Pri);
#endif 
		}
		else
		{
			break;
		}
	}
	Radars.PriPara.ToaMod = ToaMod-RadarBufSize;
	if(NumTemp == 0)
	{
		Radars.PriPara.NumTemp = PriNum-1;
	}
	else
	{
		Radars.PriPara.NumTemp = NumTemp-1;
	}
	Radars.PriPara.GroupNumTemp = GroupNumTemp;
}

void _iRadarAlgorithm::PriSlider(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t RadarBufSize = RadarBuf.RadarBufSize;				//Buffer长度 0x1000000
	size_t BufSignSize = RadarBuf.SignSize;					//Buffer标记右移长度 8
	size_t ToaBufferSignLoc,GroupNum,GroupNumTemp,Pri;			//Sign位置       Pri  GroupNum
	int PriNum,NumTemp,PriStep;
	size_t ToaMod;												//ToaMod
	size_t *pPri;
	ushort_t PlatRadar = Radars.PlatRadar;						//平台雷达号
	size_t ToaNum = 0;											//Toa生成编号

	GroupNum = Radars.PriPara.uPri.PriSlider.nGroupNum;
	PriNum = Radars.PriPara.uPri.PriSlider.nPriNum;
	PriStep = Radars.PriPara.uPri.PriSlider.dPriStep;
	ToaMod = Radars.PriPara.ToaMod;
	NumTemp = Radars.PriPara.NumTemp;
	pPri = Radars.PriPara.uPri.PriSlider.dpPri;
	GroupNumTemp = Radars.PriPara.GroupNumTemp;
	Pri = pPri[NumTemp];
	NumTemp += PriStep;
	if(NumTemp >= PriNum)
	{
		NumTemp = NumTemp-PriNum;
	}
	else if(NumTemp < 0)
	{
		NumTemp = PriNum+NumTemp;
	}
	
	for(;ToaMod<RadarBufSize;ToaMod+=Pri)
	{
		while(RadarBuf.PRNBuffer[ToaMod].PlatRadar != PRNBUFFER_SIGN){ToaMod++;}
		if(ToaMod<RadarBufSize)
		{
			RadarBuf.PRNBuffer[ToaMod].PlatRadar = PlatRadar;
			RadarBuf.PRNBuffer[ToaMod].ToaNum = ToaNum;
			ToaNum++;
			ToaBufferSignLoc = ToaMod >> BufSignSize;
			if(RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num < SIGN_NUM)
			{
				RadarBuf.ToaBufferSign[ToaBufferSignLoc].Index[RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num] = ToaMod;
			}
			RadarBuf.ToaBufferSign[ToaBufferSignLoc].Num++;
			
			if(GroupNumTemp >= GroupNum)
			{
				GroupNumTemp = 0;
				Pri = pPri[NumTemp];
				NumTemp += PriStep;
				if(NumTemp >= PriNum)
				{
					NumTemp = NumTemp-PriNum;
				}
				else if(NumTemp < 0)
				{
					NumTemp = PriNum+NumTemp;
				}
			}
			GroupNumTemp++;
#if DEBUG_PRISLIDER
			NLOGF(Pri);
#endif 
		}
		else
		{
			break;
		}
	}
	Radars.PriPara.ToaMod = ToaMod-RadarBufSize;
	NumTemp -= PriStep;
	if(NumTemp >= PriNum)
	{
		NumTemp = NumTemp-PriNum;
	}
	else if(NumTemp < 0)
	{
		NumTemp = PriNum+NumTemp;
	}
	Radars.PriPara.NumTemp = NumTemp;
	Radars.PriPara.GroupNumTemp = GroupNumTemp;
}

void _iRadarAlgorithm::PriIdle(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	;
}

void _iRadarAlgorithm::PwFix(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t Pw = Radars.PwPara.sPw.dPw;
	ushort_t PlatRadar = Radars.PlatRadar;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
#if DEBUG_PWGEN
	NLogF("|******************************* PwFix *******************************|");
#endif
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Pw = Pw;
#if DEBUG_PWGEN
//		{
//			static int i = 0;
//			if(i<10)
//			{
				NLOGF(Pw);
//				NLOGF(RadarBuf.NoToaPara[PlatRadar][i].Pw);
//				i++;
//			}
//		}
#endif
	}
#if DEBUG_PWGEN
	NLogF("|******************************* PwFix *******************************|");
#endif
}

void _iRadarAlgorithm::PwDefine(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t *pPw,Pw;
	pPw = Radars.PwPara.sPw.PwDefine.dpPw;
	ushort_t PlatRadar = Radars.PlatRadar;
    uchar_t	PwNum = Radars.PwPara.sPw.PwDefine.nPwNum;
    uchar_t NumTemp = Radars.PwPara.NumTemp;
    ushort_t GroupNumTemp = Radars.PwPara.GroupNumTemp,GroupNum = Radars.PwPara.sPw.PwDefine.nGroupNum;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
	
	Pw = pPw[NumTemp];
	NumTemp++;
	if(NumTemp >= PwNum)
	{
		NumTemp = 0;
	}

#if DEBUG_PWGEN
	NLogF("|******************************* PwDefine *******************************|");
#endif
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Pw = Pw;
		GroupNumTemp++;
#if DEBUG_PWGEN
//		{
//			static int i = 0;
//			if(i<10)
//			{
				NLOGF(Pw);
//				NLOGF(RadarBuf.NoToaPara[PlatRadar][i].Pw);
//				i++;
//			}
//		}
#endif
		if(GroupNumTemp >= GroupNum)
		{
			GroupNumTemp = 0;
			Pw = pPw[NumTemp];
			NumTemp++;
			if(NumTemp >= PwNum)
			{
				NumTemp = 0;
			}
		}
	}
#if DEBUG_PWGEN
	NLogF("|******************************* PwDefine *******************************|");
#endif
	if(NumTemp == 0)
	{
		Radars.PwPara.NumTemp = PwNum-1;
	}
	else
	{
		Radars.PwPara.NumTemp = NumTemp-1;
	}
	Radars.PwPara.GroupNumTemp = GroupNumTemp;
}

void _iRadarAlgorithm::FreContinuously(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t RF = Radars.RFPara.sFre.dFre;
	ushort_t PlatRadar = Radars.PlatRadar;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
#if DEBUG_RFGEN
	NLogF("|******************************* Frecontinuously *******************************|");
#endif
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Flag = 1;
		RadarBuf.RadarPara[PlatRadar][i].Rf = RF;
#if DEBUG_RFGEN
//		{
//			static int i = 0;
//			if(i<10)
//			{
				NLOGF(RF);
//				NLOGF(RadarBuf.NoToaPara[PlatRadar][i].Rf);
//				i++;
//			}
//		}
#endif
	}
#if DEBUG_RFGEN
	NLogF("|******************************* Frecontinuously *******************************|");
#endif
}

void _iRadarAlgorithm::FreFix(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t RF = Radars.RFPara.sFre.dFre;
	ushort_t PlatRadar = Radars.PlatRadar;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
#if DEBUG_RFGEN
	NLogF("|******************************* FreFix *******************************|");
#endif
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Flag = 0;
		RadarBuf.RadarPara[PlatRadar][i].Rf = RF;
#if DEBUG_RFGEN
//		{
//			static int i = 0;
//			if(i<10)
//			{
				NLOGF(RF);
//				NLOGF(RadarBuf.NoToaPara[PlatRadar][i].Rf);
//				i++;
//			}
//		}
#endif
	}
#if DEBUG_RFGEN
	NLogF("|******************************* FreFix *******************************|");
#endif
}

void _iRadarAlgorithm::FreAgility(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t *pFre,RF;
	pFre = Radars.RFPara.sFre.FreAgility.dpFre;
	ushort_t PlatRadar = Radars.PlatRadar;
    ushort_t RFNum = Radars.RFPara.sFre.FreAgility.nFreNum;
    uchar_t NumTemp = Radars.RFPara.NumTemp;
    ushort_t GroupNumTemp = Radars.RFPara.GroupNumTemp,GroupNum = Radars.RFPara.sFre.FreAgility.nGroupNum;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
	
	RF = pFre[NumTemp];
	NumTemp++;
	if(NumTemp >= RFNum)
	{
		NumTemp = 0;
	}
#if DEBUG_RFGEN
	NLogF("|******************************* FreAgility *******************************|");
#endif
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Flag = 0;
		RadarBuf.RadarPara[PlatRadar][i].Rf = RF;
		GroupNumTemp++;
#if DEBUG_RFGEN
//		{
//			static int i = 0;
//			if(i<10)
//			{
				NLOGF(RF);
//				NLOGF(RadarBuf.NoToaPara[PlatRadar][i].Rf);
//				i++;
//			}
//		}
#endif
		if(GroupNumTemp >= GroupNum)
		{
			GroupNumTemp = 0;
			RF = pFre[NumTemp];
			NumTemp++;
			if(NumTemp >= RFNum)
			{
				NumTemp = 0;
			}
		}
	}
#if DEBUG_RFGEN
	NLogF("|******************************* FreAgility *******************************|");
#endif
	if(NumTemp == 0)
	{
		Radars.RFPara.NumTemp = RFNum-1;
	}
	else
	{
		Radars.RFPara.NumTemp = NumTemp-1;
	}
	Radars.RFPara.GroupNumTemp = GroupNumTemp;
}

template<typename _OutPdwType>
class _iRadarSimPro
{
private:
	enum 
	{
		Diff 	 = 1,
		Slider 	 = 2,
		RandPara = 3		
	};
	_PlatForm				*PlatForm;
	_RadarBuffer			RadarBuffer1, RadarBuffer2;
	_Receiver<_OutPdwType>	Receiver1, Receiver2;
	_Pri 					Pri;
	_Rf 					Rf;
	_Pw 					Pw;
	_XkMsg<_OutPdwType>		XkMsg;
	size_t 					*iRadarStatus;
public:
	_iRadarSimPro()
	{
		PlatForm = new _PlatForm[PLAT_NUM];
		iRadarStatus = new size_t[PLAT_NUM];
		memset(iRadarStatus,0,PLAT_NUM*sizeof(size_t));
		RadarBuffer1.NoToaParaBufferLink(RadarBuffer2);
		ReceiverInit();
		PlatFormInit();
	}
	
	~_iRadarSimPro()
	{
		delete[] PlatForm;
		delete[] iRadarStatus;
	}
	
	bool ReceiverInit(int dRCS_ = 100, short nLmd_ = -10, ushort_t nEsmType_ = BroadBank)
	{
		RadarBuffer1.BufferClear();
		RadarBuffer2.BufferClear();
		Receiver1.BufferClear();
		Receiver2.BufferClear();
		XkMsg.RecMsgGen(dRCS_,nLmd_,nEsmType_);
		XkMsg.RecMsgProcess(Receiver1);
		XkMsg.RecMsgProcess(Receiver2);
		return true;
	}
	
	bool PlatFormInit(_PlatMsgType PlatMsgType = SingleRadar)		//快速初始化平台	
	{
		PlatFormClear();
		switch (PlatMsgType)
		{
		case SingleRadar:
			iRadarStatus[0] = 1;
			break;
		case SinglePlat:
			iRadarStatus[0] = 10;
			break;
		case FullNum:
			for(size_t i = 0;i < PLAT_NUM; i++)
			{
				iRadarStatus[i] = 10;
			}
			break;
		default:
			break;
		}
		XkMsg.PlatMsgGen(PlatMsgType);
		XkMsg.PlatMsgProcess(PlatForm);
		return true;
	}

	bool PlatFormInit(size_t PlatNo)								//按个数初始化平台
	{
		PlatFormClear();
		if(PlatNo < PLAT_NUM)
		{
			for(size_t i = 0;i < PlatNo; i++)
			{
				iRadarStatus[i] = 10;
			}
			XkMsg.PlatMsgGen(PlatNo);
			XkMsg.PlatMsgProcess(PlatForm);
		}
		else
		{
			for(size_t i = 0;i < PLAT_NUM; i++)
			{
				iRadarStatus[i] = 10;
			}
			XkMsg.PlatMsgGen(PLAT_NUM);
			XkMsg.PlatMsgProcess(PlatForm);
		}
		return true;
	}
	
	bool PlatFormClear(size_t PlatNo = PLAT_NUM)
	{
		if(PlatNo == PLAT_NUM)
		{
			for(size_t i = 0;i < PLAT_NUM; i++)
			{
				PlatForm[i].Clear();
			}
			memset(iRadarStatus,0,PLAT_NUM*sizeof(size_t));
			return true;
		}
		else if(PlatNo < PLAT_NUM)
		{
			PlatForm[PlatNo].Clear();
			iRadarStatus[PlatNo] = 0;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	int RadarParaGen(size_t *ArrTemp,size_t PriL,size_t PriR,size_t nPriNum,size_t ParaGenType)
	{
		ArrTemp[0] = PriL;
		ArrTemp[nPriNum - 1] = PriR;
		double x = 0.0, y = 0.0;
		size_t RandTime = (size_t)time(NULL);
		switch(ParaGenType)
		{
		case Diff:
			x = double(PriR)/double(PriL);
			y = pow(x,1.0/(nPriNum-1));
			for(size_t i = 1; i < nPriNum - 1; i++)
			{
				ArrTemp[i] = size_t(y*ArrTemp[i-1]);
			}
			break;
		case Slider:
			x = PriR - PriL;
			y = x/(nPriNum-1);
			for(size_t i = 1; i < nPriNum - 1; i++)
			{
				ArrTemp[i] = ArrTemp[i-1] + size_t(y);
			}
			break;
		case RandPara:
			x = PriR - PriL;
			for(size_t i = 1; i < nPriNum - 1; i++)
			{
				RandTime = size_t(RandTime*69069+12345);
				RandTime = RandTime%size_t(x);
				ArrTemp[i] = PriL + RandTime;
			}
			break;
		default:
			break;
		}
		return int(y);
	}
	
	bool RadarPriTypeModify(_PriType PriType 	= PRI_FIX,
							size_t dPri 		= 10000,
							size_t dRange 		= 5000,		//重频范围  重频范围不大于中心重复周期的100%
							size_t nPriNum 		= 10,		//参差或滑动数
							size_t nGroupNum 	= 1,		//脉组脉冲数  1-1000  1－脉间 
							size_t *PriArr 		= NULL,		//自定义重频数组
							size_t PriArrLen 	= 0,		//自定义重频数组长度
							size_t dPriStep		= 1			//滑动步进 有正有负
							)
	{
		size_t *ArrTemp = new size_t[nPriNum];
		memset(ArrTemp,0,nPriNum*sizeof(size_t));
		if (PriType == PRI_FIX)
		{
			Pri.dPri = dPri;
		}
		else if (PriType == PRI_DITHER)
		{
			Pri.PriDither.dPri	  		= dPri;
			Pri.PriDither.dRange		= dRange;
			Pri.PriDither.nGroupNum 	= nGroupNum;
		}
		else if (PriType == PRI_JAGGING)
		{
			Pri.PriJagging.nPriNum 		= nPriNum;
			Pri.PriJagging.nGroupNum	= nGroupNum;
			if(PriArrLen == 0)
			{
				size_t PriL = (dPri <= dRange)?100:(dPri-dRange);
				size_t PriR = dPri + dRange;
				RadarParaGen(ArrTemp,PriL,PriR,nPriNum,Diff);
				for (size_t i = 0; i < nPriNum; i++)
				{
					Pri.PriJagging.dpRatio[i] = ArrTemp[i];
				}
			}
			else
			{
				if((PriArrLen != nPriNum) && (PriArrLen == 2))
				{
					size_t PriL = PriArr[0];
					size_t PriR = PriArr[1];
					RadarParaGen(ArrTemp,PriL,PriR,nPriNum,Diff);
					for (size_t i = 0; i < nPriNum; i++)
					{
						Pri.PriJagging.dpRatio[i] = ArrTemp[i];
					}
				}
				else if(PriArrLen >= nPriNum)
				{
					for (size_t i = 0; i < nPriNum; i++)
					{
						Pri.PriJagging.dpRatio[i] = PriArr[i];
					}
				}
				else
				{
					Pri.PriJagging.nPriNum = PriArrLen;
					for (size_t i = 0; i < PriArrLen; i++)
					{
						Pri.PriJagging.dpRatio[i] = PriArr[i];
					}
				}
			}
		}
		else if (PriType == PRI_SLIDER)
		{
			Pri.PriSlider.nPriNum 		= nPriNum;
			Pri.PriSlider.nGroupNum 	= nGroupNum;
			Pri.PriSlider.dPriStep 		= dPriStep;
			if(PriArrLen == 0)
			{
				size_t PriL = (dPri <= dRange)?100:(dPri-dRange);
				size_t PriR = dPri + dRange;
				RadarParaGen(ArrTemp,PriL,PriR,nPriNum,Slider);
				for (size_t i = 0; i < nPriNum; i++)
				{
					Pri.PriSlider.dpPri[i] = ArrTemp[i];
				}
			}
			else
			{
				if((PriArrLen != nPriNum) && (PriArrLen == 2))
				{
					size_t PriL = PriArr[0];
					size_t PriR = PriArr[1];
					RadarParaGen(ArrTemp,PriL,PriR,nPriNum,Slider);
					for (size_t i = 0; i < nPriNum; i++)
					{
						Pri.PriSlider.dpPri[i] = ArrTemp[i];
					}
				}
				else if(PriArrLen >= nPriNum)
				{
					for (size_t i = 0; i < nPriNum; i++)
					{
						Pri.PriSlider.dpPri[i] = PriArr[i];
					}
				}
				else
				{
					Pri.PriSlider.nPriNum = PriArrLen;
					for (size_t i = 0; i < PriArrLen; i++)
					{
						Pri.PriSlider.dpPri[i] = PriArr[i];
					}
				}
			}
		}
		XkMsg.RadarMsgPriParaModify(PriType,&Pri);
		delete[] ArrTemp;
		return true;
	}

	bool RadarPwTypeModify(_PwType PwType 		= PW_FIX,
						   size_t dPw			= 50,
						   size_t nPwNum		= 10,		//脉宽数量
						   size_t nGroupNum		= 1,		//脉组脉冲数  1-1000
						   size_t *PwArr 		= NULL,		//自定义脉宽数组
						   size_t PwArrLen 		= 0			//自定义脉宽数组长度
						   )
	{
		size_t *ArrTemp = new size_t[nPwNum];
		memset(ArrTemp,0,nPwNum*sizeof(size_t));
		if (PwType == PW_FIX)
		{
			Pw.dPw = dPw;
		}
		else if (PwType == PW_DEFINE)
		{
			Pw.PwDefine.nPwNum = nPwNum;
			Pw.PwDefine.nGroupNum = nGroupNum;
			if(PwArrLen == 0)
			{
				size_t PwL = dPw*5/10;
				size_t PwR = dPw*2;
				RadarParaGen(ArrTemp,PwL,PwR,nPwNum,RandPara);
				for (size_t i = 0; i < nPwNum; i++)
				{
					Pw.PwDefine.dpPw[i] = ArrTemp[i];
				}
			}
			else
			{
				if((PwArrLen != nPwNum) && (PwArrLen == 2))
				{
					size_t PwL = PwArr[0];
					size_t PwR = PwArr[1];
					RadarParaGen(ArrTemp,PwL,PwR,nPwNum,RandPara);
					for (size_t i = 0; i < nPwNum; i++)
					{
						Pw.PwDefine.dpPw[i] = ArrTemp[i];
					}
				}
				else if(PwArrLen >= nPwNum)
				{
					for (size_t i = 0; i < nPwNum; i++)
					{
						Pw.PwDefine.dpPw[i] = PwArr[i];
					}
				}
				else
				{
					Pw.PwDefine.nPwNum = PwArrLen;
					for (size_t i = 0; i < PwArrLen; i++)
					{
						Pw.PwDefine.dpPw[i] = PwArr[i];
					}
				}
			}
		}
		XkMsg.RadarMsgPwParaModify(PwType,&Pw);
		delete[] ArrTemp;
		return true;
	}

	bool RadarRfTypeModify(_RfType FreType 		= RF_FIX,
						   size_t dFre			= 256000,
						   size_t nFreNum		= 10,		//点数 2-DIVNUM
						   size_t nGroupNum 	= 1,		//脉组脉冲数  1-1000  1－脉间 
						   size_t *RfArr 		= NULL,		//自定义载频数组
						   size_t RfArrLen 		= 0			//自定义载频数组长度
						   )
	{
		size_t *ArrTemp = new size_t[nFreNum];
		memset(ArrTemp,0,nFreNum*sizeof(size_t));
		if (FreType == RF_CONTINUOUSLY)
		{
			Rf.dFre = dFre;
		}
		else if (FreType == RF_FIX)
		{
			Rf.dFre = dFre;
		}
		else if (FreType == RF_AGILITY)
		{
			Rf.FreAgility.nFreNum 	= nFreNum;
			Rf.FreAgility.nGroupNum = nGroupNum;
			if(RfArrLen == 0)
			{
				size_t RfL = dFre*5/10;
				size_t RfR = dFre*2;
				RadarParaGen(ArrTemp,RfL,RfR,nFreNum,RandPara);
				for (size_t i = 0; i < nFreNum; i++)
				{
					Rf.FreAgility.dpFre[i] = ArrTemp[i];
				}
			}
			else
			{
				if((RfArrLen != nFreNum) && (RfArrLen == 2))
				{
					size_t RfL = RfArr[0];
					size_t RfR = RfArr[1];
					RadarParaGen(ArrTemp,RfL,RfR,nFreNum,RandPara);
					for (size_t i = 0; i < nFreNum; i++)
					{
						Rf.FreAgility.dpFre[i] = ArrTemp[i];
					}
				}
				else if(RfArrLen >= nFreNum)
				{
					for (size_t i = 0; i < nFreNum; i++)
					{
						Rf.FreAgility.dpFre[i] = RfArr[i];
					}
				}
				else
				{
					Pw.PwDefine.nPwNum = RfArrLen;
					for (size_t i = 0; i < RfArrLen; i++)
					{
						Rf.FreAgility.dpFre[i] = RfArr[i];
					}
				}
			}
		}
		else if (FreType == RF_DIVERSITY)
		{
			;
		}
		XkMsg.RadarMsgRfParaModify(FreType,&Rf);
		delete[] ArrTemp;
		return true;
	}
	
	bool RadarModify(size_t RadarSn)
	{
		size_t PlatNo 	= RadarSn/10;
		size_t RadarNo 	= RadarSn%10;
		if(iRadarStatus[PlatNo] > RadarNo)
		{
			XkMsg.RadarMsgPlatRadarNumModify(PlatNo,RadarNo);
			XkMsg.RadarMsgProcess(PlatForm);
			return true;
		}
		else
		{
			return false;
		}
	}

	int RadarModify(size_t *RadarSnArr,size_t RadarSnArrLen)
	{
		size_t rtn = 0;
		for(size_t i = 0; i < RadarSnArrLen; i++)
		{
			size_t PlatNo 	= RadarSnArr[i]/10;
			size_t RadarNo 	= RadarSnArr[i]%10;
			if(iRadarStatus[PlatNo] > RadarNo)
			{
				XkMsg.RadarMsgPlatRadarNumModify(PlatNo,RadarNo);
				XkMsg.RadarMsgProcess(PlatForm);
				rtn++;
			}
		}
		return rtn;
	}
	
	void Init(UINT PlatNo = 1)
	{
		ReceiverInit();
		PlatFormInit(PlatNo);
	}
	
	void RadarSimGen(size_t PdwType, size_t RadarSn, size_t Rf, size_t Pri, size_t RfNum = 0, size_t PriNumOrRange = 0)		//重频或载频为参差或者抖动时，Rf、Pri传入数组地址
	{
		const size_t nGroupNum 	= 1;
		const size_t dRange 	= 30;
		size_t *PriArr = (size_t *)Pri;
		size_t *RfArr = (size_t *)Rf;
		switch(PdwType)
		{
		case 1:														//FixRfFixPri
			RadarRfTypeModify(RF_FIX, Rf);
			RadarPriTypeModify(PRI_FIX, Pri);
			RadarModify(RadarSn);
			break;
		case 2:														//FixRfJaggPri
			RadarRfTypeModify(RF_FIX, Rf);
			RadarPriTypeModify(PRI_JAGGING, 0, dRange, PriNumOrRange, nGroupNum, PriArr, PriNumOrRange);
			RadarModify(RadarSn);
			break;
		case 3:														//FixRfDitherPri
			RadarRfTypeModify(RF_FIX, Rf);
			RadarPriTypeModify(PRI_DITHER, Pri, PriNumOrRange);
			RadarModify(RadarSn);
			break;
		case 4:														//AgileRfFixPri
			RadarRfTypeModify(RF_AGILITY, 0, RfNum, 1, RfArr, RfNum);
			RadarPriTypeModify(PRI_FIX, Pri);
			RadarModify(RadarSn);
			break;
		case 5:														//CotinuePulse
			break;
		case 6:														//AgileRfDitherPri
			RadarRfTypeModify(RF_AGILITY, 0, RfNum, 1, RfArr, RfNum);
			RadarPriTypeModify(PRI_DITHER, Pri, PriNumOrRange);
			RadarModify(RadarSn);
			break;
		default:
			break;
		}
	}

	void CoutStatus(size_t Platsel = 0,size_t RadarBuffersel = 0,size_t Receiversel = 0)
	{
		CoutPlatStatus(Platsel);
		CoutRadarBufferStatus(RadarBuffersel);
		CoutReceiverStatus(Receiversel);
	}
	
	void CoutPlatStatus(size_t sel = 0)
	{
		if(sel == 0)
		{
			for(size_t i = 0;i < PLAT_NUM; i++)
			{
				if(iRadarStatus[i] != 0)
				{
					NLogF("\n|-------------------------------------| PlatForm[%d] Start |----------------------------|\n",i);
					PlatForm[i].showStatus();
					NLogF("\n|-------------------------------------| PlatForm[%d] End   |----------------------------|\n",i);
				}
			}
		}
		else if(sel < PLAT_NUM)
		{
			NLogF("\n|-------------------------------------| PlatForm[%d] Start |----------------------------|\n",sel);
			PlatForm[sel].showStatus();
			NLogF("\n|-------------------------------------| PlatForm[%d] End   |----------------------------|\n",sel);
		}
	}
	
	void CoutRadarBufferStatus(size_t sel = 0)
	{
		if(sel == 0)
		{
			NLogF("\n|---------------------------| RadarBuffer1 Start |----------------------------|\n");
			RadarBuffer1.showStatus();
			NLogF("\n|---------------------------| RadarBuffer1 End   |----------------------------|\n");
			NLogF("\n|---------------------------| RadarBuffer2 Start |----------------------------|\n");
			RadarBuffer2.showStatus();
			NLogF("\n|---------------------------| RadarBuffer2 End   |----------------------------|\n");
		}
		else if(sel == 1)
		{
			NLogF("\n|---------------------------| RadarBuffer1 Start |----------------------------|\n");
			RadarBuffer1.showStatus();
			NLogF("\n|---------------------------| RadarBuffer1 End   |----------------------------|\n");
		}
		else if(sel == 2)
		{
			NLogF("\n|---------------------------| RadarBuffer2 Start |----------------------------|\n");
			RadarBuffer2.showStatus();
			NLogF("\n|---------------------------| RadarBuffer2 End   |----------------------------|\n");
		}
	}

	void CoutReceiverStatus(size_t sel = 0)
	{
		if(sel == 0)
		{
			NLogF("\n|---------------------------| Receiver1 Start |----------------------------|\n");
			Receiver1.showStatus();
			NLogF("\n|---------------------------| Receiver1 End   |----------------------------|\n");
			NLogF("\n|---------------------------| Receiver2 Start |----------------------------|\n");
			Receiver2.showStatus();
			NLogF("\n|---------------------------| Receiver2 End   |----------------------------|\n");
		}
		else if(sel == 1)
		{
			NLogF("\n|---------------------------| Receiver1 Start |----------------------------|\n");
			Receiver1.showStatus();
			NLogF("\n|---------------------------| Receiver1 End   |----------------------------|\n");
		}
		else if(sel == 2)
		{
			NLogF("\n|---------------------------| Receiver2 Start |----------------------------|\n");
			Receiver2.showStatus();
			NLogF("\n|---------------------------| Receiver2 End   |----------------------------|\n");
		}
	}
	
	void MutiPlatPriRun(_RadarBuffer & RadarBuf)
	{
		size_t PlatRunNum = 0;
		if ((RadarBuf.SortReadyStatus == PARABUF_IDLE) && (RadarBuf.PriStatus == IDLE))
		{
			RadarBuf.PriStatus = GENBUSY;
			for (size_t i = 0; i<PLAT_NUM; i++)
			{
				if (PlatForm[i].GetPlatFormEn() == true)
				{
					PlatRunNum++;
					for (size_t j = 0; j<RADAR_NUM; j++)
					{
						if (PlatForm[i].GetRadarMode(j) != RadarMode_Null)
						{
							(PlatForm[i].GetRadar(j).PriFunc)(PlatForm[i].GetRadar(j), RadarBuf);
						}
					}
				}
			}
			if (PlatRunNum == 0)
			{
				RadarBuf.PriStatus = IDLE;
				return;
			}
			if ((RadarBuf.PwStatus == READY) && (RadarBuf.RFStatus == READY))
			{
				RadarBuf.PriStatus = IDLE;
				RadarBuf.PwStatus = IDLE;
				RadarBuf.RFStatus = IDLE;
				RadarBuf.SortReadyStatus = PARABUF_READY;
			}
			else
			{
				RadarBuf.PriStatus = READY;
			}
		}
	}

	void MutiPlatPwRun(_RadarBuffer & RadarBuf)
	{
		size_t PlatRunNum = 0;
		if ((RadarBuf.SortReadyStatus == PARABUF_IDLE) && (RadarBuf.PwStatus == IDLE))
		{
			RadarBuf.PwStatus = GENBUSY;
			for (size_t i = 0; i<PLAT_NUM; i++)
			{
				if (PlatForm[i].GetPlatFormEn() == true)
				{
					PlatRunNum++;
					for (size_t j = 0; j<RADAR_NUM; j++)
					{
						if (PlatForm[i].GetRadarMode(j) != 0)
						{
							(PlatForm[i].GetRadar(j).PwFunc)(PlatForm[i].GetRadar(j), RadarBuf);
						}
					}
				}
			}
			if (PlatRunNum == 0)
			{
				RadarBuf.PwStatus = IDLE;
				return;
			}
			if ((RadarBuf.PriStatus == READY) && (RadarBuf.RFStatus == READY))
			{
				RadarBuf.PriStatus = IDLE;
				RadarBuf.PwStatus = IDLE;
				RadarBuf.RFStatus = IDLE;
				RadarBuf.SortReadyStatus = PARABUF_READY;
			}
			else
			{
				RadarBuf.PwStatus = READY;
			}
		}
	}

	void MutiPlatRFRun(_RadarBuffer & RadarBuf)
	{
		size_t PlatRunNum = 0;
		if ((RadarBuf.SortReadyStatus == PARABUF_IDLE) && (RadarBuf.RFStatus == IDLE))
		{
			RadarBuf.RFStatus = GENBUSY;
			for (size_t i = 0; i<PLAT_NUM; i++)
			{
				if (PlatForm[i].GetPlatFormEn() == true)
				{
					PlatRunNum++;
					for (size_t j = 0; j<RADAR_NUM; j++)
					{
						if (PlatForm[i].GetRadarMode(j) != 0)
						{
							(PlatForm[i].GetRadar(j).RFFunc)(PlatForm[i].GetRadar(j), RadarBuf);
						}
					}
				}
			}
			if (PlatRunNum == 0)
			{
				RadarBuf.RFStatus = IDLE;
				return;
			}
			if ((RadarBuf.PriStatus == READY) && (RadarBuf.PwStatus == READY))
			{
				RadarBuf.PriStatus = IDLE;
				RadarBuf.PwStatus = IDLE;
				RadarBuf.RFStatus = IDLE;
				RadarBuf.SortReadyStatus = PARABUF_READY;
			}
			else
			{
				RadarBuf.RFStatus = READY;
			}
		}
	}

	void ParallelPDWSort()
	{
		static size_t OutPdwSortNum = 1;
		if ((Receiver1.OutPdwStatus == OutPDW_IDLE) && OutPdwSortNum == 1)
		{
			if ((RadarBuffer1.InPDWStatus == InPDW_READY) && (RadarBuffer2.InPDWStatus == InPDW_READY))
			{
				if (RadarBuffer1.GetRadarBufferSortTimes() < RadarBuffer2.GetRadarBufferSortTimes())
				{
					Receiver1.PDWSort(RadarBuffer1);
					Receiver1.PDWSort(RadarBuffer2);
				}
				else
				{
					Receiver1.PDWSort(RadarBuffer2);
					Receiver1.PDWSort(RadarBuffer1);
				}
			}
			else
			{
				Receiver1.PDWSort(RadarBuffer1);
				Receiver1.PDWSort(RadarBuffer2);
			}
			if (Receiver1.OutPdwStatus == OutPDW_READY)
			{
				OutPdwSortNum = 2;
			}
		}
		if ((Receiver2.OutPdwStatus == OutPDW_IDLE) && OutPdwSortNum == 2)
		{
			if ((RadarBuffer1.InPDWStatus == InPDW_READY) && (RadarBuffer2.InPDWStatus == InPDW_READY))
			{
				if (RadarBuffer1.GetRadarBufferSortTimes() < RadarBuffer2.GetRadarBufferSortTimes())
				{
					Receiver2.PDWSort(RadarBuffer1);
					Receiver2.PDWSort(RadarBuffer2);
				}
				else
				{
					Receiver2.PDWSort(RadarBuffer2);
					Receiver2.PDWSort(RadarBuffer1);
				}
			}
			else
			{
				Receiver2.PDWSort(RadarBuffer1);
				Receiver2.PDWSort(RadarBuffer2);
			}
			if (Receiver2.OutPdwStatus == OutPDW_READY)
			{
				OutPdwSortNum = 1;
			}
		}
	}

	inline void ModifyOutPdw(_OutPdwType &OutPdw)										//自定义输出数据
	{
		OutPdw.Pa = 50;
		OutPdw.Rf = OutPdw.Rf|0x8000;
	}
	
	void ModifyOutPdwArray(size_t Start = 0,size_t Lens = 0,size_t sel = 0)				//自定义指定长度输出数据
	{
		_OutPdwType *OutPdwBuf = NULL;
		size_t OutPdwLens = 0;
		if(sel == 0)
		{
			if (Receiver1.OutPdwStatus == OutPDW_READY)
			{
				OutPdwBuf  = (_OutPdwType *)Receiver1.GetOutPdwBufAddr();
				OutPdwLens = Receiver1.GetOutPdwBufSize();
				if(Lens == 0)
				{
					for(size_t i = Start; i < OutPdwLens; i++)
					{
						ModifyOutPdw(OutPdwBuf[i]);
					}
				}
				else if(Start+Lens < OutPdwLens)
				{
					for(size_t i = Start; i < Lens; i++)
					{
						ModifyOutPdw(OutPdwBuf[i]);
					}
				}
			}
			if (Receiver2.OutPdwStatus == OutPDW_READY)
			{
				OutPdwBuf  = (_OutPdwType *)Receiver2.GetOutPdwBufAddr();
				OutPdwLens = Receiver2.GetOutPdwBufSize();
				if(Lens == 0)
				{
					for(size_t i = Start; i < OutPdwLens; i++)
					{
						ModifyOutPdw(OutPdwBuf[i]);
					}
				}
				else if(Start+Lens < OutPdwLens)
				{
					for(size_t i = Start; i < Lens; i++)
					{
						ModifyOutPdw(OutPdwBuf[i]);
					}
				}
			}
		}
		else if(sel == 1)
		{
			if (Receiver1.OutPdwStatus == OutPDW_READY)
			{
				OutPdwBuf  = (_OutPdwType *)Receiver1.GetOutPdwBufAddr();
				OutPdwLens = Receiver1.GetOutPdwBufSize();
				if(Lens == 0)
				{
					for(size_t i = Start; i < OutPdwLens; i++)
					{
						ModifyOutPdw(OutPdwBuf[i]);
					}
				}
				else if(Start+Lens < OutPdwLens)
				{
					for(size_t i = Start; i < Lens; i++)
					{
						ModifyOutPdw(OutPdwBuf[i]);
					}
				}
			}
		}
		else if(sel == 2)
		{
			if (Receiver2.OutPdwStatus == OutPDW_READY)
			{
				OutPdwBuf  = (_OutPdwType *)Receiver2.GetOutPdwBufAddr();
				OutPdwLens = Receiver2.GetOutPdwBufSize();
				if(Lens == 0)
				{
					for(size_t i = Start; i < OutPdwLens; i++)
					{
						ModifyOutPdw(OutPdwBuf[i]);
					}
				}
				else if(Start+Lens < OutPdwLens)
				{
					for(size_t i = Start; i < Lens; i++)
					{
						ModifyOutPdw(OutPdwBuf[i]);
					}
				}
			}
		}
	}
	
	void run_onetimes()
	{
		RadarBuffer1.SetNoToaParaBuf(PlatForm);
		RadarBuffer2.SetNoToaParaBuf(PlatForm);
		while ((Receiver1.OutPdwStatus != OutPDW_READY) || (Receiver2.OutPdwStatus != OutPDW_READY))
		{
			MutiPlatPriRun(RadarBuffer1);
			MutiPlatPwRun(RadarBuffer1);
			MutiPlatRFRun(RadarBuffer1);
			RadarBuffer1.ToaSort();
			MutiPlatPriRun(RadarBuffer2);
			MutiPlatPwRun(RadarBuffer2);
			MutiPlatRFRun(RadarBuffer2);
			RadarBuffer2.ToaSort();
			ParallelPDWSort();
		}
		ModifyOutPdwArray();
	}
	
	void ShowOutBuffer(uint_t NumStart,uint_t NumStop,size_t sel = 0)
	{
		if(sel == 0)
		{
			NLogF("\n|---------------------------| Receiver1 Start |----------------------------|\n");
			Receiver1.show(NumStart,NumStop);
			NLogF("\n|---------------------------| Receiver1 End   |----------------------------|\n");
			NLogF("\n|---------------------------| Receiver2 Start |----------------------------|\n");
			Receiver2.show(NumStart,NumStop);
			NLogF("\n|---------------------------| Receiver2 End   |----------------------------|\n");
		}
		else if(sel == 1)
		{
			NLogF("\n|---------------------------| Receiver1 Start |----------------------------|\n");
			Receiver1.show(NumStart,NumStop);
			NLogF("\n|---------------------------| Receiver1 End   |----------------------------|\n");
		}
		else if(sel == 2)
		{
			NLogF("\n|---------------------------| Receiver2 Start |----------------------------|\n");
			Receiver2.show(NumStart,NumStop);
			NLogF("\n|---------------------------| Receiver2 End   |----------------------------|\n");
		}
	}
	
	uint_t*	GetPdwBufAddr1() const {return (uint_t*)(Receiver1.GetOutPdwBufAddr());}
	uint_t*	GetPdwStatusAddr1() const {return (uint_t*)(Receiver1.GetOutPdwStatusAddr());}
	uint_t*	GetSortNumAddr1() const {return (uint_t*)(Receiver1.GetSortNumAddr());}
	uint_t*	GetPdwBufAddr2() const {return (uint_t*)(Receiver2.GetOutPdwBufAddr());}
	uint_t*	GetPdwStatusAddr2() const {return (uint_t*)(Receiver2.GetOutPdwStatusAddr());}
	uint_t*	GetSortNumAddr2() const {return (uint_t*)(Receiver2.GetSortNumAddr());}
};

#endif
