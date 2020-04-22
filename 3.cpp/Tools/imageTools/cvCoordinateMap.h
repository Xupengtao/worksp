#ifndef _CVCOORDINATEMAP_H
#define _CVCOORDINATEMAP_H

#include <vector>
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "../common.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;

class _cvCoordinateMap
{
	Mat imginside;  					//传入的中间的伪彩图
	Mat imgoutside;						//整个外框
	int instartcols;					//经计算得的内图像宽开始处
	int instartrows;					//经计算得的内图像高开始处
 public:
	_cvCoordinateMap() {}
	virtual ~_cvCoordinateMap() {}
	void Inputfigure(Mat insidefigure, Mat outerframe)															//把图片及外框大小加载进入大图片，并在大框中放置图片
	{
		imginside 	= insidefigure;
		imgoutside 	= outerframe;
		instartcols	= outerframe.cols - insidefigure.cols - 10;
		instartrows	= outerframe.rows - insidefigure.rows - 50;
		for (int i = 0; i < insidefigure.rows; i++)
		{
			for (int j = 0; j < insidefigure.cols; j++)
			{
				imgoutside.at<Vec3b>(i+ instartrows,j+instartcols)= imginside.at<Vec3b>(i,j);
			}
		}
	}
	void xlabel(string xlabel_name, Scalar label_color, double x_max, double x_min, const int x_number)			//添加x轴
	{
		line(imgoutside, Point(instartcols - 1, instartrows - 1), Point(instartcols + imginside.cols , instartrows - 1), Scalar(0, 0, 0), 1, 8, 0);
		line(imgoutside, Point(instartcols - 1, instartrows + imginside.rows ), Point(instartcols + imginside.cols , instartrows + imginside.rows ), label_color, 1, 8, 0);
		int xlabel_step = imginside.cols / x_number;
		for (int x = 0; x <= x_number; x++)
		{
			double x_jhj 	= double(x*xlabel_step) / double(imginside.cols)*(x_max - x_min) + x_min;
			string result 	= tostring(x_jhj);
			line(imgoutside, Point(instartcols + x*xlabel_step, instartrows + imginside.rows), Point(instartcols + x*xlabel_step, instartrows + imginside.rows +10), label_color, 1, 8, 0);
			putText(imgoutside, result, Point(instartcols + x*xlabel_step-10, instartrows + imginside.rows +20), CV_FONT_HERSHEY_SIMPLEX, 0.3, label_color, 1, 8);
		}
		putText(imgoutside, xlabel_name, Point(instartcols + imginside.cols/2, instartrows + imginside.rows + 40), CV_FONT_HERSHEY_SIMPLEX, 0.5, label_color, 1, 8);
	}
    void ylabel(string ylabel_name, Scalar label_color, double y_max, double y_min,  const int y_number)		//添加y轴	
	{
		line(imgoutside, Point(instartcols - 1,instartrows - 1 ), Point( instartcols - 1,instartrows + imginside.rows ), label_color, 1, 8, 0);
		line(imgoutside, Point(instartcols + imginside.cols ,instartrows - 1),  Point(instartcols + imginside.cols , instartrows + imginside.rows ), Scalar(0, 0, 0), 1, 8, 0);
		int ylabel_step = imginside.rows / y_number;	
		for (int y = 0; y <= y_number; y++)
		{
			double y_jhj = double(y*ylabel_step) / double(imginside.rows)*(y_max - y_min) + y_min;
			string result = tostring(y_jhj);
			line(imgoutside, Point(instartcols - 10, instartrows + imginside.rows - y*ylabel_step), Point(instartcols - 1, instartrows + imginside.rows - y*ylabel_step), label_color, 1, 8, 0);
			putText(imgoutside, result, Point(instartcols - 30, instartrows + imginside.rows - y*ylabel_step +3), CV_FONT_HERSHEY_SIMPLEX, 0.3, label_color, 1, 8);
		}
		int baseline;
		Size text_size = cv::getTextSize(ylabel_name, CV_FONT_HERSHEY_SIMPLEX, 1, 8, &baseline);
		Mat TextSizeframe = Mat(text_size.height + 5, text_size.width/2, CV_8UC3, Scalar(255, 255, 255));
		putText(TextSizeframe, ylabel_name, Point(0, text_size.height-1), CV_FONT_HERSHEY_SIMPLEX, 0.5, label_color, 1, 8);
		Mat TextSizeframe1, TextSizeframe2;
		transpose(TextSizeframe, TextSizeframe2);
		flip(TextSizeframe2, TextSizeframe1, 0);
		for (int i = 0; i < TextSizeframe1.rows; i++)
		{
			for (int j = 0; j < TextSizeframe1.cols; j++)
			{
				imgoutside.at<Vec3b>(i + instartrows + imginside.rows/2 - 80, j + instartcols -60) = TextSizeframe1.at<Vec3b>(i, j);
			}
		}
	}
	void title(string title_name, Scalar title_color = Scalar(0, 0, 0))										//添加标题
	{
		putText(imgoutside, title_name, Point(instartcols + imginside.cols / 2 -200, instartrows - 20), CV_FONT_HERSHEY_SIMPLEX, 1, title_color, 1, 8);
	}
};
#endif // !_CVCOORDINATEMAP_H