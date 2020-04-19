/*
 * @Author: xpt 
 * @Date: 2020-01-19 09:26:11 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-04-14 15:11:37
 */

#ifndef _CVTEXT_IMAGE_HPP
#define _CVTEXT_IMAGE_HPP

#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
#include "../Timeit.h"

template<CUINT fontFaceDefault>
struct _cvTextFont
{
    int     fontFace;
    double  fontScale;
    Scalar  color;
    int     thickness;
    int     lineType;
    bool    bottomLeftOrigin;

    _cvTextFont(double fontScale_, Scalar color_, int thickness_)
          :fontFace(fontFaceDefault), fontScale(fontScale_), color(color_), thickness(thickness_)
    {
        lineType = LINE_AA;
        bottomLeftOrigin = false;
    }
    virtual ~_cvTextFont(){};
};

template<CUINT FontFace = cv::FONT_HERSHEY_COMPLEX_SMALL>
class _cvTextImage
{
private:
    cv::Mat &TextImage;
    int rows;
    int cols;
    UINT lineRows;
    UINT lineCols;
    UINT lineHeight;
    UINT lineWidth;
    UINT lineX;
    UINT lineY;
    CUINT BackGround;
    bool TextImageWrTag;
    _cvTextFont<FontFace> TitleFont;
    _cvTextFont<FontFace> TextFont;

public:
    _cvTextImage(cv::Mat &srcMat, UINT lineRows_, UINT lineCols_, CUINT BackGround_)
                :TextImage(srcMat), lineRows(lineRows_), lineCols(lineCols_), BackGround(BackGround_),
                 TitleFont(0.7, Scalar(255, 0, 0), 1),
                 TextFont(0.6, Scalar(100, 60, 128), 1)
    {
        CV_Assert((rows = TextImage.rows) != 0);
        CV_Assert((cols = TextImage.cols) != 0);
        CV_Assert((lineRows != 0) && (lineCols != 0));
        lineHeight = rows/lineRows;
        lineWidth  = cols/lineCols;
        if(TextImage.channels() != 3)
        {
            TextImage.create(rows, cols, CV_8UC3);
        }
        Clear();
    }
    virtual ~_cvTextImage(){};
    void Clear()
    {
        lineX = 20;
        lineY = 20;
        TextImageWrTag = true;
        Scalar ScalarTmp;
        switch(BackGround)
        {
        case 0:
            ScalarTmp = Scalar(0, 0, 0);
            break;
        case 1:
            ScalarTmp = Scalar(255, 255, 255);
            break;
        default:
            break;
        }
        TextImage.setTo(ScalarTmp);
    }
    void SetLine(UINT lineRows_, UINT lineCols_)
    {
        lineRows = lineRows_;
        lineCols = lineCols_;
        CV_Assert((lineRows != 0) && (lineCols != 0));
        lineHeight = rows/lineRows;
        lineWidth  = cols/lineCols;
        Clear();
    }
    void CoordinateAdjust()
    {
        lineY += lineHeight;
        if(lineY/lineHeight >= lineRows)
        {
            lineY = 20;
            lineX += lineWidth;
            if(lineX/lineWidth >= lineCols)
            {
                TextImageWrTag = false;
            }
        }
    }
    void Title(const cv::String &text)
    {
        if(TextImageWrTag == true)
        {
            cv::putText(TextImage,
                        text,
                        Point(lineX, lineY), 
                        TitleFont.fontFace,
                        TitleFont.fontScale,
                        TitleFont.color,
                        TitleFont.thickness,
                        TitleFont.lineType,
                        TitleFont.bottomLeftOrigin);
            CoordinateAdjust();
        }
    }
    void BGRCircleText(Scalar CircleColor, const cv::String &text, int radius = 5)
    {
        if(TextImageWrTag == true)
        {
            cv::circle(TextImage,
                       Point(lineX, lineY - radius),
                       radius,
                       CircleColor,
                       -1,
                       LINE_AA);
            cv::putText(TextImage,
                        text,
                        Point(lineX + 2*radius, lineY), 
                        TextFont.fontFace,
                        TextFont.fontScale,
                        TextFont.color,
                        TextFont.thickness,
                        TextFont.lineType,
                        TextFont.bottomLeftOrigin);
            CoordinateAdjust();
        }
    }
    void showImage()
    {
        cv::imshow("TextImage", TextImage);
        cv::waitKey();
    }
};

#endif // !_CVTEXT_IMAGE_HPP