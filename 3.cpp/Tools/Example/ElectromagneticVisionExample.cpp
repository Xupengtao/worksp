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
    ElectromagneticVision.OpenFile(filepath + "iRadarSimProGenData.data");
    while(1)
    {
        static float i = 1;
        // ElectromagneticVision.SetAddTime(0.1 * i);
        i++;
        _rtnQtFrameDesc& rtnQtFrameDesc = ElectromagneticVision.ProcessFrame();
        if(rtnQtFrameDesc.GetEnable())
        {
            imshow("VideoFrameMat", rtnQtFrameDesc.VideoFrameMat);
            COUT(rtnQtFrameDesc.Progress);
            cv::waitKey();
        }
        else
        {
            break;
        }
    }
    ElectromagneticVision.CloseFile();
}