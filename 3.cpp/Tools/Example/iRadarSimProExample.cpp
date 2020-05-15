#include "../iRadarSimPro/iRadarSimPro.h"
#include "../PdwStruct/PdwStruct.hpp"
#include "../fileStream/binaryFileStream.hpp"

UINT 	_RadarBuffer::GlobalToaH = 0;
UINT 	_RadarBuffer::GlobalToaL = 0;
UINT 	_RadarBuffer::RadarRCCLNum = 0;
UINT	_RadarBuffer::SortTimes = 0;
typedef _Pdw<UINT, UINT, UINT, UINT, UINT>      PdwType;

int main()
{
	_LocVec	AzElDis0, AzElDis1;
	AzElDis0.resize(100);
	for(UINT i = 0; i < 100; i++)
	{
		AzElDis0[i].Az 		  	= 100 + i;
		AzElDis0[i].El 		  	= 100 + i;
		AzElDis0[i].Distance  	= 100 + i;
		AzElDis0[i].JitterRange	= 10;
		AzElDis0[i].SortTimes 	= i;
	}
	AzElDis1.resize(10);
	for(UINT i = 0; i < 10; i++)
	{
		AzElDis1[i].Az 		  	= i * 10;
		AzElDis1[i].El 		  	= i * 10;
		AzElDis1[i].Distance  	= i * 10;
		AzElDis1[i].JitterRange	= 10;
		AzElDis1[i].SortTimes 	= i;
	}
	_iRadarSimPro<PdwType> iRadarSimPro, iRadarSimPro1;
	/* 单通道 3 载频固定重频固定  */
	iRadarSimPro.PriTypeModify(PRI_FIX,1000);
	iRadarSimPro.GetPlatForm(0).SetAzElDis(AzElDis0);
	iRadarSimPro.GetPlatForm(0).SetRadarStTime(0, 10);
	iRadarSimPro.RadarModify(0);
	// iRadarSimPro.PriTypeModify(PRI_FIX,3223);
	// iRadarSimPro.RadarModify(1);
	// iRadarSimPro.PriTypeModify(PRI_FIX,5215);
	// iRadarSimPro.RadarModify(2);
	/* 单通道 1 载频固定重频参差  */
	// size_t JaggingArr[] = {1000,2000};
	// iRadarSimPro.PriTypeModify(PRI_JAGGING,0,0,3,1,JaggingArr,2);
	// iRadarSimPro.RadarModify(3);
	/* 单通道 1 载频固定重频抖动  */
	// iRadarSimPro.PriTypeModify(PRI_DITHER,1000,100);
	// iRadarSimPro.RadarModify(4);
	/* 单通道 10 载频固定重频固定  */
	iRadarSimPro.PriTypeModify();
	size_t Tmp[] = {11,12,13,14,15,16,17,18,19};
	iRadarSimPro.RadarModify(Tmp,9);
	iRadarSimPro.GetPlatForm(1).SetAzElDis(AzElDis1);
	/* 单通道 3 载频捷变重频固定  */
	// iRadarSimPro.PriTypeModify(PRI_FIX,1000);
	// size_t AgiliArr[] = {0x1210,0x1218,0x1220,0x1228,0x1230,0x1238,0x1240,0x1249};
	// iRadarSimPro.RfTypeModify(RF_AGILITY,0x1200,8,1,AgiliArr,8);
	// iRadarSimPro.RadarModify(20);
	iRadarSimPro.run_onetimes();
	iRadarSimPro.CoutStatus();
	iRadarSimPro.ShowOutBuffer(5000,6000,11);
    iRadarSimPro1.PriTypeModify(PRI_FIX,1000);
	iRadarSimPro1.GetPlatForm(0).SetAzElDis(AzElDis0);
	// iRadarSimPro1.GetPlatForm(0).SetRadarStTime(0, 10);
	iRadarSimPro1.RadarModify(0);
	iRadarSimPro1.WriteFile("/home/admin/WorkSp/Data/iRadarSimProGenData.data", 1000);
    _binaryFile<PdwType, PdwType> binaryFile("/home/admin/WorkSp/Data/iRadarSimProGenData.data", 0, 9965650);
    binaryFile.showFileHeader();
    binaryFile.showStatus();
    binaryFile.read();
    binaryFile.registShowDataFunc(PrintPdw<PdwType>);
    binaryFile.showData(0, 100);
    binaryFile.showStatus();
    binaryFile.close();
	return 1;
}