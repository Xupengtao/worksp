/*
 * main.cpp
 *
 *  Created on: 2018-9-13
 *      Author: xupengtao
 */

#include "struct.h"
#include "Gpio.hpp"
#include "SataIO.h"
#include "SpiCtrl.h"

Xuint8 SelfResult;

int main(void)
{
	SelfResult = 0;
	xil_printf("SpiCtrl Next!\n");
	SpiCtrl Spi(XPAR_ACLK_SPI_DEVICE_ID);
	xil_printf("SataIO Next!\n");
	SataIO 	ptr(XPAR_XPS_MCH_EMC0_MEM0_BASEADDR,XPAR_SYSRDFIFO_MEM0_BASEADDR,XPAR_COM_FIFO_MEM0_BASEADDR,
			XPAR_XPS_GPIO_1_DEVICE_ID,Direction_in,XPAR_XPS_GPIO_0_DEVICE_ID,Direction_out,XPAR_XPS_GPIO_2_DEVICE_ID,Direction_out);

	xil_printf("CheckOnline Next!\n");
	ptr.CheckOnline();
	xil_printf("SetAddrVal Next!\n");
	ptr.SetAddrVal(5,0);
	xil_printf("GetDrivevld Next!\n");
	if(ptr.GetDrivevld())
	{
		xil_printf("Change_Drive Next!\n");
		ptr.Init_Drive();
		ptr.Change_Drive(1);
		ptr.showStatus();
	}
	else
	{
		SelfResult=SelfResult+2;
	}
//
	xil_printf("while Next!\n");
	ptr.DebugSetCmd(1);
	ptr.CmdProcess();
	ptr.DebugSetCmd(0);
	ptr.DebugStart(60);
	while(ptr.DebugStop())
	{
		ptr.CmdProcess();
	}
	ptr.DebugSetCmd(2);
	ptr.CmdProcess();
	ptr.ReadBlockBeginAndEnd(1,10);
	ptr.ReadBlockBeginAndEnd(2,10);
	return 0;
}

/*   For reading data from Disk    */
//int main(void)
//{
//	SelfResult = 0;
//	xil_printf("SpiCtrl Next!\n");
//	SpiCtrl Spi(XPAR_ACLK_SPI_DEVICE_ID);
//	xil_printf("SataIO Next!\n");
//	SataIO 	ptr(XPAR_XPS_MCH_EMC0_MEM0_BASEADDR,XPAR_SYSRDFIFO_MEM0_BASEADDR,XPAR_COM_FIFO_MEM0_BASEADDR,
//			XPAR_XPS_GPIO_1_DEVICE_ID,Direction_in,XPAR_XPS_GPIO_0_DEVICE_ID,Direction_out,XPAR_XPS_GPIO_2_DEVICE_ID,Direction_out);
//
//	xil_printf("CheckOnline Next!\n");
//	ptr.CheckOnline();
//	xil_printf("SetAddrVal Next!\n");
//	ptr.SetAddrVal(5,0);
//	xil_printf("GetDrivevld Next!\n");
//	if(ptr.GetDrivevld())
//	{
//		xil_printf("Change_Drive Next!\n");
////		ptr.Init_Drive();
//		ptr.Change_Drive(1);
//		ptr.showStatus();
//	}
//	else
//	{
//		SelfResult=SelfResult+2;
//	}
//
//	xil_printf("Read!\n");
//
//	UINT Addr = BLOCK_SIZE * 2;
//	xil_printf("Read!\n");
//	ptr.ReadByByte(Addr,0,1);
//	return 0;
//}
