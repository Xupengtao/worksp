//#include "iRadarSimPro.h"
//
//uint_t 	_RadarBuffer::GlobalToaH = 0;
//uint_t 	_RadarBuffer::GlobalToaL = 0;
//uint_t 	_RadarBuffer::RadarRCCLNum = 0;
//uint_t	_RadarBuffer::SortTimes = 0;
//
//void main()
//{
//	_iRadarSimPro<_OutPdw> iRadarSimPro;
//	iRadarSimPro.ReceiverInit();
//	iRadarSimPro.PlatFormInit(1);
//	/* 单通道 3 载频固定重频固定  */
//	//iRadarSimPro.RadarPriTypeModify(PRI_FIX,1000);
//	//iRadarSimPro.RadarModify(0);
//	//iRadarSimPro.RadarPriTypeModify(PRI_FIX,3223);
//	//iRadarSimPro.RadarModify(1);
//	//iRadarSimPro.RadarPriTypeModify(PRI_FIX,5215);
//	//iRadarSimPro.RadarModify(2);
//	/* 单通道 1 载频固定重频参差  */
//	//size_t JaggingArr[] = {1000,2000};
//	//iRadarSimPro.RadarPriTypeModify(PRI_JAGGING,0,0,3,1,JaggingArr,2);
//	//iRadarSimPro.RadarModify(0);
//	/* 单通道 1 载频固定重频抖动  */
//	//iRadarSimPro.RadarPriTypeModify(PRI_DITHER,1000,100);
//	//iRadarSimPro.RadarModify(0);
//	/* 单通道 10 载频固定重频固定  */
//	//iRadarSimPro.RadarPriTypeModify();
//	//size_t Tmp[] = {1,2,3,4,5,6,7,8,9};
//	//iRadarSimPro.RadarModify(Tmp,9);
//
//	/* 单通道 3 载频捷变重频固定  */
//	iRadarSimPro.RadarPriTypeModify(PRI_FIX,1000);
//	size_t AgiliArr[] = {0x1210,0x1218,0x1220,0x1228,0x1230,0x1238,0x1240,0x1249};
//	iRadarSimPro.RadarRfTypeModify(RF_AGILITY,0x1200,8,1,AgiliArr,8);
//	iRadarSimPro.RadarModify(0);
//	
//	iRadarSimPro.run_onetimes();
//	iRadarSimPro.CoutStatus();
//	iRadarSimPro.ShowOutBuffer(0,100);
//}
