#include "../iRadarSimPro/iRadarSimPro.h"
#include "../PdwStruct/PdwStruct.hpp"
#include "../fileStream/binaryFileStream.hpp"

UINT 	_RadarBuffer::GlobalToaH    = 0;
UINT 	_RadarBuffer::GlobalToaL    = 0;
UINT 	_RadarBuffer::RadarRCCLNum  = 0;
UINT	_RadarBuffer::SortTimes     = 0;

typedef _Pdw<UINT, UINT, UINT, UINT, UINT>      PdwType;
typedef _iRadar_Xyg iRadarXygType;

int main()
{
	_LocVec	AzElDis[6];
	AzElDis[0].resize(100);
	for(UINT i = 0; i < 100; i++)
	{
		AzElDis[0][i].Az 		  	= 100 + i;
		AzElDis[0][i].El 		  	= 100 + i;
		AzElDis[0][i].Distance  	= 100 + i;
		AzElDis[0][i].JitterRange	= 10;
		AzElDis[0][i].SortTimes 	= i;
	}
	AzElDis[1].resize(100);
	for(UINT i = 0; i < 100; i++)
	{
		AzElDis[1][i].Az 		  	= i;
		AzElDis[1][i].El 		  	= i;
		AzElDis[1][i].Distance  	= i;
		AzElDis[1][i].JitterRange	= 10;
		AzElDis[1][i].SortTimes 	= i;
	}
	AzElDis[2].resize(100);
	for(UINT i = 0; i < 100; i++)
	{
		AzElDis[2][i].Az 		  	= 200 + i * 2;
		AzElDis[2][i].El 		  	= 200 + i * 2;
		AzElDis[2][i].Distance  	= 200 + i * 2;
		AzElDis[2][i].JitterRange	= 5;
		AzElDis[2][i].SortTimes 	= i;
	}
	AzElDis[3].resize(100);
	for(UINT i = 0; i < 100; i++)
	{
		AzElDis[3][i].Az 		  	= 300 + i * 3;
		AzElDis[3][i].El 		  	= 300 + i * 3;
		AzElDis[3][i].Distance  	= 300 + i * 3;
		AzElDis[3][i].JitterRange	= 6;
		AzElDis[3][i].SortTimes 	= i;
	}
	AzElDis[4].resize(100);
	for(UINT i = 0; i < 100; i++)
	{
		AzElDis[4][i].Az 		  	= 500 + i;
		AzElDis[4][i].El 		  	= 500 + i;
		AzElDis[4][i].Distance  	= 500 + i;
		AzElDis[4][i].JitterRange	= 10;
		AzElDis[4][i].SortTimes 	= i;
	}
	AzElDis[5].resize(100);
	for(UINT i = 0; i < 100; i++)
	{
		AzElDis[5][i].Az 		  	= 700 - i;
		AzElDis[5][i].El 		  	= 700 - i;
		AzElDis[5][i].Distance  	= 700 - i;
		AzElDis[5][i].JitterRange	= 2;
		AzElDis[5][i].SortTimes 	= i;
	}
	size_t PriJaggingArr0[] = {200,   500};
	size_t PriJaggingArr1[] = {50,    60};
	size_t PriJaggingArr2[] = {75,    150};
	size_t PriJaggingArr3[] = {60,    124};
	size_t PriJaggingArr4[] = {50,    100};
	size_t PriJaggingArr5[] = {50,    60};
	size_t PriJaggingArr6[] = {50,    70};
	size_t PriJaggingArr7[] = {1000,  2000};
	size_t PriJaggingArr8[] = {1000,  2000};
	size_t PriJaggingArr9[] = {1000,  2000};
	size_t RfAgiliArr0[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t RfAgiliArr1[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t RfAgiliArr2[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t RfAgiliArr3[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t RfAgiliArr4[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t RfAgiliArr5[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t RfAgiliArr6[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t RfAgiliArr7[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t RfAgiliArr8[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t RfAgiliArr9[]    = {0x1210, 0x1218, 0x1220, 0x1228, 0x1230, 0x1238, 0x1240, 0x1249};
	size_t PwDefArr0[]      = {5, 10};
	size_t PwDefArr1[]      = {1, 10};
	size_t PwDefArr2[]      = {5, 10};
	size_t PwDefArr3[]      = {5, 10};
	size_t PwDefArr4[]      = {5, 10};
	size_t PwDefArr5[]      = {5, 10};
	size_t PwDefArr6[]      = {5, 10};
	size_t PwDefArr7[]      = {5, 10};
	size_t PwDefArr8[]      = {5, 10};
	size_t PwDefArr9[]      = {5, 10};
    _RadarMode RadarMode[10];
    RadarMode[0].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr0, 2);
    RadarMode[0].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr0, 8);
    RadarMode[0].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr0, 2);
    RadarMode[1].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr1, 2);
    RadarMode[1].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr1, 8);
    RadarMode[1].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr1, 2);
    RadarMode[2].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr2, 2);
    RadarMode[2].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr2, 8);
    RadarMode[2].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr2, 2);
    RadarMode[3].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr3, 2);
    RadarMode[3].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr3, 8);
    RadarMode[3].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr3, 2);
    RadarMode[4].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr4, 2);
    RadarMode[4].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr4, 8);
    RadarMode[4].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr4, 2);
    RadarMode[5].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr5, 2);
    RadarMode[5].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr5, 8);
    RadarMode[5].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr5, 2);
    RadarMode[6].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr6, 2);
    RadarMode[6].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr6, 8);
    RadarMode[6].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr6, 2);
    RadarMode[7].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr7, 2);
    RadarMode[7].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr7, 8);
    RadarMode[7].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr7, 2);
    RadarMode[8].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr8, 2);
    RadarMode[8].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr8, 8);
    RadarMode[8].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr8, 2);
    RadarMode[9].PriPara.Modify(PRI_JAGGING, 0, 0, 3, 2, PriJaggingArr9, 2);
    RadarMode[9].RfPara.Modify(RF_AGILITY, 0, 8, 2, RfAgiliArr9, 8);
    RadarMode[9].PwPara.Modify(PW_DEFINE, 0, 3, 2, PwDefArr9, 2);
    /* run_onetimes */
	_iRadarSimPro<PdwType> iRadarSimPro;
	iRadarSimPro.PriTypeModify(PRI_FIX,1000);
	iRadarSimPro.GetPlatForm(0).SetAzElDis(AzElDis[0]);
	iRadarSimPro.GetPlatForm(0).SetRadarStTime(0, 10);
	iRadarSimPro.RadarModify(0);
	iRadarSimPro.PriTypeModify();
	size_t Tmp[] = {11, 12, 13, 14, 15, 16, 17, 18, 19};
	iRadarSimPro.RadarModify(Tmp, 9);
	iRadarSimPro.GetPlatForm(1).SetAzElDis(AzElDis[1]);
	iRadarSimPro.run_onetimes();
	iRadarSimPro.CoutStatus();
	iRadarSimPro.ShowOutBuffer(5000, 6000, 11);
    /* WriteFile */
    _iRadarSimPro<iRadarXygType> iRadarSimPro1;
    iRadarSimPro1.PriTypeModify(PRI_FIX, 1000);
	iRadarSimPro1.RadarModify(0);
	iRadarSimPro1.GetPlatForm(0).SetRadarStTime(0, 10);
	iRadarSimPro1.GetPlatForm(0).SetAzElDis(AzElDis[0]);
	iRadarSimPro1.RadarModify(10);
    iRadarSimPro1.PriTypeModify(PRI_FIX, 50);
	iRadarSimPro1.GetPlatForm(1).SetRadarStTime(0, 20);
	iRadarSimPro1.GetPlatForm(1).SetRadar(RadarMode[1], 1);
	iRadarSimPro1.RadarModify(12);
	iRadarSimPro1.GetPlatForm(1).SetAzElDis(AzElDis[1]);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[2], 0);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[3], 1);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[4], 2);
	iRadarSimPro1.RadarModify(23);
	iRadarSimPro1.GetPlatForm(2).SetAzElDis(AzElDis[2]);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[2], 0);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[3], 1);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[4], 2);
	iRadarSimPro1.RadarModify(33);
	iRadarSimPro1.GetPlatForm(3).SetAzElDis(AzElDis[3]);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[2], 0);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[3], 1);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[4], 2);
	iRadarSimPro1.RadarModify(43);
	iRadarSimPro1.GetPlatForm(4).SetAzElDis(AzElDis[4]);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[2], 0);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[3], 1);
	iRadarSimPro1.GetPlatForm(2).SetRadar(RadarMode[4], 2);
	iRadarSimPro1.GetPlatForm(5).SetRadar(RadarMode[5], 3);
	iRadarSimPro1.GetPlatForm(5).SetRadar(RadarMode[6], 4);
	iRadarSimPro1.RadarModify(55);
	iRadarSimPro1.GetPlatForm(5).SetAzElDis(AzElDis[5]);
	iRadarSimPro1.WriteFile("/home/admin/WorkSp/Data/iRadarSimProGenData.data", 10);
    _binaryFile<_xyg_z, PdwType> binaryFile("/home/admin/WorkSp/Data/iRadarSimProGenData.data", 200000);
    binaryFile.showFileHeader();
    binaryFile.showStatus();
    binaryFile.read();
    binaryFile.registShowDataFunc(PrintPdw<PdwType>);
    binaryFile.showData(0, 100);
    binaryFile.showStatus();
    binaryFile.close();
	return 1;
}