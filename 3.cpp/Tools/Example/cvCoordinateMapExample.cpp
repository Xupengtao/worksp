#include "../imageTools/cvCoordinateMap.h"
 
int main()
{
	Mat img = cv::imread("/home/admin/WorkSp/Analysis/Density.bmp");
	Mat dst;
	boxFilter(img, dst, IPL_DEPTH_8U, Size(3, 3), Point(-1, -1), true, BORDER_DEFAULT);
	Mat kuangjia = Mat(dst.rows+100, dst.cols+70, CV_8UC3, Scalar(255,255,255));
	_cvCoordinateMap outputplot;
	outputplot.Inputfigure(dst, kuangjia);
	string yylabel = "Height";
	string xxlabel = "Width";
	string ttitle_name = "Temperature and strain";
	outputplot.ylabel(yylabel, Scalar(0, 0, 0),500,0,10);
	outputplot.xlabel(xxlabel, Scalar(0, 0, 0), 500, 0, 10);
	outputplot.title(ttitle_name);
	//imshow("view", outputplot.imgoutside);//显示图像
	cv::imshow("view", kuangjia);
	cv::waitKey();
}