/*
 * BlockManage.h
 *
 *  Created on: 2018-9-21
 *      Author: xupengtao
 */
#ifndef BLOCKMANAGE_H
#define BLOCKMANAGE_H

#include "struct.h"

template <typename T>
struct BlockList
{
	T FileNo;
	T Next;
//	BlockList<T>* next;
};

template <typename Type>
class BlockManage
{
private:
	UINT BlockPlace;
	BlockList<Type> BlockL[BLOCK_NUM];

public:
	UINT ValidBlockInd;
	UINT ValidBlockLength;
	UINT BlockListSize;
	USHORT	ValidBlock[BLOCK_NUM];
	UINT DebugInd;
	BlockManage();
	~BlockManage(){};
	BlockList<Type>* GetBlockLAddr();
	void CalculateBlockLSize();
	UINT GetBlockPlace() const {return BlockPlace;}
	void clear();
	void SetValidBlock();
	UINT GetValidBlock(Type Ind = 0){return UINT(ValidBlock[Ind == 0?ValidBlockInd:Ind]);}
	void SetBlock(Type Ind,Type FileNo,Type NextBlock = 0){BlockL[Ind].FileNo = FileNo;BlockL[Ind].Next = NextBlock;}
	int  NextUnusedBlock();
	UINT CreateFileNoBlock(UINT FileNo);
	void DeleteFileNoBlock(UINT FileNo);
	void EndFileNoBlockSet(UINT Num){BlockL[Num].Next = 0;}
	UINT GetNextBlockNo(UINT BlockNo,UINT FileNo = 0x10000);
	UINT GetFileNoBlockNum(Type FileNo);
	UINT GetUsedBlockNum();
	UINT CheckFileValid(Type FileNo,UINT BlockNum = 0);
	void showFileIndex(UINT FileNo,UINT Num) const;
	void showMutiFileIndex() const;
};

template <typename Type>
BlockManage<Type>::BlockManage():BlockPlace(1),ValidBlockInd(0),ValidBlockLength(0),DebugInd(0)
{
	clear();
	CalculateBlockLSize();
	xil_printf("BlockManage Init Success!\n");
}

template <typename Type>
inline BlockList<Type>* BlockManage<Type>::GetBlockLAddr()
{
	return BlockL;
}

template <typename Type>
inline void BlockManage<Type>::CalculateBlockLSize()
{
	BlockListSize = sizeof(BlockList<Type>)*BLOCK_NUM/512;
}

template <typename Type>
inline void BlockManage<Type>::clear()
{
	BlockPlace = 1;
	ValidBlockInd = 0;
	DebugInd = 0;
	ValidBlockLength = 0;
	for(UINT i = 1;i<BLOCK_NUM;i++)
	{
		BlockL[i].FileNo = 0;
		BlockL[i].Next = 0;
	}
}
template <typename Type>
inline void BlockManage<Type>::SetValidBlock()
{
	UINT i = 0;
	ValidBlockInd = 0;
	ValidBlock[0] = 0;
	for(UINT j = 1;j<BLOCK_NUM;j++)
	{
		ValidBlock[j] = 0;
		if(BlockL[j].FileNo == 0)
		{
			ValidBlock[i] = j;
			i++;
		}
	}
	ValidBlockLength = i;
	PRINT(ValidBlockLength);
}

template <typename Type>
inline int BlockManage<Type>::NextUnusedBlock()
{
	for(UINT i = 1;i<BLOCK_NUM;i++)
	{
		if(BlockL[i].FileNo == 0)
		{
			BlockPlace = i;
			return BlockPlace;
		}
	}
	return -1;
}

template <typename Type>
inline UINT BlockManage<Type>::CreateFileNoBlock(UINT FileNo)
{
	UINT BlockPlaceTemp = BlockPlace;
	BlockL[BlockPlaceTemp].FileNo = FileNo;
	BlockL[BlockPlaceTemp].Next = NextUnusedBlock();
	return BlockPlaceTemp;
}

template <typename Type>
inline void BlockManage<Type>::DeleteFileNoBlock(UINT FileNo)
{
	xil_printf("Enter DeleteFileNoBlock!\n");
	for(UINT i = 1;i<BLOCK_NUM;i++)
	{
		if(BlockL[i].FileNo == FileNo)
		{
			BlockL[i].FileNo = 0;
			BlockL[i].Next = 0;
		}
	}
	BlockPlace = NextUnusedBlock();
}

template <typename Type>
inline UINT BlockManage<Type>::GetFileNoBlockNum(Type FileNo)
{
	UINT Num = 0;
	for(UINT i = 1;i<BLOCK_NUM;i++)
	{
		if(BlockL[i].FileNo == FileNo)
		{
			Num++;
		}
	}
	return Num;
}

template <typename Type>
inline UINT BlockManage<Type>::GetNextBlockNo(UINT BlockNo,UINT FileNo)
{
	if(FileNo == 0x10000)
	{
		return UINT(BlockL[BlockNo].Next);
	}
	else
	{
		if(BlockL[BlockNo].FileNo == Type(FileNo))
		{
			return UINT(BlockL[BlockNo].Next);
		}
	}
	return 0x10000;
}

template <typename Type>
inline UINT BlockManage<Type>::GetUsedBlockNum()
{
	UINT Num = 0;
	for(UINT i = 1;i<BLOCK_NUM;i++)
	{
		if(BlockL[i].FileNo != 0)
		{
			Num++;
		}
	}
	return Num;
}

template <typename Type>
inline UINT BlockManage<Type>::CheckFileValid(Type FileNo,UINT BlockNum)
{
	if(BlockNum == 0)
	{
		for(UINT i = 1;i<BLOCK_NUM;i++)
		{
			if(BlockL[i].FileNo == FileNo)
			{
				return 1;
			}
		}
		return 0;
	}
	else
	{
		if(GetFileNoBlockNum(FileNo) == BlockNum)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
}

template <typename Type>
inline void BlockManage<Type>::showFileIndex(UINT FileNo,UINT Num) const
{
	if(BlockL[Num].FileNo == Type(FileNo))
	{
		xil_printf(" %d-------> ",Num);
		if(BlockL[Num].Next != 0)
		{
			showFileIndex(FileNo,BlockL[Num].Next);
		}
		else
		{
			xil_printf("end\n");
		}
	}
	else
	{
		xil_printf("error!\n");
	}
}

template <typename Type>
inline void BlockManage<Type>::showMutiFileIndex() const
{
	UINT fileTemp = 0;
	for(UINT i=1;i<BLOCK_NUM;i++)
	{
		if((BlockL[i].FileNo != 0) && (BlockL[i].FileNo != fileTemp))
		{
			xil_printf("File.%d:",BlockL[i].FileNo);
			showFileIndex(BlockL[i].FileNo,i);
			fileTemp = BlockL[i].FileNo;
		}
	}
}

#endif
