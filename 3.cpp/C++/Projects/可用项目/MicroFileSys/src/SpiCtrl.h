#ifndef __SPICTRLFIFO_H_
#define __SPICTRLFIFO_H_

#include "struct.h"
#define BUFFER_SIZE		12

class SpiCtrl
{
private:
	XSpi 	Spi_Clk1;
	int 	Status;
	unsigned char WriteBuffer[BUFFER_SIZE];
	unsigned char ReadBuffer[BUFFER_SIZE];

public:
	SpiCtrl();
	SpiCtrl(int ID, int type = 0);
	virtual ~SpiCtrl() {};
	int 	Inition_Spi_Clk(int ID);
	int 	Read_Spi_Clk();
	int 	Status_Spi_Clk();
	int 	Set_Spi_Clk(int i);
};

#endif
