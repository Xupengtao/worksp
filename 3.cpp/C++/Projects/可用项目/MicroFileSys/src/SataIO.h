/*
 * SataDataFifo.h
 *
 *  Created on: 2018-9-13
 *      Author: xupengtao
 */

#ifndef SATADATAFIFO_H_
#define SATADATAFIFO_H_

#include "struct.h"
#include "Gpio.hpp"
#include "FileManage.hpp"
#include "BlockManage.hpp"

class SataIO
{
private:
	Gpio 	Gpioin;
	Gpio 	GpioOut;
	Gpio 	GpioCtrl;
	FileManage<USHORT> FileM;
	BlockManage<USHORT> BlockM;
	volatile Xuint32 *ptr;
	volatile Xuint32 *sysfifo;
	volatile Xuint32 *comptr;
	Xuint32 GpioStatus;
	Xuint32 Drive1_vld;
	Xuint8  Drive1_woking;
	Xuint8  GetBand;
	Xuint8 	Loglen;
	Xuint8  Drive1_full;
	Xuint32 Data_ZoneL,Data_ZoneH;
	Xuint32 filenumber;
	Xuint32 blocknum;
	Xuint32	blockP;
	Xuint32	fileno;
	Xuint32 fileplace;
	Xuint32 fileblocknum;
	Xuint32 filesize;
	Xuint8  gl_Cmd;
	Xuint32	RecordStatus;
	Xuint16 Replay_Index;
	Xuint16 del_Index;
	Xuint32 ReceivedCount;
	Xuint32 g_tcpDataReadP;
	Xuint32 maxlba;
	start_file syszone[8];
	Xuint32 RecvBuffer32[40];
	Xuint32 TxBuffer[COM_BUFFER_SIZE];
	Xuint32 Rx_Tcp_Buffer[RX_COM_BUFFER_SIZE];
	enum {valid = 1,invalid = 2};
	enum
	{
		DmaCtrl 	= 8,
		BlockChain 	= 10,
		FpgaStatusAddr 	= 1029,
		FileSizeAddr 	= 1030,
	};
	enum
	{
		StartSign 	= 0xAAAA,
		StopSign	= 0xDDDD
	};
	enum
	{
		Busy 		= 0x100,
		Idle 		= 0,
		HalfSign 	= 1,
	};
	enum
	{
		Free   		= 0,
		Record 		= 1,
		Replay 		= 2,
		Download 	= 3,
		Upload 		= 4,
		RecordFull 	= 5
	};
public:
	SataIO();
	SataIO(Xuint32 Addr1,Xuint32 Addr2,Xuint32 Addr3,u16 DeviceId0,int DirectionMask0,u16 DeviceId1,int DirectionMask1,u16 DeviceId2,int DirectionMask2);
	virtual ~SataIO() {};
	Xuint32 GetDrivevld() const;
	Xuint32 GetMaxlba() const;
	Xuint32 Getfilenumber() const;
	void	DebugSetCmd(UINT Cmd){gl_Cmd = Cmd;}
	void 	SetAddrVal(UINT remote,UINT value);
	void 	showStatus();
	UINT 	CheckOnline();
	void	ReadMaxlba();
	UINT	ReadBySysFifo(UINT st_L,UINT st_H,UINT *rdata,UINT Len = 1);
	UINT	WriteBySysFifo(UINT st_L,UINT st_H,UINT *wdata,UINT Len = 1);
	UINT	WriteBySysFifoN(UINT st_L,UINT st_H,UINT *wdata,UINT Len = 1);		//not clear sysfifo
	UINT	ReadByByte(UINT st_L,UINT st_H,UINT Len);
	UINT	NetTCPpop();
	void 	Message_Decode(unsigned short start,unsigned short len);
	void	Init_Drive();
	void	Change_Drive(unsigned char driveno);
	void	ReadBlockBeginAndEnd(UINT BlockNum,UINT Len = 1);
	UINT 	WriteFileStart();
	UINT 	WriteFileEnd();
	void	Dir(unsigned char driveno);
	void	DeleteFile(unsigned char driveno);
	void	Re_messgae(char cmd,char val);
	void	Re_Full();
	void 	CmdProcess();
	UINT 	Data_Record();
	UINT 	Data_Replay();
	UINT 	export_data();
	void 	delfile();
	void	Delay(int Delaytime = 100){Gpioin.Delay(Delaytime);}
	void	DebugStart(UINT Ind){BlockM.DebugInd = Ind;}
	UINT	DebugStop(){return (BlockM.ValidBlockInd >= BlockM.DebugInd)?0:1;}
	UINT	checkstop();
	void	CheckBlockFifoStatus();
	UINT	Check_Replay_Stop();
	UINT	Check_export_Stop();
};

inline Xuint32 SataIO::GetDrivevld() const
{
	return Drive1_vld;
}

inline Xuint32 SataIO::GetMaxlba() const
{
	return maxlba;
}

inline Xuint32 SataIO::Getfilenumber() const
{
	return filenumber;
}

inline void SataIO::SetAddrVal(UINT remote,UINT value)
{
	*(ptr + remote) = value;
}

inline void SataIO::showStatus()
{
	PRINT(Drive1_vld);
	PRINT(GpioStatus);
	PRINT(maxlba);
	PRINT(filenumber);
	PRINT(blocknum);
	PRINT(blockP);
	PRINT(fileno);
	PRINT(fileplace);
	PRINT(fileblocknum);
	PRINT(filesize);
	PRINT(gl_Cmd);
	FileM.showValidFileP();
}

inline UINT SataIO::CheckOnline()
{
	SetAddrVal(4,0);
	Gpioin.Delay(8000);
	if(1 == Gpioin.DriveCheck())
	{
		Drive1_vld = valid;
		return 1;
	}
	else
	{
		Drive1_vld  = invalid;
		return 0;
	}
}

inline void SataIO::ReadMaxlba()
{
	GpioOut.IndSataCMD();
	maxlba = *(ptr+1024);
}

inline UINT SataIO::ReadBySysFifo(UINT st_L,UINT st_H,UINT *rdata,UINT Len)
{
	UINT Py = 0;
	for(UINT j=0;j<Len;j++)
	{
		*(ptr+0)=st_L+j;
		*(ptr+1)=st_H;
		*(ptr+2)=1;
		*(ptr+3)=0x55000055;
		Py = j*128;
		GpioCtrl.RfifoReset();
		GpioOut.RSataCMD();
		Gpioin.DriveCheckLoop();
		for(UINT i=0;i<128;i++)
		{
			rdata[i+Py] = *sysfifo;
		}
		*(ptr+3)=0x0;									//normal extern data
	}
	return 0;
}
inline UINT SataIO::ReadByByte(UINT st_L,UINT st_H,UINT Len)
{
	UINT temp;
	for(UINT j=0;j<Len;j++)
	{
		*(ptr+0)=st_L+j;
		*(ptr+1)=st_H;
		*(ptr+2)=1;
		*(ptr+3)=0x55000055;
		GpioCtrl.RfifoReset();
		GpioOut.RSataCMD();
		Gpioin.DriveCheckLoop();
		for(UINT i=0;i<9;i++)
		{
			temp = *sysfifo;
			xil_printf("%d\n",temp);
		}
		*(ptr+3)=0x0;									//normal extern data
	}
	return 0;
}

inline UINT SataIO::WriteBySysFifo(UINT st_L,UINT st_H,UINT *wdata,UINT Len)
{
	UINT Py = 0;
	for(UINT j=0;j<Len;j++)
	{
		*(ptr+0)=st_L+j;
		*(ptr+1)=st_H;
		*(ptr+2)=1;
		*(ptr+3)=0xaa0000aa;
		Py = j*128;
		GpioCtrl.WfifoReset();
		for(UINT i=0;i<128;i++)
		{
			 *sysfifo=*(wdata+i+Py);
		}
		GpioOut.WSataCMD();
		Gpioin.DriveCheckLoop();
		*(ptr+3)=0x0;									//normal extern data
	}
	return 0;
}

inline UINT SataIO::WriteBySysFifoN(UINT st_L,UINT st_H,UINT *wdata,UINT Len)
{
	UINT Py = 0;
	for(UINT j=0;j<Len;j++)
	{
		*(ptr+0)=st_L+j;
		*(ptr+1)=st_H;
		*(ptr+2)=1;
		*(ptr+3)=0xaa0000aa;
		Py = j*128;
		for(UINT i=0;i<128;i++)
		{
			 *sysfifo=*(wdata+i+Py);
		}
		GpioOut.WSataCMD();
		Gpioin.DriveCheckLoop();
		*(ptr+3)=0x0;									//normal extern data
	}
	return 0;
}

inline UINT SataIO::WriteFileStart()
{
	UINT BlockLength = 16;
	if(blocknum >= BLOCK_SIZE-1)
	{
		Re_messgae(0xb0,3);
		Drive1_full=1;
		return 1;
	}
	BlockM.SetValidBlock();
	GpioCtrl.BListfifoReset();
	if(BlockM.ValidBlockLength == 0)
	{
		return -1;
	}
	else if(BlockM.ValidBlockLength < 16)
	{
		BlockLength = BlockM.ValidBlockLength;
	}
	for(UINT i = 0;i < BlockLength;i++)
	{
		SetAddrVal(BlockChain,BlockM.ValidBlock[i]*BLOCK_SIZE);			//Write BlockChain
	}
	BlockM.ValidBlockInd = BlockLength;

	blockP = BlockM.GetValidBlock();
	syszone[0].File_Start_Vld_N=0;
	syszone[0].FileNo = fileno;
	syszone[0].StartBlock = blockP;
	syszone[0].BlockNum = 0;
	syszone[0].FileSize = 0;
	syszone[0].File_End_Year	= syszone[0].File_Start_Year;
	syszone[0].File_End_Month	= syszone[0].File_Start_Month;
	syszone[0].File_End_Day	= syszone[0].File_Start_Day	;
	syszone[0].File_End_Hour   = syszone[0].File_Start_Hour ;
	syszone[0].File_End_Minute = syszone[0].File_Start_Minute;
	syszone[0].File_End_Second = syszone[0].File_Start_Second;

	fileplace = FileM.GetNextFileP();
	GpioCtrl.SataChangesetMicroBlaze();
	WriteBySysFifo(FILE_BLOCKLIST_ADDR,0,(UINT *)(BlockM.GetBlockLAddr()),BlockM.BlockListSize);
	WriteBySysFifo(FILE_CONTENT_ADDR + fileplace,0,(unsigned int*)(syszone));

	GpioCtrl.SataChangesetFpga();
	GpioCtrl.WfifoReset();
	SetAddrVal(DmaCtrl,StartSign);

	Gpioin.Delay(1);
	return 0;
}

inline UINT SataIO::WriteFileEnd()
{
	UINT Status = 0;
	Status = *(ptr+FpgaStatusAddr);

	if((Status&DMA_WRITE_STATUS) == Busy)
	{
		SetAddrVal(DmaCtrl,StopSign);
		while((Status&DMA_WRITE_STATUS) == Idle)
		{
			Status = *(ptr+FpgaStatusAddr);
			GpioCtrl.Delay(100);
		}
	}
	GpioCtrl.Delay(100);
	filesize = *(ptr+FileSizeAddr);
	if(filesize%BLOCK_SIZE == 0)
	{
		fileblocknum = filesize/BLOCK_SIZE;
	}
	else
	{
		fileblocknum = filesize/BLOCK_SIZE + 1;
	}

	PRINT(filesize);
	PRINT(fileblocknum);

	if(fileblocknum == 0)
	{
		return 0xFF;
	}
	for(UINT i = 0;i<fileblocknum-1;i++)
	{
		BlockM.SetBlock(BlockM.ValidBlock[i],fileno,BlockM.ValidBlock[i+1]);
	}
	BlockM.SetBlock(BlockM.ValidBlock[fileblocknum-1],fileno);
	GpioCtrl.SataChangesetMicroBlaze();
	WriteBySysFifo(FILE_BLOCKLIST_ADDR,0,(UINT *)(BlockM.GetBlockLAddr()),BlockM.BlockListSize);

	syszone[0].File_End_Vld_N=0;
	syszone[0].BlockNum = fileblocknum;
	syszone[0].FileSize = filesize;
	WriteBySysFifo(FILE_CONTENT_ADDR + fileplace,0,(unsigned int*)(syszone));
	filenumber++;
	if(fileno <= syszone[0].FileNo)
	{
		fileno = syszone[0].FileNo + 1;
	}
	blocknum = blocknum + syszone[0].BlockNum;
	FileM.SetFileValid(fileplace,syszone[0].FileNo);
	syszone[0].File_End_Vld_N=1;
	return 0;
}

#endif /* SATADATAFIFO_H_ */
