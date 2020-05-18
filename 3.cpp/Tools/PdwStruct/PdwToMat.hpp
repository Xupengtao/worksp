/*
 * @Author: xpt 
 * @Date: 2020-01-11 15:59:32 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-05-16 02:31:13
 * 
 * @idea note
 * 2020.01.17 02:57 动态生成视频 左图片 右数据和映射位置及B、G、R值，用虚线将BGR Circle连接至映射位置 (*)
 */
#ifndef _PDWTOMAT_HPP
#define _PDWTOMAT_HPP
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
#include <boost/progress.hpp>
#include "../PdwStruct/PdwStruct.hpp"
#include "../fileStream/binaryFileStream.hpp"
#include "../imageTools/cvTextImage.hpp"
#include "../imageTools/cvCoordinateMap.hpp"
#include "../imageTools/CvTools.hpp"
#include "../Timeit.h"
#include "../CircularQueue.hpp"

inline UCHAR ucharCycleSub(UCHAR x, UCHAR y)
{
    return(x > y) ? (x - y) : (0x100 + x - y);
}

struct _TargetLoc
{
    USHORT          Row;                            // 行号
    USHORT          Col;                            // 列号
    USHORT          RowMin;                         // 行号
    USHORT          ColMin;                         // 列号
    USHORT          RowMax;                         // 行号
    USHORT          ColMax;                         // 列号
    UCHAR           TargetNo;                       // 目标号
    UCHAR           FindAtFrameCircularNo;          // 激活时运行帧号
    USHORT          PulseNums;                      // 累计脉冲数

    _TargetLoc()
    {
        Init();
    }
    virtual ~_TargetLoc()
    {
        ;
    }
    void Init()
    {
        Clear();
    }
    inline void Clear()
    {
        Row         = 0;
        Col         = 0;
        RowMin      = 0;
        ColMin      = 0;
        RowMax      = 0;
        ColMax      = 0;
        TargetNo    = 0;
        PulseNums   = 0;
        FindAtFrameCircularNo = 0;
    }
    inline void ClearPulseNums()
    {
        PulseNums = 0;
    }
    inline bool IsAtTargetTlrcArea(USHORT Row_, CUSHORT RowTlrc,  USHORT Col_,CUSHORT ColTlrc, UCHAR FrameCircularNo, CUCHAR FrameCircularNoTlrc)
    {
        if((abs(Row_ - Row) <= RowTlrc) && (abs(Col_ - Col) <= ColTlrc) && (ucharCycleSub(FrameCircularNo , FindAtFrameCircularNo) <= FrameCircularNoTlrc))
        {
            ASSERT(FrameCircularNo != 0, "ASSERT Failed!");
            FindAtFrameCircularNo = FrameCircularNo;
            RowMax = (Row_ > RowMax) ? Row_ : RowMax;
            ColMax = (Col_ > ColMax) ? Col_ : ColMax;
            RowMin = (Row_ < RowMin) ? Row_ : RowMin;
            ColMin = (Col_ < ColMin) ? Col_ : ColMin;
            return true;
        }
        else
        {
            return false;
        }
    }
};
class _PdwRfPwMap
{
    enum _PixelChannel
    {
        FrameNoCh   = 0,
        TargetNoCh  = 1,
        ActiveNoCh  = 2
    };
    static CUINT                    RF_AS_ROW               = 0xFFF;        // RfPwMat行数
    static CUINT                    PW_AS_COL               = 0xFFF;        // RfPwMat列数
    static CUINT                    RFROW_TOLERANCE         = 12;           // RfRow容差
    static CUINT                    PWCOL_TOLERANCE         = 20;           // PwCol容差
    static CUINT                    DOA_TOLERANCE           = 10;           // Doa容差范围
    static CUINT                    PIXEL_THRESHOLD         = 20;           // RfPwMat像素点激活阈值
    static CUINT                    PIXEL_ACTIVE_VALUE      = 0xFF;         // RfPwMat像素点已激活值
    static CUINT                    DISAPPERTIME_THRESHOLD  = 1000;         // RfPwMat由于累计未激活而导致Doa丢失阈值
    static CUINT                    FRAMENO_DIFF_THRESHOLD  = 100;          // 像素值帧号容差范围
    static CUINT                    NEW_TARGETNOS_FRAMESIZE = 20;           // 帧新目标号Queue长度
    static CUINT                    TARGET_LOCVEC_MAXSIZE   = 0x100;        // 目标号-RfPwMat位置索引Vec最大长度
    static CUINT                    PDW_TARGETVEC_MAXSIZE   = 100000;       // PDW-目标号索引Vec最大长度
    _CvTools                        CvTools;                                // Cv算法封装类对象
    UCHAR*                          pFrameCircularNo;                       // Read Only, 循环累加运行帧号指针
    bool                            ValidSign;                              // RfPwMat有效标志，若无效，代表可回收
    bool                            ActiveSign;                             // 当前帧RfPwMat激活标记
    Mat                             RfPwMat;                                // RfPw映射图(4通道, 通道0:目标号; 通道1:目标激活前为脉冲数,激活后为0xFF,激活阈值为PIXEL_THRESHOLD,通道2:;通道3:循环运行帧号)
    UINT                            DoaSt;                                  // RfPwMat Doa起始值，每帧进行动态调整
    UINT                            DoaEd;                                  // RfPwMat Doa末尾值，每帧进行动态调整
    UINT                            DisappearTimes;                         // RfPwMat累计未激活次数, RfPwMat丢失标记阈值DISAPPERTIME_THRESHOLD
    UCHAR                           TargetNums;                             // 目标数, 1 - 255
    vector<UCHAR>                   NewTargetNoVec;                         // 当前帧新目标号Vec
    _CircularQueue<vector<UCHAR> >  NewTargetNosQueue;                      // 帧新目标号Queue
    vector<_TargetLoc>              TargetLocVec;                           // 目标号-RfPwMat位置索引Vec, 最大长度为TARGET_LOCVEC_MAXSIZE
    vector<UCHAR>                   PdwTargetVec;                           // PDW-目标号索引Vec, 最大长度为PDW_TARGETVEC_MAXSIZE
public:
    _PdwRfPwMap():RfPwMat(RF_AS_ROW, PW_AS_COL, CV_8UC4), NewTargetNosQueue(NEW_TARGETNOS_FRAMESIZE)
    {
        pFrameCircularNo = NULL;
        Init();
    }
    virtual ~_PdwRfPwMap()
    {
        pFrameCircularNo = NULL;
    }
    void SetFrameCircularNoAddr(UCHAR* pFrameCircularNo_)
    {
        pFrameCircularNo = pFrameCircularNo_;
    }
    void Init()
    {
        NewTargetNoVec.reserve(TARGET_LOCVEC_MAXSIZE);
        TargetLocVec.reserve(TARGET_LOCVEC_MAXSIZE);
        PdwTargetVec.reserve(PDW_TARGETVEC_MAXSIZE);
        Clear();
    }
    inline void Clear()
    {
        ValidSign       = false;
        ActiveSign      = false;
        DoaSt           = 0;
        DoaEd           = 0;
        DisappearTimes  = 0;
        TargetNums      = 0;
        RfPwMat.setTo(Scalar(0, 0, 0, 0));
        NewTargetNoVec.clear();
        NewTargetNosQueue.clear();
        TargetLocVec.clear();
        TargetLocVec.push_back(_TargetLoc());
        PdwTargetVec.clear();
    }
    inline UINT GetDoaSt() const {return DoaSt;}
    inline UINT GetDoaEd() const {return DoaEd;}
    inline bool GetValidSign()  const {return ValidSign;}
    inline bool GetActiveSign() const {return ActiveSign;}
    inline void ClearPdwTargetVec()                                                     // PdwVec映射前清空上一帧PdwTargetVec缓存和ActiveSign
    {
        ActiveSign = false;
        for(UINT i = 0; i < TargetLocVec.size(); i++)
        {
            TargetLocVec[i].ClearPulseNums();
        }
        PdwTargetVec.clear();
    }
    void drawTargetDescribed()
    {
        _CvTools CvTools;
        Mat      srcMat(1080 , 1920, CV_8UC3, Scalar(255, 255, 255));
        Mat      TargetDescribedMat = cv::imread("/home/admin/WorkSp/3.cpp/Tools/Imagesrc/TargetDescribed.jpg");
        UCHAR    IndTmp = 0, TargetNums = 0;
        UINT     FrameNewTargetNums = 0;
        UINT     drawRow = 100, drawCol = 100;
        for(UINT i = 0; i < NewTargetNosQueue.GetSize(); i++)
        {
            FrameNewTargetNums  = NewTargetNosQueue[i].size();
            for(UINT j = 0; j < FrameNewTargetNums; j++)
            {
                if(TargetNums > 10)
                {
                    break;
                }
                IndTmp = NewTargetNosQueue[i][j];
                Mat srcRectMat = srcMat(Rect(drawCol + 350 * (TargetNums % 5), drawRow + 250 * (TargetNums / 5),
                                             TargetDescribedMat.cols, TargetDescribedMat.rows));
                CvTools.cvAddWeighted(TargetDescribedMat, srcRectMat, srcRectMat, 0.9);
                string Str = tostring(UINT(IndTmp));                                // TargetNo
                cv::putText(srcRectMat, Str, Point(152, 33),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 1.2,
                            Scalar(100, 100, 100), 2.0, LINE_AA, false);
                Str = tostring(TargetLocVec[IndTmp].Row << 4);                      // Rf
                cv::putText(srcRectMat, Str, Point(185, 55),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7,
                            Scalar(50, 50, 50), 1.0, LINE_AA, false);
                Str = tostring(TargetLocVec[IndTmp].Col << 4);                      // Pw
                cv::putText(srcRectMat, Str, Point(185, 80),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7,
                            Scalar(50, 50, 50), 1.0, LINE_AA, false);
                Str = tostring(DoaSt);                                              // Doa
                cv::putText(srcRectMat, Str, Point(185, 105),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7,
                            Scalar(50, 50, 50), 1.0, LINE_AA, false);
                Str = tostring(TargetLocVec[IndTmp].PulseNums);                     // Pulse Density
                cv::putText(srcRectMat, Str, Point(185, 130),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7,
                            Scalar(50, 50, 50), 1.0, LINE_AA, false);
                Str = tostring(UINT(TargetLocVec[IndTmp].FindAtFrameCircularNo));   // Timestamp
                cv::putText(srcRectMat, Str, Point(185, 155),
                            cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7,
                            Scalar(50, 50, 50), 1.0, LINE_AA, false);
                TargetNums++;
            }
        }
        cv::imshow("Target", srcMat);
        cv::waitKey();
    }
    inline void NewFrameInit()                                                          // 帧起始初始化操作
    {
        NewTargetNoVec.clear();
        ClearPdwTargetVec();
    }
    inline bool DoaAdapt(UINT DoaSt_, UINT DoaEd_)                                      // Doa适配检测,!表示DoaSt_和DoaEd_,|表示DoaSt和DoaEd
    {
        bool RtnBool = false;
        if(ValidSign == true)
        {
            if(DoaEd_ <= DoaSt)                                             // ! ! | |
            {
                if(DoaSt - DoaSt_ < DOA_TOLERANCE)
                {
                    RtnBool = true;
                }
                else
                {
                    RtnBool = false;
                }
            }                                                               // 以下均默认为(DoaEd_ > DoaSt)
            else if((DoaSt_ <= DoaSt) && (DoaEd_ <= DoaEd))                 // ! | ! |
            {
                if(DoaEd_ - DoaSt > DoaSt - DoaSt_)
                {
                    RtnBool = true;
                }
                else
                {
                    RtnBool = false;
                }
            }
            else if((DoaSt_ <= DoaSt) && (DoaEd_ > DoaEd))                  // ! | | !
            {
                RtnBool = true;
            }                                                               // 以下均默认为(DoaSt_ > DoaSt)
            else if((DoaSt_ <= DoaEd) && (DoaEd_ <= DoaEd))                 // | ! ! |  由于DoaSt_ <= DoaEd_, 所以DoaSt_ <= DoaEd
            {
                RtnBool = true;
            }
            else if((DoaSt_ <= DoaEd) && (DoaEd_ > DoaEd))                  // | ! | !
            {
                if(DoaEd - DoaSt_ > DoaEd_ - DoaEd)
                {
                    RtnBool = true;
                }
                else
                {
                    RtnBool = false;
                }
            }                                                               // 以下均默认为(DoaSt_ > DoaEd)
            else                                                            // | | ! !
            {
                if(DoaEd_ - DoaEd < DOA_TOLERANCE)
                {
                    RtnBool = true;
                }
                else
                {
                    RtnBool = false;
                }
            }
        }
        else
        {
            RtnBool = false;
        }
        if(RtnBool == true)
        {
            DoaSt = DoaSt_;
            DoaEd = DoaEd_;
        }
        return RtnBool;
    }
    inline bool CanCreateOrRecycle(UINT DoaSt_, UINT DoaEd_)                            // 是否未使用或者可以回收当前对象数据用于创建新的RfPwMat
    {
        if(ValidSign == false)
        {
            Clear();
            ValidSign = true;
            DoaSt = DoaSt_;
            DoaEd = DoaEd_;
            return true;
        }
        else
        {
            return false;
        }
    }
    template<typename _PdwType>
    void PdwVecMap(vector<_PdwType>& srcPdwVec)
    {
        if(CvTools.GetMatChannels(RfPwMat) != 4)
        {
            ERRORMSG("Check srcPdwVec`s channels!");
            return;
        }
        cv::Vec4b PixelTmp;
        UINT    PdwSize = srcPdwVec.size();
        UINT    RfRow = 0, PwCol = 0;
        UCHAR   TargetNoTmp = 0;
        for(UINT i = 0; i < PdwSize; i++)
        {
            if((srcPdwVec[i].Rf > 0xFFFF) || (srcPdwVec[i].Pw > 0xFFFF))
            {
                continue;
            }
            RfRow       = srcPdwVec[i].Rf >> 4;
            PwCol       = srcPdwVec[i].Pw >> 4;
            PixelTmp    = RfPwMat.at<cv::Vec4b>(RfRow, PwCol);
            if(PixelTmp[TargetNoCh] != 0)
            {
                TargetNoTmp = 0xFF - PixelTmp[TargetNoCh];
                if(TargetLocVec[TargetNoTmp].IsAtTargetTlrcArea(RfRow, RFROW_TOLERANCE, PwCol, PWCOL_TOLERANCE, *pFrameCircularNo, FRAMENO_DIFF_THRESHOLD))
                {
                    if(PixelTmp[FrameNoCh] != *pFrameCircularNo)
                    {
                        ASSERT(TargetNoTmp == TargetLocVec[TargetNoTmp].TargetNo, "assert failed!");
                        CvTools.drawRectAreaAtMatChannel(RfPwMat, TargetLocVec[TargetNoTmp].Row, RFROW_TOLERANCE, TargetLocVec[TargetNoTmp].Col, PWCOL_TOLERANCE, *pFrameCircularNo, FrameNoCh);
                    }
                    TargetProcess(TargetNoTmp);
                }
                else
                {
                    TargetLocVec[TargetNoTmp].IsAtTargetTlrcArea(RfRow, RFROW_TOLERANCE, PwCol, PWCOL_TOLERANCE, *pFrameCircularNo, FRAMENO_DIFF_THRESHOLD);
                    COUTSWIDTH(5,   "TargetNoTmp: "               , UINT(TargetNoTmp),
                                    ", RfRow: "                   , RfRow,
                                    ", TargetRow: "               , TargetLocVec[TargetNoTmp].Row,
                                    ", PwCol: "                   , PwCol,
                                    ", TargetCol: "               , TargetLocVec[TargetNoTmp].Col,
                                    ", FrameCircularNo: "         , UINT(*pFrameCircularNo),
                                    ", FindAtFrameCircularNo: "   , UINT(TargetLocVec[TargetNoTmp].FindAtFrameCircularNo));
                }
                continue;
            }
            else                                                                                                // 尚未创建目标号
            {
                if(PixelTmp[FrameNoCh] == *pFrameCircularNo)
                {
                    if(PixelTmp[ActiveNoCh] == PIXEL_ACTIVE_VALUE)                                              // RfPwPixel已激活
                    {
                        TargetNoTmp = NewTarget(RfRow, PwCol);
                        if(TargetNoTmp == 0)
                        {
                            COUTS("Cannot create new Target!");
                            return;
                        }
                        PixelTmp[TargetNoCh] = 0xFF - TargetNoTmp;                                              // 创建新的目标号
                        PixelTmp[FrameNoCh] = *pFrameCircularNo;
                        CvTools.drawRectAreaAtMatChannel(RfPwMat, RfRow, RFROW_TOLERANCE, PwCol, PWCOL_TOLERANCE, PixelTmp[TargetNoCh], TargetNoCh);
                        CvTools.drawRectAreaAtMatChannel(RfPwMat, RfRow, RFROW_TOLERANCE, PwCol, PWCOL_TOLERANCE, *pFrameCircularNo, FrameNoCh);
                        TargetProcess(TargetNoTmp);
                    }
                    else
                    {
                        PixelTmp[ActiveNoCh]++;
                        if(PixelTmp[ActiveNoCh] > PIXEL_THRESHOLD)
                        {
                            PixelTmp[ActiveNoCh] = PIXEL_ACTIVE_VALUE;
                        }
                    }
                }
                else
                {
                    PixelTmp[FrameNoCh] = *pFrameCircularNo;
                    PixelTmp[ActiveNoCh] = 1;
                }
                RfPwMat.at<cv::Vec4b>(RfRow, PwCol) = PixelTmp;
            }
        }
    }
    inline UCHAR NewTarget(UINT RfRow, UINT PwCol)
    {
        UCHAR NewTargetNo = 0;
        if(TargetNums == 0xFF)
        {
            for(UINT i = 1; i < TargetNums; i++)
            {
                if(ucharCycleSub(*pFrameCircularNo, TargetLocVec[i].FindAtFrameCircularNo) > FRAMENO_DIFF_THRESHOLD)
                {
                    NewTargetNo = i;
                    NewTargetNoVec.push_back(NewTargetNo);
                    break;
                }
            }
        }
        else
        {
            TargetNums++;
            NewTargetNo = TargetNums;
            NewTargetNoVec.push_back(NewTargetNo);
            TargetLocVec.push_back(_TargetLoc());
        }
        TargetLocVec[NewTargetNo].Row       = RfRow;
        TargetLocVec[NewTargetNo].Col       = PwCol;
        TargetLocVec[NewTargetNo].RowMin    = RfRow;
        TargetLocVec[NewTargetNo].RowMax    = RfRow;
        TargetLocVec[NewTargetNo].ColMin    = PwCol;
        TargetLocVec[NewTargetNo].ColMax    = PwCol;
        TargetLocVec[NewTargetNo].ColMax    = PwCol;
        TargetLocVec[NewTargetNo].TargetNo  = NewTargetNo;
        TargetLocVec[NewTargetNo].PulseNums = 1;
        TargetLocVec[NewTargetNo].FindAtFrameCircularNo = *pFrameCircularNo;
        return NewTargetNo;
    }
    inline void TargetProcess(UCHAR TargetNo)                                               // 脉冲目标号操作
    {
        ASSERT((TargetNo != 0), "assert failed!");
        ASSERT((TargetLocVec[TargetNo].TargetNo == TargetNo), "assert failed!");
        ActiveSign = true;
        TargetLocVec[TargetNo].PulseNums++;
        PdwTargetVec.push_back(TargetNo);
    }
    template<CINT FontFace = cv::FONT_HERSHEY_COMPLEX_SMALL>
    inline UINT EndFrameProcess(_cvTextImage<FontFace>& AnalysisTextMat)                    // 帧结束处理
    {
        UINT    rtnNewTargetNums   = 0;                                 // 返回最近NEW_TARGETNOS_FRAMESIZE帧新目标总数
        UINT    FrameNewTargetNums = 0;
        UCHAR   IndTmp = 0;
        NewTargetNosQueue.Push(NewTargetNoVec);
        if(ValidSign == true)
        {
            DisappearTimes = (ActiveSign == false) ? (DisappearTimes + 1) : 0;
            if(DisappearTimes > DISAPPERTIME_THRESHOLD)
            {
                ValidSign       = false;
                DisappearTimes  = 0;
            }
        }
        else
        {
            ActiveSign      = false;
            DisappearTimes  = 0;
        }
        if(ActiveSign == true)
        {
            for(UINT i = 0; i < NewTargetNosQueue.GetSize(); i++)
            {
                FrameNewTargetNums  = NewTargetNosQueue[i].size();
                rtnNewTargetNums    += FrameNewTargetNums;
                for(UINT j = 0; j < FrameNewTargetNums; j++)
                {
                    IndTmp = NewTargetNosQueue[i][j];
                    string Str = "New Target(Rf : " + tostring(TargetLocVec[IndTmp].Row << 4) + 
                                 ", " + "Pw : " + tostring(TargetLocVec[IndTmp].Col << 4) +")";
                    AnalysisTextMat.BGRCircleText(Scalar(TargetLocVec[IndTmp].Row/16, TargetLocVec[IndTmp].Col/16, 255), Str);
                }
            }
            // drawTargetDescribed();
        }
        return rtnNewTargetNums;
    }
    inline void showRfPwMat(UINT ZoomLevel, string Str = "")
    {
        if(ValidSign == true)
        {
            if(Str == "")
            {
                string Str = "RfPwMat(Doa : " + tostring(DoaSt) + " - " + tostring(DoaEd) + ")";
            }
            Mat pyrDownRfPwMat;
            switch(ZoomLevel)
            {
            case 0:
                cv::imshow(Str, RfPwMat);
                break;
            case 1:
                cv::pyrDown(RfPwMat, pyrDownRfPwMat);
                cv::imshow(Str, pyrDownRfPwMat);
                break;
            case 2:
                cv::pyrDown(RfPwMat, pyrDownRfPwMat);
                cv::pyrDown(pyrDownRfPwMat, pyrDownRfPwMat);
                cv::imshow(Str, pyrDownRfPwMat);
                break;
            default:
                cv::imshow(Str, RfPwMat);
                break;
            }
        }
    }
};
class _PdwRfPwMapClt
{
    static CUINT                    NEW_TARGETNOS_FRAMESIZE = 20;           // 帧新目标号Queue长度
    static CUINT                    PDW_RFPWMAPVEC_SIZE     = 100;
    _CvTools                        CvTools;
    Mat                             NewDoaTargetMat;
    Mat                             NewDiscoverTargetMat;
    UCHAR                           FrameCircularNo;                        // 运行帧号循环累加, 跳过0
    vector<_PdwRfPwMap>             PdwRfPwMapVec;
    vector<UCHAR>                   NewDoaTargetNoVec;                      // 当前帧新目标号Vec
    _CircularQueue<vector<UCHAR> >  NewDoaTargetNosQueue;                   // 帧新目标号Queue
    vector<bool>                    PdwRfPwMapValidVec;
public:
    _PdwRfPwMapClt():NewDoaTargetNosQueue(NEW_TARGETNOS_FRAMESIZE)
    {
        NewDoaTargetMat      = cv::imread("/home/admin/WorkSp/3.cpp/Tools/Imagesrc/NewDoaTarget.jpg");
        NewDiscoverTargetMat = cv::imread("/home/admin/WorkSp/3.cpp/Tools/Imagesrc/FindNewTarget.jpg");
        Init();
    }
    virtual ~_PdwRfPwMapClt()
    {
        ;
    }
    void Init()
    {
        PdwRfPwMapVec.resize(PDW_RFPWMAPVEC_SIZE);
        for(UINT i = 0; i < PDW_RFPWMAPVEC_SIZE; i++)
        {
            PdwRfPwMapVec[i].SetFrameCircularNoAddr(&FrameCircularNo);
        }
        NewDoaTargetNoVec.reserve(PDW_RFPWMAPVEC_SIZE);
        PdwRfPwMapValidVec.resize(PDW_RFPWMAPVEC_SIZE, false);
        Clear();
    }
    inline void Clear()
    {
        FrameCircularNo = 1;
        for(UINT i = 0; i < PDW_RFPWMAPVEC_SIZE; i++)
        {
            PdwRfPwMapVec[i].Clear();
        }
        PdwRfPwMapValidVec.assign(PDW_RFPWMAPVEC_SIZE, false);
        NewDoaTargetNoVec.clear();
        NewDoaTargetNosQueue.clear();
    }
    void drawNewDoaTarget(Mat& srcMat, Mat& srcNewTargetMat, UINT row, UINT col)
    {
        UINT drawRow = row, drawCol = (col > 12) ? (col - 12) : 0;
        if((drawRow + srcNewTargetMat.rows > srcMat.rows) || (drawCol + srcNewTargetMat.cols > srcMat.cols))
        {
            ERRORMSG("(drawRow + srcNewTargetMat.rows > srcMat.rows) || (drawCol + srcNewTargetMat.cols > srcMat.cols)");
            return;
        }
        Mat srcRectMat = srcMat(Rect(drawCol, drawRow, srcNewTargetMat.cols, srcNewTargetMat.rows));
        CvTools.cvAddWeighted(srcNewTargetMat, srcRectMat, srcRectMat, 0.9);
    }
    void drawNewDiscoverTarget(Mat& srcMat, Mat& srcNewTargetMat, UINT row, UINT col, UINT DiscoverDoa, UINT FindNewTargetNums)
    {
        UINT drawRow = row, drawCol = (col > 20) ? (col - 20) : 0;
        if((drawRow + srcNewTargetMat.rows > srcMat.rows) || (drawCol + srcNewTargetMat.cols > srcMat.cols))
        {
            ERRORMSG("(drawRow + srcNewTargetMat.rows > srcMat.rows) || (drawCol + srcNewTargetMat.cols > srcMat.cols)");
            return;
        }
        Mat srcRectMat = srcMat(Rect(drawCol, drawRow, srcNewTargetMat.cols, srcNewTargetMat.rows));
        CvTools.cvAddWeighted(srcNewTargetMat, srcRectMat, srcRectMat, 0.9);
        string Str = tostring(DiscoverDoa);
        cv::putText(srcRectMat, Str, Point(191, 136),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.9,
                    Scalar(255, 255, 255), 1.5, LINE_AA, false);
        Str = tostring(FindNewTargetNums);
        cv::putText(srcRectMat, Str, Point(250, 160),
                    cv::FONT_HERSHEY_COMPLEX_SMALL, 0.9,
                    Scalar(255, 255, 255), 1.5, LINE_AA, false);
    }
    inline void NewFrameInit()                                                              // 帧起始初始化操作
    {
        NewDoaTargetNoVec.clear();
        for (UINT i = 0; i < PDW_RFPWMAPVEC_SIZE; i++)
        {
            PdwRfPwMapVec[i].NewFrameInit();
        }
    }
    UINT VecDoaAdaptInd(UINT DoaSt, UINT DoaEd)                                             // Doa适配
    {
        bool FindSign = false;
        UINT i;
        for (i = 0; i < PDW_RFPWMAPVEC_SIZE; i++)
        {
            FindSign = PdwRfPwMapVec[i].DoaAdapt(DoaSt, DoaEd);
            if(FindSign == true)
            {
                break;
            }
        }
        if(FindSign == false)
        {
            i = NewRfPwMap(DoaSt, DoaEd);
        }
        return i;
    }
    inline UINT NewRfPwMap(UINT DoaSt, UINT DoaEd)
    {
        bool NewSign = false;
        UINT i;
        for (i = 0; i < PDW_RFPWMAPVEC_SIZE; i++)
        {
            NewSign = PdwRfPwMapVec[i].CanCreateOrRecycle(DoaSt, DoaEd);
            if(NewSign == true)
            {
                NewDoaTargetNoVec.push_back(i);
                break;
            }
        }
        if(NewSign == false)
        {
            i = PDW_RFPWMAPVEC_SIZE;
            ERRORMSG("PdwRfPwMapVec is full!");
        }
        return i;
    }
    template<typename _PdwType>
    inline void PdwVecDoaMap(vector<_PdwType>& srcPdwVec, UINT DoaSt, UINT DoaEd)           // 以Doa提取PdwVec映射至PdwRfPwMap,并将结果显示在srcMat、srcPdwVec
    {
        UINT i = VecDoaAdaptInd(DoaSt, DoaEd);
        if(i == PDW_RFPWMAPVEC_SIZE)
        {
            ERRORMSG("PdwRfPwMapVec is full!");
            return;
        }
        PdwRfPwMapVec[i].PdwVecMap<_PdwType>(srcPdwVec);
    }
    inline void EndFrameProcess(Mat& srcMat, _cvTextImage<cv::FONT_HERSHEY_COMPLEX_SMALL>& AnalysisTextMat)     // 帧结束处理
    {
        NewDoaTargetNosQueue.Push(NewDoaTargetNoVec);
        FrameCircularNo++;
        if(FrameCircularNo == 0)
        {
            FrameCircularNo = 1;
        }
        bool    FindNewDoaTargetSign = false;
        UINT    NewTargetNumTmp = 0;
        for(UINT i = 0; i < PDW_RFPWMAPVEC_SIZE; i++)
        {
            FindNewDoaTargetSign = false;
            for(UINT j = 0; j < NewDoaTargetNosQueue.GetSize(); j++)
            {
                for(UINT l = 0; l < NewDoaTargetNosQueue[j].size(); l++)
                {
                    if(i == NewDoaTargetNosQueue[j][l])
                    {
                        FindNewDoaTargetSign = true;
                        break;
                    }
                }
                if(FindNewDoaTargetSign == true)
                {
                    break;
                }
            }
            if(FindNewDoaTargetSign == true)
            {
                string Str = "Doa(New targets) : " + 
                            tostring(PdwRfPwMapVec[i].GetDoaSt()) + " - " + 
                            tostring(PdwRfPwMapVec[i].GetDoaEd()) + " No." + tostring(i);
                AnalysisTextMat.Title(Str);
                drawNewDoaTarget(srcMat, NewDoaTargetMat, 290, PdwRfPwMapVec[i].GetDoaSt());
            }
            else
            {
                if(PdwRfPwMapVec[i].GetActiveSign() == true)
                {
                    string Str = "Doa : " + 
                                tostring(PdwRfPwMapVec[i].GetDoaSt()) + " - " + 
                                tostring(PdwRfPwMapVec[i].GetDoaEd()) + " No." + tostring(i);
                    AnalysisTextMat.Title(Str);
                }
                else if(PdwRfPwMapVec[i].GetValidSign() == true)
                {
                    string Str = "Doa(Miss targets) : " + 
                                 tostring(PdwRfPwMapVec[i].GetDoaSt()) + " - " +
                                 tostring(PdwRfPwMapVec[i].GetDoaEd()) + " No." + tostring(i);
                    AnalysisTextMat.Title(Str);
                }
            }
            NewTargetNumTmp = PdwRfPwMapVec[i].EndFrameProcess<cv::FONT_HERSHEY_COMPLEX_SMALL>(AnalysisTextMat);
            if(NewTargetNumTmp > 0)
            {
                drawNewDiscoverTarget(srcMat, NewDiscoverTargetMat, 10, PdwRfPwMapVec[i].GetDoaSt(), PdwRfPwMapVec[i].GetDoaSt(), NewTargetNumTmp);
            }
        }
        for(UINT i = 0; i < PDW_RFPWMAPVEC_SIZE; i++)
        {
            PdwRfPwMapValidVec[i] = PdwRfPwMapVec[i].GetValidSign();
        }
    }
    inline void ShowVaildMat(UINT ZoomLevel)
    {
        for(UINT i = 0; i < PDW_RFPWMAPVEC_SIZE; i++)
        {
            PdwRfPwMapVec[i].showRfPwMat(ZoomLevel);
        }
    }
};

template <typename _FileMsgTy, typename _PdwType, typename _CvVecType>
class _PdwToMat
{
	typedef _PdwToMat<_FileMsgTy, _PdwType, _CvVecType> _ThisType;
    typedef _binaryFile<_FileMsgTy, _PdwType>           binaryFileType;
    typedef _PdwMap2D<_PdwType, BGRA>                   PdwMap2DType;
    typedef _PdwArrayDesc<_PdwType>                     PdwArrayDescType;
public:
    CUINT                           ReadDataType;               // 0:文件, 1:iRadarSimPro
    CUINT                           DoaSpansUnit;				// 轮廓Doa分区角度单位
    UINT                            PdwSize;
    UINT                            ReadDataSize;
    UINT                            Row;
    UINT                            Col;
    UINT                            RemapRow;
    UINT                            RemapCol;
    UINT                            AddRow;
    UINT                            AddCol;
    Mat                             ImageMat;
    Mat                             RemapImageMat;
    Mat                             xMapImage;
    Mat                             yMapImage;
    Mat                             ROIMat;                     // 感兴趣区域
    Mat                             rstROIMat;                  // 感兴趣区域输出图
    Mat                             ROIHistogramMat;            // 感兴趣区域直方图
    binaryFileType                  binaryFile;
    _Timeit                         Timeit;
    _PdwType*                       PdwAddr;
    PdwMap2DType                    PdwMap2D;
    _BackGround                     BackGround;
    _CvTools                        CvTools;                    // Cv算法封装类对象
    _PdwMemorySwap<_PdwType>        PdwMemorySwap;              // 模拟数据输入流接口
    _MatPdwMatrix<_PdwType>         PdwMatrix;                  // 图像原始数据反射矩阵vector
    _PdwRfPwMapClt                  PdwRfPwMapClt;              // PdwVec映射RfPwMap集合
    vector<_PdwType>                PdwAnalyVec;                // 原始脉冲分析vecotr
    vector<vector<cv::Point> >      Contours;                   // 轮廓检测vector
    vector<pair<int, int> >         ContourCenters;             // 轮廓中心vector
    vector<pair<int,double> >       ToaMaintains;               // 轮廓Toa保持时间vector
    vector<vector<int> >            DoaSpans;                   // 轮廓Doa分区vector
    vector<Rect>                    Rectangles;                 // 轮廓矩形包围框vector
    vector<pair<UINT, cv::Scalar> > RectangleMeans;             // 轮廓内均值vector
    vector<UINT>                    ContourScanNos;             // 轮廓扫描分析vector
    vector<UINT>                    ContourTraceNos;            // 轮廓跟踪分析vector
    vector<UINT>                    RowAxisPixelNums;           // 按行统计二值图像素和vector
    vector<pair<int, int> >         HistogramBinLocs;           // 感兴趣直方图柱位置标定vector

    _PdwToMat(const string& filepath, UINT Row_, UINT Col_, UINT ReadDataSize_ = 1)                     // 构造函数(文件读取)
             :binaryFile(filepath, ReadDataSize_), ReadDataSize(ReadDataSize_),
              Timeit(20),
              BackGround(White),
              DoaSpansUnit(32),
              ReadDataType(0)
    {
        if(ReadDataSize_ == 1)
        {
            ReadDataResize(binaryFile.GetFileLines());
        }
        PdwAddr = binaryFile.GetDataAddr();
        binaryFile.registShowDataFunc(PrintPdw<_PdwType>);
        ImageResize(Row_, Col_);
        PdwAnalyVec.reserve(0x1000000);
    }
    _PdwToMat(UINT Row_, UINT Col_, UINT ReadDataSize_ = 1)                                             // 构造函数(文件读取)
             :binaryFile(ReadDataSize_), ReadDataSize(ReadDataSize_),
              Timeit(20),
              BackGround(White),
              DoaSpansUnit(32),
              ReadDataType(0)
    {
        PdwAddr = binaryFile.GetDataAddr();
        binaryFile.registShowDataFunc(PrintPdw<_PdwType>);
        ImageResize(Row_, Col_);
        PdwAnalyVec.reserve(0x1000000);
    }
    _PdwToMat(_iRadarSimPro<_PdwType>& iRadarSimPro, UINT Row_, UINT Col_)                              // 构造函数(内存交换)
             :binaryFile(),
              PdwMemorySwap(iRadarSimPro, &PdwAddr, PdwSize),
              Timeit(20),
              BackGround(White),
              DoaSpansUnit(32),
              ReadDataType(1)
    {
        ImageResize(Row_, Col_);
        PdwAnalyVec.reserve(0x1000000);
    }
    virtual ~_PdwToMat()                                                                                // 析构函数，结束时显示Timeit
    {
        Timeit.Show();
        if(ReadDataType == 0)
        {
            binaryFile.close();
        }
    }
    _ThisType& ImageResize(UINT Row_, UINT Col_)                                                        // ImageMat调整
    {
        Row = Row_;
        Col = Col_;
        AddRow = Row/100;
        AddCol = Col/100;
        PdwMatrix.init(Col);
        ImageMat.create(Row, Col, CV_8UC4);
        DoaSpans.resize(Row/DoaSpansUnit);
        RemapImageResize(Col, Row);
        return *this;
    }
    _ThisType& RemapImageResize(UINT RemapRow_, UINT RemapCol_)                                         // 重映射图像调整
    {
        RemapRow = RemapRow_;
        RemapCol = RemapCol_;
        RemapImageMat.create(RemapRow, RemapCol, CV_8UC4);
        xMapImage.create(RemapRow, RemapCol, CV_32FC1);
        yMapImage.create(RemapRow, RemapCol, CV_32FC1);
        SetRemap();
        return *this;
    }
    _ThisType& ReadDataResize(UINT ReadDataSize_)                                                       // 设置文件最大读取长度
    {
        if(ReadDataType == 0)
        {
            ReadDataSize = ReadDataSize_;
            binaryFile.ReadDataResize(ReadDataSize);
            PdwAddr = binaryFile.GetDataAddr();
        }
        return *this;
    }
    _ThisType& SetAddRow(UINT AddRow_)                                                                  // 设置AddCol
    {
        AddRow = AddRow_;
        return *this;
    }
    _ThisType& SetAddCol(UINT AddCol_)                                                                  // 设置AddCol
    {
        AddCol = AddCol_;
        return *this;
    }
    _ThisType& SetBackGround(_BackGround BackGround_)                                                   // 设置图像背景色
    {
        BackGround = BackGround_;
        return *this;
    }
    _ThisType& SetRemap()                                                                               // 设置重映射
    {
        for (int i = 0; i < RemapRow; i++)
        {
            for (int j = 0; j < RemapCol; j++)
            {
                xMapImage.at<float>(i, j) = RemapRow - i;
                yMapImage.at<float>(i, j) = j;
            }
        }
        return *this;
    }
    _ThisType& SetPdwArrayDesc(UINT PaMin = 0, UINT PaMax = 0,                                          // 设置PdwArray
                               UINT RfMin = 0, UINT RfMax = 0,
                               UINT PwMin = 0, UINT PwMax = 0)
    {
        _PdwType::PdwArrayDesc.PaMin = (PaMin == 0) ? _PdwType::PdwArrayDesc.PaMin : PaMin;
        _PdwType::PdwArrayDesc.PaMax = (PaMax == 0) ? _PdwType::PdwArrayDesc.PaMax : PaMax;
        _PdwType::PdwArrayDesc.RfMin = (RfMin == 0) ? _PdwType::PdwArrayDesc.RfMin : RfMin;
        _PdwType::PdwArrayDesc.RfMax = (RfMax == 0) ? _PdwType::PdwArrayDesc.RfMax : RfMax;
        _PdwType::PdwArrayDesc.PwMin = (PwMin == 0) ? _PdwType::PdwArrayDesc.PwMin : PwMin;
        _PdwType::PdwArrayDesc.PwMax = (PwMax == 0) ? _PdwType::PdwArrayDesc.PwMax : PwMax;
        return *this;
    }
    _ThisType& OpenFile(const string& filepath)                                                         // 打开文件
    {
        this->binaryFile.open(filepath);
        if(ReadDataSize == 1)
        {
            ReadDataResize(binaryFile.GetFileLines());
        }
        this->CvTools.ImageMatClear(this->ImageMat, this->BackGround);
        this->PdwMatrix.clear();
        return *this;
    }
    _ThisType& CloseFile()                                                                              // 关闭文件
    {
        this->binaryFile.close();
        return *this;
    }
    _ThisType& ReadData(int seekst = -1)                                                                // 文件读取Pdw至PdwAddr
    {
        Timeit.Start(0, "读取文件ReadData，共计(us): ");
        _PdwType::PdwArrayDescClear();                          //读取之前清零
        binaryFile.read(seekst);
        PdwSize = binaryFile.GetDataValidLines();
        Timeit.End(0);
        return *this;
    }
    _ThisType& AddTimeReadData(UINT AddTime_ms = 100, UINT ToaUnit_ns = 80)                             // 文件追加读取以ToaUnit_ns为TOA单位，AddTime_ms内的数据
    {
        Timeit.Start(1, "读取文件AddTimeReadData，共计(us): ");
        _PdwType::PdwArrayDescClear();                          //读取之前清零
        binaryFile.AddRead(AddTime_ms, ToaUnit_ns);
        PdwSize = binaryFile.GetDataValidLines();
        Timeit.End(1);
        return *this;
    }
    _ThisType& FilterData(int Type, int FilterintervalL, int FilterintervalR)                           // 以Type为类型过滤PdwAddr数据，Type: 0,PA; 1,RF; 2:PW; 3:TOA; 4:DOA
    {
        switch(Type)
        {
        case 0:                     // PA
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Pa < FilterintervalL) || (PdwAddr[i].Pa > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        case 1:                     // RF
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Rf < FilterintervalL) || (PdwAddr[i].Rf > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        case 2:                     // PW
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Pw < FilterintervalL) || (PdwAddr[i].Pw > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        case 3:                     // TOA
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Toa < FilterintervalL) || (PdwAddr[i].Toa > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        case 4:                     // DOA
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Doa < FilterintervalL) || (PdwAddr[i].Doa > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        default:
            break;
        }
        return *this;
    }
    _ThisType& SavePdwData(const string& save_path)                                                     // 文件读取数据按_PdwType格式存储至save_path
    {
        Timeit.Start(2, "保存文件，共计(us): ");
        binaryFile.save(save_path);
        Timeit.End(2);
        return *this;
    }
    _ThisType& Remap()                                                                                  // 图像ImageMat重映射至RemapImageMat
    {
        CvTools.Remap(ImageMat, RemapImageMat, xMapImage, yMapImage, BackGround);
        return *this;
    }
    _ThisType& ImageMatTranslation(int RTranslation = 0, int UpTranslation = 0)                         // ImageMat图像位移
    {
        CvTools.MatTranslation(ImageMat, RTranslation, UpTranslation, BackGround);
        return *this;
    }
    _ThisType& SetPdwMap2D(UINT ToaCol, UINT PaMin = 0, float PaNormalUnit = 0,                         // 自定义Pdw二维映射参数
                           UINT RfMin = 0, float RfNormalUnit = 0,
                           UINT PwMin = 0, float PwNormalUnit = 0)
    {
        UINT PaRange = _PdwType::PdwArrayDesc.PaMax - _PdwType::PdwArrayDesc.PaMin;
        UINT RfRange = _PdwType::PdwArrayDesc.RfMax - _PdwType::PdwArrayDesc.RfMin;
        UINT PwRange = _PdwType::PdwArrayDesc.PwMax - _PdwType::PdwArrayDesc.PwMin;
        if(PaNormalUnit == 0)
        {
            PaMin        = _PdwType::PdwArrayDesc.PaMin;
            PaNormalUnit = float(PaRange/256.0);
        }
        if(RfNormalUnit == 0)
        {
            RfMin        = _PdwType::PdwArrayDesc.RfMin;
            RfNormalUnit = float(RfRange/256.0);
        }
        if(PwNormalUnit == 0)
        {
            PwMin        = _PdwType::PdwArrayDesc.PwMin;
            PwNormalUnit = float(PwRange/256.0);
        }
        float ToaNormalUnit;
        if(_PdwType::PdwArrayDesc.ToaReverseTimes == 0)
        {
            ToaNormalUnit = float(PdwAddr[PdwSize-1].Toa - PdwAddr[0].Toa)/ToaCol;
        }
        else
        {
            if(PdwAddr[0].Toa > PdwAddr[PdwSize-1].Toa)
            {
                ToaNormalUnit = (float(PdwAddr[PdwSize-1].Toa - PdwAddr[0].Toa)/ToaCol) + 
                                (_PdwType::PdwArrayDesc.ToaReverseTimes - 1) * (0x100000000/float(ToaCol));
            }
            else
            {
                ToaNormalUnit = (float(PdwAddr[PdwSize-1].Toa - PdwAddr[0].Toa)/ToaCol) + 
                                 _PdwType::PdwArrayDesc.ToaReverseTimes * (0x100000000/float(ToaCol));
            }
        }
        ToaNormalUnit++;                                        // 消除边界超出问题
        PdwMap2D.SetPaPara(PaMin, PaNormalUnit);
        PdwMap2D.SetRfPara(RfMin, RfNormalUnit);
        PdwMap2D.SetPwPara(PwMin, PwNormalUnit);
        PdwMap2D.SetToaPara(PdwAddr[0].Toa, ToaNormalUnit);
        return *this;
    }
    _ThisType& ClctPdwToAnalyVec(Rect& srcRect, vector<_PdwType>& PdwAnalyVec_, UINT xMin = 0, UINT yMin = 0)   // 收集图像Rect内反射原始脉冲至PdwAnalyArray（不清空）
    {
        UINT Left   = (srcRect.x > xMin) ? srcRect.x : xMin;
        UINT Up     = (srcRect.y > yMin) ? srcRect.y : yMin;
        UINT Right  = srcRect.x + srcRect.width;
        UINT Down   = srcRect.y + srcRect.height;
        PdwMatrix.Filter(PdwAnalyVec_, Left, Right, Up, Down);
        return *this;
    }
    _ThisType& AddPdwMatrixMat(Mat&         srcMat,                                                             // 生成AddCol脉冲映射BGRA图像帧和相应图像反射原始数据矩阵
                               _MatPdwMatrix<_PdwType>& PdwMatrix_,
                               _PdwType*    PdwBuf,
                               UINT         PdwBufSize,
                               UINT         AddMatCol,
                               _Map2DType   Map2DType = ToaDoa,
                               UINT         DensityThreshold = 10,
                               UINT         PixelMin = 0,
                               const float  DoaNormalUnit = 1.0,
                               UINT PaMin = 0, float PaNormalUnit = 0,
                               UINT RfMin = 0, float RfNormalUnit = 0,
                               UINT PwMin = 0, float PwNormalUnit = 0)
    {
        _CvVecType CvVecPixelTmp;
        CvTools.MatTranslation(srcMat, -AddMatCol, 0, BackGround);
        if(PdwMatrix_.PdwMatrixSize != srcMat.cols)
        {
            PdwMatrix_.init(srcMat.cols);
        }
        PdwMatrix_.Translation(AddMatCol);
        SetPdwMap2D(AddMatCol, PaMin, PaNormalUnit, RfMin, RfNormalUnit, PwMin, PwNormalUnit);
        switch(Map2DType)
        {
        case ToaDoa:
            break;
        case ToaPw:
            PdwMap2D.PwNormalUnit *= 256.0/srcMat.rows;
            break;
        case ToaRf:
            PdwMap2D.RfNormalUnit *= 256.0/srcMat.rows;
            break;
        case ToaPa:
            PdwMap2D.PaNormalUnit *= 256.0/srcMat.rows;
            break;
        default:
            break;
        }
        Timeit.Start(3, "图像添加映射AddPdwMatrixMat，共计(us): ");
        for(int i = 0; i < PdwBufSize; i++)
        {
            if(PdwBuf[i].ToaS != -1)
            {
                switch(Map2DType)
                {
                case ToaDoa:
                    PdwBuf[i].ToaDoaMap2D(PdwMap2D, DoaNormalUnit);
                    break;
                case ToaPw:
                    PdwBuf[i].ToaPwMap2D(PdwMap2D);
                    break;
                case ToaRf:
                    PdwBuf[i].ToaRfMap2D(PdwMap2D);
                    break;
                case ToaPa:
                    PdwBuf[i].ToaPaMap2D(PdwMap2D);
                    break;
                default:
                    break;
                }
                if((PdwMap2D.x < Row) && (PdwMap2D.y < AddMatCol))
                {
                    PdwMap2D.y = (PdwMap2D.y == Col) ? PdwMap2D.y - 1 : PdwMap2D.y;
                    CvVecPixelTmp = ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y + Col - AddMatCol);
                    CvVecPixelTmp[3] = (CvVecPixelTmp[3] < 0xFF) ? CvVecPixelTmp[3]+1 : CvVecPixelTmp[3];
                    PdwMatrix_[PdwMap2D.y + Col - AddMatCol].Push(PdwBuf[i]);
                    if(CvVecPixelTmp[3] > DensityThreshold)
                    {
                        CvVecPixelTmp[0] = (PdwMap2D.Map2DUnit.b > PixelMin) ? PdwMap2D.Map2DUnit.b : PixelMin;
                        CvVecPixelTmp[1] = (PdwMap2D.Map2DUnit.g > PixelMin) ? PdwMap2D.Map2DUnit.g : PixelMin;
                        CvVecPixelTmp[2] = (PdwMap2D.Map2DUnit.r > PixelMin) ? PdwMap2D.Map2DUnit.r : PixelMin;
                        ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y + Col - AddMatCol) = CvVecPixelTmp;
                    }
                    else
                    {
                        ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y + Col - AddMatCol) = CvVecPixelTmp;
                    }
                }
                else
                {
                    ERRORMSG("(PdwMap2D.x >= Row) || (PdwMap2D.y >= AddMatCol)");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
        }
        Timeit.End(3);
        return *this;
    }
    _ThisType& AddBGRAImageAndPdwMatrix(_Map2DType Map2DType,                                           // 生成AddCol脉冲映射BGRA图像帧和相应图像反射原始数据矩阵
                                        UINT Threshold, UINT PixelMin = 0,
                                        const float DoaNormalUnit = 1.0,
                                        UINT PaMin = 0, float PaNormalUnit = 0,
                                        UINT RfMin = 0, float RfNormalUnit = 0,
                                        UINT PwMin = 0, float PwNormalUnit = 0)
    {
        PdwSize = (ReadDataType == 0) ? binaryFile.GetDataValidLines() : PdwSize;
        AddPdwMatrixMat(ImageMat, PdwMatrix, PdwAddr, PdwSize, AddCol, Map2DType, Threshold, PixelMin, DoaNormalUnit,
                        PaMin, PaNormalUnit, RfMin, RfNormalUnit, PwMin, PwNormalUnit);
        return *this;
    }
    _ThisType& ContourAnalyVideo(string save_path, int seekst = 0,                                      // 以添加的方式生成轮廓分析视频流，以AVI格式保存至save_path
                                 CINT AddTime_ms = 100, CINT ToaUnit_ns = 80,
                                 UINT ReadDataSize_ = 1000000, int readSize = 0)
    {
        int rows = ImageMat.rows;
        int cols = ImageMat.cols;
        Mat BGRMat                      (rows, cols, CV_8UC3, cv::Scalar::all(0));                  // BGR
        Mat DensityMat                  (rows, cols, CV_8UC1);                                      // 密度图，为ImageMat的alpha通道
        Mat DensityBinMat               (rows, cols, CV_8UC1);                                      // 二值图
        Mat DensityBinInvMat            (rows, cols, CV_8UC1);                                      // 二值互斥图
        Mat DensityDilateMat            (rows, cols, CV_8UC1);                                      // 密度膨胀图
        Mat BGRDensityMat               (rows, cols, CV_8UC3);                                      // 密度BGR图
        Mat BGRDensityDilateMat         (rows, cols, CV_8UC3);                                      // 密度BGR膨胀图
        Mat RemapBGRDensityMat          (rows, cols, CV_8UC3);                                      // 密度BGR映射图
        Mat RemapBGRDensityDilateMat    (rows, cols, CV_8UC3);                                      // 密度BGR膨胀映射图
        Mat VideoFrameMat(cols + 400, 3 * rows + 600, CV_8UC3, cv::Scalar::all(255));               // 视频写入帧
        Mat VideoFrame1  = VideoFrameMat(Rect(0, 100, rows+200, cols+200));                         // BGR映射图
        Mat VideoFrame21 = VideoFrameMat(Rect(rows+199, 0, rows/2+200, cols/2+200));                // 密度图ParDown
        Mat VideoFrame22 = VideoFrameMat(Rect(rows+199, cols/2+199, rows/2+200, cols/2+200));       // 密度膨胀图ParDown
        Mat VideoFrame31 = VideoFrameMat(Rect(rows+rows/2+399, 0, rows/2+200, cols/2+200));         // 颜色直方图
        Mat VideoFrame32 = VideoFrameMat(Rect(rows+rows/2+399, cols/2+199, rows/2+200, cols/2+200));// 密度直方图
        Mat AnalysisTextMat = VideoFrameMat(Rect(2 * rows +599, 200, rows, cols));                  // 分析图
        Mat RemapBGRMat;
        Mat pyrDownDensityMat;
        Mat pyrDownDensityDilateMat;
        Mat BGRHistogram;
        Mat DensityHistogram;
        _cvCoordinateMap<> CoordinateVideoFrame1(RemapBGRMat, VideoFrame1, rows, cols);
        {
            CoordinateVideoFrame1.xlabel("DOA", Scalar(100, 50, 50), 0, 1024, 10);
            CoordinateVideoFrame1.ylabel("TOA(s)", Scalar(100, 50, 50), 10, 0, 10);
            CoordinateVideoFrame1.title("DOA-TOA Analysis");
        }
        _cvCoordinateMap<> CoordinateVideoFrame21(pyrDownDensityMat, VideoFrame21, rows/2, cols/2);
        {
            CoordinateVideoFrame21.xlabel("DOA", Scalar(100, 50, 50), 0, 1024, 10);
            CoordinateVideoFrame21.ylabel("TOA(s)", Scalar(100, 50, 50), 10, 0, 10);
            CoordinateVideoFrame21.title("Density Map");
        }
        _cvCoordinateMap<> CoordinateVideoFrame22(pyrDownDensityDilateMat, VideoFrame22, rows/2, cols/2);
        {
            CoordinateVideoFrame22.xlabel("DOA", Scalar(100, 50, 50), 0, 1024, 10);
            CoordinateVideoFrame22.ylabel("TOA(s)", Scalar(100, 50, 50), 10, 0, 10);
            CoordinateVideoFrame22.title("Density Dilate Map");
        }
        _cvCoordinateMap<> CoordinateVideoFrame31(BGRHistogram, VideoFrame31, rows/2, cols/2);
        {
            CoordinateVideoFrame31.xlabel("Pixel value", Scalar(100, 50, 50), 0, 255, 10);
            CoordinateVideoFrame31.ylabel("NaN", Scalar(100, 50, 50), 0, 100, 10);
            CoordinateVideoFrame31.title("Channels Histogram");
        }
        _cvCoordinateMap<> CoordinateVideoFrame32(DensityHistogram, VideoFrame32, rows/2, cols/2);
        {
            CoordinateVideoFrame32.xlabel("Pixel value", Scalar(100, 50, 50), 0, 255, 10);
            CoordinateVideoFrame32.ylabel("NaN", Scalar(100, 50, 50), 0, 500, 10);
            CoordinateVideoFrame32.title("Density Histogram");
        }

        _cvTextImage<>  cvTextMat(AnalysisTextMat, 36, 3, BackGround);					                // 分析cvTextImage对象
        Mat         ChannelOutMat[] = {BGRMat, DensityMat};										        // 输出通道
        UINT        CurrentFrameNo = 0;                                                                 // 当前帧数
        int         from_to[] = {0,0,1,1,2,2,3,3};												        // 0, 1, 2 -> BGRMat; 3 -> DensityMat
        int         nPairs = 4;																	        // 四通道
        const float PixelToaUnit = float(AddTime_ms)/AddCol;							                // 像素值Toa单位
        double      Threshold = (PixelToaUnit<=0xFE) ? ((PixelToaUnit > 1) ? PixelToaUnit : 1) : 0xFE;  // 像素点阈值
        int         SizeTemp = 0;																        // 数据文件读取长度值
        VideoWriter writer;																                // 创建视频流对象
        int         codec = VideoWriter::fourcc('M', 'J', 'P', 'G');							        // 设置视频流格式
        double      fps = 10.0;																            // 设置视频流帧数
        ReadDataResize(ReadDataSize_);													                // 调整数据文件读取缓存区大小
        binaryFile.seekg(seekst);														                // 设置数据文件读取起始位置
        writer.open(save_path, codec, fps, VideoFrameMat.size(), true);					                // 创建视频流文件
        if (!writer.isOpened())
        {
            ERRORMSG("Could not open the output video file for write");
            return *this;
        }
        Timeit.Start(5, "保存视频AddSaveHistogramVideo，共计(us): ");
        CvTools.ImageMatClear(ImageMat, BackGround);
        PdwMatrix.clear();
        if(ReadDataType == 1)
        {
            PdwMemorySwap.CheckBufferReady();
        }
        readSize = (readSize == 0) ? binaryFile.GetFileLines() : readSize;
        boost::progress_display pd(readSize);
        while(1)
        {
            if(ReadDataType == 0)
            {
                if(binaryFile.isEOF())												    // 文件读取至尾部
                {
                    break;
                }
                AddTimeReadData(AddTime_ms, ToaUnit_ns);							    // 以ToaUnit_ns为单位，按AddTime_ms时进读取数据。
                PdwSize = binaryFile.GetDataValidLines();							    // 获取读取有效数据长度
            }
            else
            {
                PdwMemorySwap.AddRead(AddTime_ms, ToaUnit_ns);
            }
            if(SizeTemp >= readSize)													// 文件读取长度大于readSize
            {
                break;
            }
            else
            {
                SizeTemp += PdwSize;
                pd.restart(readSize);
                pd += SizeTemp;
            }
            cvTextMat.Clear();											                // 清空分析结果区图像
            PdwRfPwMapClt.NewFrameInit();
            string Str = "CurrentFrameNo : " + tostring(CurrentFrameNo);
            cvTextMat.Title(Str);
            Str = " ";
            cvTextMat.Title(Str);                                                       // 空行
            cvTextMat.Title(Str);
            AddBGRAImageAndPdwMatrix(ToaDoa, 1, 0, 1.0, 0, 1, 2000, 255, 0, 10);	    // 以时进拼接方式生成图片
            cv::mixChannels(&ImageMat, 1, ChannelOutMat, 2, from_to, 4);				// ImageMat多通道分离至ChannelOutMat中的BGRMat, DensityMat
            CvTools.CalcMatHistogram(BGRMat, BGRHistogram, BackGround);				    // 计算BGRMat的直方图BGRHistogram
            CvTools.CalcMatHistogram(DensityMat, DensityHistogram, BackGround);		    // 计算DensityMat的直方图DensityHistogram
            // Generage Image
            cv::threshold(DensityMat, DensityBinMat, Threshold, 255, THRESH_BINARY);	// 密度图以Threshold为最小阈值进行阈值操作生成二值图DensityBinMat
            cv::threshold(DensityMat, DensityBinInvMat, Threshold,0, THRESH_TOZERO_INV);// 密度图以Threshold为最大阈值进行阈值操作(高于阈值像素值设置为0)生成生成DensityBinInvMat
            cv::threshold(DensityBinInvMat, DensityBinInvMat, 0, 255, THRESH_BINARY);	// DensityBinInvMat以Threshold为最小阈值进行阈值操作生成二值图DensityBinInvMat
            CvTools.cvDilate(DensityBinMat, DensityDilateMat);					        // DensityBinMat进行图像膨胀操作
            // Analysis
            PdwImageAnalysis(BGRMat, DensityDilateMat, PdwRfPwMapClt, cvTextMat, PixelToaUnit); // 图像分析
            // Generage VideoFrame
            CvTools.MatInv(DensityMat, DensityMat);
            CvTools.MatInv(DensityDilateMat, DensityDilateMat);
            cv::cvtColor(DensityMat, BGRDensityMat, COLOR_GRAY2BGR);					// 灰度图DensityMat转BGR图BGRDensityMat以实现图像组合
            cv::cvtColor(DensityDilateMat, BGRDensityDilateMat, COLOR_GRAY2BGR);		// 灰度图DensityDilateMat转BGR图BGRDensityDilateMat以实现图像组合
            if(ImageMat.rows == ImageMat.cols)
            {
                CvTools.Remap(BGRDensityMat, RemapBGRDensityMat, xMapImage, yMapImage, BackGround);			    // 图像重映射
                CvTools.Remap(BGRDensityDilateMat, RemapBGRDensityDilateMat, xMapImage, yMapImage, BackGround); // 图像重映射
                cv::pyrDown(RemapBGRDensityMat, pyrDownDensityMat);						// 图像缩放
                cv::pyrDown(RemapBGRDensityDilateMat, pyrDownDensityDilateMat);			// 图像缩放
            }
            else
            {
                cv::pyrDown(BGRDensityMat, pyrDownDensityMat);
                cv::pyrDown(BGRDensityDilateMat, pyrDownDensityDilateMat);
            }
            cv::drawContours(BGRMat, Contours, -1, Scalar(0, 255, 0), 2, CV_AA);		// 在图像BGRMat上绘制轮廓组Contours
            CvTools.Remap(BGRMat, RemapBGRMat, xMapImage, yMapImage, BackGround);	    // 图像重映射
            PdwRfPwMapClt.EndFrameProcess(RemapBGRMat, cvTextMat);
            CurrentFrameNo++;
            writer.write(VideoFrameMat);												// 将图像VideoFrameMat写入视频帧
            // showImage(BGRMat);
            // cv::imshow("DensityBinMat", DensityBinMat);
            // cv::imshow("DensityBinInvMat", DensityBinInvMat);
            // PdwRfPwMapClt.ShowVaildMat(0);
            // cvTextMat.showImage();
            // cv::waitKey();
        }
        Timeit.End(5);
        return *this;
    }
    _ThisType& AddVideoAnalyStream(string save_path, int seekst = 0,                                    // 生成AddCol视频帧分析流，以AVI格式保存至save_path
                                   CINT AddTime_ms = 100, CINT ToaUnit_ns = 80,
                                   UINT ReadDataSize_ = 1000000, int readSize = 6000000)
    {
        return *this;
    }

    /****************************************** PdwAnalysis Area Beg ************************************/
    _ThisType& PdwImageAnalysis(Mat& srcMat,                                                            // 脉冲映射图像分析
                                Mat& srcBinMat,
                                _PdwRfPwMapClt& PdwRfPwMapClt_,
                                _cvTextImage<cv::FONT_HERSHEY_COMPLEX_SMALL> &AnalysisTextMat,
                                const float PixelToaUnit)
    {
        int rows = srcBinMat.rows;
        int cols = srcBinMat.cols;
        cv::findContours(srcBinMat, Contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);		// 轮廓检测
        ContourCenters.resize(Contours.size());																// 轮廓中心(Doa Toa Pair)Vector调整长度
        ToaMaintains.resize(Contours.size());																// 轮廓Toa保持时间(轮廓标号 Toa Pair)Vector调整长度
        for(UINT i = 0; i < DoaSpans.size(); i++)
        {
            DoaSpans[i].resize(0);																			// Doa等分覆盖范围内包含轮廓Vector重置大小
        }
        Rectangles.resize(Contours.size());																	// 轮廓最小包围矩形Vector调整长度
        for(UINT i = 0; i < Contours.size(); i++)
        {
            Rectangles[i] = boundingRect(Contours[i]);
            if(Rectangles[i].height < rows/6)																// 轮廓最小包围矩形方位变化小于60度
            {
                ContourCenters[i].first  = Rectangles[i].y + Rectangles[i].height/2;						// 轮廓中心(Doa Toa Pair) Doa
                ContourCenters[i].second = Rectangles[i].x + Rectangles[i].width/2;							// 轮廓中心(Doa Toa Pair) Toa
                ToaMaintains[i].first    = i;
                ToaMaintains[i].second   = Rectangles[i].width;												// 轮廓Toa保持时间(轮廓标号 Toa Pair) Toa
                DoaSpans[ContourCenters[i].first/DoaSpansUnit].push_back(i);								// 当前轮廓标号存储至当前轮廓命中Doa等分覆盖范围
            }
            else																							// 轮廓最小包围矩形方位变化大于60度，轮廓不进行分析
            {
                ContourCenters[i].first  = -1;
                ContourCenters[i].second = -1;
                ToaMaintains[i].first    = -1;
                ToaMaintains[i].second   = 0;
            }
        }
        CvTools.CalcAxisHistogram(srcBinMat, RowAxisPixelNums);										        // 输入图srcBinMat生成按行统计二值图像素和vector
        CvTools.findSectAtHistogram(RowAxisPixelNums, HistogramBinLocs, 5, 10);					            // 区间检测，阈值为5，最小间隔为10
        for(UINT i = 0; i < HistogramBinLocs.size(); i++)
        {
            UINT DoaSt = HistogramBinLocs[i].first, DoaEd = HistogramBinLocs[i].second;
            if(DoaEd - DoaSt < rows/6)								                                        // 方位变化小于60度
            {
                ContourScanNos.resize(0);																	// 轮廓内扫描雷达类型标号Vector长度清零
                ContourTraceNos.resize(0);																	// 轮廓内跟踪雷达类型标号Vector长度清零
                UINT DoaSpanNost = DoaSt / DoaSpansUnit;								                    // 轮廓起始位置落入等分方位间隔位置
                UINT DoaSpanNoed = DoaEd / DoaSpansUnit;								                    // 轮廓结尾位置落入等分方位间隔位置
                for(UINT DoaSpanNo = DoaSpanNost; DoaSpanNo <= DoaSpanNoed; DoaSpanNo++)					// 遍历间隔覆盖等分方位间隔范围
                {
                    for(UINT j = 0; j < DoaSpans[DoaSpanNo].size(); j++)									// 遍历等分方位间隔包含轮廓
                    {
                        UINT ContourNo = DoaSpans[DoaSpanNo][j];
                        if((ContourCenters[ContourNo].first > DoaSt) &&
                           (ContourCenters[ContourNo].first < DoaEd))					                    // 轮廓中心ContourCenters的Doa命中当前间隔
                        {
                            if(ToaMaintains[ContourNo].second < cols/5)										// 轮廓Toa保持时间小于cols/5视为扫描信号，否则视为跟踪信号
                            {
                                ContourScanNos.push_back(ContourNo);
                            }
                            else
                            {
                                ContourTraceNos.push_back(ContourNo);
                            }
                        }
                    }
                }
                if(ContourScanNos.size() > 3)                                                               // 扫描信号分析
                {
                    PdwAnalyVec.clear();
                    for(int i = ContourScanNos.size() - 1; i >= 0; i--)
                    {
                        ClctPdwToAnalyVec(Rectangles[ContourScanNos[i]], PdwAnalyVec, Col - AddCol, 0);
                    }
                    PdwVecProcess(PdwAnalyVec, PdwRfPwMapClt_, DoaSt, DoaEd);
                }
                if(ContourTraceNos.size() > 0)                                                              // 跟踪信号分析
                {
                    PdwAnalyVec.clear();
                    for(int i = ContourTraceNos.size() - 1; i >= 0; i--)
                    {
                        ClctPdwToAnalyVec(Rectangles[ContourTraceNos[i]], PdwAnalyVec, Col - AddCol, 0);
                    }
                    PdwVecProcess(PdwAnalyVec, PdwRfPwMapClt_, DoaSt, DoaEd);
                }
            }
        }
        return *this;
    }
    // template<CINT FontFace = cv::FONT_HERSHEY_COMPLEX_SMALL>
    // _ThisType& PdwImageProcess(Mat& srcPdwMat, )
    // {
    //     CvTools.CalcMatRectanglesMean(srcMat, RectangleMeans, Rectangles, ContourScanNos);
    //     for(UINT i = 0; i < RectangleMeans.size(); i++)
    //     {
    //         string Str = "Rf : " + tostring(RectangleMeans[i].second[0]) + ", " +
    //                         "Pw : " + tostring(RectangleMeans[i].second[1]) + ", " +
    //                         "Pa : " + tostring(RectangleMeans[i].second[2]);
    //         AnalysisTextMat.BGRCircleText(Scalar(RectangleMeans[i].second[0],
    //                                                 RectangleMeans[i].second[1],
    //                                                 255), Str);
    //     }
    //     CvTools.drawRectanglesAtMat(srcMat, ROIMat, Rectangles, ContourTraceNos);
    //     CvTools.MutiChannelRemap(ROIMat, rstROIMat, 1);
    //     CvTools.CalcMatHistogram(rstROIMat, ROIHistogramMat, BackGround);
    //     return *this;
    // }
    _ThisType& PdwVecProcess(vector<_PdwType>& srcPdwVec, _PdwRfPwMapClt& PdwRfPwMapClt_, UINT DoaSt, UINT DoaEd)
    {
        PdwRfPwMapClt_.PdwVecDoaMap<_PdwType>(srcPdwVec, DoaSt, DoaEd);
        return *this;
    }
    /****************************************** PdwAnalysis Area End ************************************/

    /****************************************** Print Area Beg ******************************************/
    _ThisType& showImage()                                                                              // 图像ImageMat show
    {
        cv::imshow("ImageMat", ImageMat);
        return *this;
    }
    _ThisType& showImage(Mat &ImageTmp)                                                                 // 图像show
    {
        cv::imshow("showImage", ImageTmp);
        return *this;
    }
    _ThisType& showRemapImage()                                                                         // 重映射图像show
    {
        cv::imshow("RemapImage", RemapImageMat);
        return *this;
    }
    _ThisType& showData(int st, int ed)                                                                 // 文件数据show
    {
        binaryFile.showData(st, ed);
        return *this;
    }
    _ThisType& showRect(Rect& srcRect)                                                                  // 矩形区域show
    {
        COUTS("Left: ", srcRect.x, "Right: ", srcRect.x + srcRect.width, "Up: ", srcRect.y, "Down: ", srcRect.y + srcRect.height);
        return *this;
    }
    _ThisType& showPdwMatrix(int Type = 0)                                                              // 图像反射原始数据show -1:dont showPdw; 0:showPdw
    {
        COUTSECBEG("showPdwMatrix");
        PdwMatrix.print(Type);
        COUTSECEND("showPdwMatrix");
        return *this;
    }
    _ThisType& showPdwArrayDesc()                                                                       // Pdw描述结构show
    {
        _PdwType::PdwArrayDesc.print();
        return *this;
    }
    _ThisType& showFileStatus()                                                                         // 文件状态show
    {
        binaryFile.showStatus();
        return *this;
    }
    template<typename T>
    _ThisType& showVector(vector<T> &VecTmp)                                                            // vectorshow
    {
        for(int i = 0; i < VecTmp.size(); i++)
        {
            COUT(i, VecTmp[i]);
        }
        return *this;
    }
    /****************************************** Print Area End ******************************************/

    /**************************************** Cold Code Area BEG ****************************************/
    _ThisType& GenBGRAImageAndPdwMatrix(UINT Threshold, UINT PixelMin = 0,                              // 生成脉冲映射BGRA图像帧和相应图像反射原始数据矩阵
                                        const float DoaNormalUnit = 1.0,
                                        UINT PaMin = 0, float PaNormalUnit = 0,
                                        UINT RfMin = 0, float RfNormalUnit = 0,
                                        UINT PwMin = 0, float PwNormalUnit = 0)
    {
        CvTools.ImageMatClear(ImageMat, BackGround);
        PdwMatrix.clear();
        PdwSize = binaryFile.GetDataValidLines();
        SetPdwMap2D(Col, PaMin, PaNormalUnit, RfMin, RfNormalUnit, PwMin, PwNormalUnit);
        Timeit.Start(6, "生成图像GenBGRAImageAndPdwArray，共计(us): ");
        _CvVecType CvVecPixelTmp;
        for(int i = 0; i < PdwSize; i++)
        {
            if(PdwAddr[i].ToaS != -1)
            {
                PdwAddr[i].ToaDoaMap2D(PdwMap2D, DoaNormalUnit);
                if((PdwMap2D.x < Row) && (PdwMap2D.y < Col))
                {
                    PdwMap2D.y = (PdwMap2D.y == Col) ? PdwMap2D.y - 1 : PdwMap2D.y;
                    CvVecPixelTmp = ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y);
                    CvVecPixelTmp[3] = (CvVecPixelTmp[3] < 0xFF) ? CvVecPixelTmp[3]+1 : CvVecPixelTmp[3];
                    PdwMatrix[PdwMap2D.y].Push(PdwAddr[i]);
                    if(CvVecPixelTmp[3] > Threshold)
                    {
                        CvVecPixelTmp[0] = (PdwMap2D.Map2DUnit.b > PixelMin) ? PdwMap2D.Map2DUnit.b : PixelMin;
                        CvVecPixelTmp[1] = (PdwMap2D.Map2DUnit.g > PixelMin) ? PdwMap2D.Map2DUnit.g : PixelMin;
                        CvVecPixelTmp[2] = (PdwMap2D.Map2DUnit.r > PixelMin) ? PdwMap2D.Map2DUnit.r : PixelMin;
                        ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y) = CvVecPixelTmp;
                    }
                    else
                    {
                        ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y) = CvVecPixelTmp;
                    }
                }
                else
                {
                    ERRORMSG("(PdwMap2D.x >= Row) || (PdwMap2D.y >= Col)");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
            else
            {
                // ERRORMSG("PdwAddr[i].ToaS < 0");
            }
        }
        Timeit.End(6);
        return *this;
    }
    _ThisType& GenImage(const float DoaNormalUnit = 1.0,                                                // 生成脉冲映射图像帧
                        UINT PaMin = 0, float PaNormalUnit = 0,
                        UINT RfMin = 0, float RfNormalUnit = 0,
                        UINT PwMin = 0, float PwNormalUnit = 0)
    {
        CvTools.ImageMatClear(ImageMat, BackGround);
        PdwSize = binaryFile.GetDataValidLines();
        SetPdwMap2D(Col, PaMin, PaNormalUnit, RfMin, RfNormalUnit, PwMin, PwNormalUnit);
        Timeit.Start(7, "生成图像GenImage，共计(us): ");
        _CvVecType CvVecPixelTmp;
        for(int i = 0; i < PdwSize; i++)
        {
            if(PdwAddr[i].ToaS != -1)
            {
                PdwAddr[i].ToaDoaMap2D(PdwMap2D, DoaNormalUnit);
                if((PdwMap2D.x < Row) && (PdwMap2D.y < Col))
                {
                    PdwMap2D.y = (PdwMap2D.y == Col) ? PdwMap2D.y - 1 : PdwMap2D.y;
                    CvVecPixelTmp[0] = PdwMap2D.Map2DUnit.b;
                    CvVecPixelTmp[1] = PdwMap2D.Map2DUnit.g;
                    CvVecPixelTmp[2] = PdwMap2D.Map2DUnit.r;
                    ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y) = CvVecPixelTmp;
                }
                else
                {
                    ERRORMSG("(PdwMap2D.x >= Row) || (PdwMap2D.y >= Col)");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
            else
            {
                // ERRORMSG("PdwAddr[i].ToaS < 0");
            }
        }
        Timeit.End(7);
        return *this;
    }
    _ThisType& GenBGRAImage(UINT Threshold, UINT PixelMin = 0,                                          // 生成脉冲映射BGRA图像帧
                            const float DoaNormalUnit = 1.0,
                            UINT PaMin = 0, float PaNormalUnit = 0,
                            UINT RfMin = 0, float RfNormalUnit = 0,
                            UINT PwMin = 0, float PwNormalUnit = 0)
    {
        CvTools.ImageMatClear(ImageMat, BackGround);
        PdwSize = binaryFile.GetDataValidLines();
        SetPdwMap2D(Col, PaMin, PaNormalUnit, RfMin, RfNormalUnit, PwMin, PwNormalUnit);
        Timeit.Start(8, "生成图像GenBGRAImage，共计(us): ");
        _CvVecType CvVecPixelTmp;
        for(int i = 0; i < PdwSize; i++)
        {
            if(PdwAddr[i].ToaS != -1)
            {
                PdwAddr[i].ToaDoaMap2D(PdwMap2D, DoaNormalUnit);
                if((PdwMap2D.x < Row) && (PdwMap2D.y < Col))
                {
                    PdwMap2D.y = (PdwMap2D.y == Col) ? PdwMap2D.y - 1 : PdwMap2D.y;
                    CvVecPixelTmp = ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y);
                    CvVecPixelTmp[3] = (CvVecPixelTmp[3] < 0xFF) ? CvVecPixelTmp[3]+1 : CvVecPixelTmp[3];
                    if(CvVecPixelTmp[3] > Threshold)
                    {
                        CvVecPixelTmp[0] = (PdwMap2D.Map2DUnit.b > PixelMin) ? PdwMap2D.Map2DUnit.b : PixelMin;
                        CvVecPixelTmp[1] = (PdwMap2D.Map2DUnit.g > PixelMin) ? PdwMap2D.Map2DUnit.g : PixelMin;
                        CvVecPixelTmp[2] = (PdwMap2D.Map2DUnit.r > PixelMin) ? PdwMap2D.Map2DUnit.r : PixelMin;
                        ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y) = CvVecPixelTmp;
                    }
                    else
                    {
                        ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y) = CvVecPixelTmp;
                    }
                }
                else
                {
                    ERRORMSG("(PdwMap2D.x >= Row) || (PdwMap2D.y >= Col)");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
            else
            {
                // ERRORMSG("PdwAddr[i].ToaS < 0");
            }
        }
        Timeit.End(8);
        return *this;
    }
    _ThisType& AddBGRAImage(UINT Threshold, UINT PixelMin = 0,                                          // 生成AddCol脉冲映射BGRA图像帧
                            const float DoaNormalUnit = 1.0,
                            UINT PaMin = 0, float PaNormalUnit = 0,
                            UINT RfMin = 0, float RfNormalUnit = 0,
                            UINT PwMin = 0, float PwNormalUnit = 0)
    {
        ImageMatTranslation(-AddCol, 0);
        PdwSize = binaryFile.GetDataValidLines();
        SetPdwMap2D(AddCol, PaMin, PaNormalUnit, RfMin, RfNormalUnit, PwMin, PwNormalUnit);
        Timeit.Start(9, "图像添加AddBGRAImage，共计(us): ");
        _CvVecType CvVecPixelTmp;
        for(int i = 0; i < PdwSize; i++)
        {
            if(PdwAddr[i].ToaS != -1)
            {
                PdwAddr[i].ToaDoaMap2D(PdwMap2D, DoaNormalUnit);
                if((PdwMap2D.x < Row) && (PdwMap2D.y < AddCol))
                {
                    PdwMap2D.y = (PdwMap2D.y == Col) ? PdwMap2D.y - 1 : PdwMap2D.y;
                    CvVecPixelTmp = ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y + Col - AddCol);
                    CvVecPixelTmp[3] = (CvVecPixelTmp[3] < 0xFF) ? CvVecPixelTmp[3]+1 : CvVecPixelTmp[3];
                    if(CvVecPixelTmp[3] > Threshold)
                    {
                        CvVecPixelTmp[0] = (PdwMap2D.Map2DUnit.b > PixelMin) ? PdwMap2D.Map2DUnit.b : PixelMin;
                        CvVecPixelTmp[1] = (PdwMap2D.Map2DUnit.g > PixelMin) ? PdwMap2D.Map2DUnit.g : PixelMin;
                        CvVecPixelTmp[2] = (PdwMap2D.Map2DUnit.r > PixelMin) ? PdwMap2D.Map2DUnit.r : PixelMin;
                        ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y + Col - AddCol) = CvVecPixelTmp;
                    }
                    else
                    {
                        ImageMat.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y + Col - AddCol) = CvVecPixelTmp;
                    }
                }
                else
                {
                    ERRORMSG("(PdwMap2D.x >= Row) || (PdwMap2D.y >= AddCol)");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
            else
            {
                // ERRORMSG("PdwAddr[i].ToaS < 0");
            }
        }
        Timeit.End(9);
        return *this;
    }
    _ThisType& SaveBGRAPolarImage(const string& savePath, UINT Threshold,                               // 保存BGRA极坐标图至savePath
                                  UINT PixelMin = 0,
                                  const float DoaUnit = 1.0, 
                                  UINT R0 = 100, UINT R1 = 2000,
                                  UINT PaMin = 0, float PaNormalUnit = 0,
                                  UINT RfMin = 0, float RfNormalUnit = 0,
                                  UINT PwMin = 0, float PwNormalUnit = 0)
    {
        Mat PolarImage(2*R1, 2*R1, CV_8UC4);
        CvTools.ImageMatClear(PolarImage, BackGround);
        PdwSize = binaryFile.GetDataValidLines();
        SetPdwMap2D(R1 - R0, PaMin, PaNormalUnit, RfMin, RfNormalUnit, PwMin, PwNormalUnit);
        Timeit.Start(10, "生成图像SaveBGRAPolarImage，共计(us): ");
        _CvVecType CvVecPixelTmp;
        for(int i = 0; i < PdwSize; i++)
        {
            if(PdwAddr[i].ToaS != -1)
            {
                PdwAddr[i].mapPolar2D(PdwMap2D, DoaUnit, R0, R1);
                if((PdwMap2D.x < 2*R1) && (PdwMap2D.y < 2*R1))
                {
                    PdwMap2D.y = (PdwMap2D.y == 2*R1) ? PdwMap2D.y - 1 : PdwMap2D.y;
                    CvVecPixelTmp = PolarImage.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y);
                    CvVecPixelTmp[3] = (CvVecPixelTmp[3] < 0xFF) ? CvVecPixelTmp[3]+1 : CvVecPixelTmp[3];
                    if(CvVecPixelTmp[3] > Threshold)
                    {
                        CvVecPixelTmp[0] = (PdwMap2D.Map2DUnit.b > PixelMin) ? PdwMap2D.Map2DUnit.b : PixelMin;
                        CvVecPixelTmp[1] = (PdwMap2D.Map2DUnit.g > PixelMin) ? PdwMap2D.Map2DUnit.g : PixelMin;
                        CvVecPixelTmp[2] = (PdwMap2D.Map2DUnit.r > PixelMin) ? PdwMap2D.Map2DUnit.r : PixelMin;
                        PolarImage.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y) = CvVecPixelTmp;
                    }
                    else
                    {
                        PolarImage.at<_CvVecType>(PdwMap2D.x, PdwMap2D.y) = CvVecPixelTmp;
                    }
                }
                else
                {
                    ERRORMSG("(PdwMap2D.x >= 2*R1) || (PdwMap2D.y >= 2*R1)");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
            else
            {
                // ERRORMSG("PdwAddr[i].ToaS < 0");
            }
        }
        Timeit.End(10);
        SaveImage(savePath, PolarImage);
        UINT pos=savePath.find_last_of('/');
        std::string filepath(savePath.substr(0, pos+1));
        std::string filename(savePath.substr(pos+1));
        std::string DensitysavePath = filepath + "Density_" + filename;
        std::string PasavePath = filepath + "Pa_" + filename;
        std::string RfsavePath = filepath + "Rf_" + filename;
        std::string PwsavePath = filepath + "Pw_" + filename;
        SaveDensityImage(DensitysavePath, PolarImage);
        SaveChannelImage(PasavePath, PolarImage, Rch);
        SaveChannelImage(RfsavePath, PolarImage, Bch);
        SaveChannelImage(PwsavePath, PolarImage, Gch);
        return *this;
    }
    _ThisType& SaveImage(const string& save_path)                                                       // 保存ImageMat至save_path
    {
        cv::imwrite(save_path, ImageMat);
        return *this;
    }
    _ThisType& SaveImage(const string& save_path, Mat &ImageTmp)                                        // 保存ImageTmp至save_path
    {
        cv::imwrite(save_path, ImageTmp);
        return *this;
    }
    _ThisType& SaveChannelImage(const string& savePath, Mat &ImageTmp, _ImageChannel ImageChannel)      // 保存ImageTmp的ImageChannel通道至savePath
    {
        int rows = ImageTmp.rows;
        int cols = ImageTmp.cols;
        Mat ChannelImage(rows, cols, CV_8UC1);
        _CvVecType CvVecPixelTmp;
        UCHAR *pCvVecPixelChannelTmp = NULL;
        switch(ImageChannel)
        {
        case Rch:
            pCvVecPixelChannelTmp = &(CvVecPixelTmp[2]);
            break;
        case Gch:
            pCvVecPixelChannelTmp = &(CvVecPixelTmp[1]);
            break;
        case Bch:
            pCvVecPixelChannelTmp = &(CvVecPixelTmp[0]);
            break;
        case Ach:
            pCvVecPixelChannelTmp = &(CvVecPixelTmp[3]);
            break;
        default:
            break;
        }
        Timeit.Start(11, "生成图像SaveChannelImage，共计(us): ");
        for(int Row = 0; Row < rows; Row++)
        {
            for(int Col = 0; Col < cols; Col++)
            {
                CvVecPixelTmp = ImageTmp.at<_CvVecType>(Row, Col);
                ChannelImage.at<UCHAR>(Row, Col) = *pCvVecPixelChannelTmp;
            }
        }
        Timeit.End(11);
        imwrite(savePath, ChannelImage);
        return *this;
    }
    _ThisType& SaveDensityImage(const string& savePath)                                                 // 保存ImageMat密度图至savePath
    {
        SaveDensityImage(savePath, ImageMat);
        return *this;
    }
    _ThisType& SaveDensityImage(const string& savePath, Mat &ImageTmp)                                  // 保存ImageTmp密度图至savePath
    {
        int rows = ImageTmp.rows;
        int cols = ImageTmp.cols;
        Mat DensityImage(rows, cols, CV_8UC1);
        _CvVecType CvVecPixelTmp;
        Timeit.Start(12, "生成图像SaveDensityImage，共计(us): ");
        for(int Row = 0; Row < rows; Row++)
        {
            for(int Col = 0; Col < cols; Col++)
            {
                CvVecPixelTmp = ImageTmp.at<_CvVecType>(Row, Col);
                if(BackGround == Black)
                {
                    DensityImage.at<UCHAR>(Row, Col) = CvVecPixelTmp[3];
                }
                else if(BackGround == White)
                {
                    DensityImage.at<UCHAR>(Row, Col) = 255 - CvVecPixelTmp[3];
                }
            }
        }
        Timeit.End(12);
        imwrite(savePath, DensityImage);
        return *this;
    }
    _ThisType& SaveVideo(string save_path, int seekst = 0,                                              // 生成视频流，，以AVI格式保存至save_path
                         int interval = 10000, UINT ReadDataSize_ = 100000, int readSize = 6000000)
    {
        int rows = RemapImageMat.rows;
        int cols = RemapImageMat.cols;
        Mat BGRImageMat(rows, cols, CV_8UC3);
        ReadDataResize(ReadDataSize_);
        int SizeTemp = 0;
        VideoWriter writer;
        int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
        double fps = 10.0;
        writer.open(save_path, codec, fps, BGRImageMat.size(), true);
        if (!writer.isOpened())
        {
            ERRORMSG("Could not open the output video file for write");
            return *this;
        }
        Timeit.Start(13, "保存视频SaveVideo，共计(us): ");
        while(1)
        {
            ReadData(seekst + SizeTemp);
            PdwSize = binaryFile.GetDataValidLines();
            if(PdwSize == ReadDataSize_)
            {
                SizeTemp += interval;
            }
            else
            {
                break;
            }
            if(SizeTemp >= readSize)
            {
                break;
            }
            GenBGRAImage(10,0);
            Remap();
            cvtColor(RemapImageMat, BGRImageMat, COLOR_BGRA2BGR);
            writer.write(BGRImageMat);
        }
        Timeit.End(13);
        return *this;
    }
    _ThisType& AddSaveVideo(string save_path, int seekst = 0,                                           // 生成Addol视频流，以AVI格式保存至save_path
                            CINT AddTime_ms = 100, CINT ToaUnit_ns = 80,
                            UINT ReadDataSize_ = 1000000, int readSize = 6000000)
    {
        int rows = RemapImageMat.rows;
        int cols = RemapImageMat.cols;
        Mat BGRImageMat(rows, cols, CV_8UC3);
        ReadDataResize(ReadDataSize_);
        binaryFile.seekg(seekst);
        int SizeTemp = 0;
        VideoWriter writer;
        int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
        double fps = 10.0;
        writer.open(save_path, codec, fps, BGRImageMat.size(), true);
        if (!writer.isOpened())
        {
            ERRORMSG("Could not open the output video file for write");
            return *this;
        }
        Timeit.Start(14, "保存视频AddSaveVideo，共计(us): ");
        CvTools.ImageMatClear(ImageMat, BackGround);
        while(1)
        {
            if(binaryFile.isEOF())
            {
                break;
            }
            AddTimeReadData(AddTime_ms, ToaUnit_ns);
            PdwSize = binaryFile.GetDataValidLines();
            if(SizeTemp >= readSize)
            {
                break;
            }
            else
            {
                SizeTemp += PdwSize;
            }
            AddBGRAImage(1);
            Remap();
            cvtColor(RemapImageMat, BGRImageMat, COLOR_BGRA2BGR);
            writer.write(BGRImageMat);
        }
        Timeit.End(14);
        return *this;
    }
    /**************************************** Cold Code Area END ****************************************/
};

#endif // !_PDWTOMAT_HPP
