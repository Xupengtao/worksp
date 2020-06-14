#include"../Tools/Error.h"
#include"../Tools/Macro.h"
#include"../Tools/XptType.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

UINT GetRCCLNum(UINT Num)
{
    UINT i;
    for(i = 0; Num > 1; i++)
    {
        Num = Num>>1;
    }
    return i;
}

class _CaseA
{
    int a;
public:
    _CaseA(int m):a(m){};
};

#define UCHAR_CYCLE_SUB(x, y)  (((x) >= (y)) ? ((x) - (y)) : (0x100 + (x) - (y)))

inline UCHAR ucharCycleSub(UCHAR x, UCHAR y)
{
    return (x > y) ? (x - y) : (0x100 + x - y);
}

void cvAddWeighted(Mat& srcMat1, Mat& srcMat2, Mat& dstMat, double alpha)
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

void drawFindNewTarget(Mat& srcMat, Mat& srcFindNewTargetMat, UINT row, UINT col, UINT FindNewTargetNums)
{
    Mat srcRectMat = srcMat(Rect(row, col, srcFindNewTargetMat.cols, srcFindNewTargetMat.rows));
    cvAddWeighted(srcFindNewTargetMat, srcRectMat, srcRectMat, 0.7);
    string Str = tostring(FindNewTargetNums);
    cv::putText(srcRectMat, Str, Point(68, 86),
                cv::FONT_HERSHEY_COMPLEX_SMALL, 0.9,
                Scalar(50, 100, 200), 1.5, LINE_AA, false);      
}


struct _QtPriPara
{
    size_t   dPri       = 10000;
    size_t   dRange     = 5000;
    size_t   nPriNum    = 10;
    size_t   nGroupNum 	= 1;
    size_t   PriArrLen 	= 0;
    size_t   dPriStep   = 1;
    size_t   PriArr[10] = {0};
};

int main()
{
    /**
     *  测试new数组类对象
     */
    // COUT(GetRCCLNum(0x1000000));
    // COUT(sizeof(UINT));
    // _CaseA *CaseA = new _CaseA[2]{(1), (2)};

    /**
     * 测试UCHAR_CYCLE_SUB
     */
    // COUT(UCHAR_CYCLE_SUB(7, 7));
    // COUT(UINT(ucharCycleSub(7, 7)));

    /** 20200421
     * 测试OpenCV读取图片以Rect加载图片至背景上
     */
    // Mat srcMat(1024, 1024, CV_8UC3);
    // Mat FindNewTargetMat = cv::imread("/home/admin/WorkSp/3.cpp/Tools/Imagesrc/FindNewTarget.png", cv::IMREAD_COLOR);
    // drawFindNewTarget(srcMat, FindNewTargetMat, 180, 360, 6);
    // cv::imshow("FindNewTargetMat", FindNewTargetMat);
    // cv::imshow("srcMat", srcMat);
    // cv::waitKey();

    /** 20200526
     * 测试结构体初始赋值
     */
    _QtPriPara QtPriPara;
    COUT(QtPriPara.dPri);
    return 1;
}