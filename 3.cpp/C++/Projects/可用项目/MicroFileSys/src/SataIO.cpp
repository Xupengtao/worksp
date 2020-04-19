/*
 * SataDataFifo.cpp
 *
 *  Created on: 2018-9-13
 *      Author: xupengtao
 */

#include "SataIO.h"

SataIO::SataIO(Xuint32 Addr1,Xuint32 Addr2,Xuint32 Addr3,u16 DeviceId0,int DirectionMask0,u16 DeviceId1,int DirectionMask1,u16 DeviceId2,int DirectionMask2)
								:Gpioin(DeviceId0,DirectionMask0),
								 GpioOut(DeviceId1,DirectionMask1),
								 GpioCtrl(DeviceId2,DirectionMask2)
{
	Drive1_woking = 0;
	RecordStatus = 0;
	Data_ZoneL = 0;
	Data_ZoneH = 0;
	filenumber = 0;
	fileplace = 0;
	blocknum = 0;
	blockP = 0;
	fileno = 0;
	filesize = 0;
	fileblocknum = 0;
	Drive1_vld = invalid;
	maxlba = 0;
	ReceivedCount = 0;
	g_tcpDataReadP = 0;
	GetBand = 0;
	Loglen = 0;
	gl_Cmd = 0;
	Replay_Index = 0;
	del_Index = 0;
	Drive1_full = 0;
	ptr = (volatile Xuint32 *)Addr1;
	sysfifo = (volatile Xuint32 *)Addr2;
	comptr = (volatile Xuint32 *)Addr3;
	if((Gpioin.GetStatus() == 0)||(GpioOut.GetStatus() == 0)||(GpioCtrl.GetStatus() == 0))
	{
		GpioStatus = 0;
	}
	else
	{
		xil_printf("Gpio Check Success!\n");
		GpioStatus = 1;
	}

	GpioCtrl.SataReset();
	GpioCtrl.SataChangesetMicroBlaze();
//	for(int i=0;i<128;i++)
//	{
//		GpioCtrl.SataReset(3000);
//		GpioCtrl.Delay(2000);
//		if(4 == Gpioin.AuroraChannelUp())
//		{
//			break;
//		}
//	}
	xil_printf("AuroraChannelUp Success!\n");

	GpioCtrl.WfifoReset(1000);
	GpioCtrl.RfifoReset(1000);
	GpioCtrl.comReset(1000);
	GpioCtrl.canReset(1000);
	GpioCtrl.checkset1(1000);
	xil_printf("GpioCtrl Reset Success!\n");

//	memset(RecvBuffer32,0,40*sizeof(Xuint32));
//	memset(TxBuffer,0,COM_BUFFER_SIZE*sizeof(Xuint32));
//	memset(Rx_Tcp_Buffer,0,COM_BUFFER_SIZE*sizeof(Xuint32));
//	memset(syszone,0,8*sizeof(start_file));
	xil_printf("memset  Success!\n");
}

/****************************NetTCPpop****************************/

UINT SataIO::NetTCPpop()
{
	unsigned int i=0;
	unsigned short message_len=0;
	unsigned short lenData=0;

//	Message_Decode(lenData,message_len+1);
	while(1)
	{
		if(Gpioin.ComfifoEmpty()== 0x2)
		{
			break;
		}
		RecvBuffer32[ReceivedCount]=*comptr;
		Gpioin.Delay(2);
		ReceivedCount++;
	}
	if(ReceivedCount!=0)
	{
		for(i=0;i<ReceivedCount;i++)
		{
			if(((RecvBuffer32[i]>>24)!= 0))
			{
				Rx_Tcp_Buffer[g_tcpDataReadP++] = (RecvBuffer32[i] >>24)&0xff;
				Rx_Tcp_Buffer[g_tcpDataReadP++] = (RecvBuffer32[i] >>16)&0xff;
				Rx_Tcp_Buffer[g_tcpDataReadP++] = (RecvBuffer32[i] >>8)&0xff;
				Rx_Tcp_Buffer[g_tcpDataReadP++] = (RecvBuffer32[i] >>0)&0xff;
			}
		}
		ReceivedCount=0;
		if (g_tcpDataReadP < 8)
		{
			return 0;
		}

		for (lenData = 0; lenData <= g_tcpDataReadP-1; lenData++)
		{
			if((Rx_Tcp_Buffer[lenData+1]!=0xaa))
			{
				continue;
			}
			message_len=Rx_Tcp_Buffer[lenData+3] + Rx_Tcp_Buffer[lenData+2] *256;
			if(message_len%3 != 0)
			{
				if(((g_tcpDataReadP-lenData)/4) < (message_len/3 + 2))
				break;
			}
			else
			{
				if(((g_tcpDataReadP-lenData)/4) < (message_len/3 +1))
				break;
			}
			if(message_len%3 != 0)
			{
				message_len = (message_len/3 + 2)*4;
			}
			else
			{
				message_len = (message_len/3 + 1)*4;
			}
			Message_Decode(lenData,message_len);
			lenData=lenData+message_len-1;
		}

		if(lenData==0) return 0;
		if(lenData>=g_tcpDataReadP)
			{
				g_tcpDataReadP=0;
				return 0;
			}

		for(i=lenData;i<g_tcpDataReadP;i++)
		{
			Rx_Tcp_Buffer[i-lenData] = Rx_Tcp_Buffer[i];
		}
		g_tcpDataReadP=g_tcpDataReadP-lenData;
	}
	return 0;
}

void SataIO::Message_Decode(unsigned short start,unsigned short len)
{
	unsigned char icmd;
	unsigned int timeflagsec;
	unsigned int timeflag_us;

	icmd=Rx_Tcp_Buffer[start];
	GetBand = Rx_Tcp_Buffer[start + 5];
	xil_printf("Message_Decode icmd = %d\n",(UINT)icmd);
	if(icmd==0xA0)
	{
		if(0xAA==Rx_Tcp_Buffer[start + 6])
		{
			gl_Cmd = 1;
			syszone[0].File_Band        =Rx_Tcp_Buffer[start + 5];
			syszone[0].File_Start_Year	=Rx_Tcp_Buffer[start + 9] + (unsigned short)(Rx_Tcp_Buffer[start + 7])* 256;//Rx_Tcp_Buffer[start+6] +Rx_Tcp_Buffer[start+7]*256;
			syszone[0].File_Start_Month	=Rx_Tcp_Buffer[start + 10];//Rx_Tcp_Buffer[start+8];
			syszone[0].File_Start_Day	=Rx_Tcp_Buffer[start + 11];//Rx_Tcp_Buffer[start+9];			//1
			syszone[0].File_Start_Hour  =Rx_Tcp_Buffer[start + 13];//Rx_Tcp_Buffer[start+10];
			syszone[0].File_Start_Minute=Rx_Tcp_Buffer[start + 14];//Rx_Tcp_Buffer[start+11];
			syszone[0].File_Start_Second=Rx_Tcp_Buffer[start + 15];//Rx_Tcp_Buffer[start+12];		//2
		}
		if(0x55==Rx_Tcp_Buffer[start + 6])
		{
			gl_Cmd = 2;
			syszone[0].File_Band        =Rx_Tcp_Buffer[start + 5];
			syszone[0].File_End_Year	=Rx_Tcp_Buffer[start + 9] + (unsigned short)(Rx_Tcp_Buffer[start + 7])* 256;;//Rx_Tcp_Buffer[start+6] +Rx_Tcp_Buffer[start+7]*256;
			syszone[0].File_End_Month	=Rx_Tcp_Buffer[start + 10];//Rx_Tcp_Buffer[start+8];
			syszone[0].File_End_Day	=Rx_Tcp_Buffer[start + 11];//Rx_Tcp_Buffer[start+9];			//1
			syszone[0].File_End_Hour  =Rx_Tcp_Buffer[start + 13];//Rx_Tcp_Buffer[start+10];
			syszone[0].File_End_Minute=Rx_Tcp_Buffer[start + 14];//Rx_Tcp_Buffer[start+11];
			syszone[0].File_End_Second=Rx_Tcp_Buffer[start + 15];//Rx_Tcp_Buffer[start+12];		//2
		}
	}
	else if(icmd == 0xA1)
	{
		if(0xAA==Rx_Tcp_Buffer[start + 6])
		{
			Replay_Index =  (Rx_Tcp_Buffer[start + 10]<<8 )+ Rx_Tcp_Buffer[start + 11];
			gl_Cmd = 3;
			timeflagsec = (Rx_Tcp_Buffer[start + 14]<<8 )+  Rx_Tcp_Buffer[start + 15];
			timeflag_us = (timeflagsec * 745);     //48.828  MHz >>16 bit
			*(ptr+6)=timeflag_us;
			Gpioin.Delay(1);
			*(ptr+7)=0;
			Gpioin.Delay(1);
		}
		if(0x55==Rx_Tcp_Buffer[start + 6])
		{
			gl_Cmd = 4;
		}
	}
	else if(icmd == 0xA5)
	{
		del_Index =  (Rx_Tcp_Buffer[start + 9]<<8 )+ Rx_Tcp_Buffer[start + 10];
		PRINT((UINT)del_Index);
		gl_Cmd = 0xE;
	}

	else if(icmd == 0xA2)
	{
		GetBand = Rx_Tcp_Buffer[start + 5];
		Dir(1);										//Dir
	}
	else if(icmd == 0xA3)
	{
		DeleteFile(1);								//DeleteFile
		Change_Drive(1);
		Re_messgae(0xb3,3);
	}
	else if(icmd == 0xAA)
	{
		Replay_Index =  Rx_Tcp_Buffer[start + 7];
		gl_Cmd = 5;
	}
	else if(icmd == 0xAB)
	{
		gl_Cmd = 9;
		syszone[0].File_Band        =Rx_Tcp_Buffer[start + 5];
		syszone[0].File_Start_Year	=Rx_Tcp_Buffer[start + 9] + (unsigned short)(Rx_Tcp_Buffer[start + 7])* 256;//Rx_Tcp_Buffer[start+6] +Rx_Tcp_Buffer[start+7]*256;
		syszone[0].File_Start_Month	=Rx_Tcp_Buffer[start + 10];//Rx_Tcp_Buffer[start+8];
		syszone[0].File_Start_Day	=Rx_Tcp_Buffer[start + 11];//Rx_Tcp_Buffer[start+9];			//1
		syszone[0].File_Start_Hour  =Rx_Tcp_Buffer[start + 13];//Rx_Tcp_Buffer[start+10];
		syszone[0].File_Start_Minute=Rx_Tcp_Buffer[start + 14];//Rx_Tcp_Buffer[start+11];
		syszone[0].File_Start_Second=Rx_Tcp_Buffer[start + 15];//Rx_Tcp_Buffer[start+12];		//2

		//DeleteFile(1);
		//Change_Drive(1);
	}
	else if(icmd == 0xAC)
	{
		gl_Cmd = 0xA;
		syszone[0].File_Band        =Rx_Tcp_Buffer[start + 5];
		syszone[0].File_End_Year	=Rx_Tcp_Buffer[start + 9] + (unsigned short)(Rx_Tcp_Buffer[start + 7])* 256;//Rx_Tcp_Buffer[start+6] +Rx_Tcp_Buffer[start+7]*256;
		syszone[0].File_End_Month	=Rx_Tcp_Buffer[start + 10];//Rx_Tcp_Buffer[start+8];
		syszone[0].File_End_Day	=Rx_Tcp_Buffer[start + 11];//Rx_Tcp_Buffer[start+9];			//1
		syszone[0].File_End_Hour  =Rx_Tcp_Buffer[start + 13];//Rx_Tcp_Buffer[start+10];
		syszone[0].File_End_Minute=Rx_Tcp_Buffer[start + 14];//Rx_Tcp_Buffer[start+11];
		syszone[0].File_End_Second=Rx_Tcp_Buffer[start + 15];//Rx_Tcp_Buffer[start+12];		//2

		//DeleteFile(1);
		//Change_Drive(1);
	}
}

void SataIO::CmdProcess()
{
	UINT Status = 0;
	switch(gl_Cmd)
	{
	case 0x1 :
		if(filenumber>FILE_NUM)
		{
			//Re_Full();
			Re_messgae(0xb0,3);
			return ;
		}
		if(Drive1_woking == 1)
		{
			 Re_messgae(0xb0,1);
			 RecordStatus = Record;
			 WriteFileStart();
			 xil_printf("Time Begin\n");
			 GpioCtrl.checkReset10();
		}
		break;
	case 0x2 :
		xil_printf("Time end\n");
		RecordStatus = Free;
		WriteFileEnd();
		GpioCtrl.checkset1();
		GpioCtrl.WfifoReset();
		Re_messgae(0xb0,2);
		break;
	case 0x3 :
		Re_messgae(0xb1,0xaa);
		Data_Replay();
		Re_messgae(0xb1,0x55);
		break;
	case 0x4 :
		//FormatFileSys();
		//Inisys();
		break;

	case 0x5 :
		export_data();
		gl_Cmd =0;
		break;
	case 0x6 :
		break;
	case 0x7 :
		break;
	case 0x8 :
		break;
	case 0xE :
		delfile();
		gl_Cmd =0;
		break;
	case 0x9 :
		GpioCtrl.TimeFlagset1();
		if(1==Drive1_woking)
		{
			// Send2Com(1,SendBuffer);
			WriteFileStart();
			GpioCtrl.WfifoReset();
			//sim start
			GpioCtrl.checkReset10();

			Data_Record();
			WriteFileEnd();
			// Change_Drive(1);
			GpioCtrl.WfifoReset();
			//sim end
			GpioCtrl.checkset1();
		}
		break;
	default:
		switch(RecordStatus)
		{
		case Record:
			CheckBlockFifoStatus();
			break;
		case RecordFull:
			Status = *(ptr+FpgaStatusAddr);
			if((Status&DMA_WRITE_STATUS) == Idle)
			{
				gl_Cmd = 0x2;
			}
			break;
		default:
			break;
		}
		break;
	}
}

void SataIO::Init_Drive()
{
	syszone[0].File_Start_Vld_N 	= 1;
	syszone[0].File_Start_Year 		= 0;
	syszone[0].File_Start_Month 	= 0;
	syszone[0].File_Start_Day 		= 0;
	syszone[0].File_Start_Hour 		= 0;
	syszone[0].File_Start_Minute 	= 0;
	syszone[0].File_Start_Second 	= 0;
	syszone[0].File_Band 			= 0;
	syszone[0].File_Start_ZoneL 	= 0;
	syszone[0].File_Start_ZoneH 	= 0;
	syszone[0].File_End_Vld_N 		= 1;
	syszone[0].File_End_Year 		= 0;
	syszone[0].File_End_Month 		= 0;
	syszone[0].File_End_Day 		= 0;
	syszone[0].File_End_Hour 		= 0;
	syszone[0].File_End_Minute 		= 0;
	syszone[0].File_End_Second 		= 0;
	syszone[0].File_Type 			= 0;
	syszone[0].File_End_ZoneL 		= 0;
	syszone[0].File_End_ZoneH 		= 0;
	syszone[0].FileNo 				= 0;
	syszone[0].FileSize				= 0;
	syszone[0].StartBlock 			= 0;
	syszone[0].BlockNum 			= 0;
	syszone[0].File_Info4 			= 0;
	syszone[0].File_Info5 			= 0;

	GpioCtrl.SataChangesetMicroBlaze();


	for(UINT i=0;i<FILENUM_MAX;i++)
	{
		WriteBySysFifo(FILE_CONTENT_ADDR+i,0,(unsigned int*)(syszone));
	}
	BlockM.clear();
	WriteBySysFifo(FILE_BLOCKLIST_ADDR,0,(UINT *)(BlockM.GetBlockLAddr()),BlockM.BlockListSize);
	xil_printf("Init_Drive Success!\n");
}

void SataIO::Change_Drive(unsigned char driveno)
{
	if(driveno==1)
	{
		GpioCtrl.SataChangesetMicroBlaze();
    	SetAddrVal(4,0);
    	Gpioin.Delay(8000);
		ReadMaxlba();
		PRINT(maxlba);
		SetAddrVal(9,BLOCK_SIZE);
    	Drive1_woking=1;

    	fileno = 1;
		filenumber = 0;
		fileplace = 0;
		blocknum = 0;
		FileM.clear();

		ReadBySysFifo(FILE_BLOCKLIST_ADDR,0,(UINT *)(BlockM.GetBlockLAddr()),BlockM.BlockListSize);

		BlockM.showMutiFileIndex();
		for(UINT i=0;i<FILENUM_MAX;i++)
		{
			ReadBySysFifo(FILE_CONTENT_ADDR+i,0,(UINT *)(syszone));
			if(0==syszone[0].File_Start_Vld_N && 0==syszone[0].File_End_Vld_N)
			{
				filenumber++;
				if(fileno <= syszone[0].FileNo)
				{
					fileno = syszone[0].FileNo + 1;
				}
				blocknum = blocknum + syszone[0].BlockNum;
				FileM.SetFileValid(i,syszone[0].FileNo);
			}
			else if(0==syszone[0].File_Start_Vld_N && 0!=syszone[0].File_End_Vld_N)
			{
				fileplace = i;
//				WriteFileEnd();
				break;
			}
		}
		if((blocknum >= BLOCK_NUM)||(filenumber >= FILE_NUM))
		{
			Drive1_full=1;
		}
		else
		{
			Drive1_full=0;
		}
	}
}

void SataIO::Dir(unsigned char driveno)
{
	int index,dataindex;
	UINT i,sendlen,FileNum,fileSum,FileNo;
	char *txbuf;
	Xuint32 FileLen,remain_vector;
//	Xuint8  band;

	if(driveno==1)
	{
    	SetAddrVal(4,0);
    	Gpioin.Delay(800);

    	fileSum=0;
		Data_ZoneH=	0;

		ReadMaxlba();
		txbuf  = (char *)TxBuffer;
		memset(TxBuffer,0,COM_BUFFER_SIZE*sizeof(Xuint32));
		index  = 18;
		dataindex = 10;
		FileNum = FileM.GetLastNum();
		fileSum = FileM.GetFileNum();
		BlockM.showMutiFileIndex();
		PRINT(FileNum);
		i = FileM.GetMinFileNo();
		while(i != 1025)
		{
			ReadBySysFifo(FILE_CONTENT_ADDR+i,0,(unsigned int*)(syszone));
			if(0==syszone[0].File_Start_Vld_N || 0==syszone[0].File_End_Vld_N)
			{
				FileNo = syszone[0].FileNo;
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					if(0==syszone[0].File_Start_Vld_N && 0==syszone[0].File_End_Vld_N)
					{
						txbuf[index]=1;
					}
					else
					{
						txbuf[index]=2;
					}
					index++;
					dataindex++;
				}
				else
				{
					if(0==syszone[0].File_Start_Vld_N && 0==syszone[0].File_End_Vld_N)
					{
						txbuf[index]=1;
					}
					else
					{
						txbuf[index]=2;
					}
					index++;
					dataindex++;
				}

				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;txbuf[index]=((syszone[0].File_Start_Year)>>8)&0xff;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=((syszone[0].File_Start_Year)>>8)&0xff;
					index++;
					dataindex++;
				}

				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_Start_Year)&0xff;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_Start_Year)&0xff;
					index++;
					dataindex++;
				}

				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_Start_Month);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_Start_Month);
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_Start_Day);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_Start_Day);
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_Start_Hour);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_Start_Hour);
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_Start_Minute);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_Start_Minute);
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_Start_Second);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_Start_Second);
					index++;
					dataindex++;
				}

				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=((syszone[0].File_End_Year)>>8)&0xff;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=((syszone[0].File_End_Year)>>8)&0xff;
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_End_Year)&0xff;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_End_Year)&0xff;
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_End_Month);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_End_Month);
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_End_Day);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_End_Day);
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_End_Hour);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_End_Hour);
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_End_Minute);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_End_Minute);
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(syszone[0].File_End_Second);
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(syszone[0].File_End_Second);
					index++;
					dataindex++;
				}

				FileLen = syszone[0].FileSize/2048;
				if(0==FileLen)
				{
					FileLen =1;
				}
				FileLen = FileLen *10;
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(FileLen >>24)& 0xff;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(FileLen >>24)& 0xff;
					index++;
					dataindex++;
				}

				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(FileLen >>16)& 0xff;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(FileLen >>16)& 0xff;
					index++;
					dataindex++;
				}

				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(FileLen >>8)& 0xff;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(FileLen >>8)& 0xff;
					index++;
					dataindex++;
				}

				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=(FileLen )& 0xff;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=(FileLen )& 0xff;
					index++;
					dataindex++;
				}


				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=0;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=0;
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=0;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=0;
					index++;
					dataindex++;
				}
				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=((FileNo) >>8)& 0xff;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=((FileNo) >>8)& 0xff;
					index++;
					dataindex++;
				}

				if(index%4 ==0)
				{
					txbuf[index]=0x65;
					index++;
					txbuf[index]=FileNo;
					index++;
					dataindex++;
				}
				else
				{
					txbuf[index]=FileNo;
					index++;
					dataindex++;
				}
			}
			else
			{
//		 		ohters=  ohters +(syszone.File_End_ZoneL - syszone.File_Start_ZoneL+1)/2048;
			}
			PRINT(i);
			i = FileM.GetNextNum();
		}
		txbuf[0] = 0x65;
		txbuf[1] = 0xAA;
		txbuf[2] = (dataindex >>8)& 0xff;
		txbuf[3] = (dataindex    ) & 0xff;

		txbuf[4] = 0x65;
		txbuf[5] = 0;
		txbuf[6] = 0;
		txbuf[7] = (dataindex >>8)& 0xff;

		txbuf[8] = 0x65;
		txbuf[9] = (dataindex    ) & 0xff;
		txbuf[10] = GetBand;
		remain_vector = (BLOCK_NUM - blocknum) * 256;
		txbuf[11] = (remain_vector>>24) & 0xff;

		txbuf[12] = 0x65;
		txbuf[13] = (remain_vector>>16) & 0xff;
		txbuf[14] = (remain_vector>>8) & 0xff;
		txbuf[15] =  remain_vector & 0xff;

		txbuf[16] = 0x65;
		txbuf[17] = fileSum;
		if(index%4)
		{
			sendlen = index/4+1;
		}
		else
		{
			sendlen = index/4;
		}
		for(i=0;i<sendlen;i++)
		{
			*comptr = TxBuffer[i];
			Gpioin.Delay(1);
		}
	}
}

void SataIO::DeleteFile(unsigned char driveno)
{
	int i;
	Xuint8 band;
	char *txbuf;
	xil_printf("Enter DeleteFile!\n");
    if(driveno==1)
	{
    	SetAddrVal(4,0);
    	Gpioin.Delay(8000);
    	fileno = 1;
		filenumber=0;
		fileplace=0;
		fileblocknum = 0;
		filesize = 0;
		blocknum = 0;
		blockP = 0;
		ReadMaxlba();
		for(i=0;i<FILENUM_MAX;i++)						//
		{
			ReadBySysFifo(FILE_CONTENT_ADDR+i,0,(unsigned int*)(syszone));
			if(0==syszone[0].File_Start_Vld_N || 0==syszone[0].File_End_Vld_N)
			{
				syszone[0].File_Start_Vld_N = 1;
				syszone[0].File_End_Vld_N =1;
				band = syszone[0].File_Band;
				WriteBySysFifo(FILE_CONTENT_ADDR+i,0,(unsigned int*)(syszone));
			}
		}
		FileM.clear();
		BlockM.clear();
		WriteBySysFifo(FILE_BLOCKLIST_ADDR,0,(UINT *)(BlockM.GetBlockLAddr()),BlockM.BlockListSize);
		Gpioin.Delay(100);
		txbuf = (char *)TxBuffer;
		txbuf[0]= 0x69;
		txbuf[1]= 0xAA;
		txbuf[2]= 0x00;
		txbuf[3]= 0x02;
		txbuf[4]= 0x69;
		txbuf[5]= band;
		txbuf[6]= 0x00;
		txbuf[7]= 0x00;
		for(i=0;i<2;i++)
		{
		   *comptr = TxBuffer[i];
		   Gpioin.Delay(1);
	    }
	}
}

void SataIO::Re_messgae(char cmd,char val)

{
	char * txbuf;
	int i;
	txbuf = (char *)TxBuffer;
	txbuf[0]= cmd;
	txbuf[1]= 0xAA;
	txbuf[2]= 0x00;
	txbuf[3]= 0x02;
	txbuf[4]= cmd;
	txbuf[5]= GetBand;
	txbuf[6]= val;
	txbuf[7]= 0x00;
	for(i=0;i<2;i++)
	{
	   *comptr = TxBuffer[i];
	   Gpioin.Delay(1);
    }
}

void SataIO::Re_Full()
{
	char * txbuf;
	int i;
	txbuf = (char *)TxBuffer;
	txbuf[0]= 0x80;
	txbuf[1]= 0xAA;
	txbuf[2]= 0x00;
	txbuf[3]= 0x02;
	txbuf[4]= 0x80;
	txbuf[5]= 1;
	txbuf[6]= 0x00;
	txbuf[7]= 0x00;
	for(i=0;i<2;i++)
	{
	   *comptr = TxBuffer[i];
	   Gpioin.Delay(1);
	}
}

/****************************CmdProcess****************************/
unsigned int SataIO::Data_Record()
{
	UINT Data;
	UINT onelen;
	UINT BlockUsedSpace = 0;
	Data_ZoneL = blockP*BLOCK_SIZE;
	fileblocknum = 1;
	filesize = 0;
	while(1)
	{
		if(checkstop())
		{
			if(BlockUsedSpace < BLOCK_SIZE)
			{
				*(ptr+0)=Data_ZoneL;
				*(ptr+1)=Data_ZoneH;
				*(ptr+2)=1;
				filesize++;
				GpioOut.WSataCMD();
				Gpioin.DriveCheckLoop();
				return 1;
			}
			else
			{
				return 2;
			}
		}
		Data = Gpioin.FifoSize();
		if(Data == 0)
		{
			continue;
		}

		onelen= Data;

		BlockUsedSpace += onelen;
		if(BlockUsedSpace > BLOCK_SIZE)
		{
			*(ptr+0)=Data_ZoneL;
			*(ptr+1)=Data_ZoneH;
			*(ptr+2)=BLOCK_SIZE + onelen - BlockUsedSpace;

			GpioOut.WSataCMD();
			filesize += BLOCK_SIZE + onelen - BlockUsedSpace;
			Gpioin.DriveCheckLoop();


			if(blocknum+fileblocknum < BLOCK_NUM-1)
			{
				blockP = BlockM.GetValidBlock();
				Data_ZoneL = blockP*BLOCK_SIZE;

				Data = Gpioin.FifoSize();
				if(Data == 0)
				{
					continue;
				}
				onelen= Data;
				*(ptr+0)=Data_ZoneL;
				*(ptr+1)=Data_ZoneH;
				*(ptr+2)=onelen;

				GpioOut.WSataCMD();
				Data_ZoneL += onelen;
				BlockUsedSpace = onelen;
				filesize += onelen;
				Gpioin.DriveCheckLoop();

				WriteBySysFifo(FILE_BLOCKLIST_ADDR,0,(UINT *)(BlockM.GetBlockLAddr()),BlockM.BlockListSize);
				fileblocknum++;
				continue;
			}
			else
			{
				Re_messgae(0xb0,3);
				Drive1_full=1;
				return 1;
			}
		}

		*(ptr+0)=Data_ZoneL;
		*(ptr+1)=Data_ZoneH;
		*(ptr+2)=onelen;

		GpioOut.WSataCMD();
		Data_ZoneL += onelen;
		filesize += onelen;
		Gpioin.DriveCheckLoop();
	}
	return 1;
}

unsigned int SataIO::Data_Replay()
{
	unsigned int i;
	UINT endAddr;
	UINT st_L,st_B,BlockNum,LastBlockSize;
	char *txbuf;
	unsigned char cmdstop = 0;
	Xuint32   datainfifo;

	fileplace = FileM.GetFileP(Replay_Index);
	ReadBySysFifo(FILE_CONTENT_ADDR+fileplace,0,(unsigned int*)(syszone));

	Gpioin.Delay(100);
	SetAddrVal(3,0x55000055);							// read to sysfifo status fpga;
	GpioCtrl.RfifoReset(1);
	SetAddrVal(5,0x2);
	Gpioin.Delay(1);

	st_B = syszone[0].StartBlock;
	BlockNum = syszone[0].BlockNum;
	LastBlockSize = syszone[0].FileSize - BLOCK_SIZE*(BlockNum - 1);
	for(UINT j=0;j<BlockNum;j++)
	{
		if(j == 0)
		{
			st_L = BLOCK_SIZE*st_B;
			if(BlockNum == 1)
			{
				endAddr = LastBlockSize + st_L;
			}
			else
			{
				endAddr = BLOCK_SIZE + st_L;
			}
		}
		else if(j < BlockNum-1)
		{
			st_B = BlockM.GetNextBlockNo(st_B);
			st_L = BLOCK_SIZE*st_B;
			endAddr = BLOCK_SIZE + st_L;
		}
		else
		{
			st_B = BlockM.GetNextBlockNo(st_B);
			st_L = BLOCK_SIZE*st_B;
			endAddr = LastBlockSize + st_L;
		}

		for(i=st_L;i<endAddr;i=i+4)
		{
			while(1)
			{
				if(Check_Replay_Stop())
				{
					cmdstop=1;
					goto line_end;
				}
				datainfifo = *(ptr+1028);
				if (datainfifo>512)
				{
					continue;
				}
				else
				{
					break;
				}
			}
			SetAddrVal(0,i);
			SetAddrVal(1,0);
			SetAddrVal(2,4);

			GpioOut.RSataCMD();
			Gpioin.DriveCheckLoop();
		}
	}

	 if(cmdstop==0)
	 {
		 i=i-4;
		 if ((endAddr-i>0)&& (endAddr-i)<=3 )
		 {
			do
			{
				datainfifo = *(ptr+1028);
			}while(datainfifo>512);

			SetAddrVal(0,i);
			SetAddrVal(1,0);
			SetAddrVal(2,endAddr-i);
			GpioOut.RSataCMD();
			Gpioin.DriveCheckLoop();
		 }
	 }
line_end:
	SetAddrVal(3,0);								//normal extern data
	SetAddrVal(5,0);
	txbuf = (char *)TxBuffer;
	txbuf[0]= 0xB1;
	txbuf[1]= 0xAA;
	txbuf[2]= 0x00;
	txbuf[3]= 0x02;
	txbuf[4]= 0xB1;
	txbuf[5]= GetBand;
	txbuf[6]= 0x55;
	txbuf[7]= 0;
	for(i=0;i<2;i++)
	{
		*comptr = TxBuffer[i];
		Gpioin.Delay(1);
	}
	Gpioin.Delay(1);
	gl_Cmd =0;
	return 0;
}

unsigned int SataIO::export_data()
{
	UINT i;
	int cmd_export;
	UINT st_L,st_B,BlockNum,LastBlockSize,endAddr;
	char *txbuf;
	Xuint32   datainfifo;

	fileplace = FileM.GetFileP(Replay_Index);
	ReadBySysFifo(FILE_CONTENT_ADDR+fileplace,0,(unsigned int*)(syszone));

	Gpioin.Delay(100);
	SetAddrVal(3,0x55000055);									// read to sysfifo status fpga;
	GpioCtrl.RfifoReset(1);
	SetAddrVal(5,0x1);
	Gpioin.Delay(1);

	st_B = syszone[0].StartBlock;
	BlockNum = syszone[0].BlockNum;
	LastBlockSize = syszone[0].FileSize - BLOCK_SIZE*(BlockNum - 1);

	for(UINT j=0;j<BlockNum;j++)
	{
		if(j == 0)
		{
			st_L = BLOCK_SIZE*st_B;
			if(BlockNum == 1)
			{
				endAddr = LastBlockSize + st_L;
			}
			else
			{
				endAddr = BLOCK_SIZE + st_L;
			}
		}
		else if(j < BlockNum-1)
		{
			st_B = BlockM.GetNextBlockNo(st_B);
			st_L = BLOCK_SIZE*st_B;
			endAddr = BLOCK_SIZE + st_L;
		}
		else
		{
			st_B = BlockM.GetNextBlockNo(st_B);
			st_L = BLOCK_SIZE*st_B;
			endAddr = LastBlockSize + st_L;
		}

		for(i=st_L;i<endAddr;i=i+4)
		{
			datainfifo = *(ptr+1028);
			if(0==datainfifo)
			{
				SetAddrVal(0,i);
				SetAddrVal(1,0);
				SetAddrVal(2,4);
				GpioOut.RSataCMD();
				Gpioin.DriveCheckLoop();
			}
			cmd_export =Check_export_Stop();
			if(1==cmd_export)
			{
				goto export_end;
			}
			if(2==cmd_export)
			{
				while(1)
				{
					if(3==Check_export_Stop())
					{
						goto export_end;
					}
				}
			}
			Gpioin.Delay(10);
		}
	}

	i=i-4;
	if (((endAddr-i) > 0) && (endAddr-i) <= 3 )
	{
		do
		{
			datainfifo = *(ptr+1028);
		}while(datainfifo>0);

		SetAddrVal(0,i);
		SetAddrVal(1,0);
		SetAddrVal(2,endAddr-i);
		GpioOut.RSataCMD();
		Gpioin.DriveCheckLoop();
	}
	Gpioin.Delay(5000);
	Gpioin.Delay(5000);

export_end:
	SetAddrVal(3,0);									//normal extern data
	SetAddrVal(5,0);
	txbuf = (char *)TxBuffer;
	txbuf[0]= 0xBF;
	txbuf[1]= 0xAA;
	txbuf[2]= 0x00;
	txbuf[3]= 0x01;
	txbuf[4]= 0xBF;
	txbuf[5]= GetBand;
	txbuf[6]= 0x00;
	txbuf[7]= 0x00;
	gl_Cmd =0;
	for(i=0;i<2;i++)
	{
		*comptr = TxBuffer[i];
		Gpioin.Delay(1);
	}
	Gpioin.Delay(1);

	return 0;
}

void SataIO::delfile()
{
	fileplace = FileM.GetFileP(del_Index);
	if(fileplace != 1024)
	{
		ReadBySysFifo(FILE_CONTENT_ADDR+fileplace,0,(unsigned int*)(syszone));
		syszone[0].File_Start_Vld_N=1;
		syszone[0].File_End_Vld_N=1;
		BlockM.DeleteFileNoBlock(syszone[0].FileNo);
		WriteBySysFifo(FILE_CONTENT_ADDR+fileplace,0,(unsigned int*)(syszone));
		WriteBySysFifo(FILE_BLOCKLIST_ADDR,0,(UINT *)(BlockM.GetBlockLAddr()),BlockM.BlockListSize);
		filenumber--;
		blocknum -= syszone[0].BlockNum;
		showStatus();
		Gpioin.Delay(1);
	}
	FileM.DeleteFileP(fileplace);
	BlockM.showMutiFileIndex();
}

unsigned int SataIO::checkstop()
{
	NetTCPpop();
	if(2==gl_Cmd)
	{
		Loglen=0;
		return 1;
	}
	if(0xA==gl_Cmd)
	{
		Loglen=0;
		return 1;
	}
	return 0;
}

void SataIO::CheckBlockFifoStatus()
{
	UINT Status = 0;
	Status = *(ptr+FpgaStatusAddr);
	if((Status&DMA_BLOCKLFIFOHALF_STATUS) == HalfSign)
	{
		if(BlockM.ValidBlockLength - BlockM.ValidBlockInd > 8)
		{
			for(UINT i = BlockM.ValidBlockInd;i < BlockM.ValidBlockInd+8;i++)
			{
				SetAddrVal(BlockChain,BlockM.ValidBlock[i]*BLOCK_SIZE);			//Write BlockChain
			}
			BlockM.ValidBlockInd += 8;
			xil_printf("Func CheckBlockFifoStatus\n");
		}
		else
		{
			for(UINT i = BlockM.ValidBlockInd;i < BlockM.ValidBlockLength;i++)
			{
				SetAddrVal(BlockChain,BlockM.ValidBlock[i]*BLOCK_SIZE);			//Write BlockChain
			}
			BlockM.ValidBlockInd = BlockM.ValidBlockLength-1;
			RecordStatus = RecordFull;
			xil_printf("Func CheckBlockFifoStatus(status): full!\n");
			PRINT(BlockM.ValidBlockLength);
		}
	}
}

unsigned int SataIO::Check_Replay_Stop()
{
	NetTCPpop();
	if(4==gl_Cmd)
	{
		return 1;
	}
	return 0;
}

unsigned int SataIO::Check_export_Stop()
{
	NetTCPpop();
	if(6==gl_Cmd)
	{
		return 1;
	}
	if(7==gl_Cmd)
	{
		return 2;
	}
	if(8==gl_Cmd)
	{
		return 3;
	}
	return 0;
}

void SataIO::ReadBlockBeginAndEnd(UINT BlockNum,UINT Len)
{
	UINT Addr = 0;
	xil_printf("Block%d Begin : -----------------------------\n",BlockNum);
	Addr = BlockNum*BLOCK_SIZE;
	PRINT(Addr);
	ReadByByte(BlockNum*BLOCK_SIZE,0,Len);
	xil_printf("Block%d End : 	-----------------------------\n",BlockNum);
	Addr = BlockNum*BLOCK_SIZE + BLOCK_SIZE - Len;
	PRINT(Addr);
	ReadByByte(BlockNum*BLOCK_SIZE + BLOCK_SIZE - Len,0,Len);
}
