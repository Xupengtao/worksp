#ifndef _IRADARSIMPRO_H_
#define _IRADARSIMPRO_H_

#include"RadarStruct.h"
#include"CircularList.hpp"

class _Radar;
class _PlatForm;
class _RadarBuffer;
class _iRadarAlgorithm;

class _iRadarAlgorithm
{
	_iRadarAlgorithm(){}
	virtual ~_iRadarAlgorithm(){}
public:
	friend _Radar;
	friend _RadarBuffer;
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
	USHORT 		PlatRadar;
	UINT 		FrameNum;
	_PriPara 	PriPara;
	_RfPara		RFPara;
	_PwPara		PwPara;
public:
	friend class _RadarBuffer;
	friend class _iRadarAlgorithm;
	typedef void (*RADAR_FUNCPTR)(_Radar & Radars,_RadarBuffer & RadarBuf);
	RADAR_FUNCPTR PriFunc;
	RADAR_FUNCPTR PwFunc;
	RADAR_FUNCPTR RFFunc;
	_Radar()
	{
		Init();
	}
	virtual  ~_Radar()
	{
		;
	}
	void 	 Init()
	{
		PlatRadar	= 0;
		Clear();
	}
	void	 Clear()
	{
		FrameNum 	= 0;
		memset(&PriPara,0,sizeof(_PriPara));
		memset(&PwPara, 0, sizeof(_PwPara));
		memset(&RFPara, 0, sizeof(_RfPara));
		PriFunc 	= _iRadarAlgorithm::PriFix;
		PwFunc 		= _iRadarAlgorithm::PwFix;
		RFFunc 		= _iRadarAlgorithm::FreFix;
	}
	void 	 SetPlatRadar(USHORT PlatRadar_){PlatRadar = PlatRadar_;}
	USHORT	 GetPlatRadar() const {return PlatRadar;}
	void 	 SetRadarPara(_RadarMode RadarMode)
	{
		SetPriPara(RadarMode.PriPara);
		SetPwPara(RadarMode.PwPara);
		SetRFPara(RadarMode.RfPara);
	}
	void	 CalculateFrameNum(UINT RadarBufSize_)
	{
		UINT FrameNumTemp = 0;
		switch(PriPara.PriType)
		{
		case PRI_FIX:
			FrameNumTemp = (PriPara.Pri.dPri != 0)?(RadarBufSize_/PriPara.Pri.dPri+100):0;
			SetFrameNum(FrameNumTemp);
			break;
		case PRI_DITHER:
			FrameNumTemp = ((PriPara.Pri.PriDither.dPri-PriPara.Pri.PriDither.dRange) != 0)?
							(RadarBufSize_/(PriPara.Pri.PriDither.dPri-PriPara.Pri.PriDither.dRange)):0;
			SetFrameNum(FrameNumTemp);
			break;
		case PRI_JAGGING:
			FrameNumTemp = (min(PriPara.Pri.PriJagging.dpRatio,PriPara.Pri.PriJagging.nPriNum) != 0)?
						   (RadarBufSize_/min(PriPara.Pri.PriJagging.dpRatio,PriPara.Pri.PriJagging.nPriNum)):0;
			SetFrameNum(FrameNumTemp);
			break;
		case PRI_SLIDER:
			FrameNumTemp = (min(PriPara.Pri.PriSlider.dpPri,PriPara.Pri.PriSlider.nPriNum) != 0)?
						   (RadarBufSize_/min(PriPara.Pri.PriSlider.dpPri,PriPara.Pri.PriSlider.nPriNum)):0;
			SetFrameNum(FrameNumTemp);
			break;
		default:
			break;
		}
	}
	void	 SetFrameNum(UINT FrameNum_)
	{
		if(FrameNum_ != 0)
		{
			FrameNum = FrameNum_;
		}
		else
		{
			ERRORMSG("Exception!");
		}
	}
	UINT	 GetFrameNum() const {return FrameNum;}
	void	 SetPriPara(_PriPara	&PriPara_)			
	{
		switch(PriPara_.PriType)
		{
		case PRI_FIX:
			PriFunc = _iRadarAlgorithm::PriFix;
			break;
		case PRI_DITHER:
			PriFunc = _iRadarAlgorithm::PriDither;
			break;
		case PRI_JAGGING:
			PriFunc = _iRadarAlgorithm::PriJagging;
			break;
		case PRI_SLIDER:
			PriFunc = _iRadarAlgorithm::PriSlider;
			break;
		default:
			PriFunc = _iRadarAlgorithm::PriFix;
			break;
		}
		PriPara = PriPara_;
	}
	void	 SetPwPara(_PwPara	&PwPara_)
	{
		switch(PwPara_.PwType)
		{
		case PW_FIX:
			PwFunc = _iRadarAlgorithm::PwFix;
			break;
		case PW_DEFINE:
			PwFunc = _iRadarAlgorithm::PwDefine;
			break;
		default:
			PwFunc = _iRadarAlgorithm::PwFix;
			break;
		}
		PwPara = PwPara_;
	}
	void	 SetRFPara(_RfPara	&RFPara_)
	{
		switch(RFPara_.RfType)
		{
		case RF_CONTINUOUSLY:
			RFFunc = _iRadarAlgorithm::FreContinuously;
			break;
		case RF_FIX:
			RFFunc = _iRadarAlgorithm::FreFix;
			break;
		case RF_AGILITY:
			RFFunc = _iRadarAlgorithm::FreAgility;
			break;
		case RF_DIVERSITY:
			break;
		default:
			RFFunc = _iRadarAlgorithm::FreFix;
			break;
		}
		RFPara = RFPara_;
	}
	template <typename T>
	T 		 min(T *pType,UINT Num)
	{
		T Temp = pType[0];
		for(UINT i=0;i<Num;i++)
		{
			if (pType[i]<Temp)
			{
				Temp = pType[i];
			}
		}
		return Temp;
	}
	void	 Show()
	{
		COUT(PlatRadar, FrameNum);
		COUT(PriPara.PriType);
		if(PriPara.PriType ==  PRI_FIX)
		{
			COUT(PriPara.Pri.dPri);
		}
		else if(PriPara.PriType ==  PRI_DITHER)
		{
			COUT(PriPara.Pri.PriDither.dPri);
			COUT(PriPara.Pri.PriDither.dRange);
			COUT(PriPara.Pri.PriDither.nGroupNum);
		}
		else if(PriPara.PriType ==  PRI_JAGGING)
		{
			COUT(PriPara.Pri.PriJagging.nPriNum);
			COUT(PriPara.Pri.PriJagging.nGroupNum);
			for(UINT i = 0;i<PriPara.Pri.PriJagging.nPriNum;i++)
			{
				COUTS("PriPara.Pri.PriJagging.dpRatio[", i, "] = ",PriPara.Pri.PriJagging.dpRatio[i]);
			}
		}
		else if(PriPara.PriType ==  PRI_SLIDER)
		{
			COUT(PriPara.Pri.PriSlider.nPriNum);
			COUT(PriPara.Pri.PriSlider.dPriStep);
			COUT(PriPara.Pri.PriSlider.nGroupNum);
			for(UINT i = 0;i<PriPara.Pri.PriSlider.nPriNum;i++)
			{
				COUTS("PriPara.Pri.PriSlider.dpPri[", i, "] = ", PriPara.Pri.PriSlider.dpPri[i]);
			}
		}
		COUT(PriPara.ToaMod);
		COUT(PriPara.ToaModRand);
		COUT(PriPara.NumTemp);
		COUT(PriPara.GroupNumTemp);
		COUT(RFPara.RfType);
		if(RFPara.RfType ==  RF_CONTINUOUSLY)
		{
			COUT(RFPara.Rf.dFre);
		}
		else if(RFPara.RfType ==  RF_FIX)
		{
			COUT(RFPara.Rf.dFre);
		}
		else if(RFPara.RfType ==  RF_AGILITY)
		{
			COUT(RFPara.Rf.FreAgility.nFreNum);
			COUT(RFPara.Rf.FreAgility.nGroupNum);
			for(UINT i=0;i<RFPara.Rf.FreAgility.nFreNum;i++)
			{
				COUTS("RFPara.Rf.FreAgility.dpFre[", i, "] = ", RFPara.Rf.FreAgility.dpFre[i]);
			}
		}
		else if(RFPara.RfType ==  RF_DIVERSITY)
		{
			COUT(RFPara.Rf.FreDiversity.nFreNum);
			for(UINT i=0;i<RFPara.Rf.FreDiversity.nFreNum;i++)
			{
				COUTS("RFPara.Rf.FreDiversity.dpFre[", i, "] = ", RFPara.Rf.FreDiversity.dpFre[i]);
			}
			for(UINT i=0;i<RFPara.Rf.FreDiversity.nFreNum;i++)
			{
				COUTS("RFPara.Rf.FreDiversity.dInterval[", i, "] = ", RFPara.Rf.FreDiversity.dInterval[i]);
			}
		}
		COUT(RFPara.NumTemp);
		COUT(RFPara.PriSumTemp);
		COUT(RFPara.GroupNumTemp);
		COUT(PwPara.PwType);
		if(PwPara.PwType ==  PW_FIX)
		{
			COUT(PwPara.Pw.dPw);
		}
		else if(PwPara.PwType ==  PW_DEFINE)
		{
			COUT(PwPara.Pw.PwDefine.nPwNum);
			COUT(PwPara.Pw.PwDefine.nGroupNum);
			for(UINT i=0;i<PwPara.Pw.PwDefine.nPwNum;i++)
			{
				COUTS("PwPara.Pw.PwDefine.dpPw[", i, "] = ", PwPara.Pw.PwDefine.dpPw[i]);
			}
		}
		COUT(PwPara.NumTemp);
		COUT(PwPara.GroupNumTemp);
	}
};

class _PlatForm
{
private:
	USHORT 	 	PlatNo;
	bool 	 	PlatFormEn;
	USHORT 	 	RadarModeList[RADAR_NUM];			// RADAR_NUM 单平台最大雷达数
	UINT		RadarStTimes[RADAR_NUM];			// 雷达开机时间
	_Radar*  	Radars;
	_AzElDis 	AzElDis;
public:
	_PlatForm()
	{	
		PlatNo = 0;
		Radars = new _Radar[RADAR_NUM];
		Init();
	}
	virtual ~_PlatForm()
	{
		delete[] Radars;
	}
	void    Init()
	{
		PlatFormEn = false;
		for (UINT i = 0; i < RADAR_NUM; i++)
		{
			RadarModeList[i] = RadarMode_Null;
			RadarStTimes[i]	 = 0;
			Radars[i].Init();
		}
	}
	void 	EnablePlat(){PlatFormEn= true;}
	void 	DisenablePlat(){PlatFormEn= false;}
	bool	GetPlatFormEn() const{return PlatFormEn;}
	UCHAR 	SetPlatNo(const UCHAR PlatNo_)
	{
		PlatNo = PlatNo_;
		for (UINT i = 0; i < RADAR_NUM; i++)
		{
			Radars[i].SetPlatRadar(PlatNo * RADAR_NUM + i);
		}
	}
	UCHAR 	GetPlatNo() const {return PlatNo;}
	void 	SetRadarEn(UCHAR RadarNo_){RadarModeList[RadarNo_] = RadarMode_En;}
	void 	SetRadarDisEn(UCHAR RadarNo_){RadarModeList[RadarNo_] = RadarMode_Null;}
	USHORT 	GetRadarMode(UCHAR RadarNo_) const {return RadarModeList[RadarNo_];}
	void	SetRadarStTime(UCHAR RadarNo_, UINT StTime){RadarStTimes[RadarNo_] = StTime;}
	UINT	GetRadarStTime(UCHAR RadarNo_) const {return RadarStTimes[RadarNo_];}
	void 	SetRadar(_RadarMode &RadarMode, UCHAR RadarSn)
	{
		EnablePlat();
		if(RadarSn < RADAR_NUM)
		{
			SetRadarEn(RadarSn);
			GetRadar(RadarSn).Clear();
			Radars[RadarSn].SetRadarPara(RadarMode);
		}
		else
		{
			ERRORMSG("Out of range!");
		}
	}
	_Radar& GetRadar(UCHAR RadarNo_){return Radars[RadarNo_];}
	void	SetAzElDis(_LocVec &SetVec_)
	{
		AzElDis.Set(SetVec_);
	}
	UINT	GetAz(UINT Loc){return AzElDis.GetAz(Loc);}
	UINT	GetEl(UINT Loc){return AzElDis.GetEl(Loc);}
	UINT	GetDistance(UINT Loc){return AzElDis.GetDistance(Loc);}
	UINT 	GetJitter(UINT Loc){return AzElDis.GetJitter(Loc);}
	void	Clear()
	{
		PlatFormEn = false;
		for(UINT i=0;i<RADAR_NUM;i++)
		{
			if(RadarModeList[i] != RadarMode_Null)
			{
				RadarModeList[i] = RadarMode_Null;
				RadarStTimes[i]	 = 0;
				Radars[i].Clear();
			}
		}
	}
	void	Show()
	{
		COUT(PlatNo);
		COUT(PlatFormEn);
		for(UINT i = 0; i < RADAR_NUM; i++)
		{
			if(GetRadarMode(i) != 0)
			{
				COUTS("|*********************** Radar[", i ,"] Start ***********************|");
				COUTS("RadarModeList[", i, "] = ", GetRadarMode(i));
				GetRadar(i).Show();
				COUTS("|*********************** Radar[", i ,"] End   ***********************|");
			}
		}
	}
};

class _RadarBuffer
{
private:
	static UINT 	GlobalToaH;
	static UINT 	GlobalToaL;
	static UINT 	RadarRCCLNum;
	static UINT		SortTimes;
	UINT 			RadarBufSize;
	UINT 			SignSize;
	UINT 			InPDWSize;
	UINT			RadarBufferSortTime;
public:
	friend class 	_PlatForm;
	friend class 	_Radar;
	friend class 	_iRadarAlgorithm;
	UINT 			SortNum;
	_PlatRadarNum 	*PRNBuffer;
	_PRNSign 	 	*ToaBufferSign;
	CircularList<_RadarPara> 	*RadarPara;
	_InPDW 			*InPDW;
	UINT			ParaPRFrames[(PLAT_NUM)*(RADAR_NUM)];
	UINT			PriStatus;
	UINT			PwStatus;
	UINT			RFStatus;
	UINT			SortReadyStatus;
	UINT 			InPDWStatus;
	_RadarBuffer(UINT InPDWSize_ = 1000000)
				:RadarBufSize(RADAR_BUFSIZE),SignSize(SIGN_SIZE),InPDWSize(InPDWSize_)
	{
		PriStatus 		= IDLE;
		PwStatus 		= IDLE;
		RFStatus 		= IDLE;
		SortReadyStatus = IDLE;
		InPDWStatus 	= IDLE;
		SortNum = 0;
		RadarBufferSortTime = 0;
		PRNBuffer = new _PlatRadarNum[RadarBufSize];
		memset(PRNBuffer, 0, RadarBufSize*sizeof(_PlatRadarNum));
		for(UINT i=0;i<RadarBufSize;i++)
		{
			PRNBuffer[i].PlatRadar = PRNBUFFER_SIGN;
		}
		ToaBufferSign = new _PRNSign[RadarBufSize>>SignSize];
		memset(ToaBufferSign, 0, (RadarBufSize>>SignSize)*sizeof(_PRNSign));
		InPDW = new _InPDW[InPDWSize];
		memset(InPDW,0,InPDWSize*sizeof(_InPDW));
		memset(ParaPRFrames,0,(PLAT_NUM)*(RADAR_NUM)*sizeof(UINT));
		RadarPara = new CircularList<_RadarPara>[PLAT_NUM*RADAR_NUM];
	}
	~_RadarBuffer()
	{
		delete[] PRNBuffer;
		delete[] ToaBufferSign;
		delete[] InPDW;
		delete[] RadarPara;
	}
	void ShellSort(UINT *pToa, UINT Num)
	{
		UINT i, j, Inc;
		UINT Tmp;
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
	void ToaSort(_PlatForm *PlatForm)
	{
		UINT DoaArr[PLAT_NUM] 	 = {0};
		UINT JitterArr[PLAT_NUM] = {0};
		for(UINT i = 0; i < PLAT_NUM; i++)
		{
			DoaArr[i] 	 = PlatForm[i].GetAz(SortTimes);
			JitterArr[i] = PlatForm[i].GetJitter(SortTimes);
		}
		if ((SortReadyStatus == READY) && (InPDWStatus == IDLE))
		{
			InPDWStatus = BUSY;
			ParaPRReadReset();
			RadarBufferSortTime = SortTimes;
			SortTimes++;
			UINT i, j, SortNum_ = 0, ToaLocal_, ToaLocalStep_, ToaNum_, SortNumTemp_, GlobalToaHTemp_ = this->GlobalToaH, GlobalToaLTemp_ = this->GlobalToaL;
			USHORT	PlatRadarTemp_ 	= 0;
			USHORT	PlatSn 			= 0;
			UINT	RadarsToaNum_  	= 0;
			UINT 	RandomJitter	= (UINT)time(NULL);
			UINT	JitterTmp	 	= 0;
			CUINT 	BufSignSize_ 	= RadarBufSize >> SignSize;
			CUINT 	StepMax_ 		= 1 << SignSize;
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
							if(PlatRadarTemp_ != PRNBUFFER_SIGN)
							{
								PlatSn	= PlatRadarTemp_/RADAR_NUM;
								RadarsToaNum_ = PRNBuffer[SortNumTemp_].ToaNum;
								PRNBuffer[SortNumTemp_].PlatRadar = PRNBUFFER_SIGN;
								InPDW[SortNum_].PlatRadar = PlatRadarTemp_;
								InPDW[SortNum_].RadarPara = RadarPara[PlatRadarTemp_][RadarsToaNum_];
								if(JitterArr[PlatSn] == 0)
								{
									InPDW[SortNum_].RadarPara.Az = DoaArr[PlatSn];
								}
								else
								{
									RandomJitter = UINT(RandomJitter*69069+12345);
									JitterTmp 	 = RandomJitter%JitterArr[PlatSn];
									if(JitterTmp > JitterArr[PlatSn]/2)
									{
										JitterTmp = JitterTmp - JitterArr[PlatSn]/2;
										InPDW[SortNum_].RadarPara.Az = (DoaArr[PlatSn] + JitterTmp > 1023) ? 1023 : DoaArr[PlatSn] + JitterTmp;
									}
									else
									{
										JitterTmp = JitterArr[PlatSn]/2 - JitterTmp;
										InPDW[SortNum_].RadarPara.Az = (DoaArr[PlatSn] < JitterTmp) ? 0 : DoaArr[PlatSn] - JitterTmp;
									}
								}
								InPDW[SortNum_].ToaH = GlobalToaHTemp_;
								InPDW[SortNum_].ToaL = GlobalToaLTemp_ | SortNumTemp_;
								ParaPRFrames[PlatRadarTemp_] = RadarsToaNum_;
								SortNum_++;
							}
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
								PlatSn	= PlatRadarTemp_/RADAR_NUM;
								RadarsToaNum_ = PRNBuffer[ToaLocal_].ToaNum;
								PRNBuffer[ToaLocal_].PlatRadar = PRNBUFFER_SIGN;
								InPDW[SortNum_].PlatRadar = PlatRadarTemp_;
								InPDW[SortNum_].RadarPara = RadarPara[PlatRadarTemp_][RadarsToaNum_];
								if(JitterArr[PlatSn] == 0)
								{
									InPDW[SortNum_].RadarPara.Az = DoaArr[PlatSn];
								}
								else
								{
									RandomJitter = UINT(RandomJitter*69069+12345);
									JitterTmp 	 = RandomJitter%JitterArr[PlatSn];
									if(JitterTmp > JitterArr[PlatSn]/2)
									{
										JitterTmp = JitterTmp - JitterArr[PlatSn]/2;
										InPDW[SortNum_].RadarPara.Az = (DoaArr[PlatSn] + JitterTmp > 1023) ? 1023 : DoaArr[PlatSn] + JitterTmp;
									}
									else
									{
										JitterTmp = JitterArr[PlatSn]/2 - JitterTmp;
										InPDW[SortNum_].RadarPara.Az = (DoaArr[PlatSn] < JitterTmp) ? 0 : DoaArr[PlatSn] - JitterTmp;
									}
								}
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
			SortReadyStatus = IDLE;
			InPDWStatus = READY;
		}
	}
	static void ResetGlobalToa()
	{
		GlobalToaH = 0;
		GlobalToaL = 0;
	}
    static void ClearSortTimes()
    {
        SortTimes = 0;
    }
	static UINT GetRCCLNum(UINT Num)
	{
		UINT i;
		for(i = 0; Num > 1; i++)
		{
			Num = Num>>1;
		}
		return i;
	}
	static void GlobalToaAdd(UINT RadarBufSize)
	{
		RadarRCCLNum = GetRCCLNum(RadarBufSize);
		UINT ToaLMax;
		ToaLMax = 0xFFFFFFFF>>RadarRCCLNum;
		UINT GlobalToaLRccl = GlobalToaL>>RadarRCCLNum;
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
	static UINT GetGlobalToaH(){return GlobalToaH;}
	static UINT GetGlobalToaL(){return GlobalToaL;}
	UINT 	GetStatus(){return InPDWStatus;}
	void 	SetStatus(const UINT InPDWStatus_){InPDWStatus = InPDWStatus_;}
	UINT	GetSortTimes() const {return SortTimes;}
	UINT  	GetRadarBufSize() const {return RadarBufSize;}
	UINT  	GetSignSize() const {return SignSize;}
	UINT 	GetSortNum(){return SortNum;}
	UINT	GetRadarBufferSortTimes() const {return RadarBufferSortTime;}
	void	NoToaParaBufferLink(_RadarBuffer &RadarBuf)
	{
		for(UINT i=0;i<(PLAT_NUM)*(RADAR_NUM);i++)
		{
			RadarPara[i].ChainLink(RadarBuf.RadarPara[i]);
		}
	}
	void	SetNoToaParaBuf(_PlatForm *PlatForm)
	{
		for (UINT i = 0; i<PLAT_NUM; i++)
		{
			if (PlatForm[i].GetPlatFormEn() == true)
			{
				for (UINT j = 0; j<RADAR_NUM; j++)
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
		for(UINT i=0;i<(PLAT_NUM)*(RADAR_NUM);i++)
		{
			RadarPara[i].ReadReset();
		}
	}
	void	ParaPRWriteReset()
	{
		for(UINT i=0;i<(PLAT_NUM)*(RADAR_NUM);i++)
		{
			if(ParaPRFrames[i] != 0)
			{
				RadarPara[i].WriteReset(ParaPRFrames[i]);
				ParaPRFrames[i] = 0;
			}
		}
	}
	void	Clear()
	{
		GlobalToaH 		= 0;
		GlobalToaL 		= 0;
		RadarRCCLNum 	= 0;
		SortNum 		= 0;
		SortTimes 		= 0;
		RadarBufferSortTime = 0;
		PriStatus 		= IDLE;
		PwStatus 		= IDLE;
		RFStatus 		= IDLE;
		SortReadyStatus = IDLE;
		InPDWStatus 	= IDLE;
		for(UINT i=0;i<RadarBufSize;i++)
		{
			PRNBuffer[i].PlatRadar = PRNBUFFER_SIGN;
		}
		memset(ToaBufferSign,0,(RadarBufSize>>SignSize)*sizeof(_PRNSign));
		memset(ParaPRFrames,0,(PLAT_NUM)*(RADAR_NUM)*sizeof(UINT));
		memset(InPDW,0,InPDWSize*sizeof(_InPDW));
	}
	void	showStatus()
	{
		COUT(GlobalToaH);
		COUT(GlobalToaL);
		COUT(SortTimes);
		COUT(RadarBufSize);
		COUT(SignSize);
		COUT(SortNum);
		COUT(RadarBufferSortTime);
		COUT_STATUS(PriStatus);
		COUT_STATUS(PwStatus);
		COUT_STATUS(RFStatus);
		COUT_STATUS(SortReadyStatus);
		COUT_STATUS(InPDWStatus);
	}
	void 	show(UINT Num)
	{
		COUT(Num);
		COUT(InPDW[Num].PlatRadar);
		COUT(InPDW[Num].RadarPara.Flag);
		COUT(InPDW[Num].RadarPara.Pw);
		COUT(InPDW[Num].RadarPara.Rf);
		COUT(InPDW[Num].RadarPara.Az);
		COUT(InPDW[Num].RadarPara.El);
		COUT(InPDW[Num].RadarPara.Pa);
		COUTBYHEX(InPDW[Num].ToaH);
		COUTBYHEX(InPDW[Num].ToaL);
	}
	void 	show(UINT NumStart,UINT NumStop)
	{
		if(NumStart<NumStop)
		{
			for(UINT i=NumStart;i<=NumStop;i++)
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
	USHORT 		EsmType;
	UINT 		OutPdwBufferSize;
	UINT 		SortNum;
	UINT		SortTimes;
	_OutPdwType	*OutPdw;
	USHORT		*OutPdwSign;
public:
	UINT		OutPdwStatus;
	_Receiver(float Rcs_=0, short Sensitivity_=10, short DynamicEquil_=500, UINT OutPdwBufferSize_=1000000, USHORT EsmType_ = 1)
			  :OutPdwBufferSize(OutPdwBufferSize_)
	{
		Init(Rcs_, Sensitivity_, DynamicEquil_, EsmType_);
		OutPdw = new _OutPdwType[OutPdwBufferSize];
		OutPdwSign = new USHORT[OutPdwBufferSize];
		memset(OutPdw, 0, sizeof(_OutPdwType) * OutPdwBufferSize);
		memset(OutPdwSign, 0, sizeof(USHORT) * OutPdwBufferSize);
		SortNum = 0;
		SortTimes = 0;
		OutPdwStatus = IDLE;
	}
	virtual ~_Receiver()
	{
		delete[] OutPdw;
		delete[] OutPdwSign;
	}
	void 	Init(float Rcs_ = 0, short Sensitivity_ = 10, short DynamicEquil_ = 500, USHORT EsmType_ = 1)
	{
		Rcs 		 = Rcs_;
		Sensitivity  = Sensitivity_;
		DynamicEquil = DynamicEquil_;
		EsmType 	 = EsmType_;
	}
	void 	SetRcs(float Rcs_){Rcs = Rcs_;}
	float	GetRcs(){return Rcs;}
	void 	SetSensitivity(short Sensitivity_){Sensitivity = Sensitivity_;}
	short	GetSensitivity(){return Sensitivity;}
	void 	SetDynamicEquil(short DynamicEquil_){DynamicEquil = DynamicEquil_;}
	short	GetDynamicEquil(){return DynamicEquil;}
	void 	SetEsmType(USHORT EsmType_){EsmType = EsmType_;}
	short	GetEsmType(){return EsmType;}
	_OutPdwType* GetOutPdwAddr() 	{return OutPdw;}
	USHORT* GetOutPdwSignAddr()  	{return OutPdwSign;}
	UINT*	GetOutPdwStatusAddr()   {return &OutPdwStatus;}
	UINT*	GetSortNumAddr() 		{return &SortNum;}
	UINT	GetOutPdwBufSize() const {return OutPdwBufferSize;}
	void	ClearSortNum(){SortNum = 0;}
	UINT	GetSortNum() const {return SortNum;}
	void	ClearSortTime(){SortTimes = 0;}
	UINT	GetSortTime() const {return SortTimes;}
	UINT 	PDWSort(_RadarBuffer & RadarBuf)
	{
		if ((RadarBuf.InPDWStatus == READY) && (OutPdwStatus == IDLE))
		{
			OutPdwStatus = BUSY;
			SortTimes++;

			UINT SortNum_ = SortNum;
			UINT RadarBufSortNum_ = RadarBuf.SortNum;
			UINT PwTemp_ = 0;

			RadarBuf.SortNum = 0;
			if (SortNum_ + 2 * RadarBufSortNum_ < OutPdwBufferSize)
			{
				for (UINT i = 0; i <= RadarBufSortNum_;)
				{
					_InPDW &InPDWTemp = RadarBuf.InPDW[i];
					if ((InPDWTemp.ToaL>PwTemp_) && (InPDWTemp.RadarPara.Pa>Sensitivity) && (InPDWTemp.RadarPara.Pa<Sensitivity + DynamicEquil))
					{
//						OutPdw[SortNum_].Flag = InPDWTemp.RadarPara.Flag;
//						OutPdw[SortNum_].ToaL = InPDWTemp.ToaL;
//						OutPdw[SortNum_].ToaH = InPDWTemp.ToaH;
//						OutPdw[SortNum_].Az = InPDWTemp.RadarPara.Az;
//						OutPdw[SortNum_].El = InPDWTemp.RadarPara.El;
						OutPdw[SortNum_].Toa 	= InPDWTemp.ToaL;
						OutPdw[SortNum_].ToaS 	= InPDWTemp.ToaH;
						OutPdw[SortNum_].Pa 	= InPDWTemp.RadarPara.Pa;
						OutPdw[SortNum_].Pw 	= InPDWTemp.RadarPara.Pw;
						OutPdw[SortNum_].Rf 	= InPDWTemp.RadarPara.Rf;
						OutPdw[SortNum_].Doa 	= InPDWTemp.RadarPara.Az;
						OutPdwSign[SortNum_]	= InPDWTemp.PlatRadar;
						SortNum_++;
						PwTemp_ = InPDWTemp.ToaL + InPDWTemp.RadarPara.Pw;
					}
					i++;
				}
			}
			else
			{
				for (UINT i = 0; i <= RadarBufSortNum_;)
				{
					_InPDW &InPDWTemp = RadarBuf.InPDW[i];
					if ((InPDWTemp.ToaL > PwTemp_) && (InPDWTemp.RadarPara.Pa>Sensitivity) && (InPDWTemp.RadarPara.Pa<Sensitivity + DynamicEquil))
					{
						if (SortNum_ < OutPdwBufferSize)
						{
//							OutPdw[SortNum_].Flag = InPDWTemp.RadarPara.Flag;
//							OutPdw[SortNum_].ToaL = InPDWTemp.ToaL;
//							OutPdw[SortNum_].ToaH = InPDWTemp.ToaH;
//							OutPdw[SortNum_].Az = InPDWTemp.RadarPara.Az;
//							OutPdw[SortNum_].El = InPDWTemp.RadarPara.El;
							OutPdw[SortNum_].Toa 	= InPDWTemp.ToaL;
							OutPdw[SortNum_].ToaS 	= InPDWTemp.ToaH;
							OutPdw[SortNum_].Pa 	= InPDWTemp.RadarPara.Pa;
							OutPdw[SortNum_].Pw 	= InPDWTemp.RadarPara.Pw;
							OutPdw[SortNum_].Rf 	= InPDWTemp.RadarPara.Rf;
							OutPdw[SortNum_].Doa 	= InPDWTemp.RadarPara.Az;
							OutPdwSign[SortNum_]	= InPDWTemp.PlatRadar;
							SortNum_++;
							PwTemp_ = InPDWTemp.ToaL + InPDWTemp.RadarPara.Pw;
						}
						else
						{
							SortNum = SortNum_ - 1;
							RadarBuf.InPDWStatus = IDLE;
							OutPdwStatus = READY;
							return OutPdwStatus;
						}
					}
					i++;
				}
				SortNum = SortNum_ - 1;
				RadarBuf.InPDWStatus = IDLE;
				OutPdwStatus = READY;
				return OutPdwStatus;
			}
			SortNum = SortNum_;
			if ((SortTimes >= RECIVERSORTTIMES_MAX) && (SortNum >= RECIVERSORTNUM_MAX))
			{
				SortNum = SortNum - 1;
				RadarBuf.InPDWStatus = IDLE;
				OutPdwStatus = READY;
			}
			else
			{
				RadarBuf.InPDWStatus = IDLE;
				OutPdwStatus = IDLE;
			}
		}
		return OutPdwStatus;
	}
	void 	showStatus()
	{
		COUT(Sensitivity);
		COUT(DynamicEquil);
		COUT(OutPdwBufferSize);
		COUT(GetOutPdwAddr());
		COUT(SortNum);
		COUT(SortTimes);
		COUT_STATUS(OutPdwStatus);
	}
	void 	show(UINT Num) const
	{
		COUTSWIDTH(6, "Pdw No.", Num,
					  ": PlatRadar = ", OutPdwSign[Num],
					  ", RF = ", 	OutPdw[Num].Rf,
					  ", PW = ", 	OutPdw[Num].Pw,
					  ", PA = ", 	OutPdw[Num].Pa,
					  ", TOA = ", 	OutPdw[Num].Toa,
					  ", ToaS = ", 	OutPdw[Num].ToaS,
					  ", DOA = ", 	OutPdw[Num].Doa
				   );
	}
	void 	show(UINT NumStart, UINT NumStop, USHORT PlatRadarSn = PLAT_NUM*RADAR_NUM)
	{
		if(NumStart<NumStop)
		{
			for(UINT i = NumStart; i < NumStop; i++)
			{
				if(PlatRadarSn >= PLAT_NUM*RADAR_NUM)
				{
					show(i);
				}
				else
				{
					if(OutPdwSign[i] == PlatRadarSn)
					{
						show(i);
					}
				}
			}
		}
	}
	void	BufferClear()
	{
		SortNum = 0;
		SortTimes = 0;
		memset(OutPdw, 0, sizeof(_OutPdwType) * OutPdwBufferSize);
		memset(OutPdwSign, 0, sizeof(USHORT) * OutPdwBufferSize);
		OutPdwStatus = IDLE;
	}
	void	StatusClear()
	{
		Rcs = 0;
		Sensitivity = 0;
		DynamicEquil = 500;
	}
};

void _iRadarAlgorithm::PriFix(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t RadarBufSize = RadarBuf.RadarBufSize;		//Buffer长度 0x1000000
	size_t BufSignSize = RadarBuf.SignSize;				//Buffer标记右移长度 8
	size_t ToaBufferSignLoc,Pri;						//Sign位置       Pri
	size_t ToaMod;										//ToaMod
	USHORT PlatRadar = Radars.PlatRadar;				//平台雷达号
	size_t ToaNum = 0;									//Toa生成编号
	Pri = Radars.PriPara.Pri.dPri;
	ToaMod = Radars.PriPara.ToaMod;
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
	USHORT PlatRadar = Radars.PlatRadar;									//平台雷达号
	size_t ToaNum = 0;														//Toa生成编号
	GroupNum = Radars.PriPara.Pri.PriDither.nGroupNum;
	GroupNumTemp = Radars.PriPara.GroupNumTemp;
	Pri = Radars.PriPara.Pri.PriDither.dPri;
	DitherRange = Radars.PriPara.Pri.PriDither.dRange;
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
}
void _iRadarAlgorithm::PriJagging(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t RadarBufSize = RadarBuf.RadarBufSize;						//Buffer长度 0x1000000
	size_t BufSignSize = RadarBuf.SignSize;								//Buffer标记右移长度 8
	size_t ToaBufferSignLoc,GroupNum,GroupNumTemp,PriNum,NumTemp,Pri;	//Sign位置       Pri  GroupNum
	size_t ToaMod;														//ToaMod
	UINT *pRatio;
	USHORT PlatRadar = Radars.PlatRadar;								//平台雷达号
	size_t ToaNum = 0;													//Toa生成编号
	GroupNum = Radars.PriPara.Pri.PriJagging.nGroupNum;
	PriNum = Radars.PriPara.Pri.PriJagging.nPriNum;
	ToaMod = Radars.PriPara.ToaMod;
	NumTemp = Radars.PriPara.NumTemp;
	GroupNumTemp = Radars.PriPara.GroupNumTemp;
	pRatio = Radars.PriPara.Pri.PriJagging.dpRatio;
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
	size_t BufSignSize = RadarBuf.SignSize;						//Buffer标记右移长度 8
	size_t ToaBufferSignLoc,GroupNum,GroupNumTemp,Pri;			//Sign位置       Pri GroupNum
	int PriNum,NumTemp,PriStep;
	size_t ToaMod;												//ToaMod
	UINT *pPri;
	USHORT PlatRadar = Radars.PlatRadar;						//平台雷达号
	size_t ToaNum = 0;											//Toa生成编号
	GroupNum = Radars.PriPara.Pri.PriSlider.nGroupNum;
	PriNum = Radars.PriPara.Pri.PriSlider.nPriNum;
	PriStep = Radars.PriPara.Pri.PriSlider.dPriStep;
	ToaMod = Radars.PriPara.ToaMod;
	NumTemp = Radars.PriPara.NumTemp;
	pPri = Radars.PriPara.Pri.PriSlider.dpPri;
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
	size_t Pw = Radars.PwPara.Pw.dPw;
	USHORT PlatRadar = Radars.PlatRadar;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Pw = Pw;
	}
}
void _iRadarAlgorithm::PwDefine(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	UINT *pPw,Pw;
	pPw = Radars.PwPara.Pw.PwDefine.dpPw;
	USHORT PlatRadar = Radars.PlatRadar;
    UCHAR	PwNum = Radars.PwPara.Pw.PwDefine.nPwNum;
    UCHAR NumTemp = Radars.PwPara.NumTemp;
    USHORT GroupNumTemp = Radars.PwPara.GroupNumTemp,GroupNum = Radars.PwPara.Pw.PwDefine.nGroupNum;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
	Pw = pPw[NumTemp];
	NumTemp++;
	if(NumTemp >= PwNum)
	{
		NumTemp = 0;
	}
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Pw = Pw;
		GroupNumTemp++;
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
	size_t RF = Radars.RFPara.Rf.dFre;
	USHORT PlatRadar = Radars.PlatRadar;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Flag = 1;
		RadarBuf.RadarPara[PlatRadar][i].Rf = RF;
	}
}
void _iRadarAlgorithm::FreFix(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	size_t RF = Radars.RFPara.Rf.dFre;
	USHORT PlatRadar = Radars.PlatRadar;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Flag = 0;
		RadarBuf.RadarPara[PlatRadar][i].Rf = RF;
	}
}
void _iRadarAlgorithm::FreAgility(_Radar & Radars,_RadarBuffer & RadarBuf)
{
	UINT *pFre,RF;
	pFre = Radars.RFPara.Rf.FreAgility.dpFre;
	USHORT PlatRadar = Radars.PlatRadar;
    USHORT RFNum = Radars.RFPara.Rf.FreAgility.nFreNum;
    UCHAR NumTemp = Radars.RFPara.NumTemp;
    USHORT GroupNumTemp = Radars.RFPara.GroupNumTemp,GroupNum = Radars.RFPara.Rf.FreAgility.nGroupNum;
	size_t FrameNum = RadarBuf.RadarPara[PlatRadar].GetBufferCapacity();
	RF = pFre[NumTemp];
	NumTemp++;
	if(NumTemp >= RFNum)
	{
		NumTemp = 0;
	}
	for(size_t i=0;i<FrameNum;i++)
	{
		RadarBuf.RadarPara[PlatRadar][i].Flag = 0;
		RadarBuf.RadarPara[PlatRadar][i].Rf = RF;
		GroupNumTemp++;
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
	_PlatForm				*PlatForm;
	_RadarBuffer			RadarBuffer1, RadarBuffer2;
	_Receiver<_OutPdwType>	Receiver1, Receiver2;
	_RadarMode				RadarMode;
public:
	_iRadarSimPro()
	{
		PlatForm = new _PlatForm[PLAT_NUM];
		for(UINT i = 0; i < PLAT_NUM; i++)
		{
			PlatForm[i].SetPlatNo(i);
		}
		RadarBuffer1.NoToaParaBufferLink(RadarBuffer2);
		ReceiverInit();
		PlatFormInit();
		PriTypeModify(PRI_FIX,1000);
		PwTypeModify(PW_FIX,50);
		RfTypeModify(RF_FIX,8000);
	}
	~_iRadarSimPro()
	{
		delete[] PlatForm;
	}
	_OutPdwType* GetPdwBufAddr1()	 {return Receiver1.GetOutPdwAddr();}
	_OutPdwType* GetPdwBufAddr2()	 {return Receiver2.GetOutPdwAddr();}
	UINT*		 GetPdwStatusAddr1() {return Receiver1.GetOutPdwStatusAddr();}
	UINT*		 GetPdwStatusAddr2() {return Receiver2.GetOutPdwStatusAddr();}
	UINT*		 GetSortNumAddr1() 	 {return Receiver1.GetSortNumAddr();}
	UINT*		 GetSortNumAddr2() 	 {return Receiver2.GetSortNumAddr();}
	bool ReceiverInit(float Rcs_ = 100, short Sensitivity_ = -10, short DynamicEquil_ = 500, USHORT EsmType_ = BroadBank)
	{
		RadarBuffer1.Clear();
		RadarBuffer2.Clear();
		Receiver1.BufferClear();
		Receiver2.BufferClear();
		Receiver1.Init(Rcs_, Sensitivity_, DynamicEquil_, EsmType_);
		Receiver2.Init(Rcs_, Sensitivity_, DynamicEquil_, EsmType_);
		return true;
	}
	void Init(size_t PlatNum = PLAT_NUM)
	{
		ReceiverInit();
		PlatFormInit(PlatNum);
	}
	bool PlatFormInit(size_t PlatNum = PLAT_NUM)						//按个数初始化平台
	{
		PlatFormClear(PlatNum);
		PlatNum = (PlatNum > PLAT_NUM) ? PLAT_NUM : PlatNum;
		for(UINT i = 0; i < PlatNum; i++)
		{
			PlatForm[i].EnablePlat();
		}
		return true;
	}
	bool PlatFormClear(size_t PlatNum = PLAT_NUM)
	{
		PlatNum = (PlatNum > PLAT_NUM) ? PLAT_NUM : PlatNum;
		for(size_t i = 0; i < PlatNum; i++)
		{
			PlatForm[i].Clear();
		}
		return true;
	}
	_PlatForm& GetPlatForm(size_t PlatSn)
	{
		if(PlatSn < PLAT_NUM)
		{
			return PlatForm[PlatSn];
		}
		else
		{
			ERRORMSG("GetPlatForm index out of range!");
			return PlatForm[0];
		}
	}
	bool PriTypeModify(_PriType PriType_ 	= PRI_FIX,
					   size_t dPri 			= 10000,
					   size_t dRange 		= 5000,		//重频范围  重频范围不大于中心重复周期的100%
					   size_t nPriNum 		= 10,		//参差或滑动数
					   size_t nGroupNum 	= 1,		//脉组脉冲数  1-1000  1－脉间 
					   size_t *PriArr 		= NULL,		//自定义重频数组
					   size_t PriArrLen 	= 0,		//自定义重频数组长度
					   size_t dPriStep		= 1			//滑动步进 有正有负
					   )
	{
		RadarMode.PriPara.Modify(PriType_, dPri, dRange, nPriNum, nGroupNum, PriArr, PriArrLen, dPriStep);
		return true;
	}
	bool RfTypeModify(_RfType RfType_ 	= RF_FIX,
					  size_t dFre		= 256000,
					  size_t nFreNum	= 10,			//点数 2-DIVNUM
					  size_t nGroupNum 	= 1,			//脉组脉冲数  1-1000  1－脉间 
					  size_t *RfArr 	= NULL,			//自定义载频数组
					  size_t RfArrLen 	= 0				//自定义载频数组长度
					  )
	{
		RadarMode.RfPara.Modify(RfType_, dFre, nFreNum, nGroupNum, RfArr, RfArrLen);
		return true;
	}
	bool PwTypeModify(_PwType PwType_ 	= PW_FIX,
					  size_t dPw		= 50,
					  size_t nPwNum		= 10,			//脉宽数量
					  size_t nGroupNum	= 1,			//脉组脉冲数  1-1000
					  size_t *PwArr 	= NULL,			//自定义脉宽数组
					  size_t PwArrLen 	= 0				//自定义脉宽数组长度
					  )
	{
		RadarMode.PwPara.Modify(PwType_, dPw, nPwNum, nGroupNum, PwArr, PwArrLen);
		return true;
	}
	bool RadarModify(size_t RadarSn)
	{
		size_t PlatNo 	= RadarSn/RADAR_NUM;
		size_t RadarNo 	= RadarSn%RADAR_NUM;
		GetPlatForm(PlatNo).SetRadar(RadarMode, RadarNo);
		return true;
	}
	int RadarModify(size_t *RadarSnArr, size_t RadarSnArrLen)
	{
		size_t rtn = 0;
		for(size_t i = 0; i < RadarSnArrLen; i++)
		{
			size_t PlatNo  = RadarSnArr[i]/RADAR_NUM;
			size_t RadarNo = RadarSnArr[i]%RADAR_NUM;
			GetPlatForm(PlatNo).SetRadar(RadarMode, RadarNo);
			rtn++;
		}
		return rtn;
	}
	bool RadarModify(_RadarMode &RadarMode_, size_t RadarSn)
	{
		size_t PlatNo 	= RadarSn/RADAR_NUM;
		size_t RadarNo 	= RadarSn%RADAR_NUM;
		GetPlatForm(PlatNo).SetRadar(RadarMode_, RadarNo);
		return true;
	}
	int RadarModify(_RadarMode &RadarMode_, size_t *RadarSnArr, size_t RadarSnArrLen)
	{
		size_t rtn = 0;
		for(size_t i = 0; i < RadarSnArrLen; i++)
		{
			size_t PlatNo  = RadarSnArr[i]/RADAR_NUM;
			size_t RadarNo = RadarSnArr[i]%RADAR_NUM;
			GetPlatForm(PlatNo).SetRadar(RadarMode_, RadarNo);
			rtn++;
		}
		return rtn;
	}
	void RadarSimGen(size_t PdwType,
					 size_t RadarSn,
					 size_t Rf,					//重频或载频为参差或者抖动时，Rf、Pri传入数组地址
					 size_t Pri,
					 size_t RfNum = 0,
					 size_t PriNumOrRange = 0)
	{
		const size_t nGroupNum 	= 1;
		const size_t dRange 	= 30;
		size_t *PriArr = (size_t *)Pri;
		size_t *RfArr = (size_t *)Rf;
		switch(PdwType)
		{
		case 1:														//FixRfFixPri
			RadarMode.RfPara.Modify(RF_FIX, Rf);
			RadarMode.PriPara.Modify(PRI_FIX, Pri);
			RadarModify(RadarSn);
			break;
		case 2:														//FixRfJaggPri
			RadarMode.RfPara.Modify(RF_FIX, Rf);
			RadarMode.PriPara.Modify(PRI_JAGGING, 0, dRange, PriNumOrRange, nGroupNum, PriArr, PriNumOrRange);
			RadarModify(RadarSn);
			break;
		case 3:														//FixRfDitherPri
			RadarMode.RfPara.Modify(RF_FIX, Rf);
			RadarMode.PriPara.Modify(PRI_DITHER, Pri, PriNumOrRange);
			RadarModify(RadarSn);
			break;
		case 4:														//AgileRfFixPri
			RadarMode.RfPara.Modify(RF_AGILITY, 0, RfNum, 1, RfArr, RfNum);
			RadarMode.PriPara.Modify(PRI_FIX, Pri);
			RadarModify(RadarSn);
			break;
		case 5:														//CotinuePulse
			break;
		case 6:														//AgileRfDitherPri
			RadarMode.RfPara.Modify(RF_AGILITY, 0, RfNum, 1, RfArr, RfNum);
			RadarMode.PriPara.Modify(PRI_DITHER, Pri, PriNumOrRange);
			RadarModify(RadarSn);
			break;
		default:
			break;
		}
	}
	void MutiPlatPriRun(_RadarBuffer & RadarBuf)
	{
		size_t PlatRunNum = 0, RadarRunNum = 0;
		if((RadarBuf.SortReadyStatus == IDLE) && (RadarBuf.PriStatus == IDLE))
		{
			RadarBuf.PriStatus = BUSY;
			for(size_t i = 0; i<PLAT_NUM; i++)
			{
				if(PlatForm[i].GetPlatFormEn() == true)
				{
					PlatRunNum++;
					RadarRunNum = 0;
					for(size_t j = 0; j<RADAR_NUM; j++)
					{
						if((PlatForm[i].GetRadarMode(j) != RadarMode_Null) && (PlatForm[i].GetRadarStTime(j) <= RadarBuf.GetSortTimes()))
						{
							RadarRunNum++;
							(PlatForm[i].GetRadar(j).PriFunc)(PlatForm[i].GetRadar(j), RadarBuf);
						}
					}
					if(RadarRunNum == 0)
					{
						PlatRunNum--;
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
				RadarBuf.PriStatus 	= IDLE;
				RadarBuf.PwStatus 	= IDLE;
				RadarBuf.RFStatus 	= IDLE;
				RadarBuf.SortReadyStatus = READY;
			}
			else
			{
				RadarBuf.PriStatus = READY;
			}
		}
	}
	void MutiPlatPwRun(_RadarBuffer & RadarBuf)
	{
		size_t PlatRunNum = 0, RadarRunNum = 0;
		if ((RadarBuf.SortReadyStatus == IDLE) && (RadarBuf.PwStatus == IDLE))
		{
			RadarBuf.PwStatus = BUSY;
			for (size_t i = 0; i<PLAT_NUM; i++)
			{
				if (PlatForm[i].GetPlatFormEn() == true)
				{
					PlatRunNum++;
					RadarRunNum = 0;
					for (size_t j = 0; j<RADAR_NUM; j++)
					{
						if((PlatForm[i].GetRadarMode(j) != RadarMode_Null) && (PlatForm[i].GetRadarStTime(j) <= RadarBuf.GetSortTimes()))
						{
							RadarRunNum++;
							(PlatForm[i].GetRadar(j).PwFunc)(PlatForm[i].GetRadar(j), RadarBuf);
						}
					}
					if(RadarRunNum == 0)
					{
						PlatRunNum--;
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
				RadarBuf.PriStatus 	= IDLE;
				RadarBuf.PwStatus 	= IDLE;
				RadarBuf.RFStatus 	= IDLE;
				RadarBuf.SortReadyStatus = READY;
			}
			else
			{
				RadarBuf.PwStatus = READY;
			}
		}
	}
	void MutiPlatRFRun(_RadarBuffer & RadarBuf)
	{
		size_t PlatRunNum = 0, RadarRunNum = 0;
		if ((RadarBuf.SortReadyStatus == IDLE) && (RadarBuf.RFStatus == IDLE))
		{
			RadarBuf.RFStatus = BUSY;
			for (size_t i = 0; i<PLAT_NUM; i++)
			{
				if (PlatForm[i].GetPlatFormEn() == true)
				{
					PlatRunNum++;
					RadarRunNum = 0;
					for (size_t j = 0; j<RADAR_NUM; j++)
					{
						if((PlatForm[i].GetRadarMode(j) != RadarMode_Null) && (PlatForm[i].GetRadarStTime(j) <= RadarBuf.GetSortTimes()))
						{
							RadarRunNum++;
							(PlatForm[i].GetRadar(j).RFFunc)(PlatForm[i].GetRadar(j), RadarBuf);
						}
					}
					if(RadarRunNum == 0)
					{
						PlatRunNum--;
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
				RadarBuf.PriStatus 	= IDLE;
				RadarBuf.PwStatus 	= IDLE;
				RadarBuf.RFStatus 	= IDLE;
				RadarBuf.SortReadyStatus = READY;
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
		if ((Receiver1.OutPdwStatus == IDLE) && OutPdwSortNum == 1)
		{
			if ((RadarBuffer1.InPDWStatus == READY) && (RadarBuffer2.InPDWStatus == READY))
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
			if (Receiver1.OutPdwStatus == READY)
			{
				OutPdwSortNum = 2;
			}
		}
		if ((Receiver2.OutPdwStatus == IDLE) && OutPdwSortNum == 2)
		{
			if ((RadarBuffer1.InPDWStatus == READY) && (RadarBuffer2.InPDWStatus == READY))
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
			if (Receiver2.OutPdwStatus == READY)
			{
				OutPdwSortNum = 1;
			}
		}
	}
	inline void ModifyOutPdw(_OutPdwType &OutPdw)										//自定义输出数据
	{
		OutPdw.Pa = 50;
	}
	void ModifyOutPdwArray(size_t Start = 0,size_t Lens = 0,size_t sel = 0)				//自定义指定长度输出数据
	{
		_OutPdwType *OutPdwBuf = NULL;
		size_t OutPdwLens = 0;
		if(sel == 0)
		{
			if (Receiver1.OutPdwStatus == READY)
			{
				OutPdwBuf  = Receiver1.GetOutPdwAddr();
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
			if (Receiver2.OutPdwStatus == READY)
			{
				OutPdwBuf  = Receiver2.GetOutPdwAddr();
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
			if (Receiver1.OutPdwStatus == READY)
			{
				OutPdwBuf  = Receiver1.GetOutPdwAddr();
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
			if (Receiver2.OutPdwStatus == READY)
			{
				OutPdwBuf  = Receiver2.GetOutPdwAddr();
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
		while ((Receiver1.OutPdwStatus != READY) || (Receiver2.OutPdwStatus != READY))
		{
			MutiPlatPriRun(RadarBuffer1);
			MutiPlatPwRun(RadarBuffer1);
			MutiPlatRFRun(RadarBuffer1);
			RadarBuffer1.ToaSort(PlatForm);
			MutiPlatPriRun(RadarBuffer2);
			MutiPlatPwRun(RadarBuffer2);
			MutiPlatRFRun(RadarBuffer2);
			RadarBuffer2.ToaSort(PlatForm);
			ParallelPDWSort();
		}
		ModifyOutPdwArray();
        Receiver1.OutPdwStatus = IDLE;
        Receiver1.ClearSortNum();
        Receiver1.ClearSortTime();
        Receiver2.OutPdwStatus = IDLE;
        Receiver2.ClearSortNum();
        Receiver2.ClearSortTime();
	}
    void WriteFile(const string& filepath, UINT runSec)
    {
        RadarBuffer1.ResetGlobalToa();
        RadarBuffer1.ClearSortTimes();
        UINT runTimes = runSec*1000/336;
        ofstream svfile(filepath, ios::binary);
        if(!svfile.is_open())
        {
            ERRORMSG("保存文件无法打开!");
            return;
        }
		RadarBuffer1.SetNoToaParaBuf(PlatForm);
		RadarBuffer2.SetNoToaParaBuf(PlatForm);
        while(1)
        {  
            COUT(runTimes);
            while ((Receiver1.OutPdwStatus != READY) || (Receiver2.OutPdwStatus != READY))
            {
                MutiPlatPriRun(RadarBuffer1);
                MutiPlatPwRun(RadarBuffer1);
                MutiPlatRFRun(RadarBuffer1);
                RadarBuffer1.ToaSort(PlatForm);
                MutiPlatPriRun(RadarBuffer2);
                MutiPlatPwRun(RadarBuffer2);
                MutiPlatRFRun(RadarBuffer2);
                RadarBuffer2.ToaSort(PlatForm);
                ParallelPDWSort();
                runTimes--;
                if(runTimes == 0)
                {
                    break;
                }
            }
            ModifyOutPdwArray();
            COUTS("BufferSortNum1 - ", *GetSortNumAddr1(), ", BufferSortNum2 - ", *GetSortNumAddr2());
            svfile.write((char*) (GetPdwBufAddr1()), (*GetSortNumAddr1() + 1) * sizeof(_OutPdwType));
            svfile.write((char*) (GetPdwBufAddr2()), (*GetSortNumAddr2() + 1) * sizeof(_OutPdwType));
            Receiver1.OutPdwStatus = IDLE;
            Receiver1.ClearSortNum();
            Receiver1.ClearSortTime();
            Receiver2.OutPdwStatus = IDLE;
            Receiver2.ClearSortNum();
            Receiver2.ClearSortTime();
            if(runTimes == 0)
            {
                break;
            }
        }
        svfile.close();
    }
	void ShowOutBuffer(UINT NumStart, UINT NumStop, USHORT PlatRadarSn = PLAT_NUM*RADAR_NUM, size_t sel = 0)
	{
		if(sel == 0)
		{
			COUTSECBEG("Receiver1");
			Receiver1.show(NumStart, NumStop, PlatRadarSn);
			COUTSECEND("Receiver1");
			COUTSECBEG("Receiver2");
			Receiver2.show(NumStart, NumStop, PlatRadarSn);
			COUTSECEND("Receiver2");
		}
		else if(sel == 1)
		{
			COUTSECBEG("Receiver1");
			Receiver1.show(NumStart, NumStop, PlatRadarSn);
			COUTSECEND("Receiver1");
		}
		else if(sel == 2)
		{
			COUTSECBEG("Receiver2");
			Receiver2.show(NumStart, NumStop, PlatRadarSn);
			COUTSECEND("Receiver2");
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
				COUTS("|-------------------------------------| PlatForm[", i, "] Start |----------------------------|");
				PlatForm[i].Show();
				COUTS("|-------------------------------------| PlatForm[", i, "] End   |----------------------------|");
			}
		}
		else if(sel < PLAT_NUM)
		{
			COUTS("|-------------------------------------| PlatForm[", sel, "] Start |----------------------------|");
			PlatForm[sel].Show();
			COUTS("|-------------------------------------| PlatForm[", sel, "] End   |----------------------------|");
		}
	}
	void CoutRadarBufferStatus(size_t sel = 0)
	{
		if(sel == 0)
		{
			COUTSECBEG("RadarBuffer1");
			RadarBuffer1.showStatus();
			COUTSECEND("RadarBuffer1");
			COUTSECBEG("RadarBuffer2");
			RadarBuffer2.showStatus();
			COUTSECEND("RadarBuffer2");
		}
		else if(sel == 1)
		{
			COUTSECBEG("RadarBuffer1");
			RadarBuffer1.showStatus();
			COUTSECEND("RadarBuffer1");
		}
		else if(sel == 2)
		{
			COUTSECBEG("RadarBuffer2");
			RadarBuffer2.showStatus();
			COUTSECEND("RadarBuffer2");
		}
	}
	void CoutReceiverStatus(size_t sel = 0)
	{
		if(sel == 0)
		{
			COUTSECBEG("Receiver1");
			Receiver1.showStatus();
			COUTSECEND("Receiver1");
			COUTSECBEG("Receiver2");
			Receiver2.showStatus();
			COUTSECEND("Receiver2");
		}
		else if(sel == 1)
		{
			COUTSECBEG("Receiver1");
			Receiver1.showStatus();
			COUTSECEND("Receiver1");
		}
		else if(sel == 2)
		{
			COUTSECBEG("Receiver2");
			Receiver2.showStatus();
			COUTSECEND("Receiver2");
		}
	}
};
#endif	/* _IRADARSIMPRO_H_ */