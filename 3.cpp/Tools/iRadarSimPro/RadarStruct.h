#ifndef _RADARSTRUCT_H_
#define _RADARSTRUCT_H_
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include <fstream>
#include<vector>
#include<iomanip>
using namespace std;
#include"../Macro.h"
#include"../XptType.h"
#include"../Error.h"
#include<time.h>

/************************* Define *************************/
#ifndef MAX
#define MAX(x,y) ({										\
	typeof(x) _max1 = (x);								\
	typeof(y) _max2 = (y);								\
	(void)(&_max1 == &_max2);							\
	(_max1 > _max2) ? _max1 : _max2;					\
})	
#endif
#define container_of(ptr,type,member) ({				\
	const typeof(((type *)0)->member) *_mptr = (ptr);	\
	(type *)((char *)_mptr - offsetof(type,member));	\
})
#define STRUCTMEMBER_OFFSET(type,member) ({				\
	UINT member;										\
	member = offsetof(type,member);						\
	COUTBYHEX(member);									\
	member;												\
})
#define	STRUCT_OFFSET(ptr,type,member) ({				\
	UINT n##type;										\
	n##type = (size_t)container_of(ptr,type,member);	\
	COUTBYHEX(n##type);									\
	n##type;											\
})
#define StructMSet(ptr,type,member) ({					\
	UINT n##ptr,n##member;								\
	n##ptr = (size_t)&ptr;								\
	n##member = offsetof(type,member);					\
	n##ptr = n##ptr+n##member;							\
	*((UCHAR *)(n##ptr)) = (UCHAR)n##member;			\
})
#define StructMCheck(ptr,type,member) ({				\
	UINT n##ptr,n##member;								\
	n##ptr = (size_t)&ptr;								\
	n##member = offsetof(type,member);					\
	n##ptr = n##ptr+n##member;							\
	if(*((UCHAR *)(n##ptr)) == (UCHAR)n##member)		\
	{																\
		COUTS(#member" = ", (UCHAR)n##member, ", Check Success!");	\
	}																\
	else															\
	{																\
		COUTS(#member" Check failed!");								\
	}																\
})
#define StructMLog(ptr,member) ({									\
	COUTBYHEX(ptr.member);											\
	1;																\
})
#define PRNBUFFER_SIGN 0x0A00
enum _EsmType
{ 
	BroadBank = 0
};
enum
{
	RadarMode_Null = 0,
	RadarMode_En   = 1
};
enum _PriType			//PRI参数类型
{
      PRI_FIX,
      PRI_DITHER,
      PRI_JAGGING,
      PRI_SLIDER,
};
enum 					//RadarParaGen类型
{
	Diff 	 = 1,
	Slider 	 = 2,
	RandPara = 3		
};
enum _PwType			//PW参数类型
{
	  PW_FIX,
	  PW_DEFINE
};
enum _RfType			//RF参数类型
{
	  RF_CONTINUOUSLY,
	  RF_FIX,
	  RF_AGILITY,
	  RF_DIVERSITY
};
enum { IDLE = 0, BUSY = 1, READY = 2 };
enum
{
	PLAT_NUM 			 = 60,			//最大平台数
	RADAR_NUM			 = 10,			//最大雷达数
	AGI_NUM  			 = 64,			//频率捷变点数
	DIV_NUM  			 = 64,			//频率分集点数
	JAG_NUM  			 = 64,			//重频参差点数
	SLIP_NUM 			 = 64,			//重频滑动点数
	PW_NUM	 			 = 64,			//脉宽点数
	SIGN_NUM 			 = 10,			//Buffer采用排序最大数
	RADAR_BUFSIZE		 = 0x1000000,	//Pri排序数组长度
	SIGN_SIZE			 = 8,			//标记位移长度
	RECIVERSORTTIMES_MAX = 120,
	RECIVERSORTNUM_MAX 	 = 5000
};
/************************* RF *************************/
struct FREAGILITY	//频率捷变
{
      USHORT		nFreNum;			//点数2-AGINUM
      USHORT		nGroupNum;			//脉组脉冲数  1-1000  1－脉间 
      UINT			dpFre[AGI_NUM];		//频率点 MHz
};
struct FREDIVERSITY	//频率分集
{
      UINT			nFreNum;			//分集数 2-DIVNUM
      UINT			dpFre[DIV_NUM];		//频率点 MHz
      UINT			dInterval[DIV_NUM];	//间隔时间 2us-3000us
};
union _Rf
{
	  UINT			dFre;				//中心频率
      FREAGILITY	FreAgility;			//捷变参数
      FREDIVERSITY	FreDiversity;		//分集参数
};
/************************* PRI *************************/
struct PRIDITHER	//重频抖动
{
      UINT			nGroupNum;			//脉组脉冲数  1-1000  1－脉间 
      UINT			dPri;				//中心重复周期  2us～100ms
      UINT			dRange;				//重频范围  重频范围不大于中心重复周期的100%
};
struct PRIJAGGING	//重频参差
{
      USHORT		nPriNum;			//参差数  2-JAGNUM
      USHORT		nGroupNum;			//脉组脉冲数  1-1000  1－脉间
      UINT			dpRatio[JAG_NUM];	//参差比  参差比*基准重频值在(2us～100ms)之间
};
struct PRISLIDER	//重频滑动
{	
      USHORT		nPriNum;			//滑动数  2-SLIPNUM
      USHORT		nGroupNum;			//脉组脉冲数  1-1000  1－脉间 
      int			dPriStep;			//滑动步进 有正有负
      UINT			dpPri[SLIP_NUM];	//重频值在  (2us～100ms)之间	
};
union _Pri
{
	  UINT			dPri;				//重频
      PRIDITHER		PriDither;			//抖动参数
      PRIJAGGING	PriJagging;			//参差参数
      PRISLIDER		PriSlider;			//滑动参数
};
/************************* PW *************************/
struct PWDEFINE		//脉宽自定义
{
      UCHAR			nPwNum;				//脉宽数量
      USHORT		nGroupNum;	    	//脉组脉冲数  1-1000
      UINT			dpPw[PW_NUM];		//脉宽0.05～1024us   脉宽设置步长：0.025us（0.05～10us）, 0.1us（10～1024us）
};
struct _Pw
{
	UINT			dPw;				//固定脉宽0.05～1024us
    PWDEFINE		PwDefine;			//脉宽自定义
};
/************************* AzElDis *************************/
struct _LocCoordinate
{
	UINT 			Az;
	UINT 			El;
	UINT 			Distance;
	UINT			JitterRange;
	UINT			SortTimes;
	_LocCoordinate()
	{
		Az 			= 0;
		El 			= 0;
		Distance 	= 0;
		JitterRange	= 0;
		SortTimes	= 0;
	}
};
typedef vector<_LocCoordinate> _LocVec;
struct _AzElDis
{
	_LocVec 		SetVec;				//设置AzElDis
	_LocVec			SortVec;			//读取AzElDis
	UINT			SortVecEd;			//读取AEDEd
	_AzElDis()
	{
		SetVec.resize(1);
		SetVec[0].Az 			= 0;
		SetVec[0].El 			= 0;
		SetVec[0].Distance 		= 1000;
		SetVec[0].JitterRange	= 0;
		SetVec[0].SortTimes 	= 0;
		CalcAzElDis();
	}
	void Set(_LocVec &SetVec_)
	{
		SetVec = SetVec_;
		CalcAzElDis();
	}
	UINT GetAz(UINT Loc) const
	{
		Loc = (Loc >= SortVecEd) ? SortVecEd : Loc;
		if(SortVec[Loc].SortTimes != Loc)
		{
			ERRORMSG("SortVec[Loc].SortTimes != Loc");
			return 0;
		}
		else
		{
			return SortVec[Loc].Az;
		}
	}
	UINT GetEl(UINT Loc) const
	{
		Loc = (Loc >= SortVecEd) ? SortVecEd : Loc;
		if(SortVec[Loc].SortTimes != Loc)
		{
			ERRORMSG("SortVec[Loc].SortTimes != Loc");
			return 0;
		}
		else
		{
			return SortVec[Loc].El;
		}
	}
	UINT GetDistance(UINT Loc) const
	{
		Loc = (Loc >= SortVecEd) ? SortVecEd : Loc;
		if(SortVec[Loc].SortTimes != Loc)
		{
			ERRORMSG("SortVec[Loc].SortTimes != Loc");
			return 0;
		}
		else
		{
			return SortVec[Loc].Distance;
		}
	}
	UINT GetJitter(UINT Loc) const
	{
		Loc = (Loc >= SortVecEd) ? SortVecEd : Loc;
		if(SortVec[Loc].SortTimes != Loc)
		{
			ERRORMSG("SortVec[Loc].SortTimes != Loc");
			return 0;
		}
		else
		{
			return SortVec[Loc].JitterRange;
		}
	}
	void CalcAzElDis()
	{
		UINT SetVecSize = SetVec.size();
		if(SetVecSize == 0)
		{
			ERRORMSG("AzElDis Error!");
			return;
		}
		SortVec.resize(SetVec[SetVecSize - 1].SortTimes + 1);
		SortVecEd = SortVec.size() - 1;
		if(SetVecSize == 1)
		{
			SortVec[0].Az		 	= SetVec[0].Az;
			SortVec[0].El 		 	= SetVec[0].El;
			SortVec[0].Distance	 	= SetVec[0].Distance;
			SortVec[0].JitterRange	= SetVec[0].JitterRange;
			SortVec[0].SortTimes 	= SetVec[0].SortTimes;
		}
		else
		{
			for(UINT i = 0; i < SetVecSize - 1; i++)
			{
				if(SetVec[i].SortTimes > SetVec[i+1].SortTimes)
				{
					ERRORMSG("SetVec[i].SortTimes > SetVec[i+1].SortTimes");
					return;
				}
				for(UINT j = SetVec[i].SortTimes; j <= SetVec[i+1].SortTimes; j++)
				{
					SortVec[j].Az 		 	= StraightDataGen(SetVec[i].Az, SetVec[i+1].Az, SetVec[i].SortTimes, SetVec[i+1].SortTimes, j);
					SortVec[j].El 		 	= StraightDataGen(SetVec[i].El, SetVec[i+1].El, SetVec[i].SortTimes, SetVec[i+1].SortTimes, j);
					SortVec[j].Distance	 	= StraightDataGen(SetVec[i].Distance, SetVec[i+1].Distance, SetVec[i].SortTimes, SetVec[i+1].SortTimes, j);
					SortVec[j].JitterRange 	= SetVec[i].JitterRange;
					SortVec[j].SortTimes 	= j;
				}
			}
		}
	}
	inline UINT StraightDataGen(UINT ValSt, UINT ValEd, UINT VecLocSt, UINT VecLocEd, UINT VecLoc)
	{
		return ValSt + (ValEd - ValSt) * ((VecLoc - VecLocSt) / (VecLocEd - VecLocSt));
	}
};
struct _RadarPara
{
	USHORT			Flag;				//连续波标志
	UINT			Pw;
	UINT			Rf;
	UINT			Az;
	UINT			El;
	int				Pa;
};
inline int RadarParaGen(size_t *ArrTemp,size_t PriL,size_t PriR,size_t nPriNum,size_t ParaGenType)
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
struct _PriPara		//雷达Pri参数
{	
	_PriType	PriType;
	_Pri		Pri;
	UINT 		ToaMod;					//单位：10ns
	UINT 		ToaModRand;				//单位：10ns
	UINT		NumTemp;				//数组序号缓存
	UINT		GroupNumTemp;			//脉组序号缓存
	_PriPara()
	{
		Init();
	}
	void Init()
	{
		PriType 	 = PRI_FIX;
		ToaMod 		 = 0;
		ToaModRand 	 = 0;
		NumTemp 	 = 0;
		GroupNumTemp = 0;
	}
	bool Modify(_PriType PriType_ 	= PRI_FIX,
				size_t dPri 		= 10000,
				size_t dRange 		= 5000,		//重频范围  重频范围不大于中心重复周期的100%
				size_t nPriNum 		= 10,		//参差或滑动数
				size_t nGroupNum 	= 1,		//脉组脉冲数  1-1000  1－脉间 
				size_t *PriArr 		= NULL,		//自定义重频数组
				size_t PriArrLen 	= 0,		//自定义重频数组长度
				size_t dPriStep		= 1			//滑动步进 有正有负
				)
	{
		Init();
		PriType = PriType_;
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
		delete[] ArrTemp;
		return true;
	}
};
struct _PwPara		//雷达Pw参数
{
	_PwType		PwType;
	_Pw			Pw;
	UINT		NumTemp;				//数组序号缓存
	UINT		GroupNumTemp;			//脉组序号缓存
	_PwPara()
	{
		Init();
	}
	void Init()
	{
		PwType 	 	 = PW_FIX;
		NumTemp 	 = 0;
		GroupNumTemp = 0;
	}
	bool Modify(_PwType PwType_ 	= PW_FIX,
				size_t dPw			= 50,
				size_t nPwNum		= 10,		//脉宽数量
				size_t nGroupNum	= 1,		//脉组脉冲数  1-1000
				size_t *PwArr 		= NULL,		//自定义脉宽数组
				size_t PwArrLen 	= 0			//自定义脉宽数组长度
				)
	{
		Init();
		PwType = PwType_;
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
		delete[] ArrTemp;
		return true;
	}
};
struct _RfPara		//雷达RF参数
{
	_RfType		RfType;
	_Rf			Rf;
	UINT		NumTemp;				//数组序号缓存
	UINT		GroupNumTemp;			//脉组序号缓存
	UINT		PriSumTemp;				//分集Pri累加和缓存
	_RfPara()
	{
		Init();
	}
	void Init()
	{
		RfType 	 	 = RF_FIX;
		NumTemp 	 = 0;
		GroupNumTemp = 0;
		PriSumTemp   = 0;
	}
	bool Modify(_RfType RfType_ 	= RF_FIX,
				size_t dFre			= 256000,
				size_t nFreNum		= 10,		//点数 2-DIVNUM
				size_t nGroupNum 	= 1,		//脉组脉冲数  1-1000  1－脉间 
				size_t *RfArr 		= NULL,		//自定义载频数组
				size_t RfArrLen 	= 0			//自定义载频数组长度
				)
	{
		Init();
		RfType = RfType_;
		size_t *ArrTemp = new size_t[nFreNum];
		memset(ArrTemp,0,nFreNum*sizeof(size_t));
		if (RfType == RF_CONTINUOUSLY)
		{
			Rf.dFre = dFre;
		}
		else if (RfType == RF_FIX)
		{
			Rf.dFre = dFre;
		}
		else if (RfType == RF_AGILITY)
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
					Rf.FreAgility.nFreNum = RfArrLen;
					for (size_t i = 0; i < RfArrLen; i++)
					{
						Rf.FreAgility.dpFre[i] = RfArr[i];
					}
				}
			}
		}
		else if (RfType == RF_DIVERSITY)
		{
			;
		}
		delete[] ArrTemp;
		return true;
	}
};
struct _RadarMode
{
	_PriPara PriPara;
	_RfPara	 RfPara;
	_PwPara  PwPara;
};
struct _PRNSign
{
	UINT     		Num;
	UINT     		Index[SIGN_NUM];
};
struct _PlatRadarNum
{
	USHORT 		PlatRadar;
	UINT 		ToaNum;
};
struct _InPDW
{
	USHORT 		PlatRadar;
	_RadarPara	RadarPara;
	UINT 		ToaH;
	UINT 		ToaL;
};
struct _OutPdw
{
	UINT 	Flag;				//连续波标志
	UINT 	Az;
	UINT 	El;
	int	   	Pa;
	UINT 	Rf;
	UINT 	Pw;
	UINT 	Toa;
	UINT 	Ton;
};
#endif