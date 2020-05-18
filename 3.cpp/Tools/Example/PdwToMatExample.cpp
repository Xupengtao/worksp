/*
 * @Author: xpt 
 * @Date: 2020-01-12 01:15:06 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-04-14 16:31:36
 */

#include "../PdwStruct/PdwToMat.hpp"

UINT 	_RadarBuffer::GlobalToaH = 0;
UINT 	_RadarBuffer::GlobalToaL = 0;
UINT 	_RadarBuffer::RadarRCCLNum = 0;
UINT	_RadarBuffer::SortTimes = 0;

typedef _Pdw<UINT, UINT, UINT, UINT, UINT>      PdwType;

std::string filepath("/home/admin/WorkSp/Data/");
std::string savepath("/home/admin/WorkSp/Analysis/");

int main(int argc, char **argv)
{
    // _PdwToMat<_fileMsg, PdwType, cv::Vec4b> PdwToMat1(filepath + "文件32.data", 1024, 4096);
    // PdwToMat1.ImageResize(1024, 1024)
    //          .RemapImageResize(1024, 1024)
    //          .ReadDataResize(26000000)
    //          .ReadData()
    //          .SetBackGround(White)
    //          .SetPdwArrayDesc(0, 0, 0, 0, 0, 255000)
    //          .GenImage()
    //          .GenBGRAImage(1, 0)
    //          .SaveBGRAPolarImage(savepath + "Polar.bmp", 1, 0, CV_PI/512, 100, 1000)
    //          .SaveDensityImage(savepath + "Density.bmp")
    //          .SaveImage(savepath + "文件32.bmp")
    //          .AddSaveVideo(savepath + "文件32.avi", 0, 100, 80, 1000000,25000000);
    // _PdwToMat<_xyg_z, PdwType, cv::Vec4b> PdwToMat2(filepath + "z", 1024, 1024);
    // PdwToMat2.ReadData(100)
    //          .showData(0,100)
    //          .SetBackGround(White)
    //          .GenImage()
    //         //  .showImage()
    //          .ImageMatTranslation(100,0)
    //         //  .showImage()
    //          .GenBGRAImage(10,0)
    //         //  .showImage()
    //          .GenBGRAImageAndPdwMatrix(100, 0)
    //         //  .showPdwMatrix()
    //         //  .showImage()
    //          .Remap()
    //         //  .showRemapImage()
    //          .SaveBGRAPolarImage(savepath + "Polar_z_1.bmp", 1, 0, CV_PI/512, 100, 1000)
    //          .SaveDensityImage(savepath + "Density_z.bmp")
    //          .SaveImage(savepath + "z.bmp")
    //          .ContourAnalyVideo(savepath + "AddSaveHistogramVideo_z1.avi", 0, 100);
	// _LocVec	AzElDis;
	// AzElDis.resize(100);
	// for(UINT i = 0; i < 100; i++)
	// {
	// 	AzElDis[i].Az 		   = 100+i;
	// 	AzElDis[i].El 		   = i;
	// 	AzElDis[i].Distance    = i;
    //     AzElDis[i].JitterRange = i;
	// 	AzElDis[i].SortTimes   = i;
	// }
	// _iRadarSimPro<PdwType> iRadarSimPro;
	// /* 单通道 3 载频捷变重频固定  */
	// iRadarSimPro.PriTypeModify();
	// iRadarSimPro.RfTypeModify(RF_FIX,1000);
	// iRadarSimPro.PwTypeModify(PW_FIX,50);
	// size_t Tmp[] = {11,12,13,14,15,16,17,18,19};
	// iRadarSimPro.RadarModify(Tmp,9);
	// iRadarSimPro.GetPlatForm(1).SetAzElDis(AzElDis);
	// iRadarSimPro.run_onetimes();
	// iRadarSimPro.CoutStatus();
	// iRadarSimPro.ShowOutBuffer(0,100);
    // _PdwToMat<_xyg_z, PdwType, cv::Vec4b> PdwToMat3(iRadarSimPro, 1024, 1024);
    // PdwToMat3.SetBackGround(White).ContourAnalyVideo(savepath + "AddSaveHistogramVideo_z1.avi", 0, 100);
    _PdwToMat<_xyg_z, PdwType, cv::Vec4b> PdwToMat4(filepath + "iRadarSimProGenData.data", 1024, 1024);
    PdwToMat4.ReadData(100)
             .showData(0,100)
             .SetBackGround(White)
             .GenImage()
            //  .showImage()
             .ImageMatTranslation(100,0)
            //  .showImage()
             .GenBGRAImage(10,0)
            //  .showImage()
             .GenBGRAImageAndPdwMatrix(100, 0)
            //  .showPdwMatrix()
            //  .showImage()
             .Remap()
            //  .showRemapImage()
             .SaveBGRAPolarImage(savepath + "Polar_z_1.bmp", 1, 0, CV_PI/512, 100, 1000)
             .SaveDensityImage(savepath + "Density_z.bmp")
             .SaveImage(savepath + "z.bmp")
             .ContourAnalyVideo(savepath + "AddSaveHistogramVideo_z1.avi", 0, 100);
    return 1;
}