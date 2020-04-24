#include "../imageTools/cvCoordinateMap.hpp"
 
int main()
{
	Mat readimg = cv::imread("/home/admin/WorkSp/Analysis/Density.bmp");
	Mat insidemat;
	Mat kuangjia = Mat(readimg.rows+200, readimg.cols+200, CV_8UC3, Scalar(255,255,255));
	_cvCoordinateMap<> outputplot(insidemat, kuangjia, readimg.rows, readimg.cols);
	cv::addWeighted(insidemat, 0.1, readimg, 0.9, 0.0, insidemat);
	string yylabel = "Height";
	string xxlabel = "Width";
	string ttitle_name = "Temperature and strain";
	outputplot.ylabel(yylabel, Scalar(100, 50, 50), 0, 500, 10);
	outputplot.xlabel(xxlabel, Scalar(100, 50, 50), 0, 500, 10);
	outputplot.title(ttitle_name);
	//imshow("view", outputplot.imgoutside);//显示图像
	cv::imshow("view", kuangjia);
	cv::waitKey();
}