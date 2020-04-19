/*
 * @Author: xpt 
 * @Date: 2020-01-19 11:14:44 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-01-19 11:40:22
 */

#include"../imageTools/cvTextImage.hpp"

int main(int argc, char **argv)
{
    Mat srcMat(1024, 1024, CV_8UC3);
    cvTextImage<> srcTextMat(srcMat, 20, 3, 1);
    srcTextMat.Title("This is Title...");
    srcTextMat.BGRCircleText(Scalar(125, 255, 0), "This is BGRCircleText...");
    imshow("srcMat", srcMat);
    waitKey();
}