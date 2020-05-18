#ifndef NRJ6MSG_H_
#define NRJ6MSG_H_
enum _MsgCmd
{
	LongMsgType 		= 0x1B,
	ShortMsgType 		= 0x1C,
	MissingMsgType		= 0x1D,
	ParaUpdataMsgType	= 0xC9
};
enum _MsgPdwType
{
	Unknowed			= 0,
	FixRfFixPri			= 1,
	FixRfJaggPri		= 2,
	FixRfDitherPri		= 3,
	AgileRfFixPri		= 4,
	CotinuePulse		= 5,
	AgileRfDitherPri	= 6
};
#pragma pack(1)
union _BigToLittle
{
	USHORT uSh;
	UCHAR  uCh[2];
};
struct _MsgPdwPara								/*0x1b		0x1c		0x1d		0xc9*/
{		
	USHORT 	Rf;						//2-3		Long								ParaUpdata
	USHORT 	Doa;					//4-5		Long								ParaUpdata
	USHORT 	Pri;					//6-7		Long								ParaUpdata
	USHORT 	Far;					//8-9		Long								ParaUpdata
	USHORT 	Pw;						//10-11		Long								ParaUpdata
	UCHAR 	Pa;						//12		Long								ParaUpdata
	UCHAR 	JaggNums;				//13		Long
	USHORT 	PhasePeriods[8];			//14-29		Long
	USHORT 	FramePeriods;			//30-31		Long
	UCHAR 	DitherRange;			//32		Long
};
struct _MsgScanPara
{		
	USHORT 	AntenaRpm;				//33-34					Short
	UCHAR 	ScanType;				//35		Long		Short
	USHORT 	ScanPeriod;				//36-37					Short
	USHORT 	IrradiateTime;			//38-39								Missing
};
struct _Msg
{
	UCHAR 			Cmd;			//0			Long		Short		Missing		ParaUpdata
	UCHAR 			Type;			//1			Long								ParaUpdata
	_MsgPdwPara 	PdwPara;
	_MsgScanPara	ScanPara;
	USHORT 			BatchNo;		//40-41
	
	_Msg() {memset(this,0,sizeof(_Msg));}
	~_Msg(){}
	USHORT BigToLittle(USHORT sh)
	{
		UCHAR chTmp;
		_BigToLittle *BigToLittle = (_BigToLittle *)(&sh);
		chTmp = BigToLittle->uCh[1];
		BigToLittle->uCh[1] = BigToLittle->uCh[0];
		BigToLittle->uCh[0] = chTmp;
		return sh;
	}
	void SetPdwPara(_MsgPdwType MsgPdwType, _MsgPdwPara &MsgPdwPara)
	{
		Type = MsgPdwType;
		PdwPara.Rf		= BigToLittle(MsgPdwPara.Rf);			
		PdwPara.Doa		= BigToLittle(MsgPdwPara.Doa);				
		PdwPara.Pri		= BigToLittle(MsgPdwPara.Pri);				
		PdwPara.Far		= BigToLittle(MsgPdwPara.Far);				
		PdwPara.Pw		= BigToLittle(MsgPdwPara.Pw);				
		PdwPara.Pa		= MsgPdwPara.Pa;
		if(Cmd == LongMsgType)
		{
			PdwPara.JaggNums = MsgPdwPara.JaggNums;
			for(int i = 0; i < 8; i++)
			{
				PdwPara.PhasePeriods[i] = BigToLittle(MsgPdwPara.PhasePeriods[i]);
			}
			PdwPara.FramePeriods = BigToLittle(MsgPdwPara.FramePeriods);
			PdwPara.DitherRange = MsgPdwPara.DitherRange;
		}
	}
	void SetScanPara(_MsgScanPara &MsgScanPara)
	{
		ScanPara.AntenaRpm		= BigToLittle(MsgScanPara.AntenaRpm);			
		ScanPara.ScanType		= MsgScanPara.ScanType;				
		ScanPara.ScanPeriod		= BigToLittle(MsgScanPara.ScanPeriod);				
		ScanPara.IrradiateTime	= BigToLittle(MsgScanPara.IrradiateTime);	
	}
	void SetBatchNo(USHORT BatchNo_ = 0)
	{
		BatchNo = BigToLittle(BatchNo_+1);
	}
	void SetLongMsg(_MsgPdwType MsgPdwType, _MsgPdwPara &MsgPdwPara, USHORT BatchNo = 0)
	{
		Cmd = LongMsgType;
		SetPdwPara(MsgPdwType, MsgPdwPara);
		ScanPara.ScanType = 1;
		SetBatchNo(BatchNo);
	}
	void SetShrotMsg(_MsgScanPara &MsgScanPara, USHORT BatchNo = 0)
	{
		Cmd = ShortMsgType;
		SetScanPara(MsgScanPara);
		SetBatchNo(BatchNo);
	}
	void SetMissingMsg(USHORT BatchNo = 0)
	{
		Cmd = MissingMsgType;
		SetBatchNo(BatchNo);
	}
	void SetParaUpdataMsg(_MsgPdwType MsgPdwType, _MsgPdwPara &MsgPdwPara, USHORT BatchNo = 0)
	{
		Cmd = ParaUpdataMsgType;
		SetPdwPara(MsgPdwType, MsgPdwPara);
		SetBatchNo(BatchNo);
	}
};
struct _SysCmd
{
	int 		DoaUpdata;
};
struct _DoaPaMsg
{
	USHORT		BatchNo;		//0-1
	USHORT		Blank;			//2-3
	USHORT 		Doa;			//4-5
	UCHAR 		Pa;				//6
};
union _DoaOrEdMsg
{
	_DoaPaMsg 	DoaPaMsg;
	USHORT		EndMsg;
};
struct _DoaUpdataMsg
{
	CONUCHAR 	Cmd;
	_DoaOrEdMsg DoaOrEdMsg[0x100];
	int			Lens;
	int			CharLens;					//按Char算
	_DoaUpdataMsg():Cmd(0xc7),Lens(0),CharLens(0){};
	USHORT BigToLittle(USHORT sh)
	{
		UCHAR chTmp;
		_BigToLittle *BigToLittle = (_BigToLittle *)(&sh);
		chTmp = BigToLittle->uCh[1];
		BigToLittle->uCh[1] = BigToLittle->uCh[0];
		BigToLittle->uCh[0] = chTmp;
		return sh;
	}
	void SetDoaPaMsg(USHORT BatchNo,USHORT Doa,UCHAR Pa)
	{
		DoaOrEdMsg[Lens].DoaPaMsg.BatchNo 	= BigToLittle(BatchNo+1);
		DoaOrEdMsg[Lens].DoaPaMsg.Doa 		= BigToLittle(Doa);
		DoaOrEdMsg[Lens].DoaPaMsg.Pa 		= Pa;
		Lens++;
		CharLens += 7;
	}
	void SetEndTag()
	{
		DoaOrEdMsg[Lens].EndMsg = 0xAA55;
		Lens++;
		CharLens += 2;
	}
	void Clear()
	{
		Lens = 0;
		CharLens = 1;
		memset(DoaOrEdMsg,0,sizeof(_DoaOrEdMsg)*0x100);
	}
};
#pragma pack()
#endif /* NRJ6MSG_H_ */
