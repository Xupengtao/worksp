/*
 * @Author: xpt 
 * @Date: 2020-05-16 02:04:25 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-05-16 02:32:57
 */

#include "../PdwStruct/ElectromagneticVision.hpp"

std::string filepath("/home/admin/WorkSp/Data/");
std::string savepath("/home/admin/WorkSp/Analysis/");

int main(int argc, char **argv)
{
    _ElectromagneticVision<_xyg_z, PdwType, cv::Vec4b> ElectromagneticVision(1024, 1024);
    ElectromagneticVision.OpenFile(filepath + "z");
    while(1)
    {
        Mat procMat = ElectromagneticVision.ProcessFrame();
        imshow("procMat", procMat);
        cv::waitKey();
    }
    ElectromagneticVision.CloseFile();
}