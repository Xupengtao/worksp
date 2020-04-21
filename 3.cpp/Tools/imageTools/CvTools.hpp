/*
 * @Author: xpt 
 * @Date: 2020-04-14 15:10:52 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-04-14 16:23:50
 */

#ifndef _CVTOOLS_HPP
#define _CVTOOLS_HPP

#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

enum    _BackGround{Black = 0, White = 1};
enum    _ImageChannel{Rch = 0, Gch = 1, Bch = 2, Ach = 3};

class _CvTools
{
public:
    void ImageMatClear(Mat &MatTmp, int BackGround)                                            // 图像清空
    {
        double BackGroundVal;
        switch(BackGround)
        {
        case Black:
            BackGroundVal = 0;
            break;
        case White:
            BackGroundVal = 255;
            break;
        default:
            BackGroundVal = 0;
            break;
        }
        Scalar ScalarTmp;
        switch(MatTmp.channels())
        {
        case 1:
            ScalarTmp = Scalar(BackGroundVal); 
            break;
        case 3:
            ScalarTmp = Scalar(BackGroundVal, BackGroundVal, BackGroundVal); 
            break;
        case 4:
            ScalarTmp = Scalar(BackGroundVal, BackGroundVal, BackGroundVal, 0); 
            break;
        default:
            break;
        }
        MatTmp.setTo(ScalarTmp);
    }
    template<typename _CvVecType>
    _CvVecType& GetMatPixelRef(Mat &srcMat, UINT Row, UINT Col) {return srcMat.at<_CvVecType>(Row, Col);}
    UINT GetMatChannels(Mat &srcMat) const {return srcMat.channels();}
    void Remap(Mat& srcMat, Mat& dstMat, Mat& xMapImage, Mat& yMapImage, int BackGround)       // 图像srcMat以BackGround_为背景参数重映射至dstMat
    {
        ImageMatClear(dstMat, BackGround);
        cv::remap(srcMat, dstMat, xMapImage, yMapImage, CV_INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    }
    void MatInv(Mat& srcMat, Mat& dstMat)                                                       // 图像反色
    {
        dstMat = 0xFF - srcMat;
    }
    void MatTranslation(Mat& TranslationMat, int RTranslation, int UpTranslation, int BackGround)   // 图像位移
    {
        cv::Size MatSize  = TranslationMat.size();
        cv::Mat  TransMat = cv::Mat::zeros(2, 3, CV_32FC1);
        TransMat.at<float>(0, 0) = 1;
        TransMat.at<float>(0, 2) = RTranslation;                                // 水平平移量
        TransMat.at<float>(1, 1) = 1;
        TransMat.at<float>(1, 2) = UpTranslation;                               // 竖直平移量
        switch(BackGround)
        {
        case Black:
            cv::warpAffine(TranslationMat, TranslationMat, TransMat, MatSize, INTER_LINEAR, 0, Scalar(0, 0, 0, 0));         // 应用仿射变换
            break;
        case White:
            cv::warpAffine(TranslationMat, TranslationMat, TransMat, MatSize, INTER_LINEAR, 0, Scalar(255, 255, 255, 0));   // 应用仿射变换
            break;
        default:
            break;
        }
    }
    void cvDft(Mat &srcMat, CUINT DftType = 0)                                                  // cv一维或二维离散傅立叶变换
    {
        Mat padded;                                                                                 //以0填充输入图像矩阵
        int m = getOptimalDFTSize(srcMat.rows);
        int n = getOptimalDFTSize(srcMat.cols);
        copyMakeBorder(srcMat, padded, 0, m - srcMat.rows, 0, n - srcMat.cols, BORDER_CONSTANT, Scalar::all(0));   //填充输入图像I，输入矩阵为padded，上方和左方不做填充处理
        Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
        Mat complexI;
        merge(planes, 2, complexI);                                                                 //将planes融合合并成一个多通道数组complexI
        if(DftType == 0)                                                                            //进行傅里叶变换
        {
            dft(complexI, complexI);
        }
        else if(DftType == 1)
        {
            dft(complexI, complexI, DFT_ROWS);
        }
        else
        {
            dft(complexI, complexI);
        }
        split(complexI, planes);                                                                    //计算幅值，转换到对数尺度(logarithmic scale) => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2)) planes[0] = Re(DFT(I),planes[1] = Im(DFT(I)), 即planes[0]为实部,planes[1]为虚部
        magnitude(planes[0], planes[1], planes[0]);                                                 //planes[0] = magnitude
        Mat magI = planes[0];
        magI += Scalar::all(1);
        log(magI, magI);                                                                            //转换到对数尺度(logarithmic scale)
        magI = magI(Rect(0, 0, magI.cols&-2, magI.rows&-2));                                        //如果有奇数行或列，则对频谱进行裁剪
        if(DftType == 0)
        {
            int cx = magI.cols / 2;                                                                 //重新排列傅里叶图像中的象限，使得原点位于图像中心
            int cy = magI.rows / 2;
            Mat q0(magI, Rect(0, 0, cx, cy));                                                       //左上角图像划定ROI区域
            Mat q1(magI, Rect(cx, 0, cx, cy));                                                      //右上角图像
            Mat q2(magI, Rect(0, cy, cx, cy));                                                      //左下角图像
            Mat q3(magI, Rect(cx, cy, cx, cy));                                                     //右下角图像
            Mat tmp;                                                                                //变换左上角和右下角象限
            q0.copyTo(tmp);
            q3.copyTo(q0);
            tmp.copyTo(q3);
            q1.copyTo(tmp);                                                                         //变换右上角和左下角象限
            q2.copyTo(q1);
            tmp.copyTo(q2);
        }
        normalize(magI, magI, 0, 1, CV_MINMAX);                                                     //归一化处理，用0-1之间的浮点数将矩阵变换为可视的图像格式
        imshow("输入图像", srcMat);
        imshow("频谱图", magI);
        cv::waitKey(0);
    }
    void CalcMatHistogram(Mat &srcMat)                                                          // 计算图像通道像素值直方图，imshow方式显示
    {
        vector<Mat> BGRPlanes;
        cv::split(srcMat, BGRPlanes);
        int histSize   = 256;
        int histHeight = 400;                                                            
        int histWight  = 512;
        int binWight   = histWight / histSize;
        Mat BlueHistogram, GreenHistogram, RedHistogram;
        Mat histImage(histHeight, histWight, CV_8UC3, Scalar(0, 0, 0));
        if(srcMat.channels() >= 3)
        {
            float range[] = {1, 0xFF};
            const float *histRanges = { range };
            cv::calcHist(&BGRPlanes[0], 1, 0, Mat(), BlueHistogram , 1, &histSize, &histRanges, true, false);   // 计算直方图
            cv::calcHist(&BGRPlanes[1], 1, 0, Mat(), GreenHistogram, 1, &histSize, &histRanges, true, false);
            cv::calcHist(&BGRPlanes[2], 1, 0, Mat(), RedHistogram  , 1, &histSize, &histRanges, true, false);
            cv::normalize(BlueHistogram , BlueHistogram , 0, histHeight, NORM_MINMAX, -1, Mat());               // 归一化
            cv::normalize(GreenHistogram, GreenHistogram, 0, histHeight, NORM_MINMAX, -1, Mat());
            cv::normalize(RedHistogram  , RedHistogram  , 0, histHeight, NORM_MINMAX, -1, Mat());
            for(int i = 1; i < histSize; i++)                                                                   // 绘制直方图
            {
                cv::line(histImage, Point((i - 1) * binWight, histHeight - cvRound(BlueHistogram.at<float>(i - 1))),
                         Point(i * binWight, histHeight - cvRound(BlueHistogram.at<float>(i))), Scalar(255, 0, 0), 1, CV_AA);
                cv::line(histImage, Point((i - 1) * binWight, histHeight - cvRound(GreenHistogram.at<float>(i - 1))),
                         Point(i * binWight, histHeight - cvRound(GreenHistogram.at<float>(i))), Scalar(0, 255, 0), 1, CV_AA);
                cv::line(histImage, Point((i - 1) * binWight, histHeight - cvRound(RedHistogram.at<float>(i - 1))),
                         Point(i * binWight, histHeight - cvRound(RedHistogram.at<float>(i))), Scalar(0, 0, 255), 1, CV_AA);
            }
        }
        else
        {
            float range[] = {1, 0xFF};
            const float *histRanges = { range };
            cv::calcHist(&BGRPlanes[0], 1, 0, Mat(), BlueHistogram, 1, &histSize, &histRanges, true, false);    // 计算直方图
            cv::normalize(BlueHistogram , BlueHistogram , 0, histHeight, NORM_MINMAX, -1, Mat());               // 归一化
            for(int i = 1; i < histSize; i++)                                                                   // 绘制直方图
            {
                cv::line(histImage, Point((i - 1) * binWight, histHeight - cvRound(BlueHistogram.at<float>(i - 1))),
                         Point(i * binWight, histHeight - cvRound(BlueHistogram.at<float>(i))), Scalar(255, 0, 0), 1, CV_AA);
            }
        }
        cv::imshow("CalcMatHistogram", histImage);
        cv::waitKey();
    }
    void CalcMatHistogram(Mat &srcMat, Mat &dstHistMat, int BackGround)                         // 计算图像通道像素值直方图，并输出直方图图像至dstHistMat
    {
        vector<Mat> BGRPlanes;
        cv::split(srcMat, BGRPlanes);
        int histSize   = 256;
        if(dstHistMat.cols < histSize)
        {
            dstHistMat.create(512, 512, CV_8UC3);
        }
        int histHeight = dstHistMat.rows;                                                            
        int histWight  = dstHistMat.cols;
        int binWight   = histWight / histSize;
        ImageMatClear(dstHistMat, BackGround);
        Mat BlueHistogram, GreenHistogram, RedHistogram;
        if(srcMat.channels() >= 3)
        {
            float range[] = {1, 0xFF};
            const float *histRanges = { range };
            cv::calcHist(&BGRPlanes[0], 1, 0, Mat(), BlueHistogram , 1, &histSize, &histRanges, true, false);   // 计算直方图
            cv::calcHist(&BGRPlanes[1], 1, 0, Mat(), GreenHistogram, 1, &histSize, &histRanges, true, false);
            cv::calcHist(&BGRPlanes[2], 1, 0, Mat(), RedHistogram  , 1, &histSize, &histRanges, true, false);
            cv::normalize(BlueHistogram , BlueHistogram , 0, histHeight, NORM_MINMAX, -1, Mat());               // 归一化
            cv::normalize(GreenHistogram, GreenHistogram, 0, histHeight, NORM_MINMAX, -1, Mat());
            cv::normalize(RedHistogram  , RedHistogram  , 0, histHeight, NORM_MINMAX, -1, Mat());
            for(int i = 1; i < histSize; i++)                                                                   // 绘制直方图
            {
                cv::line(dstHistMat, Point((i - 1) * binWight, histHeight - cvRound(BlueHistogram.at<float>(i - 1))),
                         Point(i * binWight, histHeight - cvRound(BlueHistogram.at<float>(i))), Scalar(255, 0, 0), 1, CV_AA);
                cv::line(dstHistMat, Point((i - 1) * binWight, histHeight - cvRound(GreenHistogram.at<float>(i - 1))),
                         Point(i * binWight, histHeight - cvRound(GreenHistogram.at<float>(i))), Scalar(0, 255, 0), 1, CV_AA);
                cv::line(dstHistMat, Point((i - 1) * binWight, histHeight - cvRound(RedHistogram.at<float>(i - 1))),
                         Point(i * binWight, histHeight - cvRound(RedHistogram.at<float>(i))), Scalar(0, 0, 255), 1, CV_AA);
            }
        }
        else
        {
            float range[] = {1, 0xFF};
            const float *histRanges = { range };
            cv::calcHist(&BGRPlanes[0], 1, 0, Mat(), BlueHistogram, 1, &histSize, &histRanges, true, false);    // 计算直方图
            cv::normalize(BlueHistogram , BlueHistogram , 0, histHeight, NORM_MINMAX, -1, Mat());               // 归一化
            for(int i = 1; i < histSize; i++)                                                                   // 绘制直方图
            {
                cv::line(dstHistMat, Point((i - 1) * binWight, histHeight - cvRound(BlueHistogram.at<float>(i - 1))),
                         Point(i * binWight, histHeight - cvRound(BlueHistogram.at<float>(i))), Scalar(255, 0, 0), 1, CV_AA);
            }
        }
    }
    void CalcMatNormalizeHistogram(Mat &srcMat, Mat &dstHistMat, CUINT histHeight)              // 计算图像通道归一化像素值直方图，并输出直方图图像至dstHistMat
    {
        int histSize = 256;
        if(srcMat.channels() >= 3)
        {
            ERRORMSG("不可进行直方图归一化计算");
        }
        else
        {
            float range[] = {1, 0xFF};
            const float *histRanges = { range };
            cv::calcHist(&srcMat, 1, 0, Mat(), dstHistMat, 1, &histSize, &histRanges, true, false);             // 计算直方图
            cv::normalize(dstHistMat , dstHistMat , 0, histHeight, NORM_MINMAX, -1, Mat());                     // 归一化
        }
    }
    void CalcAxisHistogram(Mat &srcMat, vector<UINT> &dstHist, CUINT CalcAsRow = 0)             // 统计srcMat轴向非0像素点个数直方图，CalcAsRow: 0,列; 1,行
    {
        int histSize;
        if(srcMat.channels() != 1)
        {
            ERRORMSG("不可进行像素轴直方图计算");
        }
        if(CalcAsRow == 0)
        {
            histSize = srcMat.rows;
            dstHist.resize(histSize);
            for(UINT i = 0; i < srcMat.rows; i++)
            {
                UINT AxisPixelSum = 0;
                for(UINT j = 0; j < srcMat.cols; j++)
                {
                    if(srcMat.at<UCHAR>(i, j) != 0)
                    {
                        AxisPixelSum++;
                    }
                }
                dstHist[i] = AxisPixelSum;
            }
        }
        else
        {
            histSize = srcMat.cols;
            dstHist.resize(histSize);
            for(UINT i = 0; i < srcMat.cols; i++)
            {
                UINT AxisPixelSum = 0;
                for(UINT j = 0; j < srcMat.rows; j++)
                {
                    if(srcMat.at<UCHAR>(j, i) != 0)
                    {
                        AxisPixelSum++;
                    }
                }
                dstHist[i] = AxisPixelSum;
            }
        }
    }
    void VectorToMat(vector<UINT> &srcVec, Mat &dstMat)                                         // vector<UINT>至一维Mat转换
    {
        dstMat.create(1, srcVec.size(), CV_32SC1);
        for(int i = 0; i < srcVec.size(); i++)
        {
            dstMat.at<int>(i) = srcVec[i];
        }
    }
    void MatToVector(Mat &srcMat, vector<UINT> &dstVec)                                         // 一维Mat至vector<UINT>转换
    {
        if((srcMat.rows != 1) || (srcMat.channels() != 1))
        {
            ERRORMSG("不支持的MatToVector转换类型");
        }
        dstVec.resize(srcMat.cols);
        for(int i = 0; i < srcMat.cols; i++)
        {
            dstVec[i] = srcMat.at<int>(i);
        }
    }
    void drawHistogram(Mat &HistogramMat, vector<UINT> &srcVec, int BackGround, CUINT histHeight = 512) // 绘制srcVec直方图至HistogramMat
    {
        int histSize   = srcVec.size();
        if((HistogramMat.rows != histHeight) || (HistogramMat.cols != histSize))
        {
            HistogramMat.create(histHeight, histSize, CV_8UC3);
        }
        int histWight  = HistogramMat.cols;
        int binWight   = histWight / histSize;
        Mat srcMat;
        VectorToMat(srcVec, srcMat);
        cv::normalize(srcMat , srcMat , 0, histHeight - 10, NORM_MINMAX, -1, Mat());                // 归一化
        MatToVector(srcMat, srcVec);
        ImageMatClear(HistogramMat, BackGround);
        for(int i = 1; i < histSize; i++)                                                           // 绘制直方图
        {
            cv::line(HistogramMat, Point((i - 1) * binWight, histHeight - srcVec[i - 1]),
                     Point(i * binWight, histHeight - srcVec[i]), Scalar(255, 0, 0), 1, CV_AA);
        }
    }
    void drawSectAtHistogram(Mat &HistogramMat, vector<pair<int, int> > &BinLocs)               // HistogramMat上绘制BinLocs内的左右区间
    {
        int histHeight = HistogramMat.rows;
        int histWight  = HistogramMat.cols;
        for(int i = 0; i < BinLocs.size(); i++)
        {
            if((BinLocs[i].first < histWight) && (BinLocs[i].second < histWight))
            {
                cv::line(HistogramMat, Point(BinLocs[i].first, histHeight - 1),
                         Point(BinLocs[i].first, 0), Scalar(0, 255, 255), 1, CV_AA);
                cv::line(HistogramMat, Point(BinLocs[i].second, histHeight - 1),
                         Point(BinLocs[i].second, 0), Scalar(0, 255, 255), 1, CV_AA);
            }
            else
            {
                ERRORMSG("绘制位置超出图像范围！");
                break;
            }
        }
    }
    void drawRectAtMat(Mat &srcMat, Rect &drawRect)                                             // srcMat上绘制drawRect矩形框
    {
        cv::line(srcMat,
                 Point(drawRect.x, drawRect.y),
                 Point(drawRect.x + drawRect.width, drawRect.y),
                 Scalar(255, 0, 0), 2, CV_AA);
        cv::line(srcMat,
                 Point(drawRect.x, drawRect.y),
                 Point(drawRect.x, drawRect.y + drawRect.height),
                 Scalar(255, 0, 0), 2, CV_AA);
        cv::line(srcMat,
                 Point(drawRect.x + drawRect.width, drawRect.y),
                 Point(drawRect.x + drawRect.width, drawRect.y + drawRect.height),
                 Scalar(255, 0, 0), 2, CV_AA);
        cv::line(srcMat,
                 Point(drawRect.x, drawRect.y + drawRect.height),
                 Point(drawRect.x + drawRect.width, drawRect.y + drawRect.height),
                 Scalar(255, 0, 0), 2, CV_AA);
    }
    void drawRectanglesAtMat(Mat &srcMat,                                                       // srcMat上绘制srcRectangles以RectangleNos为标号的矩形框图像输出至dstMat
                             Mat &dstMat,
                             vector<Rect> &srcRectangles,
                             vector<UINT> &RectangleNos)
    {
        UINT dstMat_X1 = srcRectangles[RectangleNos[0]].x, dstMat_Y1 = srcRectangles[RectangleNos[0]].y;
        UINT dstMat_X2 = srcRectangles[RectangleNos[0]].x + srcRectangles[RectangleNos[0]].width;
        UINT dstMat_Y2 = srcRectangles[RectangleNos[0]].y + srcRectangles[RectangleNos[0]].height;
        for(UINT i = 0; i < RectangleNos.size(); i++)
        {
            Rect &RectTmp = srcRectangles[RectangleNos[i]];
            dstMat_X1 = (dstMat_X1 < RectTmp.x) ? dstMat_X1 : RectTmp.x;
            dstMat_Y1 = (dstMat_Y1 < RectTmp.y) ? dstMat_Y1 : RectTmp.y;
            dstMat_X2 = (dstMat_X2 > RectTmp.x + RectTmp.width ) ? dstMat_X2 : RectTmp.x + RectTmp.width;
            dstMat_Y2 = (dstMat_Y2 > RectTmp.y + RectTmp.height) ? dstMat_Y2 : RectTmp.y + RectTmp.height;
        }
        dstMat = srcMat(Rect(dstMat_X1, dstMat_Y1, dstMat_X2 - dstMat_X1, dstMat_Y2 - dstMat_Y1)).clone();
        ImageMatClear(dstMat, 0);
        Mat srcROIRectMat, dstROIRectMat;
        for(int i = 0; i < RectangleNos.size(); i++)
        {
            Rect &RectTmp = srcRectangles[RectangleNos[i]];
            srcROIRectMat = srcMat(RectTmp);
            dstROIRectMat = dstMat(Rect(RectTmp.x - dstMat_X1, RectTmp.y - dstMat_Y1, RectTmp.width, RectTmp.height));
            srcROIRectMat.copyTo(dstROIRectMat);
        }
    }
    void drawRectAreaAtMatChannel(Mat &srcMat, UINT Row, UINT RowRange, UINT Col, UINT ColRange, UCHAR Val, UINT ChannelNo)
    {
        UINT RowSt = ((Row > RowRange) ? (Row - RowRange) : 0);
        UINT RowEd = ((Row + RowRange) < srcMat.rows ? (Row + RowRange) : srcMat.rows);
        UINT ColSt = ((Col > ColRange) ? (Col - ColRange) : 0);
        UINT ColEd = ((Col + ColRange) < srcMat.cols ? (Col + ColRange) : srcMat.cols);
        for(int i = RowSt; i <= RowEd; i++)
        {
            for(int j = ColSt; j <= ColEd; j++)
            {
                srcMat.at<cv::Vec4b>(i, j)[ChannelNo] = Val;
            }
        }
    }
    cv::Scalar CalcMatRectangleMean(Mat &srcMat)                                                // 计算srcMat的像素平均值
    {
        if(srcMat.channels() != 4)
        {
            ERRORMSG("不支持的srcMat类型!");
        }
        UINT Nums = 0, BSum = 0, GSum = 0, RSum = 0, ASum = 0;
        for(int i = 0; i < srcMat.rows; i++)
        {
            for(int j = 0; j < srcMat.cols; j++)
            {
                if(srcMat.at<cv::Vec4b>(i, j)[3] > 0)
                {
                    Nums++;
                    BSum += srcMat.at<cv::Vec4b>(i, j)[0];
                    GSum += srcMat.at<cv::Vec4b>(i, j)[1];
                    RSum += srcMat.at<cv::Vec4b>(i, j)[2];
                    ASum += srcMat.at<cv::Vec4b>(i, j)[3];
                }
            }
        }
        CV_Assert(Nums != 0);
        return cv::Scalar(BSum/Nums, GSum/Nums, RSum/Nums, ASum/Nums);
    }
    void CalcMatRectanglesMean(Mat &srcMat,                                                     // 计算srcMat上srcRectangles以RectangleNos为标号的矩形区域内像素平均值
                                     vector<pair<UINT, cv::Scalar> > &dstRectangleMeans,
                                     vector<Rect> &srcRectangles,
                                     vector<UINT> &RectangleNos)
    {
        Mat     ROIMat;
        pair<UINT, cv::Scalar> RectanglesMeanTmp;
        dstRectangleMeans.resize(0);
        for(int i = 0; i < RectangleNos.size(); i++)
        {
            ROIMat = srcMat(srcRectangles[RectangleNos[i]]);
            RectanglesMeanTmp.first = RectangleNos[i];
            RectanglesMeanTmp.second = CalcMatRectangleMean(ROIMat);
            dstRectangleMeans.push_back(RectanglesMeanTmp);
        }
    }
    void findSectAtHistogram(vector<UINT> &srcVec,                                              // 检测直方图上所有符合设置参数区间至BinLocs
                                   vector<pair<int, int> > &BinLocs,
                                   CUINT Threshold,                                                     // 阈值高于Threshold
                                   CUINT BinMinInterval,                                                // 间隔大于BinMinInterval
                                   CUINT BinMinSize = 1)                                                // 长度大于BinMinSize
    {
        UINT edTmp = 0;
        bool edHasTag = false;
        pair<int, int> BinLocTmp(0, 0);
        BinLocs.resize(0);
        vector<UINT> ThresholdLoc;
        ThresholdLoc.resize(0);
        UINT srcSize = srcVec.size();
        for(UINT i = 0; i < srcSize; i++)															// 将srcVec中所有大于阈值Threshold的数据位置写入ThresholdLoc中
        {
            if(srcVec[i] >= Threshold)
            {
                ThresholdLoc.push_back(i);
            }
        }
        UINT ThresholdLocSize = ThresholdLoc.size();
        for(UINT st = 0; st < ThresholdLocSize; st++)
        {
            BinLocTmp.first = ThresholdLoc[st];														// 设置当前间隔起始位置
            edTmp = st;
            edHasTag = false;																		// 当前间隔有结尾标志
            for(UINT ed = st+1; ed < ThresholdLocSize; ed++)
            {
                if(ThresholdLoc[ed] - ThresholdLoc[edTmp] < BinMinInterval)							// 相邻ThresholdLoc间隔小于BinMinInterval
                {
                    edTmp = ed;
                    continue;
                }
                else
                {
                    if(ThresholdLoc[edTmp] - ThresholdLoc[st] >= BinMinSize)						// 起始ThresholdLoc间隔大于等于BinMinSize
                    {
                        edHasTag = true;															// 设置当前间隔有结尾标志
                        BinLocTmp.second = ThresholdLoc[edTmp];										// 设置当前间隔有结尾标志
                        BinLocs.push_back(BinLocTmp);												// 将BinLocTmp记录至BinLocs中
                    }
                    st = ed-1;
                    break;
                }
            }
            if(edHasTag == false)																	// 读尾未结束判断
            {
                if(ThresholdLoc[edTmp] - ThresholdLoc[st] >= BinMinSize)
                {
                    edHasTag = true;
                    BinLocTmp.second = ThresholdLoc[edTmp];
                    BinLocs.push_back(BinLocTmp);
                    st = edTmp;
                }
            }
        }
    }
    void cvGaussianBlur(Mat &srcMat, Mat &dstMat, CUINT GaussKernelSize = 5)                    // cv高斯模糊
    {
        cv::GaussianBlur(srcMat, dstMat, Size(GaussKernelSize, GaussKernelSize), GaussKernelSize);
    }
    void cvDilate(Mat &srcMat, Mat &dstMat, CUINT DilateSize = 2)                               // cv图像膨胀
    {
        UINT s = DilateSize * 2 + 1;
        Mat structureElement = cv::getStructuringElement(MORPH_RECT, Size(s, s), Point(-1, -1));    //创建结构元
        cv::dilate(srcMat, dstMat, structureElement, Point(-1, -1), 1);
    }
    void cvAddWeighted(Mat& srcMat1, Mat& srcMat2, Mat& dstMat, double alpha)                   // cv图像叠加
    {
        if(!srcMat1.data)
        {
            ERRORMSG("!srcMat1.data Error!");
            return;
        }
        if(!srcMat2.data)
        {
            ERRORMSG("！srcMat2.data Error!");
            return;
        }
        if(srcMat1.channels() != srcMat2.channels())
        {
            ERRORMSG("srcMat1.channels != srcMat2.channels Error!");
            return;
        }
        if(srcMat1.rows != srcMat2.rows || srcMat1.cols != srcMat2.cols)
        {
            ERRORMSG("srcMat1.rows != srcMat2.rows || srcMat1.cols != srcMat2.cols Error!");
        }
        cv::addWeighted(srcMat1, alpha, srcMat2, 1 - alpha, 0.0, dstMat);
    }
    void cvVconcat(Mat &srcMat1, Mat &srcMat2, Mat &dstMat)                                     // cv图像水平拼接
    {
        cv::vconcat(srcMat1, srcMat2, dstMat);
    }
    void cvHconcat(Mat &srcMat1, Mat &srcMat2, Mat &dstMat)                                     // cv图像垂直拼接
    {
        cv::hconcat(srcMat1, srcMat2, dstMat);
    }
    void MutiChannelRemap(Mat &srcMat, Mat &dstMat, CUINT ChannelNoasX)                         // 降维多通道映射
    {
        dstMat = cv::Mat(256, srcMat.cols, CV_8UC3, Scalar::all(0));
        cv::Vec3b VecTmp(0, 0, 0);
        if(ChannelNoasX >= srcMat.channels())
        {
            ERRORMSG("不支持的转换类型!");
            
        }
        if(srcMat.channels() == 3)
        {
            cv::Vec3b srcVec(0, 0, 0);
            for(UINT i = 0; i < srcMat.rows; i++)
            {
                for(UINT j = 0; j < srcMat.cols; j++)
                {
                    srcVec = srcMat.at<cv::Vec3b>(i, j);
                    UINT m = 0;
                    for(UINT n = 0; n < 3; n++)
                    {
                        if(n != ChannelNoasX)
                        {
                            VecTmp[m++] = srcVec[n];
                        }
                    }
                    dstMat.at<cv::Vec3b>(srcMat.at<cv::Vec3b>(i, j)[ChannelNoasX], j) = VecTmp;
                }
            }
        }
        else if(srcMat.channels() == 4)
        {
            cv::Vec4b srcVec(0, 0, 0, 0);
            for(UINT i = 0; i < srcMat.rows; i++)
            {
                for(UINT j = 0; j < srcMat.cols; j++)
                {
                    srcVec = srcMat.at<cv::Vec4b>(i, j);
                    UINT m = 0;
                    for(UINT n = 0; n < 4; n++)
                    {
                        if(n != ChannelNoasX)
                        {
                            VecTmp[m++] = srcVec[n];
                        }
                    }
                    dstMat.at<cv::Vec3b>(srcMat.at<cv::Vec4b>(i, j)[ChannelNoasX], j) = VecTmp;
                }
            }
        }
        else
        {
            ERRORMSG("不支持的转换类型!");
        }
    }
    void DoubleChannelRemap(Mat &srcMat, Mat &dstMat, CUINT ChannelNoasX, CUINT ChannelNoasValue)   // 降维双通道映射
    {
        dstMat = cv::Mat(256, srcMat.cols, CV_8UC1, Scalar::all(0));
        if((ChannelNoasX >= srcMat.channels()) || (ChannelNoasValue >= srcMat.channels()))
        {
            ERRORMSG("不支持的转换类型!");
            
        }
        if(srcMat.channels() == 3)
        {
            for(UINT i = 0; i < srcMat.rows; i++)
            {
                for(UINT j = 0; j < srcMat.cols; j++)
                {
                    dstMat.at<UCHAR>(srcMat.at<cv::Vec3b>(i, j)[ChannelNoasX], j) = srcMat.at<cv::Vec3b>(i, j)[ChannelNoasValue];
                }
            }
        }
        else if(srcMat.channels() == 4)
        {
            for(UINT i = 0; i < srcMat.rows; i++)
            {
                for(UINT j = 0; j < srcMat.cols; j++)
                {
                    dstMat.at<UCHAR>(srcMat.at<cv::Vec4b>(i, j)[ChannelNoasX], j) = srcMat.at<cv::Vec4b>(i, j)[ChannelNoasValue];
                }
            }
        }
        else
        {
            ERRORMSG("不支持的转换类型!");
        }
    }
};

#endif // ！_CVTOOLS_HPP