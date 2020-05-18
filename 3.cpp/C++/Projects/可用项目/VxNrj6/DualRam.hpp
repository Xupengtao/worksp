/*
 * DualRam.hpp
 *
 *  Created on: Jun 27, 2019
 *      Author: xupengtao
 */

#ifndef DUALRAM_H_
#define DUALRAM_H_

#include<iostream>
#include<iomanip>
using namespace std;
#include"Struct.h"
#include".\Tools\Macro.h"

#define DEBUG_DUALRAM 0

template<typename _PDWChannelType,
		 size_t DualRamChannelNum,				//PDW数据通道的通道数
		 size_t ChannelRadarMaxNo,				//每个PDW数据通道的可分选雷达的最大数
		 size_t DiscernThreshold,				//每个PDW数据通道的可分选阈值的初始值
		 size_t ScanThreshold>					//参加扫描分析的最小脉冲数
class _DualRam
{
public:
	typedef _PDWChannelType 					PDWChannelType;
	typedef typename PDWChannelType::PDWType 	_PDWType;
	typedef typename PDWChannelType::RfTy 		RfTy;
	typedef typename PDWChannelType::PwTy 		PwTy;
	typedef typename PDWChannelType::PaTy 		PaTy;
	typedef typename PDWChannelType::ToaTy 		ToaTy;
	typedef typename PDWChannelType::TonTy 		TonTy;
    typedef _StaticSemVal<size_t> 				_StaticSemValType;
	typedef _DualRam<_PDWChannelType,DualRamChannelNum,ChannelRadarMaxNo,DiscernThreshold, ScanThreshold>	_ThisType;
	enum _DualRamType 	{Default = 0x00,ReadType = 0x01,WriteType = 0x02};
	enum _ChannelStatus {Readable = 0x01,Writable = 0x02,SecReadable = 0x03,SecWritable = 0x04,WriteFull = 0x05};
	enum _PDWTmpTag		{PDWTmp = 0x01,NotPDWTmp = 0x02};
private:
	_DualRamType 			DualRamType;
    _StaticSemValType		ValidChannelNum;
    bool					DualRamClearTag;
	_PDWChannelType 		*PDWChannel;
	volatile _ChannelStatus ChannelStatus[DualRamChannelNum];
	_PDWChannelType 		*LinkPDWChannel;
	volatile _ChannelStatus *LinkChannelStatus;
	_PDWChannelType 		*PDWTmpChannel;
public:
	_DualRam()
	{
		PDWChannel 	  = new _PDWChannelType[DualRamChannelNum];
		PDWTmpChannel = new _PDWChannelType;
		Clear();
		LinkPDWChannel = NULL;
		LinkChannelStatus = NULL;
	}
	virtual ~_DualRam()
	{
		delete[] PDWChannel;
		delete 	 PDWTmpChannel;
		LinkPDWChannel = NULL;
		LinkChannelStatus = NULL;
	}
	bool 	SetValidChannelNum(size_t ChannelNum){ValidChannelNum = ChannelNum; return true;}
	size_t 	GetValidChannelNum() const {return ValidChannelNum.GetVal();}
	int		GetDualRamType(){return DualRamType;}
	void	SetClearTag(bool DualRamClearTag_) {DualRamClearTag = DualRamClearTag_;}
	bool	GetClearTag() const {return DualRamClearTag;}
	bool	JudgeClear(size_t ChanNoBegin, size_t ChanNoEnd)
	{
		if(GetClearTag() == true)
		{
			for(size_t i = ChanNoBegin; i < ChanNoEnd; i++)
			{
				ClearChannel(i);
			}
			SetClearTag(false);
			return true;
		}
		else
		{
			return false;
		}
	}
	void 	Clear()
	{
		DualRamType 	= Default;
		ValidChannelNum = 0;
		DualRamClearTag = false;
		for(size_t i = 0;i < DualRamChannelNum;i++)
		{
			ClearChannelData(PDWChannel[i]);
			if(DualRamType == WriteType)
			{
				ChannelStatus[i] = Writable;
			}
			else if(DualRamType == ReadType)
			{
				ChannelStatus[i] = SecWritable;
			}
			else if(DualRamType == Default)
			{
				ChannelStatus[i] = Writable;
			}
		}
		ClearChannelData(*PDWTmpChannel,PDWTmp);
	}

	void 	ContinueWrite(size_t cH)
	{
		if(DualRamType == Default)
		{
			PDWChannel[cH].DiscernStLoc += PDWChannel[cH].DiscernPulseNums.GetVal();
		}
	}
	
	void 	ResetThreshold(size_t cH)
	{
		if(DualRamType == Default)
		{
			PDWChannel[cH].DiscernThreshold		= DiscernThreshold;
			PDWChannel[cH].DiscernReadable 		= false;
			PDWChannel[cH].DiscernPulseNums 	= 0;
			PDWChannel[cH].ScanThreshold		= ScanThreshold;
			PDWChannel[cH].ScanReadable 		= false;
			PDWChannel[cH].ScanPulseNums 		= 0;
		}
	}
	
	bool 	ClearChannel(size_t cH)
	{
		bool Rtn = false;
		switch(DualRamType)
		{
		case Default:
			ClearChannelData(PDWChannel[cH]);
			ChannelStatus[cH] = Writable;
			Rtn = true;
			break;
		case ReadType:
			if(ChannelStatus[cH] == Readable)
			{
				ClearChannelData(PDWChannel[cH]);
				if(LinkChannelStatus[cH] == Writable)
				{
					ChannelStatus[cH] = SecWritable;
				}
				else if(LinkChannelStatus[cH] == SecWritable)
				{
					LinkChannelStatus[cH] = Writable;
					ChannelStatus[cH] = SecWritable;
				}
				else if(LinkChannelStatus[cH] == SecReadable)
				{
					LinkChannelStatus[cH] = Readable;
					ChannelStatus[cH] = Writable;
				}
				else
				{
					ChannelStatus[cH] = Writable;
				}
				Rtn = true;
			}
			else if(LinkChannelStatus[cH] == Readable)
			{
				ClearChannelData(LinkPDWChannel[cH]);
				if(ChannelStatus[cH] == Writable)
				{
					LinkChannelStatus[cH] = SecWritable;
				}
				else if(ChannelStatus[cH] == SecWritable)
				{
					ChannelStatus[cH] = Writable;
					LinkChannelStatus[cH] = SecWritable;
				}
				else if(ChannelStatus[cH] == SecReadable)
				{
					ChannelStatus[cH] = Readable;
					LinkChannelStatus[cH] = Writable;
				}
				else
				{
					LinkChannelStatus[cH] = Writable;
				}
				Rtn = true;
			}
			else
			{
				Rtn = false;
			}
			break;
		default:
			Rtn = false;
			break;
		}
		return Rtn;
	}
	void	ClearChannelData(_PDWChannelType &PDWChannelTmp,_PDWTmpTag _PDWTag = NotPDWTmp)
	{
		PDWChannelTmp.SelfCheckResultTag 	= 0;
		PDWChannelTmp.DiscernStLoc 			= 0;
		PDWChannelTmp.PulseNum 				= 0;
		PDWChannelTmp.DiscernThreshold		= DiscernThreshold;
		PDWChannelTmp.DiscernReadable 		= false;
		PDWChannelTmp.DiscernPulseNums 		= 0;
		PDWChannelTmp.ScanThreshold			= ScanThreshold;
		PDWChannelTmp.ScanReadable 			= false;
		PDWChannelTmp.ScanPulseNums 		= 0;
		PDWChannelTmp.CrossChannelTag 		= 0;
		PDWChannelTmp.ScanAnaly		       	= 0;
		PDWChannelTmp.RadarNumber	       	= 0;
		memset(PDWChannelTmp.ResultArrLoc,0,sizeof(int)*ChannelRadarMaxNo);
		memset(&(PDWChannelTmp.PulseWord),0,sizeof(_PDWType));
	}
	bool 	WriteLink(_ThisType &LinkDualRam)
	{
		DualRamType = WriteType;
		LinkPDWChannel = LinkDualRam.PDWChannel;
		LinkChannelStatus = LinkDualRam.ChannelStatus;
		Clear();
		LinkDualRam.DualRamType = ReadType;
		LinkDualRam.LinkPDWChannel = PDWChannel;
		LinkDualRam.LinkChannelStatus = ChannelStatus;
		LinkDualRam.Clear();
		return true;
	}
	bool 	ReadLink(_ThisType &LinkDualRam)
	{
		DualRamType = ReadType;
		LinkPDWChannel = LinkDualRam.PDWChannel;
		LinkChannelStatus = LinkDualRam.ChannelStatus;
		Clear();
		LinkDualRam.DualRamType = WriteType;
		LinkDualRam.LinkPDWChannel = PDWChannel;
		LinkDualRam.LinkChannelStatus = ChannelStatus;
		LinkDualRam.Clear();
		return true;
	}
	bool 	WriteComplete(size_t cH)
	{
#if DEBUG_DUALRAM
		cout<<"--> WriteComplete"<<endl;
#endif
		switch(DualRamType)
		{
		case Default:
			ChannelStatus[cH] = WriteFull;
			return true;
			break;
		case WriteType:
			if(ChannelStatus[cH] == Writable)
			{
				if(LinkChannelStatus[cH] == Readable)
				{
					ChannelStatus[cH] = SecReadable;
				}
				else if(LinkChannelStatus[cH] == SecReadable)
				{
					LinkChannelStatus[cH] = Readable;
					ChannelStatus[cH] = SecReadable;
				}
				else if(LinkChannelStatus[cH] == SecWritable)
				{
					LinkChannelStatus[cH] = Writable;
					ChannelStatus[cH] = Readable;
				}
				else
				{
					ChannelStatus[cH] = Readable;
				}
				return true;
			}
			else if(LinkChannelStatus[cH] == Writable)
			{
				if(ChannelStatus[cH] == Readable)
				{
					LinkChannelStatus[cH] = SecReadable;
				}
				else if(ChannelStatus[cH] == SecReadable)
				{
					ChannelStatus[cH] = Readable;
					LinkChannelStatus[cH] = SecReadable;
				}
				else if(ChannelStatus[cH] == SecWritable)
				{
					ChannelStatus[cH] = Writable;
					LinkChannelStatus[cH] = Readable;
				}
				else
				{
					LinkChannelStatus[cH] = Readable;
				}
				return true;
			}
			else
			{
				return false;
			}
			break;
		default:
			return false;
			break;
		}
	}
	bool 	ReadComplete(size_t cH)
	{
		switch(DualRamType)
		{
		case Default:
			PDWChannel[cH].Readable = false;
			return true;
			break;
		case ReadType:
			switch(ChannelStatus[cH])
			{
			case Readable:
				PDWChannel[cH].Readable = false;
				return true;
				break;
			case SecReadable:
				if(LinkChannelStatus[cH] != Readable)
				{
					ChannelStatus[cH] = Readable;
					PDWChannel[cH].Readable = false;
					return true;
				}
				else
				{
					LinkPDWChannel[cH].Readable = false;
					return true;
				}
				break;
			case Writable:
			case SecWritable:
			default:
				if(LinkChannelStatus[cH] == Readable)
				{
					LinkPDWChannel[cH].Readable = false;
					return true;
				}
				else if(LinkChannelStatus[cH] == SecReadable)
				{
					LinkChannelStatus[cH] = Readable;
					LinkPDWChannel[cH].Readable = false;
					return true;
				}
				else
				{
					return false;
				}
				break;
			}
			break;
		default:
			return false;
			break;
		}
	}
	_PDWChannelType& operator[](size_t cH)
	{
#if DEBUG_DUALRAM
		cout<<"--> _DualRam operator[]"<<endl;
		cout<<"ChannelStatus[cH] - "<<ChannelStatus[cH]<<"     LinkChannelStatus[cH] - "<<LinkChannelStatus[cH]<<endl;
#endif
		switch(DualRamType)
		{
		case Default:
			return PDWChannel[cH];
		case WriteType:
			switch(ChannelStatus[cH])
			{
			case Writable:
				return PDWChannel[cH];
			case SecWritable:
				if(LinkChannelStatus[cH] != Writable)
				{
					ChannelStatus[cH] = Writable;
					return PDWChannel[cH];
				}
				else
				{
					return LinkPDWChannel[cH];
				}
			case Readable:
			case SecReadable:
			default:
				if(LinkChannelStatus[cH] == Writable)
				{
					return LinkPDWChannel[cH];
				}
				else if(LinkChannelStatus[cH] == SecWritable)
				{
					LinkChannelStatus[cH] = Writable;
					return LinkPDWChannel[cH];
				}
				else
				{
					return *PDWTmpChannel;
				}
			}
		case ReadType:
			switch(ChannelStatus[cH])
			{
			case Readable:
				return PDWChannel[cH];
			case SecReadable:
				if(LinkChannelStatus[cH] != Readable)
				{
					ChannelStatus[cH] = Readable;
					return PDWChannel[cH];
				}
				else
				{
					return LinkPDWChannel[cH];
				}
			case Writable:
			case SecWritable:
			default:
				if(LinkChannelStatus[cH] == Readable)
				{
					return LinkPDWChannel[cH];
				}
				else if(LinkChannelStatus[cH] == SecReadable)
				{
					LinkChannelStatus[cH] = Readable;
					return LinkPDWChannel[cH];
				}
				else
				{
					return *PDWTmpChannel;
				}
			}
		default:
			return *PDWTmpChannel;
		}
	}
	void	ShowStatus() const
	{
		NLOGKSSECBEG("DualRam Status");
		NLOGKSWIDTH(4,"PDW通道数(DualRamCha...)	",DualRamChannelNum);			//PDW数据通道的通道数
		NLOGKSWIDTH(4,"每通道雷达数(ChannelRa...)	",ChannelRadarMaxNo);			//每个PDW数据通道的可分选雷达的最大数
		NLOGKSWIDTH(4,"阈值初始值(DiscernThr...)	",DiscernThreshold);			//每个PDW数据通道的可分选阈值的初始值
		NLOGKSWIDTH(4,"扫描分析脉冲数(ScanThre...)	",ScanThreshold);				//参加扫描分析的最小脉冲数
		NLOGKSWIDTH(4,"清除标志(DualRamClear...)	",DualRamClearTag);				//清除标志
		NLOGKSSECEND("DualRam Status");
	}
};

#endif /* DUALRAM_H_ */
