/*
 * @Author: xpt 
 * @Date: 2020-05-16 02:04:18 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-05-16 02:31:16
 */

#ifndef ELECTROMAGNETICVISION_HPP
#define ELECTROMAGNETICVISION_HPP

#include "PdwToMat.hpp"

typedef _Pdw<UINT, UINT, UINT, UINT, UINT>      PdwType;

struct _rtnMatDesc
{
    string      MatName;
    Mat         rtnMat;
    bool        En;
    _rtnMatDesc()
    {
        MatName = "";
        En      = false;
    }
    void SetEnable(bool _En)
    {
        En      = _En;
    }
    bool GetEnable()
    {
        return En;
    }
};

struct _rtnQtFrameDesc
{
    Mat                 VideoFrameMat;
    bool                VideoFrameEn;
    int                 Progress;
    vector<_rtnMatDesc> rtnMatDescVec;
    _rtnQtFrameDesc(int rows, int cols, int type, const Scalar& s) : VideoFrameMat(rows, cols, type, s)
    {
        Progress = 0;
        rtnMatDescVec.clear();
        rtnMatDescVec.reserve(10);
    }
    void SetEnable(bool _En)
    {
        VideoFrameEn = _En;
    }
    bool GetEnable()
    {
        return VideoFrameEn;
    }
};

template <typename _FileMsgTy, typename _PdwType, typename _CvVecType>
class _ElectromagneticVision : public _PdwToMat<_FileMsgTy, _PdwType, _CvVecType>
{
    typedef             _PdwToMat<_FileMsgTy, _PdwType, _CvVecType> _PdwToMatType;

    UINT                CurrentFrameNo;                         // 当前帧数
    float               PixelToaUnit;		                    // 像素值Toa单位
    double              Threshold;                              // 像素点阈值
    int                 SizeTemp;                               // 数据文件读取长度值
    float               AddmsTime;
    UINT                ToansUnit;
    int                 readSize;
    Mat                 BGRMat;                                 // BGR
    Mat                 DensityMat;                             // 密度图，为ImageMat的alpha通道
    Mat                 DensityBinMat;                          // 二值图
    Mat                 DensityBinInvMat;                       // 二值互斥图
    Mat                 DensityDilateMat;                       // 密度膨胀图
    Mat                 BGRDensityMat;                          // 密度BGR图
    Mat                 BGRDensityDilateMat;                    // 密度BGR膨胀图
    Mat                 RemapBGRDensityMat;                     // 密度BGR映射图
    Mat                 RemapBGRDensityDilateMat;               // 密度BGR膨胀映射图
    _rtnQtFrameDesc     rtnQtFrameDesc;                         // 
    Mat&                VideoFrameMat;                          // 视频写入帧
    Mat                 VideoFrame1;                            // BGR映射图
    Mat                 VideoFrame21;                           // 密度图ParDown
    Mat                 VideoFrame22;                           // 密度膨胀图ParDown
    Mat                 VideoFrame31;                           // 颜色直方图
    Mat                 VideoFrame32;                           // 密度直方图
    Mat                 AnalysisTextMat;                        // 分析图
    Mat                 RemapBGRMat;
    Mat                 pyrDownDensityMat;
    Mat                 pyrDownDensityDilateMat;
    Mat                 BGRHistogram;
    Mat                 DensityHistogram;
    _cvCoordinateMap<>  CoordinateVideoFrame1;
    _cvCoordinateMap<>  CoordinateVideoFrame21;
    _cvCoordinateMap<>  CoordinateVideoFrame22;
    _cvCoordinateMap<>  CoordinateVideoFrame31;
    _cvCoordinateMap<>  CoordinateVideoFrame32;
    _cvTextImage<>      cvTextMat;
public:
    _ElectromagneticVision(UINT rows = 1024, UINT cols = 1024)
                         : _PdwToMatType(rows, cols),
                           BGRMat(rows, cols, CV_8UC3, cv::Scalar::all(0)),
                           DensityMat(rows, cols, CV_8UC1),
                           DensityBinMat(rows, cols, CV_8UC1),
                           DensityBinInvMat(rows, cols, CV_8UC1),
                           DensityDilateMat(rows, cols, CV_8UC1),
                           BGRDensityMat(rows, cols, CV_8UC3),
                           BGRDensityDilateMat(rows, cols, CV_8UC3),
                           RemapBGRDensityMat(rows, cols, CV_8UC3),
                           RemapBGRDensityDilateMat(rows, cols, CV_8UC3),
                           rtnQtFrameDesc(cols + 400, 3 * rows + 600, CV_8UC3, cv::Scalar::all(255)),
                           VideoFrameMat(rtnQtFrameDesc.VideoFrameMat),
                           VideoFrame1(VideoFrameMat, Rect(0, 100, rows+200, cols+200)),
                           VideoFrame21(VideoFrameMat, Rect(rows+199, 0, rows/2+200, cols/2+200)),
                           VideoFrame22(VideoFrameMat, Rect(rows+199, cols/2+199, rows/2+200, cols/2+200)),
                           VideoFrame31(VideoFrameMat, Rect(rows+rows/2+399, 0, rows/2+200, cols/2+200)),
                           VideoFrame32(VideoFrameMat, Rect(rows+rows/2+399, cols/2+199, rows/2+200, cols/2+200)),
                           AnalysisTextMat(VideoFrameMat, Rect(2 * rows +599, 200, rows, cols)),
                           CoordinateVideoFrame1(RemapBGRMat, VideoFrame1, rows, cols),
                           CoordinateVideoFrame21(pyrDownDensityMat, VideoFrame21, rows/2, cols/2),
                           CoordinateVideoFrame22(pyrDownDensityDilateMat, VideoFrame22, rows/2, cols/2),
                           CoordinateVideoFrame31(BGRHistogram, VideoFrame31, rows/2, cols/2),
                           CoordinateVideoFrame32(DensityHistogram, VideoFrame32, rows/2, cols/2),
                           cvTextMat(AnalysisTextMat, 36, 3, this->BackGround)
    {
        SizeTemp        = 0;
        CurrentFrameNo  = 0;
        readSize        = 0;
        SetToaUnit();
        SetAddTime();
        SetThreshold();
        {
            CoordinateVideoFrame1.xlabel("DOA", Scalar(100, 50, 50), 0, 1024, 10);
            CoordinateVideoFrame1.ylabel("TOA(s)", Scalar(100, 50, 50), 10, 0, 10);
            CoordinateVideoFrame1.title("DOA-TOA Analysis");
        }
        {
            CoordinateVideoFrame21.xlabel("DOA", Scalar(100, 50, 50), 0, 1024, 10);
            CoordinateVideoFrame21.ylabel("TOA(s)", Scalar(100, 50, 50), 10, 0, 10);
            CoordinateVideoFrame21.title("Density Map");
        }
        {
            CoordinateVideoFrame22.xlabel("DOA", Scalar(100, 50, 50), 0, 1024, 10);
            CoordinateVideoFrame22.ylabel("TOA(s)", Scalar(100, 50, 50), 10, 0, 10);
            CoordinateVideoFrame22.title("Density Dilate Map");
        }
        {
            CoordinateVideoFrame31.xlabel("Pixel value", Scalar(100, 50, 50), 0, 255, 10);
            CoordinateVideoFrame31.ylabel("NaN", Scalar(100, 50, 50), 0, 100, 10);
            CoordinateVideoFrame31.title("Channels Histogram");
        }
        {
            CoordinateVideoFrame32.xlabel("Pixel value", Scalar(100, 50, 50), 0, 255, 10);
            CoordinateVideoFrame32.ylabel("NaN", Scalar(100, 50, 50), 0, 500, 10);
            CoordinateVideoFrame32.title("Density Histogram");
        }
    }
    virtual ~_ElectromagneticVision()
    {
        ;
    }
    inline void SetToaUnit(CINT ToaUnit_ns = 80)
    {
        ToansUnit = ToaUnit_ns;
    }
    inline void SetAddTime(float AddTime_ms = 100)
    {
        AddmsTime = AddTime_ms;
        COUT(AddmsTime);
    }
    inline void SetThreshold(double Threshold_ = 0.0)
    {
        if(Threshold_ == 0.0)
        {
            PixelToaUnit = float(AddmsTime)/this->AddCol;
            Threshold = (PixelToaUnit<=0xFE) ? ((PixelToaUnit > 1) ? PixelToaUnit : 1) : 0xFE;
        }
        else
        {
            Threshold = Threshold_;
        }
    }
    void OpenFile(const string& filepath)
    {
        _PdwToMatType::OpenFile(filepath);
        SizeTemp        = 0;
        CurrentFrameNo  = 0;
        readSize        = this->binaryFile.GetFileLines();
    }
    _rtnQtFrameDesc& ProcessFrame()
    {
        Mat  ChannelOutMat[] = {BGRMat, DensityMat};			                // 输出通道
        int  from_to[] = {0,0,1,1,2,2,3,3};						                // 0, 1, 2 -> BGRMat; 3 -> DensityMat
        int  nPairs = 4;										                // 四通道
        this->AddTimeReadData(AddmsTime, ToansUnit);						    // 以ToaUnit_ns为单位，按AddTime_ms时进读取数据。
        this->PdwSize = this->binaryFile.GetDataValidLines();				    // 获取读取有效数据长度
        if((this->PdwSize == 0) || (SizeTemp >= readSize))
        {
            rtnQtFrameDesc.SetEnable(false);
        }
        else
        {
            rtnQtFrameDesc.SetEnable(true);
            SizeTemp += this->PdwSize;
            rtnQtFrameDesc.Progress = int(float(SizeTemp)/float(readSize)*100);
            cvTextMat.Clear();											                                        // 清空分析结果区图像
            this->PdwRfPwMapClt.NewFrameInit();
            string Str = "CurrentFrameNo : " + tostring(CurrentFrameNo);
            cvTextMat.Title(Str);
            Str = " ";
            cvTextMat.Title(Str);                                                                               // 空行
            cvTextMat.Title(Str);
            this->AddBGRAImageAndPdwMatrix(ToaDoa, 1, 0, 1.0, 0, 1, 2000, 255, 0, 10);	                        // 以时进拼接方式生成图片
            cv::mixChannels(&(this->ImageMat), 1, ChannelOutMat, 2, from_to, 4);				                // ImageMat多通道分离至ChannelOutMat中的BGRMat, DensityMat
            this->CvTools.CalcMatHistogram(BGRMat, BGRHistogram, this->BackGround);				                // 计算BGRMat的直方图BGRHistogram
            this->CvTools.CalcMatHistogram(DensityMat, DensityHistogram, this->BackGround);		                // 计算DensityMat的直方图DensityHistogram
            // Generage Image
            cv::threshold(DensityMat, DensityBinMat, Threshold, 255, THRESH_BINARY);	                        // 密度图以Threshold为最小阈值进行阈值操作生成二值图DensityBinMat
            cv::threshold(DensityMat, DensityBinInvMat, Threshold,0, THRESH_TOZERO_INV);                        // 密度图以Threshold为最大阈值进行阈值操作(高于阈值像素值设置为0)生成生成DensityBinInvMat
            cv::threshold(DensityBinInvMat, DensityBinInvMat, 0, 255, THRESH_BINARY);	                        // DensityBinInvMat以Threshold为最小阈值进行阈值操作生成二值图DensityBinInvMat
            this->CvTools.cvDilate(DensityBinMat, DensityDilateMat);					                        // DensityBinMat进行图像膨胀操作
            // Analysis
            this->PdwImageAnalysis(BGRMat, DensityDilateMat, this->PdwRfPwMapClt, cvTextMat);                   // 图像分析
            // Generage VideoFrame
            this->CvTools.MatInv(DensityMat, DensityMat);
            this->CvTools.MatInv(DensityDilateMat, DensityDilateMat);
            cv::cvtColor(DensityMat, BGRDensityMat, COLOR_GRAY2BGR);					                        // 灰度图DensityMat转BGR图BGRDensityMat以实现图像组合
            cv::cvtColor(DensityDilateMat, BGRDensityDilateMat, COLOR_GRAY2BGR);		                        // 灰度图DensityDilateMat转BGR图BGRDensityDilateMat以实现图像组合
            if(this->ImageMat.rows == this->ImageMat.cols)
            {
                this->CvTools.Remap(BGRDensityMat, RemapBGRDensityMat, this->xMapImage, this->yMapImage, this->BackGround);			    // 图像重映射
                this->CvTools.Remap(BGRDensityDilateMat, RemapBGRDensityDilateMat, this->xMapImage, this->yMapImage, this->BackGround); // 图像重映射
                cv::pyrDown(RemapBGRDensityMat, pyrDownDensityMat);						                        // 图像缩放
                cv::pyrDown(RemapBGRDensityDilateMat, pyrDownDensityDilateMat);			                        // 图像缩放
            }
            else
            {
                cv::pyrDown(BGRDensityMat, pyrDownDensityMat);
                cv::pyrDown(BGRDensityDilateMat, pyrDownDensityDilateMat);
            }
            cv::drawContours(BGRMat, this->Contours, -1, Scalar(0, 255, 0), 2, CV_AA);		                    // 在图像BGRMat上绘制轮廓组Contours
            this->CvTools.Remap(BGRMat, RemapBGRMat, this->xMapImage, this->yMapImage, this->BackGround);	    // 图像重映射
            this->PdwRfPwMapClt.EndFrameProcess(RemapBGRMat, cvTextMat);
            CurrentFrameNo++;
        }
        return rtnQtFrameDesc;
    }
};

#endif // !ELECTROMAGNETICVISION_HPP
