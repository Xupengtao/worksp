/*
 * MemCheck.hpp
 * �ڴ��ջ��ַ����࣬����ʹ�ú�MEMCHECK_INIT��MEMCHECK_ADDVAR��MEMCHECK_OVERFLOWCHECK��ɶ�Ӧ����
 *  Created on: Jun 27, 2019
 *      Author: xupengtao
 */

#ifndef MEMCHECK_H_
#define MEMCHECK_H_

#include"ERROR.h"
#include<iostream>
#include<iomanip>
using namespace std;
#include"Macro.h"

#define MEMCHECK_INIT 				_MemCheck MemCheck								//��ʼ��
#define MEMCHECK_ADDVAR1(var) 		MemCheck.ValAssembleAddrs(#var,var)				//��Ӽ�����
#define MEMCHECK_ADDVAR2(var,lens) 	MemCheck.ValAssembleAddrs(#var,var,lens)		//��Ӽ�����
#define MEMCHECK_ADDVAR(...) 		INVOKE_VAR_MACRO(MEMCHECK_ADDVAR,__VA_ARGS__)	//Magic��
#define MEMCHECK_OVERFLOWCHECK  	MemCheck.ValAddrOverflowCheck()					//��ӡ�����

#define MEMCHECK_ARRAY_SIZE 	100
#define MEMCHECK_STRING_LENGTH 	60

class _MemCheck
{
private:
	typedef char (_MemCheckStr)[MEMCHECK_STRING_LENGTH];
	size_t *VarStartAddrs;
	size_t *VarEndAddrs;
	_MemCheckStr *VarNames;
	size_t *ValLens;
	_MemCheckStr *VarAddrCheckResult;
	
	size_t VarAddrsSize;
public:
	_MemCheck()
	{
		VarStartAddrs = new size_t[MEMCHECK_ARRAY_SIZE];
		VarEndAddrs   = new size_t[MEMCHECK_ARRAY_SIZE];
		VarNames = new _MemCheckStr[MEMCHECK_ARRAY_SIZE];
		ValLens  = new size_t[MEMCHECK_ARRAY_SIZE];
		VarAddrCheckResult = new _MemCheckStr[MEMCHECK_ARRAY_SIZE];
		clear();
	}
	virtual ~_MemCheck()
	{
		delete[] VarStartAddrs;
		delete[] VarEndAddrs;
		delete[] VarNames;
		delete[] ValLens;
		delete[] VarAddrCheckResult;
	};
	void WriteStr(_MemCheckStr &CheckStr,char *Str)
	{
		for(size_t StrNum = 0;StrNum < MEMCHECK_STRING_LENGTH;StrNum++)
		{
			if(Str[StrNum] != '\0')
			{
				CheckStr[StrNum] = Str[StrNum];
			}
			else
			{
				CheckStr[StrNum] = Str[StrNum];
				break;
			}
		}
	}
	void CopyStr(_MemCheckStr &Strsrc,_MemCheckStr &Strdst)
	{
		for(size_t StrNum = 0;StrNum < MEMCHECK_STRING_LENGTH;StrNum++)
		{
			Strdst[StrNum] = Strsrc[StrNum];
		}
	}
	void clear()
	{
		VarAddrsSize = 0;
		_MemCheckStr MemCheckStrTmp;
		char *MemCheckStr2Char = "Correct!";
		WriteStr(MemCheckStrTmp,MemCheckStr2Char);
		for(size_t Num = 0;Num < MEMCHECK_ARRAY_SIZE;Num++)
		{
			CopyStr(MemCheckStrTmp,VarAddrCheckResult[Num]);
		}
	}
	template<typename Tx>
	int ValAssembleAddrs(char *ValName,Tx &Val)
	{
		if(VarAddrsSize < MEMCHECK_ARRAY_SIZE)
		{
			VarStartAddrs[VarAddrsSize] = size_t(&Val);
			WriteStr(VarNames[VarAddrsSize],ValName);
			ValLens[VarAddrsSize] = sizeof(Tx);
			VarEndAddrs[VarAddrsSize] = VarStartAddrs[VarAddrsSize]+ValLens[VarAddrsSize];
			VarAddrsSize++;
			return _OK;
		}
		else
		{
			return _ERROR;
		}
	}
	template<typename Tx>
	int ValAssembleAddrs(char *ValName,Tx *Val,size_t Lens)
	{
		if(VarAddrsSize < MEMCHECK_ARRAY_SIZE)
		{
			VarStartAddrs[VarAddrsSize] = size_t(Val);
			WriteStr(VarNames[VarAddrsSize],ValName);
			ValLens[VarAddrsSize] = sizeof(Tx)*Lens;
			VarEndAddrs[VarAddrsSize] = VarStartAddrs[VarAddrsSize]+ValLens[VarAddrsSize];
			VarAddrsSize++;
			return _OK;
		}
		else
		{
			return _ERROR;
		}
	}
	void showNum(int Num)
	{
		cout<<Num<<"."<<VarNames[Num]<<": "<<endl;
		cout<<"      AddressZone:  "<<hex<<uppercase<<"0x"<<VarStartAddrs[Num]<<" -> "<<"0x"<<VarEndAddrs[Num]<<endl;
		cout<<"      ValLens:      "<<hex<<"0x"<<ValLens[Num]<<endl;
		cout<<"      CheckResult:  "<<VarAddrCheckResult[Num]<<endl;
	}
	
	int ValAddrOverflowCheck()
	{
		for(size_t Seek_i = 0;Seek_i < VarAddrsSize;Seek_i++)
		{
			size_t startAddr = VarStartAddrs[Seek_i];
			size_t endAddr = VarEndAddrs[Seek_i];
			for(size_t Seek_j = Seek_i+1;Seek_j < VarAddrsSize;Seek_j++)
			{
				if((VarStartAddrs[Seek_j] < startAddr) && (VarEndAddrs[Seek_j] > startAddr))
				{
					WriteStr(VarAddrCheckResult[Seek_i],"Memory Address Error!");
					WriteStr(VarAddrCheckResult[Seek_j],"Memory Address Error!");
				}
				else if((VarStartAddrs[Seek_j] >= startAddr) && (VarStartAddrs[Seek_j] < endAddr))
				{
					WriteStr(VarAddrCheckResult[Seek_i],"Memory Address Error!");
					WriteStr(VarAddrCheckResult[Seek_j],"Memory Address Error!");
				}
			}
		}
		for(size_t Num = 0;Num < VarAddrsSize;Num++)
		{
			showNum(Num);
		}
		clear();
		return _OK;
	}

};

#endif /* MEMCHECK_H_ */
