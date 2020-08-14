/*
 * @Author: xpt 
 * @Date: 2020-08-12 17:28:12 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-08-13 15:16:51
 */

#ifndef _PDWPLOT_HPP
#define _PDWPLOT_HPP

#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "../PdwStruct/PdwStruct.hpp"
#include "../fileStream/PdwFile.hpp"
#include "../imageTools/cvTextImage.hpp"
#include "../imageTools/cvCoordinateMap.hpp"
#include "../imageTools/CvTools.hpp"
#include "../Timeit.h"
#include "../CircularQueue.hpp"
using namespace cv;

template <typename _FileMsgTy, typename _PdwType>
class _PdwPlot
{
	typedef _PdwPlot<_FileMsgTy, _PdwType>              _ThisType;
    typedef _PdwFile<_FileMsgTy, _PdwType>              PdwFileType;
    typedef _PdwMap2D<_PdwType, BGRA>                   PdwMap2DType;
    typedef _PdwArrayDesc<_PdwType>                     PdwArrayDescType;
public:
    _Timeit                 Timeit;

    PdwFileType             PdwFile;
    _PdwType*               PdwAddr;
    UINT                    ReadDataSize;

    PdwMap2DType            PdwMap2D;
    _CvTools                CvTools;                        // Cv算法封装类对象

    UINT                    Row;
    UINT                    Col;
    UMat                    RfMat;
    UMat                    PwMat;
    UMat                    PaMat;
    UMat                    DoaMat;
    UMat                    PriMat;
    _BackGround             BackGround;
    _MatPdwMatrix<_PdwType> PdwMatrix;                      // 图像原始数据反射矩阵vector
    vector<_PdwType>        PdwAnalyVec;                    // 原始脉冲分析vecotr

    _PdwPlot(UINT Row_, UINT Col_) : Timeit(20), BackGround(White)
    {
        PdwFile.registShowDataFunc(PrintPdw<_PdwType>);
        ImageResize(Row_, Col_);
        PdwAnalyVec.reserve(0x1000000);
    }
    virtual ~_PdwPlot()
    {
        Timeit.Show();
        PdwFile.close();
    }
    void ImgResize(UINT Row_, UINT Col_)
    {
        Row = Row_;
        Col = Col_;
        RfMat.create(Row, Col, CV_8UC4);
        PwMat.create(Row, Col, CV_8UC4);
        PaMat.create(Row, Col, CV_8UC4);
        DoaMat.create(Row, Col, CV_8UC4);
        PriMat.create(Row, Col, CV_8UC4);
        PdwMatrix.init(Col);
    }
    void SetBackGround(_BackGround BackGround_)
    {
        BackGround = BackGround_;
    }
    void SetPdwArrayDesc(UINT PaMin = 0, UINT PaMax = 0,
                         UINT RfMin = 0, UINT RfMax = 0,
                         UINT PwMin = 0, UINT PwMax = 0)
    {
        _PdwType::PdwArrayDesc.PaMin = (PaMin == 0) ? _PdwType::PdwArrayDesc.PaMin : PaMin;
        _PdwType::PdwArrayDesc.PaMax = (PaMax == 0) ? _PdwType::PdwArrayDesc.PaMax : PaMax;
        _PdwType::PdwArrayDesc.RfMin = (RfMin == 0) ? _PdwType::PdwArrayDesc.RfMin : RfMin;
        _PdwType::PdwArrayDesc.RfMax = (RfMax == 0) ? _PdwType::PdwArrayDesc.RfMax : RfMax;
        _PdwType::PdwArrayDesc.PwMin = (PwMin == 0) ? _PdwType::PdwArrayDesc.PwMin : PwMin;
        _PdwType::PdwArrayDesc.PwMax = (PwMax == 0) ? _PdwType::PdwArrayDesc.PwMax : PwMax;
    }
    void OpenFile(const string& filepath)
    {
        Timeit.Start(0, "Function name : - OpenFile -, 共计(us): ");
        PdwFile.open(filepath);
        _PdwType::PdwArrayDescClear();                          //读取之前清零
        PdwFile.read();
        PdwAddr = PdwFile.GetDataAddr();
        ReadDataSize = PdwFile.GetReadDataSize();
        CvTools.ImageMatClear(RfMat, BackGround);
        CvTools.ImageMatClear(PwMat, BackGround);
        CvTools.ImageMatClear(PaMat, BackGround);
        CvTools.ImageMatClear(DoaMat, BackGround);
        CvTools.ImageMatClear(PriMat, BackGround);
        PdwMatrix.clear();
        Timeit.End(0);
    }
    void CloseFile()
    {
        PdwFile.close();
    }
};
#endif