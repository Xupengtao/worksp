#ifndef _STRUCT_H_
#define _STRUCT_H_

#include "xparameters.h"
#include "xgpio.h"
#include "string.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xuartlite.h"

#include "xspi.h"		/* SPI device driver */
#include "xspi_l.h"

#define PRINT(x)	xil_printf(#x" = %d\n",(UINT)(x));

#define FILE_NUM		1024
#define BLOCK_NUM		1024

#define SATA_AB 		0

#define COM_BUFFER_SIZE 	2600
#define RX_COM_BUFFER_SIZE	500
#define FILE_BLOCKLIST_ADDR 0
#define FILE_CONTENT_ADDR 	0x2000
#define FILENUM_MAX			1024
#define BLOCK_SIZE			524288

#define	Rsv_G				2000000
#define MaxFull 			8192


#define	DMA_BLOCKLFIFOHALF_STATUS 0xFF
#define	DMA_WRITE_STATUS   		  0xFF00

typedef unsigned short 	USHORT;
typedef unsigned int UINT;

enum
{
	Direction_in = 0,
	Direction_out = 1,
};

//MemInterface
#define 	XPAR_XPS_MCH_EMC0_MEM0_BASEADDR	XPAR_AXPS_MCH_EMC0_MEM0_BASEADDR
#define 	XPAR_SYSRDFIFO_MEM0_BASEADDR	XPAR_ASYSRDFIFO_MEM0_BASEADDR
#define 	XPAR_COM_FIFO_MEM0_BASEADDR		XPAR_ACOM_FIFO_MEM0_BASEADDR

//Gpio
#define 	XPAR_XPS_GPIO_0_DEVICE_ID		XPAR_AXPS_GPIO_0_DEVICE_ID
#define 	XPAR_XPS_GPIO_1_DEVICE_ID		XPAR_AXPS_GPIO_1_DEVICE_ID
#define 	XPAR_XPS_GPIO_2_DEVICE_ID		XPAR_AXPS_GPIO_2_DEVICE_ID

//Spi
#define 	Spi_Clk1_DEVICE_ID				XPAR_CLK_SPI_DEVICE_ID

struct start_file
{
	Xuint32 File_Start_Vld_N;		//0

	Xuint16	File_Start_Year;
	Xuint8	File_Start_Month;
	Xuint8	File_Start_Day;			//1

	Xuint8	File_Start_Hour;
	Xuint8	File_Start_Minute;
	Xuint8	File_Start_Second;
	Xuint8	File_Band;				//2

	Xuint32 File_Start_ZoneL;		//3
	Xuint32 File_Start_ZoneH;		//4

	Xuint32 File_End_Vld_N;			//5

	Xuint16	File_End_Year;
	Xuint8	File_End_Month;
	Xuint8	File_End_Day;			//6

	Xuint8	File_End_Hour;
	Xuint8	File_End_Minute;
	Xuint8	File_End_Second;
	Xuint8	File_Type;				//7

	Xuint32 File_End_ZoneL;			//8
	Xuint32 File_End_ZoneH;			//9

	Xuint32	FileNo;					//10
	Xuint32	FileSize;				// Size/MB
	Xuint32	StartBlock;				//12
	Xuint32	BlockNum;				//13
	Xuint32	File_Info4;				//14
	Xuint32	File_Info5;				//15

};

#endif
