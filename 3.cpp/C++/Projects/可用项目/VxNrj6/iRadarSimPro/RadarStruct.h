#ifndef _RADARSTRUCT_H_
#define _RADARSTRUCT_H_
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<iomanip>
using namespace std;
#if PPC
#include<VxWorks.h>
#include <taskLibCommon.h>
#elif _MSC_VER
#include<time.h>
#define uchar_t 	unsigned char
#define ushort_t 	unsigned short
#define uint_t 		unsigned int
#endif
#define NLogF		printf
#define PRINT(x) 	printf(#x" = %d\n",(x))
#define PRINT_X(x) 	printf(#x" = 0x%X\n",(x))
#define PRINT_STATUS(x) (((x) == 0) ? printf(#x" = IDLE\n") : (((x) == 1) ? printf(#x" = BUSY\n") : (((x) == 2) ? printf(#x" = READY\n") : 0)))
#define SAL_Copy	memcpy

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
	uint_t member;										\
	member = offsetof(type,member);						\
	PRINT_X(member);									\
	member;												\
})
#define	STRUCT_OFFSET(ptr,type,member) ({				\
	uint_t n##type;										\
	n##type = (size_t)container_of(ptr,type,member);	\
	PRINT_X(n##type);									\
	n##type;											\
})
#define StructMSet(ptr,type,member) ({					\
	uint_t n##ptr,n##member;							\
	n##ptr = (size_t)&ptr;								\
	n##member = offsetof(type,member);					\
	n##ptr = n##ptr+n##member;							\
	*((uchar_t *)(n##ptr)) = (uchar_t)n##member;		\
})
#define StructMCheck(ptr,type,member) ({				\
	uint_t n##ptr,n##member;							\
	n##ptr = (size_t)&ptr;								\
	n##member = offsetof(type,member);					\
	n##ptr = n##ptr+n##member;							\
	if(*((uchar_t *)(n##ptr)) == (uchar_t)n##member)	\
	{																\
		NLogF(#member" = %d, Check Success!",(uchar_t)n##member);	\
	}																\
	else															\
	{																\
		NLogF(#member" Check failed!");								\
	}																\
})

#define StructMLog(ptr,member) ({									\
	PRINT_X(ptr.member);											\
	1;																\
})

enum _PlatMsgType
{ 
	SingleRadar = 0, 
	SinglePlat = 1, 
	FullNum = 2 
};

enum _EsmType
{ 
	BroadBank = 0
};

enum
{
	RadarMode_Null = 0,
	RadarMode_En   = 1
};

enum _PriType		//PRI参数类型
{
      PRI_FIX,
      PRI_DITHER,
      PRI_JAGGING,
      PRI_SLIDER,
};

enum _PwType		//PW参数类型
{
	  PW_FIX,
	  PW_DEFINE
};

enum _RfType		//RF参数类型
{
	  RF_CONTINUOUSLY,
	  RF_FIX,
	  RF_AGILITY,
	  RF_DIVERSITY
};

enum { IDLE = 0, GENBUSY = 1, READY = 2 };
enum { PARABUF_IDLE = 0, PARABUF_READY = 2 };
enum { InPDW_IDLE = 0, InPDW_SORTBUSY = 1, InPDW_READY = 2 };
enum { OutPDW_IDLE = 0, OutPDW_SORTBUSY = 1, OutPDW_READY = 2 };
enum { RecOutPDW_IDLE = 0, RecOutPDW_BUSY = 1, RecOutPDW_READY = 2 };

enum
{
	PLAT_NUM = 60,		//最大平台数
	RADAR_NUM= 10,		//最大雷达数
	AGI_NUM  = 64,		//频率捷变点数
	DIV_NUM  = 64,		//频率分集点数
	JAG_NUM  = 64,		//重频参差点数
	SLIP_NUM = 64,		//重频滑动点数
	PW_NUM	 = 64,		//脉宽点数
	SIGN_NUM = 10,		//Buffer采用排序最大数
	RECIVERSORTTIMES_MAX = 120,
	RECIVERSORTNUM_MAX 	 = 5000
};

/************************* RF *************************/
struct FREAGILITY	//频率捷变
{
      ushort_t		nFreNum;			//点数2-AGINUM
      ushort_t		nGroupNum;			//脉组脉冲数  1-1000  1－脉间 
      uint_t		dpFre[AGI_NUM];		//频率点 MHz
};

struct FREDIVERSITY	//频率分集
{
      uint_t		nFreNum;			//分集数 2-DIVNUM
      uint_t		dpFre[DIV_NUM];		//频率点 MHz
      uint_t		dInterval[DIV_NUM];	//间隔时间 2us-3000us
};

union _Rf
{
	  uint_t		dFre;				//中心频率
      FREAGILITY	FreAgility;			//捷变参数
      FREDIVERSITY	FreDiversity;		//分集参数
};
/************************* PRI *************************/
struct PRIDITHER	//重频抖动
{
      uint_t		nGroupNum;			//脉组脉冲数  1-1000  1－脉间 
      uint_t		dPri;				//中心重复周期  2us～100ms
      uint_t		dRange;				//重频范围  重频范围不大于中心重复周期的100%
};

struct PRIJAGGING	//重频参差
{
      ushort_t		nPriNum;			//参差数  2-JAGNUM
      ushort_t		nGroupNum;			//脉组脉冲数  1-1000  1－脉间
      uint_t		dpRatio[JAG_NUM];	//参差比  参差比*基准重频值在(2us～100ms)之间
};

struct PRISLIDER	//重频滑动
{	
      ushort_t		nPriNum;			//滑动数  2-SLIPNUM
      ushort_t		nGroupNum;			//脉组脉冲数  1-1000  1－脉间 
      int			dPriStep;			//滑动步进 有正有负
      uint_t		dpPri[SLIP_NUM];	//重频值在  (2us～100ms)之间	
};

union _Pri
{
	  uint_t		dPri;				//重频
      PRIDITHER		PriDither;			//抖动参数
      PRIJAGGING	PriJagging;			//参差参数
      PRISLIDER		PriSlider;			//滑动参数
};
 
/************************* PW *************************/
struct PWDEFINE		//脉宽自定义
{
      uchar_t		nPwNum;				//脉宽数量
      ushort_t		nGroupNum;	    	//脉组脉冲数  1-1000
      uint_t		dpPw[PW_NUM];		//脉宽0.05～1024us   脉宽设置步长：0.025us（0.05～10us）, 0.1us（10～1024us）
};

struct _Pw
{
	uint_t			dPw;				//固定脉宽0.05～1024us
    PWDEFINE		PwDefine;			//脉宽自定义
};

/************************* AzElDis *************************/
struct _AzElDis
{
	ushort_t 	Az;
	ushort_t 	El;
	ushort_t 	Distance;
};
struct _RadarPara
{
	ushort_t	Flag;				//连续波标志
	uint_t		Pw;
	uint_t		Rf;
	uint_t		Az;
	uint_t		El;
	int			Pa;
};
struct _PriPara		//雷达Pri参数
{	
	_Pri		uPri;
	uint_t 		ToaMod;				//单位：10ns
	uint_t 		ToaModRand;			//单位：10ns
	uint_t		NumTemp;			//数组序号缓存
	uint_t		GroupNumTemp;		//脉组序号缓存
};

struct _PwPara		//雷达Pw参数
{
	_Pw			sPw;
	uint_t		NumTemp;			//数组序号缓存
	uint_t		GroupNumTemp;		//脉组序号缓存
};

struct _RfPara		//雷达RF参数
{
	_Rf			sFre;
	uint_t		NumTemp;			//数组序号缓存
	uint_t		GroupNumTemp;		//脉组序号缓存
	uint_t		PriSumTemp;			//分集Pri累加和缓存
};

struct _RadarMode	//雷达模式
{
	uchar_t		byPriType;			//重频类型0－固定，1－抖动，2－参差，3－滑变
	uchar_t		byFreType;			//频率类型0－连续波，1－固定，2－捷变，3－分集
	uchar_t		byModType;			//调制类型0－无调制，1－线性调频，2－非线性调频，3－调相
	uchar_t		byPwType;			//脉宽类型0－固定，1－自定义
	uchar_t		byScanType;			//扫描类型0－－跟踪，1－圆扫，2－扇扫，3-锥扫、4-相扫、5-边跟边扫 6-自定义扫
	_Rf			Rf;					//载频
	_Pri		Pri;				//重复间隔参数
	_Pw			Pw;					//脉宽
};

struct _RecMsg		//初始化平台报文
{
	uchar_t      	nHead;				//0x7e，表示新的一帧报文开始
	ushort_t		nLen;				//报文长度  多少个4字节
	uchar_t     	nType;				//报文类型 101	

	int				dRCS;				//侦察机接收面积 ㎡	
	short			nLmd;				//侦察机灵敏度　-40～-200 dBm
	ushort_t		nEsmType;			//侦察机类型
};

struct _PlatMsg
{
	uint_t			nPlatNum;			//对抗平台数量
};

struct _RadarMsg	//雷达报文
{
	uchar_t			PlatSN;				//平台编号 0-59
	uchar_t			RadSN;				//雷达编号 0-9
	_RadarMode		RadarMode;			//参数
};

struct _PRNSign
{
	uint_t      Num;
	uint_t      Index[SIGN_NUM];
};

struct _PlatRadarNum
{
	ushort_t 	PlatRadar;
	uint_t 		ToaNum;
};

struct _InPDW
{
	ushort_t 	PlatRadar;
	_RadarPara	RadarPara;
	uint_t 		ToaH;
	uint_t 		ToaL;
};

struct _OutPdw
{
	uint_t Flag;				//连续波标志
	uint_t Az;
	uint_t El;
	int	   Pa;
	uint_t Rf;
	uint_t Pw;
	uint_t Toa;
	uint_t Ton;
};

#endif
