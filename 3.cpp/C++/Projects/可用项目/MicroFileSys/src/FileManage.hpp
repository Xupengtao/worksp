/*
 * FileManage.cpp
 *
 *  Created on: 2018-9-25
 *      Author: xupengtao
 */
#ifndef FILEMANAGE_H
#define FILEMANAGE_H

#include "struct.h"

template <typename T>
class FileManage
{
private:
	enum{Invalid = 0};
	UINT FileNum;
	UINT LastNum;
	UINT fileMinNo;
	T FileL[FILE_NUM];
public:
	FileManage(){clear();xil_printf("FileManage Init Success!\n");}
	~FileManage(){};
	void clear();
	void SetFileValid(UINT Num,UINT FileNo);
	UINT GetNextFileP();
	UINT GetFileP(UINT FileNo);
	void DeleteFileP(UINT fileplace);
	UINT GetFileNum() const;
	UINT GetLastNum();
	void showValidFileP();
	UINT GetMinFileNo();
	UINT GetNextNum();
};

template <typename T>
inline void FileManage<T>::clear()
{
	FileNum = 0;
	LastNum = 0;
	fileMinNo = 65536;
	for(UINT i=0;i<FILE_NUM;i++)
	{
		FileL[i] = Invalid;
	}
}

template <typename T>
inline void FileManage<T>::SetFileValid(UINT Num,UINT FileNo)
{
	FileL[Num] = FileNo;
}

template <typename T>
inline UINT FileManage<T>::GetNextFileP()
{
	UINT i;
	for(i=0;i<FILE_NUM;i++)
	{
		if(FileL[i] == Invalid)
			break;
	}
	return i;
}

template <typename T>
inline UINT FileManage<T>::GetFileP(UINT FileNo)
{
	UINT i;
	for(i=0;i<FILE_NUM;i++)
	{
		if(FileL[i] == FileNo)
		{
			return i;
		}
	}
	return 1024;
}

template <typename T>
inline void FileManage<T>::DeleteFileP(UINT fileplace)
{
	FileL[fileplace] = Invalid;
}

template <typename T>
inline UINT FileManage<T>::GetFileNum() const
{
	UINT Num = 0;
	for(UINT i=0;i<FILE_NUM;i++)
	{
		if(FileL[i] != Invalid)
			Num++;
	}
	return Num;
}

template <typename T>
inline UINT FileManage<T>::GetLastNum()
{
	UINT Num = 0;
	for(UINT i=0;i<FILE_NUM;i++)
	{
		if(FileL[i] != Invalid)
			Num = i;
	}
	LastNum = Num+1;
	return Num+1;
}

template <typename T>
inline void FileManage<T>::showValidFileP()
{
	xil_printf("showValidFileP\n");
	for(UINT i=0;i<FILE_NUM;i++)
	{
		if(FileL[i] != Invalid)
			PRINT(i);
	}
}

template <typename T>
inline UINT FileManage<T>::GetMinFileNo()
{
	if(LastNum >= FILE_NUM)
	{
		return 1025;
	}
	for(UINT i=0;i<LastNum;i++)
	{
		if((FileL[i] != 0)&&(FileL[i] < fileMinNo))
		{
			FileNum = i;
			fileMinNo = FileL[i];
		}
	}
	return FileNum;
}

template <typename T>
inline UINT FileManage<T>::GetNextNum()
{
	UINT i,flieTemp = 65536;
	for(i=0;i<LastNum;i++)
	{
		if(FileL[i] > fileMinNo)
		{
			if(FileL[i] < flieTemp)
			{
				flieTemp = FileL[i];
				FileNum = i;
			}
		}
	}
	if((fileMinNo == flieTemp)||(flieTemp == 65536))
	{
		fileMinNo = 65536;
		FileNum = 0;
		LastNum = 0;
		return 1025;
	}
	else
	{
		fileMinNo = flieTemp;
		return FileNum;
	}
}

#endif
