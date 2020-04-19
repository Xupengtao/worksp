/*
 * Gpio.h
 *
 *  Created on: 2018-9-13
 *      Author: xupengtao
 */

#ifndef _GPIO_H_
#define _GPIO_H_

#include"struct.h"

class Gpio
{
private:
	XGpio 	_Gpio;
	Xuint32	GpioValue;
	Xuint32 Status;
	u32 Data;

	enum						//SetReset
	{
		GPIO_SATA_RESET = 1<<6,
		Wfifo_RESET 	= 1<<0,
		Rfifo_RESET 	= 1<<1,
		BListfifo_RESET = 1<<7,
		check_RESET 	= 1<<2,
		SataChange		= 1<<8,
		led1 			= 1<<9,
		TimeFlag 		= 1<<5,
		com_RESET 		= 1<<3,
		can_RESET 		= 1<<14
	};
	enum						//SetCmd
	{
		WSATA 	= 1<<0,
		RSATA 	= 1<<1,
		INDSATA = 1<<2
	};
	enum						//Read
	{
		topok = 0x1,
		Aurorarecord_Channelup = 0x4,
		wr_data_count = 24,			//per 512Byte
		comfifo_empty = 0x2,
	};

public:
	Gpio();
	Gpio(u16 DeviceId,int DirectionMask);
	virtual ~Gpio() {};
	Xuint32	GetStatus() const;
	int 	Initialize(u16 DeviceId);
	void 	SetDataDirection(u32 DirectionMask, unsigned Channel = 1);
	void 	DiscreteWrite(u32 Data, unsigned Channel = 1);
	void	SetReset(Xuint32 value,Xuint32 mode);
	void	SataReset(int num = 0);
	void 	WfifoReset(int num = 0);
	void 	RfifoReset(int num = 0);
	void	BListfifoReset(int num = 0);
	void 	comReset(int num = 0);
	void 	canReset(int num = 0);
	void 	checkset1(int num = 0);
	void 	checkReset10(int num = 0);
	void	TimeFlagset1(int num = 0);
	void	SataChangesetMicroBlaze(int num = 0);
	void	SataChangesetFpga(int num = 0);
	void	SetCmd(Xuint32 value,Xuint32 mode);
	void	WSataCMD();
	void	RSataCMD();
	void	IndSataCMD();
	u32		ReadByBit(int site);
	u32 	AuroraChannelUp();
	u32		DriveCheck();
	void	DriveCheckLoop();
	u32		FifoSize();
	u32		ComfifoEmpty();
	void	Delay(int num = 1000);
};

/*
 * Gpio.cpp
 *
 *  Created on: 2018-9-13
 *      Author: xupengtao
 */

inline Gpio::Gpio(u16 DeviceId,int DirectionMask):GpioValue(0),Status(0),Data(0)
{
	if (Initialize(DeviceId) == XST_SUCCESS)
	{
		xil_printf("Gpio Initialize OK!\n");
		Status = 1;
		if(DirectionMask == Direction_in)
		{
			SetDataDirection(0xFFFFFFF);
		}
		else if(DirectionMask == Direction_out)
		{
			SetDataDirection(0x0);
		}
	}
}

inline Xuint32 Gpio::GetStatus() const
{
	return Status;
}

inline int  Gpio::Initialize(u16 DeviceId)
{
	return XGpio_Initialize(&_Gpio,DeviceId);
}

inline void Gpio::SetDataDirection(u32 DirectionMask, unsigned Channel)
{
	XGpio_SetDataDirection(&_Gpio,Channel,DirectionMask);
}

inline void Gpio::DiscreteWrite(u32 Data, unsigned Channel)
{
	XGpio_DiscreteWrite(&_Gpio,Channel,Data);
}

inline void	Gpio::SetReset(Xuint32 value,Xuint32 mode)
{
	if (mode==0)
		GpioValue &= ~value;
	else
		GpioValue |=value;
	DiscreteWrite(GpioValue);
}

inline void Gpio::SataReset(int num)
{
	SetReset(GPIO_SATA_RESET,0);
	Delay(num);
	SetReset(GPIO_SATA_RESET,1);
}

inline void Gpio::WfifoReset(int num)
{
	SetReset(Wfifo_RESET,1);
	Delay(num);
	SetReset(Wfifo_RESET,0);
}

inline void Gpio::RfifoReset(int num)
{
	SetReset(Rfifo_RESET,1);
	Delay(num);
	SetReset(Rfifo_RESET,0);
}

inline void Gpio::BListfifoReset(int num)
{
	SetReset(BListfifo_RESET,1);
	Delay(num);
	SetReset(BListfifo_RESET,0);
}

inline void Gpio::comReset(int num)
{
	SetReset(com_RESET,1);
	Delay(num);
	SetReset(com_RESET,0);
}

inline void Gpio::canReset(int num)
{
	SetReset(can_RESET,1);
	Delay(num);
	SetReset(can_RESET,0);
}

inline void Gpio::checkset1(int num)
{
	SetReset(check_RESET,1);
	Delay(num);
}

inline void Gpio::checkReset10(int num)
{
	SetReset(check_RESET,1);
	Delay(num);
	SetReset(check_RESET,0);
}

inline void Gpio::TimeFlagset1(int num)
{
	SetReset(TimeFlag,1);
	Delay(num);
}

inline void Gpio::SataChangesetMicroBlaze(int num)
{
	SetReset(SataChange,0);
	Delay(num);
}

inline void Gpio::SataChangesetFpga(int num)
{
	SetReset(SataChange,1);
	Delay(num);
}

inline void Gpio::SetCmd(Xuint32 value,Xuint32 mode)
{
	value=value<<SATA_AB;
	if (mode==0)
	{
		GpioValue &= ~value;
	}
	else
	{
		GpioValue |=value;
	}
	DiscreteWrite(GpioValue);
}

inline void Gpio::WSataCMD()
{
	SetCmd(WSATA,1);
	SetCmd(WSATA,0);
}

inline void Gpio::RSataCMD()
{
	SetCmd(RSATA,1);
	SetCmd(RSATA,0);
}

inline void Gpio::IndSataCMD()
{
	SetCmd(INDSATA,1);
	Delay(100);
	SetCmd(INDSATA,0);
}

inline u32 Gpio::ReadByBit(int site)
{
	Data = XGpio_DiscreteRead(&_Gpio,1);
	return Data&site;
}

inline u32 Gpio::AuroraChannelUp()
{
	return ReadByBit(Aurorarecord_Channelup);
}

inline u32 Gpio::DriveCheck()
{
	return ReadByBit(topok);
}

inline void Gpio::DriveCheckLoop()
{
	while(DriveCheck()!= 0x1);
}

inline u32 Gpio::FifoSize()
{
	Data = XGpio_DiscreteRead(&_Gpio,1);
	return Data>>24;
}

inline u32 Gpio::ComfifoEmpty()
{
	return ReadByBit(comfifo_empty);
}

inline void Gpio::Delay(int num)
{
	int i=0,j=0;
	for(i=0;i<num*1000;i++)
	{
		j++;
	}
}

#endif /* GPIO_H_ */
