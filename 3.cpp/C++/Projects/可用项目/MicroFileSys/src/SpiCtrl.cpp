/*
 * SpiCtrlFifo.cpp
 *
 *  Created on: 2018-9-13
 *      Author: xupengtao
 */
#include "SpiCtrl.h"

SpiCtrl::SpiCtrl(int ID, int type):Status(0)
{
	Inition_Spi_Clk(XPAR_ACLK_SPI_DEVICE_ID);
	Set_Spi_Clk(type);
	xil_printf("SpiCtrl Success!\n");
}

int SpiCtrl::Inition_Spi_Clk(int ID)
{
	Status = XSpi_Initialize(&Spi_Clk1, ID);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSpi_SetOptions(&Spi_Clk1, XSP_MASTER_OPTION |XSP_MANUAL_SSELECT_OPTION);

	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	Status = XSpi_SetSlaveSelect(&Spi_Clk1, 1);
	if(Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XSpi_Start(&Spi_Clk1);
	XSpi_IntrGlobalDisable(&Spi_Clk1);
	return 0;
}
int SpiCtrl::Read_Spi_Clk()
{
	// read ram
	//   0    1    2    3    4   5    6    7
	// 0x70 0x78 0x74 0x7c 0x72 0x7a 0x76 0x7e

	WriteBuffer[0]=0x70;
	WriteBuffer[1]=0x00;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x00;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	WriteBuffer[0]=0x78;
	WriteBuffer[1]=0x00;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x00;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	WriteBuffer[0]=0x74;
	WriteBuffer[1]=0x00;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x00;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	WriteBuffer[0]=0x7c;
	WriteBuffer[1]=0x00;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x00;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	WriteBuffer[0]=0x72;
	WriteBuffer[1]=0x00;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x00;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	WriteBuffer[0]=0x7a;
	WriteBuffer[1]=0x00;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x00;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	WriteBuffer[0]=0x76;
	WriteBuffer[1]=0x00;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x00;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	WriteBuffer[0]=0x7e;
	WriteBuffer[1]=0x00;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x00;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	return 0;
}
int SpiCtrl::Status_Spi_Clk()
{
	//read Status, write control		// 20009D98
	WriteBuffer[0]=0x19;
	WriteBuffer[1]=0xB9;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x04;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	return 0;
}
int SpiCtrl::Set_Spi_Clk(int i)
{
	//write REG008			// 20009398
	WriteBuffer[0]=0x19;
	WriteBuffer[1]=0xc9;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x04;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	//write REG000		 // EB020320
	WriteBuffer[0]=0x04;
	WriteBuffer[1]=0xC0;
	WriteBuffer[2]=0x40;
	WriteBuffer[3]=0xD7;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	//write REG001			// EB020301
	WriteBuffer[0]=0x80;
	WriteBuffer[1]=0xC0;
	WriteBuffer[2]=0x40;
	WriteBuffer[3]=0xD7;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	//write REG002		// EB860302
	WriteBuffer[0]=0x40;
	WriteBuffer[1]=0xC0;
	WriteBuffer[2]=0x61;
	WriteBuffer[3]=0xD7;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	//write REG003		// EB020303
	WriteBuffer[0]=0xC0;
	WriteBuffer[1]=0xC0;
	WriteBuffer[2]=0x61;
	WriteBuffer[3]=0xD7;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	//write REG004		// 68860314
	WriteBuffer[0]=0x28;
	WriteBuffer[1]=0xC0;
	WriteBuffer[2]=0x61;
	WriteBuffer[3]=0x16;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	//write REG005			// 10240FC5
	WriteBuffer[0]=0xA2;
	WriteBuffer[1]=0x78;
	WriteBuffer[2]=0x20;
	WriteBuffer[3]=0x08;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	//write REG006		// 04CE29B6
	WriteBuffer[0]=0x6D;
	WriteBuffer[1]=0xA0;
	WriteBuffer[2]=0x73;
	WriteBuffer[3]=0x20;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	//write REG007			// BD98B727

	WriteBuffer[0]=0xE6;
	WriteBuffer[1]=0x1E;
	WriteBuffer[2]=0x11;
	WriteBuffer[3]=0xA9;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;


	//write REG008			// 20009D98
	WriteBuffer[0]=0x19;
	WriteBuffer[1]=0xB9;
	WriteBuffer[2]=0x00;
	WriteBuffer[3]=0x04;
	Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
	if(Status != XST_SUCCESS) 				return XST_FAILURE;
	if (i==1)
	{
		//Copy RAM to EEPROM ¨C Unlock,
		WriteBuffer[0]=0xF8;
		WriteBuffer[1]=0x00;
		WriteBuffer[2]=0x00;
		WriteBuffer[3]=0x00;
		Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
		if(Status != XST_SUCCESS) 				return XST_FAILURE;
	}
	if (i==2)
	{
		//Copy RAM to EEPROM ¨C Lock
		WriteBuffer[0]=0xFC;
		WriteBuffer[1]=0x05;
		WriteBuffer[2]=0x00;
		WriteBuffer[3]=0x00;
		Status=XSpi_Transfer(&Spi_Clk1, WriteBuffer, ReadBuffer,4);
		if(Status != XST_SUCCESS) 				return XST_FAILURE;
	}
    return 0;
}
