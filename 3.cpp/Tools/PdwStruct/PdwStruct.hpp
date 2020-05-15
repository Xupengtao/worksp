/*
 * @Author: xpt 
 * @Date: 2019-12-20 17:17:39 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-04-14 15:20:24
 */

#ifndef PDW_STRUCT_
#define PDW_STRUCT_

#include "../Macro.h"
#include "../XptType.h"
#include <vector>
#include <cmath>
#include "../imageTools/BMPImage.h"
#include "../iRadarSimPro/iRadarSimPro.h"

struct _fileMsg                                     //XYG-C
{
    static CUSHORT lineHead = 0x55AA;
    static CUSHORT lineTail = 0xAA55;
    USHORT head;
    USHORT flag;
    USHORT yearmonth;
    USHORT dayhour;
    USHORT minsec;
    USHORT toal;
    USHORT toah;
    USHORT rf;
    USHORT band;
    USHORT doa;
    USHORT el;
    USHORT pa;
    USHORT pwl;
    USHORT pwh;
    USHORT resv;
    USHORT tail;
    unsigned long long time;

    inline void print()
    {
        COUTWIDTH(5, head, flag, yearmonth, dayhour, minsec);
        COUTWIDTH(5, toal, toah, rf, band, doa);
        COUTWIDTH(5, el, pa, pwl, pwh, resv, tail);
        COUTWIDTH(5, EndianSwap(time));
    }
};

struct _xyg_z                                       //XYG-Z
{
    static CUSHORT lineHead = 0xAA55;
    static CUSHORT lineTail = 0x55AA;
    USHORT head;
    USHORT unused1;
    USHORT unused2;
    USHORT unused3;
    USHORT unused4;
    USHORT toal;
    USHORT toah;
    USHORT rf;
    USHORT unused7;
    USHORT doa;
    USHORT unused9;
    USHORT pa;
    USHORT pwl;
    USHORT pwh;
    USHORT unused14;
    USHORT unused15;
    USHORT unused16;
    USHORT unused17;
    USHORT unused18;
    USHORT unused19;
    USHORT unused20;
    USHORT unused21;
    USHORT unused22;
    USHORT unused23;
    USHORT unused24;
    USHORT unused25;
    USHORT unused26;
    USHORT unused27;
    USHORT unused28;
    USHORT unused29;
    USHORT unused30;
    USHORT tail;

    inline void print()
    {
        COUTWIDTH(5, head, tail);
        COUTWIDTH(5, pwl, pwh, rf, pa, doa, toal, toah);
    }
};

enum _Map2DType{ToaDoa, ToaPw, ToaRf, ToaPa};

template<typename _PdwTy, typename _Map2DTy>
struct _PdwMap2D
{
	typedef typename _PdwTy::RfTy    RfTy;
	typedef typename _PdwTy::PwTy    PwTy;
	typedef typename _PdwTy::PaTy    PaTy;
	typedef typename _PdwTy::ToaTy   ToaTy;
	typedef typename _PdwTy::DoaTy   DoaTy;
    typedef _Map2DTy Map2DTy;
    
    RfTy    RfMin;
    float   RfNormalUnit;
    PaTy    PaMin;
    float   PaNormalUnit;
    PwTy    PwMin;
    float   PwNormalUnit;
    ToaTy   ToaMin;
    float   ToaNormalUnit;
    int x;
    int y;
    Map2DTy Map2DUnit;

    inline void SetRfPara(RfTy RfMin_, float RfNormalUnit_)
    {
        RfMin = RfMin_;
        RfNormalUnit = RfNormalUnit_;
    }
    inline void SetToaPara(ToaTy ToaMin_, float ToaNormalUnit_)
    {
        ToaMin = ToaMin_;
        ToaNormalUnit = ToaNormalUnit_;
    }
    inline void SetPaPara(PaTy PaMin_, float PaNormalUnit_)
    {
        PaMin = PaMin_;
        PaNormalUnit = PaNormalUnit_;
    }
    inline void SetPwPara(PwTy PwMin_, float PwNormalUnit_)
    {
        PwMin = PwMin_;
        PwNormalUnit = PwNormalUnit_;
    }
    inline void print()
    {
        COUT(x, y, int(Map2DUnit.b), int(Map2DUnit.g), int(Map2DUnit.r));
    }
};

template<typename _PdwTy>
struct _PdwArrayDesc
{
	typedef typename _PdwTy::RfTy    RfTy;
	typedef typename _PdwTy::PwTy    PwTy;
	typedef typename _PdwTy::PaTy    PaTy;
	typedef typename _PdwTy::ToaTy   ToaTy;
	typedef typename _PdwTy::DoaTy   DoaTy;

    UINT    InitTag;            // 初始化标志，初始值为0
    UINT    ToaReverseTimes;    // Toa翻转次数
    UINT    ToaReverseCount;    // Toa翻转时累计脉冲计数，避免错误翻转
    UINT    ToaErrorCount;      // 连续Toa错误次数, 超过10则认为PreToaTmp是错误数据
    PaTy    PaMin;
    PaTy    PaMax;
    RfTy    RfMin;
    RfTy    RfMax;
    PwTy    PwMin;
    PwTy    PwMax;
    ToaTy   PreToaTmp;

    _PdwArrayDesc()
    {
        clear();
    }
    inline void clear()
    {
        InitTag         = 0;
        ToaReverseTimes = 0;
        ToaReverseCount = 0;
        ToaErrorCount   = 0;
        RfMin           = 0;
        RfMax           = 0;
        PaMin           = 0;
        PaMax           = 0;
        PwMin           = 0;
        PwMax           = 0;
        PreToaTmp       = 0;
    }
    inline void print()
    {
        COUTSECBEG("PdwArrayDesc");
        COUTSWIDTH(5, "初始化标志 ", InitTag);
        COUTSWIDTH(5, "Pa最小值   ", PaMin, "Pa最大值   ", PaMax);
        COUTSWIDTH(5, "Rf最小值   ", RfMin, "Rf最大值   ", RfMax);
        COUTSWIDTH(5, "Pw最小值   ", PwMin, "Pw最大值   ", PwMax);
        COUTSWIDTH(5, "Toa翻转次数", ToaReverseTimes);
        COUTSECEND("PdwArrayDesc");
    }
};

template<typename _RfTy     = float,
         typename _PwTy     = float,
         typename _PaTy     = float,
         typename _ToaTy    = float,
         typename _DoaTy    = float>
struct _Pdw
{
	typedef _RfTy 	RfTy;
	typedef _PwTy 	PwTy;
	typedef _PaTy 	PaTy;
	typedef _ToaTy 	ToaTy;
	typedef _DoaTy 	DoaTy;
	typedef _Pdw<_RfTy, _PwTy, _PaTy, _ToaTy, _DoaTy>	_ThisType;

    PaTy  Pa;
    RfTy  Rf;
    PwTy  Pw;
    DoaTy Doa;
    ToaTy Toa;
    int   ToaS;                 // -1: 无效脉冲, >= 0 Toa翻转次数
    USHORT head;
    USHORT tail;
    static _PdwArrayDesc<_ThisType> PdwArrayDesc;
    static CUSHORT lineHead = 0;
    static CUSHORT lineTail = 0;
    
    // inline void operator = (_fileMsg &fileMsg)
    // {
    //     Rf  = EndianSwap(fileMsg.rf);
    //     Toa = (EndianSwap(fileMsg.toah)<<16) + EndianSwap(fileMsg.toal);
    //     Doa = EndianSwap(fileMsg.doa);
    //     Pa  = EndianSwap(fileMsg.pa);
    //     Pw  = (EndianSwap(fileMsg.pwh)<<16) + EndianSwap(fileMsg.pwl);
    // }
    inline void operator = (_fileMsg &fileMsg)
    {
        Pa  = EndianSwap(fileMsg.pa);
        Rf  = EndianSwap(fileMsg.rf);
        Doa = EndianSwap(fileMsg.doa);
        Toa = (EndianSwap(fileMsg.toah)<<16) + EndianSwap(fileMsg.toal);
        Pw  = (EndianSwap(fileMsg.pwh)<<16) + EndianSwap(fileMsg.pwl);
        if(PdwArrayDesc.InitTag == 0)
        {
            PdwArrayDesc.InitTag = 1;
            PdwArrayDesc.PaMin = Pa;
            PdwArrayDesc.PaMax = Pa;
            PdwArrayDesc.RfMin = Rf;
            PdwArrayDesc.RfMax = Rf;
            PdwArrayDesc.PwMin = Pw;
            PdwArrayDesc.PwMax = Pw;
            PdwArrayDesc.PreToaTmp = Toa;
            ToaS = PdwArrayDesc.ToaReverseTimes;
        }
        else
        {
            PdwArrayDesc.PaMin = (PdwArrayDesc.PaMin > Pa) ? Pa : PdwArrayDesc.PaMin;
            PdwArrayDesc.PaMax = (PdwArrayDesc.PaMax < Pa) ? Pa : PdwArrayDesc.PaMax;
            PdwArrayDesc.RfMin = (PdwArrayDesc.RfMin > Rf) ? Rf : PdwArrayDesc.RfMin;
            PdwArrayDesc.RfMax = (PdwArrayDesc.RfMax < Rf) ? Rf : PdwArrayDesc.RfMax;
            PdwArrayDesc.PwMin = (PdwArrayDesc.PwMin > Pw) ? Pw : PdwArrayDesc.PwMin;
            PdwArrayDesc.PwMax = (PdwArrayDesc.PwMax < Pw) ? Pw : PdwArrayDesc.PwMax;
            if(Toa > PdwArrayDesc.PreToaTmp)
            {
                PdwArrayDesc.ToaReverseCount = 0;
                PdwArrayDesc.ToaErrorCount = 0;
                ToaS = PdwArrayDesc.ToaReverseTimes;
                PdwArrayDesc.PreToaTmp = Toa;
            }
            else
            {
                if((Toa < 0x30000000) && (PdwArrayDesc.PreToaTmp > 0xD0000000))
                {
                    if(PdwArrayDesc.ToaReverseCount > 10)
                    {
                        PdwArrayDesc.ToaReverseCount = 0;
                        PdwArrayDesc.ToaErrorCount = 0;
                        PdwArrayDesc.ToaReverseTimes++;
                        ToaS = PdwArrayDesc.ToaReverseTimes;
                        PdwArrayDesc.PreToaTmp = Toa;
                    }
                    else
                    {
                        ToaS = -1;
                        PdwArrayDesc.ToaReverseCount++;
                    }
                }
                else
                {
                    if(PdwArrayDesc.ToaErrorCount > 10)
                    {
                        PdwArrayDesc.ToaReverseCount = 0;
                        PdwArrayDesc.ToaErrorCount = 0;
                        ToaS = PdwArrayDesc.ToaReverseTimes;
                        PdwArrayDesc.PreToaTmp = Toa;
                    }
                    else
                    {
                        ToaS = -1;
                        PdwArrayDesc.ToaErrorCount++;
                    }
                }
            }
        }
    }
    inline void operator = (_xyg_z &fileMsg)
    {
        Pa  = fileMsg.pa;
        Rf  = fileMsg.rf;
        Doa = fileMsg.doa;
        Toa = ((fileMsg.toah)<<16) + fileMsg.toal;
        Pw  = ((fileMsg.pwh) <<16) + fileMsg.pwl;
        if(PdwArrayDesc.InitTag == 0)
        {
            PdwArrayDesc.InitTag = 1;
            PdwArrayDesc.PaMin = Pa;
            PdwArrayDesc.PaMax = Pa;
            PdwArrayDesc.RfMin = Rf;
            PdwArrayDesc.RfMax = Rf;
            PdwArrayDesc.PwMin = Pw;
            PdwArrayDesc.PwMax = Pw;
            PdwArrayDesc.PreToaTmp = Toa;
            ToaS = PdwArrayDesc.ToaReverseTimes;
        }
        else
        {
            PdwArrayDesc.PaMin = (PdwArrayDesc.PaMin > Pa) ? Pa : PdwArrayDesc.PaMin;
            PdwArrayDesc.PaMax = (PdwArrayDesc.PaMax < Pa) ? Pa : PdwArrayDesc.PaMax;
            PdwArrayDesc.RfMin = (PdwArrayDesc.RfMin > Rf) ? Rf : PdwArrayDesc.RfMin;
            PdwArrayDesc.RfMax = (PdwArrayDesc.RfMax < Rf) ? Rf : PdwArrayDesc.RfMax;
            PdwArrayDesc.PwMin = (PdwArrayDesc.PwMin > Pw) ? Pw : PdwArrayDesc.PwMin;
            PdwArrayDesc.PwMax = (PdwArrayDesc.PwMax < Pw) ? Pw : PdwArrayDesc.PwMax;
            if(Toa > PdwArrayDesc.PreToaTmp)
            {
                PdwArrayDesc.ToaReverseCount = 0;
                PdwArrayDesc.ToaErrorCount = 0;
                ToaS = PdwArrayDesc.ToaReverseTimes;
                PdwArrayDesc.PreToaTmp = Toa;
            }
            else
            {
                if((Toa < 0x30000000) && (PdwArrayDesc.PreToaTmp > 0xD0000000))
                {
                    if(PdwArrayDesc.ToaReverseCount > 10)
                    {
                        PdwArrayDesc.ToaReverseCount = 0;
                        PdwArrayDesc.ToaErrorCount = 0;
                        PdwArrayDesc.ToaReverseTimes++;
                        ToaS = PdwArrayDesc.ToaReverseTimes;
                        PdwArrayDesc.PreToaTmp = Toa;
                    }
                    else
                    {
                        ToaS = -1;
                        PdwArrayDesc.ToaReverseCount++;
                    }
                }
                else
                {
                    if(PdwArrayDesc.ToaErrorCount > 10)
                    {
                        PdwArrayDesc.ToaReverseCount = 0;
                        PdwArrayDesc.ToaErrorCount = 0;
                        ToaS = PdwArrayDesc.ToaReverseTimes;
                        PdwArrayDesc.PreToaTmp = Toa;
                    }
                    else
                    {
                        ToaS = -1;
                        PdwArrayDesc.ToaErrorCount++;
                    }
                }
            }
        }
    }
    inline void random()
    {
        float randnum = (float)( rand() & 0xFF );
        Doa   = randnum;
        Pa    = randnum;
        Pw    = randnum;
        Rf    = randnum;
        Toa   = randnum;
        ToaS  = randnum;
    }
    /* @Author: xpt
    * 读取文件前调用！！！
    */
    static void PdwArrayDescClear()
    {
        PdwArrayDesc.clear();
    }
    /* @Author: xpt
    * 1.内联函数避免多次函数调用。
    * 2.__host__ __device__ 主机和设备均可调用该函数。
    * 3.使用引用不需要解释。
    * 4.结构体可以直接相减，在主机可以使用SIMD指令。
    */
#if CUDART_VERSION
    __inline__ __host__ __device__ _Pdw operator -(const _Pdw& PdwTmp) const
#else
    inline _Pdw operator -(const _Pdw& PdwTmp) const
#endif
    {
        _Pdw rtnPdw;
        rtnPdw.Rf   = Rf  - PdwTmp.Rf;
        rtnPdw.Doa  = Doa - PdwTmp.Doa;
        rtnPdw.Pa   = Pa  - PdwTmp.Pa;
        rtnPdw.Pw   = Pw  - PdwTmp.Pw;
        rtnPdw.Toa  = Toa - PdwTmp.Toa;
        rtnPdw.ToaS = ToaS - PdwTmp.ToaS;
        return rtnPdw;
    }
#if CUDART_VERSION
    __inline__ __host__ __device__ _Pdw operator +(const _Pdw& PdwTmp) const
#else
    inline _Pdw operator + (const _Pdw& PdwTmp) const
#endif
    {
        _Pdw rtnPdw;
        rtnPdw.Rf   = Rf  + PdwTmp.Rf;
        rtnPdw.Doa  = Doa + PdwTmp.Doa;
        rtnPdw.Pa   = Pa  + PdwTmp.Pa;
        rtnPdw.Pw   = Pw  + PdwTmp.Pw;
        rtnPdw.Toa  = Toa + PdwTmp.Toa;
        rtnPdw.ToaS = ToaS + PdwTmp.ToaS;
        return rtnPdw;
    }
#if CUDART_VERSION
    __inline__ __host__ __device__ bool operator ==(const _Pdw& PdwTmp) const
#else
    inline bool operator ==(const _Pdw& PdwTmp) const
#endif
    {
        bool EqualSign = true;
        EqualSign = (Rf  == PdwTmp.Rf )?EqualSign:false;
        EqualSign = (Doa == PdwTmp.Doa)?EqualSign:false;
        EqualSign = (Pa  == PdwTmp.Pa )?EqualSign:false;
        EqualSign = (Pw  == PdwTmp.Pw )?EqualSign:false;
        EqualSign = (Toa == PdwTmp.Toa)?EqualSign:false;
        EqualSign = (ToaS == PdwTmp.ToaS)?EqualSign:false;
        return EqualSign;
    }
#if CUDART_VERSION
    __inline__ __host__ __device__ bool operator !=(const _Pdw& PdwTmp) const
#else
    inline bool operator !=(const _Pdw& PdwTmp) const
#endif
    {
        bool EqualSign = true;
        EqualSign = (Rf  != PdwTmp.Rf )?EqualSign:false;
        EqualSign = (Doa != PdwTmp.Doa)?EqualSign:false;
        EqualSign = (Pa  != PdwTmp.Pa )?EqualSign:false;
        EqualSign = (Pw  != PdwTmp.Pw )?EqualSign:false;
        EqualSign = (Toa != PdwTmp.Toa)?EqualSign:false;
        EqualSign = (ToaS != PdwTmp.ToaS)?EqualSign:false;
        return EqualSign;
    }
    /* @Author: xpt
    * 2D坐标
    * 写法着实不太好，可以写个通用纯虚类模版让CUDA类继承并重载
    * 操作运算符，然而为避免太多的文件依赖，考虑到工具库
    * 的使用意图，因此决定遵循stl的原则。
    * 模版可偏特化，使用右移>>加速；
    * 模版线程安全，可以使用OpenMP或者GPU加速。
    */
    template<typename _PdwMap2D>
#if CUDART_VERSION
    __inline__ __host__ __device__ void ToaDoaMap2D(_PdwMap2D &PdwMap2D, float DoaZoom) const
#else
    inline void ToaDoaMap2D(_PdwMap2D &PdwMap2D, float DoaZoom) const
#endif
    {
        PdwMap2D.x = int(Doa * DoaZoom);
        if(ToaS == 0)
        {
            PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit);
        }
        else
        {
            if(PdwMap2D.ToaMin > Toa)
            {
                PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + (ToaS - 1) * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
            else
            {
                PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + ToaS * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
        }
        PdwMap2D.Map2DUnit.r = ((Pa - PdwMap2D.PaMin)/PdwMap2D.PaNormalUnit) > 0xFF ? 0xFF : ((Pa - PdwMap2D.PaMin)/PdwMap2D.PaNormalUnit);
        PdwMap2D.Map2DUnit.b = ((Rf - PdwMap2D.RfMin)/PdwMap2D.RfNormalUnit) > 0xFF ? 0xFF : ((Rf - PdwMap2D.RfMin)/PdwMap2D.RfNormalUnit);
        PdwMap2D.Map2DUnit.g = ((Pw - PdwMap2D.PwMin)/PdwMap2D.PwNormalUnit) > 0xFF ? 0xFF : ((Pw - PdwMap2D.PwMin)/PdwMap2D.PwNormalUnit);
    }
    template<typename _PdwMap2D>
#if CUDART_VERSION
    __inline__ __host__ __device__ void ToaPwMap2D(_PdwMap2D &PdwMap2D) const
#else
    inline void ToaPwMap2D(_PdwMap2D &PdwMap2D) const
#endif
    {
        PdwMap2D.x = int((Pw - PdwMap2D.PwMin)/PdwMap2D.PwNormalUnit);
        if(ToaS == 0)
        {
            PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit);
        }
        else
        {
            if(PdwMap2D.ToaMin > Toa)
            {
                PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + (ToaS - 1) * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
            else
            {
                PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + ToaS * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
        }
        PdwMap2D.Map2DUnit.r = ((Pa - PdwMap2D.PaMin)/PdwMap2D.PaNormalUnit) > 0xFF ? 0xFF : ((Pa - PdwMap2D.PaMin)/PdwMap2D.PaNormalUnit);
        PdwMap2D.Map2DUnit.b = ((Rf - PdwMap2D.RfMin)/PdwMap2D.RfNormalUnit) > 0xFF ? 0xFF : ((Rf - PdwMap2D.RfMin)/PdwMap2D.RfNormalUnit);
        PdwMap2D.Map2DUnit.g = 0;
    }
    template<typename _PdwMap2D>
#if CUDART_VERSION
    __inline__ __host__ __device__ void ToaRfMap2D(_PdwMap2D &PdwMap2D) const
#else
    inline void ToaRfMap2D(_PdwMap2D &PdwMap2D) const
#endif
    {
        PdwMap2D.x = int((Rf - PdwMap2D.RfMin)/PdwMap2D.RfNormalUnit);
        if(ToaS == 0)
        {
            PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit);
        }
        else
        {
            if(PdwMap2D.ToaMin > Toa)
            {
                PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + (ToaS - 1) * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
            else
            {
                PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + ToaS * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
        }
        PdwMap2D.Map2DUnit.r = ((Pa - PdwMap2D.PaMin)/PdwMap2D.PaNormalUnit) > 0xFF ? 0xFF : ((Pa - PdwMap2D.PaMin)/PdwMap2D.PaNormalUnit);
        PdwMap2D.Map2DUnit.b = 0;
        PdwMap2D.Map2DUnit.g = ((Pw - PdwMap2D.PwMin)/PdwMap2D.PwNormalUnit) > 0xFF ? 0xFF : ((Pw - PdwMap2D.PwMin)/PdwMap2D.PwNormalUnit);
    }
    template<typename _PdwMap2D>
#if CUDART_VERSION
    __inline__ __host__ __device__ void ToaPaMap2D(_PdwMap2D &PdwMap2D) const
#else
    inline void ToaPaMap2D(_PdwMap2D &PdwMap2D) const
#endif
    {
        PdwMap2D.x = int((Pa - PdwMap2D.PaMin)/PdwMap2D.PaNormalUnit);
        if(ToaS == 0)
        {
            PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit);
        }
        else
        {
            if(PdwMap2D.ToaMin > Toa)
            {
                PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + (ToaS - 1) * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
            else
            {
                PdwMap2D.y = floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + ToaS * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
        }
        PdwMap2D.Map2DUnit.r = 0;
        PdwMap2D.Map2DUnit.b = ((Rf - PdwMap2D.RfMin)/PdwMap2D.RfNormalUnit) > 0xFF ? 0xFF : ((Rf - PdwMap2D.RfMin)/PdwMap2D.RfNormalUnit);
        PdwMap2D.Map2DUnit.g = ((Pw - PdwMap2D.PwMin)/PdwMap2D.PwNormalUnit) > 0xFF ? 0xFF : ((Pw - PdwMap2D.PwMin)/PdwMap2D.PwNormalUnit);
    }
    /* @Author: xpt
    * 极坐标2D,
    * 默认大小 2050 * 2050,
    * 模版可偏特化，使用右移>>加速；
    * 模版线程安全，可以使用OpenMP或者GPU加速。
    * sin运算可以使用哈希表,GPU可以使用常量内存实现加速。
    */
    template<typename _PdwMap2D>
#if CUDART_VERSION
    __inline__ __host__ __device__ void mapPolar2D(_PdwMap2D &PdwMap2D, float DoaUnit, UINT R0, UINT R1) const
#else
    inline void mapPolar2D(_PdwMap2D &PdwMap2D, float DoaUnit, UINT R0, UINT R1) const
#endif
    {
        float theta = Doa * DoaUnit;
        UINT r = R0;
        if(ToaS == 0)
        {
            r += floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit);
        }
        else
        {
            if(PdwMap2D.ToaMin > Toa)
            {
                r += floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + (ToaS - 1) * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
            else
            {
                r += floor((Toa - PdwMap2D.ToaMin)/PdwMap2D.ToaNormalUnit + ToaS * (0x100000000/PdwMap2D.ToaNormalUnit));
            }
        }
        PdwMap2D.x = int(R1 - r * sin(theta));
        PdwMap2D.y = int(R1 + r * cos(theta));
        PdwMap2D.Map2DUnit.r = uchar((Pa - PdwMap2D.PaMin)/PdwMap2D.PaNormalUnit);
        PdwMap2D.Map2DUnit.b = uchar((Rf - PdwMap2D.RfMin)/PdwMap2D.RfNormalUnit);
        PdwMap2D.Map2DUnit.g = uchar((Pw - PdwMap2D.PwMin)/PdwMap2D.PwNormalUnit);
    }
    
    inline void print() const
    {
        COUTSWIDTH(12, "Rf - ", Rf,
                       "Doa - ", Doa,
                       "Pa - ", Pa,
                       "Pw - ", Pw,
                       "Toa - ", Toa,
                       "ToaS - ", ToaS
                );
    }
};

template<typename _RfTy, typename _PwTy, typename _PaTy, typename _ToaTy, typename _DoaTy>
_PdwArrayDesc<_Pdw<_RfTy, _PwTy, _PaTy, _ToaTy, _DoaTy>> _Pdw<_RfTy, _PwTy, _PaTy, _ToaTy, _DoaTy>::PdwArrayDesc;

template<typename _PdwType>
struct _PdwMemorySwap
{
    _PdwType*   PdwAddr;
    UINT        PdwAnalyCapcity;
    UINT&       PdwSize;
    _PdwType*   PdwBufAddr1;
    _PdwType*   PdwBufAddr2;
    UINT*       Status1;
    UINT*       Status2;
    UINT*       SortNum1;
    UINT*       SortNum2;
    UINT        PdwReadLocal;                                   // 记录当前Pdw缓存池读取位置
    UINT        Sel;
    _PdwMemorySwap():PdwSize(*(new UINT))
    {
        PdwAddr     = NULL;
        PdwBufAddr1 = NULL;
        PdwBufAddr2 = NULL;
        Status1     = NULL;
        Status2     = NULL;
        SortNum1    = NULL;
        SortNum2    = NULL;
        PdwAnalyCapcity = 0;
        PdwReadLocal = 0;
        Sel = 0;
    }
    _PdwMemorySwap(_iRadarSimPro<_PdwType>& iRadarSimPro, _PdwType** PdwAddr_, UINT& PdwSize_):PdwSize(PdwSize_)
    {
        Init(iRadarSimPro);
        PdwAnalyCapcity = 0x1000000;
        PdwAddr  = new _PdwType[PdwAnalyCapcity];
        *PdwAddr_ = PdwAddr;
    }
    ~_PdwMemorySwap()
    {
        PdwBufAddr1 = NULL;
        PdwBufAddr2 = NULL;
        delete[] PdwAddr;
    }
    inline void Init(_iRadarSimPro<_PdwType>& iRadarSimPro)
    {
        Sel          = 1;
        PdwReadLocal = 0;
        PdwBufAddr1 = iRadarSimPro.GetPdwBufAddr1();
        PdwBufAddr2 = iRadarSimPro.GetPdwBufAddr2();
        Status1     = iRadarSimPro.GetPdwStatusAddr1();
        Status2     = iRadarSimPro.GetPdwStatusAddr2();
        SortNum1    = iRadarSimPro.GetSortNumAddr1();
        SortNum2    = iRadarSimPro.GetSortNumAddr2();
    }
    inline void Resize(_PdwType** PdwAddr_, UINT PdwAnalyCapcity_)
    {
        delete[] PdwAddr;
        PdwAnalyCapcity = PdwAnalyCapcity_;
        PdwAddr  = new _PdwType[PdwAnalyCapcity];
        *PdwAddr_ = PdwAddr;
    }
    bool CheckBufferReady()                                                             // 第一次使用先调用这个函数，之后就可以只使用ChangeBuffer了
    {
        switch(Sel)
        {
            case 1:
                while(*Status1 != READY)
                {
                    ;
                }
                if(*Status1 == READY)
                {
                    return true;
                }
                else
                {
                    return false;
                }
                break;
            case 2:
                while(*Status2 != READY)
                {
                    ;
                }
                if(*Status2 == READY)
                {
                    return true;
                }
                else
                {
                    return false;
                }
                break;
            default:
                return false;
        }
    }
    bool ChangeBuffer()
    {
        switch(Sel)
        {
            case 1:
                *Status1 = IDLE;
                while(*Status2 != READY)
                {
                    ;
                }
                if(*Status2 == READY)
                {
                    Sel = 2;
                    PdwReadLocal = 0;
                    return true;
                }
                else
                {
                    return false;
                }
                break;
            case 2:
                *Status2 = IDLE;
                while(*Status1 != READY)
                {
                    ;
                }
                if(*Status1 == READY)
                {
                    Sel = 1;
                    PdwReadLocal = 0;
                    return true;
                }
                else
                {
                    return false;
                }
                break;
            default:
                return false;
        }
    }
    void AddRead(CINT AddTime_ms, CINT ToaUnit_ns)
    {
        _PdwType::PdwArrayDescClear();                          //读取之前清零
        CINT AddTime = AddTime_ms*(1000000/ToaUnit_ns);
        UINT ToaSt = 0, ToaReveseSt = 0;
        bool ExceedTimeTag = false;
        UINT SortNum = (Sel == 1) ? *SortNum1 : *SortNum2;
        _PdwType* PdwBufAddrTmp = (Sel == 1) ? PdwBufAddr1 : PdwBufAddr2;
        UINT i = 0;
        while(!ExceedTimeTag)
        {
            if(PdwReadLocal >= SortNum)
            {
                ChangeBuffer();
                SortNum = (Sel == 1) ? *SortNum1 : *SortNum2;
                PdwBufAddrTmp = (Sel == 1) ? PdwBufAddr1 : PdwBufAddr2;
            }
            PdwAddr[i] = PdwBufAddrTmp[PdwReadLocal++];
            if(_PdwType::PdwArrayDesc.InitTag == 0)
            {
                _PdwType::PdwArrayDesc.InitTag = 1;
                _PdwType::PdwArrayDesc.PaMin = PdwAddr[i].Pa;
                _PdwType::PdwArrayDesc.PaMax = PdwAddr[i].Pa;
                _PdwType::PdwArrayDesc.RfMin = PdwAddr[i].Rf;
                _PdwType::PdwArrayDesc.RfMax = PdwAddr[i].Rf;
                _PdwType::PdwArrayDesc.PwMin = PdwAddr[i].Pw;
                _PdwType::PdwArrayDesc.PwMax = PdwAddr[i].Pw;
                _PdwType::PdwArrayDesc.ToaReverseTimes = 0;
                ToaSt = PdwAddr[i].Toa;
                ToaReveseSt = PdwAddr[i].ToaS;
            }
            else
            {
                _PdwType::PdwArrayDesc.PaMin = (_PdwType::PdwArrayDesc.PaMin > PdwAddr[i].Pa) ? PdwAddr[i].Pa : _PdwType::PdwArrayDesc.PaMin;
                _PdwType::PdwArrayDesc.PaMax = (_PdwType::PdwArrayDesc.PaMax < PdwAddr[i].Pa) ? PdwAddr[i].Pa : _PdwType::PdwArrayDesc.PaMax;
                _PdwType::PdwArrayDesc.RfMin = (_PdwType::PdwArrayDesc.RfMin > PdwAddr[i].Rf) ? PdwAddr[i].Rf : _PdwType::PdwArrayDesc.RfMin;
                _PdwType::PdwArrayDesc.RfMax = (_PdwType::PdwArrayDesc.RfMax < PdwAddr[i].Rf) ? PdwAddr[i].Rf : _PdwType::PdwArrayDesc.RfMax;
                _PdwType::PdwArrayDesc.PwMin = (_PdwType::PdwArrayDesc.PwMin > PdwAddr[i].Pw) ? PdwAddr[i].Pw : _PdwType::PdwArrayDesc.PwMin;
                _PdwType::PdwArrayDesc.PwMax = (_PdwType::PdwArrayDesc.PwMax < PdwAddr[i].Pw) ? PdwAddr[i].Pw : _PdwType::PdwArrayDesc.PwMax;
                _PdwType::PdwArrayDesc.ToaReverseTimes = PdwAddr[i].ToaS - ToaReveseSt;
                if(PdwAddr[i].Toa - ToaSt > AddTime)
                {
                    ExceedTimeTag = true;
                }
            }
            PdwAddr[i++].ToaS = PdwAddr[i].ToaS - ToaReveseSt;
        }
        PdwSize = i;
    }
};

enum _FilterType
{
    FilterRf    = 0,
    FilterDoa   = 1,
    FilterPa    = 2,
    FilterPw    = 3,
    FilterToa   = 4
};

template<typename _PdwTy>
struct _MatPdwArray
{
	typedef typename _PdwTy::RfTy   RfTy;
	typedef typename _PdwTy::PwTy   PwTy;
	typedef typename _PdwTy::PaTy   PaTy;
	typedef typename _PdwTy::ToaTy  ToaTy;
	typedef typename _PdwTy::DoaTy  DoaTy;
	typedef _MatPdwArray<_PdwTy>    _ThisType;

    vector<_PdwTy>  PdwArray;
    vector<UINT>    PdwAnalySign;       // 0: 未分析； 1: 过滤提取

    inline void init()
    {
        PdwArray.resize(0);
        PdwAnalySign.resize(0);
        PdwArray.reserve(0x1000);
        PdwAnalySign.reserve(0x1000);
    }
    inline void clear()
    {
        PdwArray.clear();
        PdwAnalySign.clear();
    }
    _PdwTy& operator[](const int Ind)
    {
        if(Ind < PdwArray.size())
        {
            return PdwArray[Ind];
        }
        else
        {
            ERRORMSG("Index Out of Range!");
            return PdwArray[0];
        }
    }
    inline void Push(_PdwTy& Pdw_)
    {
        PdwArray.push_back(Pdw_);
        PdwAnalySign.push_back(0);
    }
    void Filter(vector<_PdwTy>& OutVec, UINT MinVal, UINT MaxVal, _FilterType FilterType = FilterDoa)
    {
        UINT PdwArraySize = PdwArray.size();
        switch(FilterType)
        {
        case FilterDoa:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Doa >= MinVal) && (PdwArray[i].Doa <= MaxVal))
                {
                    OutVec.push_back(PdwArray[i]);
                }
            }
            break;
        case FilterRf:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Rf >= MinVal) && (PdwArray[i].Rf <= MaxVal))
                {
                    OutVec.push_back(PdwArray[i]);
                }
            }
            break;
        case FilterPa:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Pa >= MinVal) && (PdwArray[i].Pa <= MaxVal))
                {
                    OutVec.push_back(PdwArray[i]);
                }
            }
            break;
        case FilterPw:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Pw >= MinVal) && (PdwArray[i].Pw <= MaxVal))
                {
                    OutVec.push_back(PdwArray[i]);
                }
            }
            break;
        case FilterToa:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Toa >= MinVal) && (PdwArray[i].Toa <= MaxVal))
                {
                    OutVec.push_back(PdwArray[i]);
                }
            }
            break;
        default:
            break;
        }
    }
    void FilterByAnalySign(vector<_PdwTy>& OutVec, UINT MinVal, UINT MaxVal, _FilterType FilterType = FilterDoa)
    {
        UINT PdwArraySize = PdwArray.size();
        switch(FilterType)
        {
        case FilterDoa:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if(PdwAnalySign[i] == 0)
                {
                    if((PdwArray[i].Doa >= MinVal) && (PdwArray[i].Doa <= MaxVal))
                    {
                        PdwAnalySign[i] = 1;
                        OutVec.push_back(PdwArray[i]);
                    }
                }
            }
            break;
        case FilterRf:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if(PdwAnalySign[i] == 0)
                {
                    if((PdwArray[i].Rf >= MinVal) && (PdwArray[i].Rf <= MaxVal))
                    {
                        PdwAnalySign[i] = 1;
                        OutVec.push_back(PdwArray[i]);
                    }
                }
            }
            break;
        case FilterPa:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if(PdwAnalySign[i] == 0)
                {
                    if((PdwArray[i].Pa >= MinVal) && (PdwArray[i].Pa <= MaxVal))
                    {
                        PdwAnalySign[i] = 1;
                        OutVec.push_back(PdwArray[i]);
                    }
                }
            }
            break;
        case FilterPw:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if(PdwAnalySign[i] == 0)
                {
                    if((PdwArray[i].Pw >= MinVal) && (PdwArray[i].Pw <= MaxVal))
                    {
                        PdwAnalySign[i] = 1;
                        OutVec.push_back(PdwArray[i]);
                    }
                }
            }
            break;
        case FilterToa:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if(PdwAnalySign[i] == 0)
                {
                    if((PdwArray[i].Toa >= MinVal) && (PdwArray[i].Toa <= MaxVal))
                    {
                        PdwAnalySign[i] = 1;
                        OutVec.push_back(PdwArray[i]);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    void print(UINT MinVal, UINT MaxVal, _FilterType FilterType = FilterDoa) const
    {
        UINT PdwArraySize = PdwArray.size();
        switch(FilterType)
        {
        case FilterDoa:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Doa >= MinVal) && (PdwArray[i].Doa <= MaxVal))
                {
                    PdwArray[i].print();
                }
            }
            break;
        case FilterRf:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Rf >= MinVal) && (PdwArray[i].Rf <= MaxVal))
                {
                    PdwArray[i].print();
                }
            }
            break;
        case FilterPa:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Pa >= MinVal) && (PdwArray[i].Pa <= MaxVal))
                {
                    PdwArray[i].print();
                }
            }
            break;
        case FilterPw:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Pw >= MinVal) && (PdwArray[i].Pw <= MaxVal))
                {
                    PdwArray[i].print();
                }
            }
            break;
        case FilterToa:
            for(UINT i = 0; i < PdwArraySize; i++)
            {
                if((PdwArray[i].Toa >= MinVal) && (PdwArray[i].Toa <= MaxVal))
                {
                    PdwArray[i].print();
                }
            }
            break;
        default:
            break;
        }
    }
};

template<typename _PdwTy>
struct _MatPdwMatrix
{
	typedef typename _PdwTy::RfTy   RfTy;
	typedef typename _PdwTy::PwTy   PwTy;
	typedef typename _PdwTy::PaTy   PaTy;
	typedef typename _PdwTy::ToaTy  ToaTy;
	typedef typename _PdwTy::DoaTy  DoaTy;
	typedef _MatPdwMatrix<_PdwTy>   _ThisType;

    vector<_MatPdwArray<_PdwTy> >   PdwMatrix;
    UINT                            PdwMatrixSize;
    UINT                            StInd;
    _FilterType                     FilterType;

    _MatPdwMatrix(UINT Lens = 0, _FilterType FilterType_ = FilterDoa)
    {
        init(Lens, FilterDoa);
    }
    inline void init(UINT Lens, _FilterType FilterType_ = FilterDoa)
    {
        FilterType = FilterType_;
        PdwMatrix.resize(Lens);
        PdwMatrixSize = Lens;
        StInd = 0;
        for(int i = 0; i < PdwMatrixSize; i++)
        {
            PdwMatrix[i].init();
        }
    }
    inline void clear()
    {
        StInd = 0;
        for(int i = 0; i < PdwMatrixSize; i++)
        {
            PdwMatrix[i].clear();
        }
    }
    inline _MatPdwArray<_PdwTy>& operator[](const int Ind)
    {
        if(StInd + Ind < PdwMatrixSize)
        {
            return PdwMatrix[StInd + Ind];
        }
        else if((StInd + Ind - PdwMatrixSize < PdwMatrixSize) && (Ind < PdwMatrixSize))
        {
            return PdwMatrix[StInd + Ind - PdwMatrixSize];
        }
        else
        {
            ERRORMSG("Index Value Out of Range!");
            return PdwMatrix[0];
        }
    }
    void Translation(int Offset)
    {
        if(StInd + Offset < PdwMatrixSize)
        {
            for(UINT i = StInd; i < StInd + Offset; i++)
            {
                PdwMatrix[i].clear();
            }
            StInd = StInd + Offset;
        }
        else if(StInd + Offset - PdwMatrixSize < PdwMatrixSize)
        {
            for(UINT i = StInd; i < PdwMatrixSize; i++)
            {
                PdwMatrix[i].clear();
            }
            for(UINT i = 0; i < StInd + Offset - PdwMatrixSize; i++)
            {
                PdwMatrix[i].clear();
            }
            StInd = StInd + Offset - PdwMatrixSize;
        }
        else
        {
            ERRORMSG("Offset Value Out of Range!");
        }
    }
    void Filter(vector<_PdwTy>& OutVec, UINT st, UINT ed, UINT FilterMinVal, UINT FilterMaxVal)
    {
        for(UINT i = st; i < ed; i++)
        {
            (*this)[i].Filter(OutVec, FilterMinVal, FilterMaxVal, FilterType);
        }
    }
    void print(int Type = 0)       // -1:dont showPdw; 0:showPdw;
    {
        for(UINT i = 0; i < PdwMatrixSize; i++)
        {
            UINT PdwArrSize = (*this)[i].PdwArray.size();
            if(PdwArrSize != 0)
            {
                COUT(i, PdwArrSize);
            }
            if(Type != -1)
            {
                for(UINT j = 0; j < PdwArrSize; j++)
                {
                    (*this)[i][j].print();
                }
            }
        }
    }
};

template<class PdwTy>
void PrintPdw(PdwTy *PdwArray, int st, int ed)
{
    for(int i = st; i < ed; i++)
    {
        PdwArray[i].print();
    }
}

template<class PdwTy>
void CheckPdw(PdwTy *PdwArray, int st, int ed)
{
    for(int i = st; i < ed; i++)
    {
        static typename PdwTy::ToaTy Pri = 0;
        Pri = (Pri == 0) ? PdwArray[i+1].Toa - PdwArray[i].Toa : Pri;
        if(PdwArray[i+1].Toa - PdwArray[i].Toa != Pri)
        {
            COUT(i, PdwArray[i].Toa, PdwArray[i+1].Toa);
        }
    }
}

template<class PdwTy>
void PrintPdw(vector<PdwTy>& PdwArray, int st, int ed)
{
    if((st < ed)&&(ed < PdwArray.size()))
    {
        for(int i = st; i < ed; i++)
        {
            PdwArray[i].print();
        }
    }
}

template<class PdwTy>
void PrintPdw(vector<PdwTy>& PdwArray)
{
    COUTSECBEG("PrintPdw");
    UINT PdwArraySize = PdwArray.size();
    COUT(PdwArraySize);
    for(int i = 0; i < PdwArraySize; i++)
    {
        PdwArray[i].print();
    }
    COUTSECEND("PrintPdw");
}
#endif // !PDW_STRUCT_