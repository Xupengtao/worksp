/*
 * MemCheck.hpp
 * 内存堆栈地址检测类，建议使用宏MEMCHECK_INIT、MEMCHECK_ADDVAR和MEMCHECK_OVERFLOWCHECK完成对应操作
 * @Author: xupengtao
 * @Created on: Jun 27, 2019
 * @Last Modified time: 2020-05-19 10:55:35 
 */

#ifndef _MEMCHECK_H_
#define _MEMCHECK_H_

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define MEMCHECK_INIT 				        _MemCheck MemCheck								// 初始化
#define MEMCHECK_BORL                       cout<<MemCheck.BorLEndianCheck()<<endl          // 字节序大小端检测
#define MEMCHECK_ADDVAR(var) 		        MemCheck.AddVal(#var, (var))		            // 添加检测变量
#define MEMCHECK_ADDARRAY(var, lens, pos)   MemCheck.AddArray(#var, (var), (lens), (pos))   // 添加检测变量
#define MEMCHECK_OVERFLOWCHECK  	        MemCheck.AddrOverflowCheck()					// 打印检测结果
#define MEMCHECK_PRINT            	        MemCheck.print()					            // 打印检测结果

struct _MemDesc
{
    string      MemDescName;
    size_t      StartAddr;
    size_t      EndAddr;
    size_t      ArrayLens;
    size_t      sizeofType;
    int*        Pos;
    string      CheckResult;
    _MemDesc()
    {
        init();
    }
    void init()
    {
        MemDescName = "";
        StartAddr   = 0;
        EndAddr     = 0;
        ArrayLens   = 0;
        sizeofType  = 0;
        Pos         = NULL;
        CheckResult = "Correct!";
    }
    void show()
    {
		cout<< MemDescName <<": "<<endl;
		cout<<"      AddressZone: "<<hex<<uppercase<<"0x"<<StartAddr<<" -> "<<"0x"<<EndAddr<<endl;
		cout<<"      sizeofType:  "<<hex<<uppercase<<"0x"<<sizeofType<<endl;
		cout<<"      ArrayLens:   "<<dec<<ArrayLens<<endl;
        cout<<"      Current Pos: "<<dec<<((Pos == NULL) ? size_t(Pos) : *Pos)<<endl;
		cout<<"      CheckResult: "<<CheckResult<<endl;
    }
};

class _MemCheck
{
private:
	vector<_MemDesc>    MemDescVec;
    size_t              Lens;
    size_t              BorLEnd;
public:
	_MemCheck(int Size = 100)
	{
		clear();
        MemDescVec.reserve(Size);
        BorLEnd = BorLEndianCheck();
	}
	virtual ~_MemCheck()
	{
        ;
	}
    int BorLEndianCheck()
    {
        union{
            int  i;
            char chTy[4];
        } BorL;
        if(size_t(&(BorL.chTy[0])) > size_t(&(BorL.chTy[3])))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
	void clear()
	{
        Lens = 0;
        MemDescVec.clear();
	}
	template<typename _T>
	int AddVal(string ValName, _T& Val)
	{
        _MemDesc MemDescTmp;
        MemDescTmp.MemDescName  = ValName;
        MemDescTmp.ArrayLens    = 1;
        MemDescTmp.sizeofType   = sizeof(_T);
        if(BorLEnd == 0)
        {
            MemDescTmp.StartAddr = size_t(&Val);
            MemDescTmp.EndAddr   = MemDescTmp.StartAddr + MemDescTmp.ArrayLens * MemDescTmp.sizeofType - 1;
        }
        else
        {
            MemDescTmp.EndAddr   = size_t(&Val);
            MemDescTmp.StartAddr = MemDescTmp.EndAddr - MemDescTmp.ArrayLens * MemDescTmp.sizeofType + 1;
        }
        MemDescTmp.Pos          = NULL;
        MemDescTmp.CheckResult  = "Correct!";
        MemDescVec.push_back(MemDescTmp);
        Lens++;
        return 0;
	}
	template<typename _T>
	int AddArray(string ArrayName, _T* ArrayAddr, size_t ArrayLens, int* pPos)
	{
        _MemDesc MemDescTmp;
        MemDescTmp.MemDescName  = ArrayName;
        MemDescTmp.ArrayLens    = ArrayLens;
        MemDescTmp.sizeofType   = sizeof(_T);
        if(BorLEnd == 0)
        {
            MemDescTmp.StartAddr = size_t(ArrayAddr);
            MemDescTmp.EndAddr   = MemDescTmp.StartAddr + MemDescTmp.ArrayLens * MemDescTmp.sizeofType - 1;
        }
        else
        {
            MemDescTmp.EndAddr   = size_t(ArrayAddr);
            MemDescTmp.StartAddr = MemDescTmp.EndAddr - MemDescTmp.ArrayLens * MemDescTmp.sizeofType + 1;
        }
        MemDescTmp.Pos          = pPos;
        MemDescTmp.CheckResult  = "Correct!";
        MemDescVec.push_back(MemDescTmp);
        Lens++;
        return 0;
	}
	int AddrOverflowCheck()
	{
		for(size_t i = 0; i < Lens; i++)
		{
			size_t startAddr = MemDescVec[i].StartAddr;
			size_t endAddr   = MemDescVec[i].EndAddr;
			for(size_t j = i+1; j < Lens; j++)
			{
				if((MemDescVec[j].StartAddr < startAddr) && (MemDescVec[j].EndAddr > startAddr))
				{
					MemDescVec[i].CheckResult = "Memory Address Error!";
					MemDescVec[j].CheckResult = "Memory Address Error!";
				}
				else if((MemDescVec[j].StartAddr >= startAddr) && (MemDescVec[j].StartAddr < endAddr))
				{
					MemDescVec[i].CheckResult = "Memory Address Error!";
					MemDescVec[j].CheckResult = "Memory Address Error!";
				}
			}
		}
        for(size_t i = 0; i < Lens; i++)
        {
            if(MemDescVec[i].ArrayLens != 1)
            {
                if(*(MemDescVec[i].Pos) > MemDescVec[i].ArrayLens)
                {
                    MemDescVec[i].CheckResult = "Error: Memory out of range!";
                }
            }
        }
		for(size_t i = 0; i < Lens; i++)
		{
            if(MemDescVec[i].CheckResult != "Correct!")
            {
			    show(i);
            }
		}
		return 0;
	}
    int print()
    {
		for(size_t i = 0; i < Lens; i++)
		{
            show(i);
		}
		return 0;
    }
	int show(int i)
	{
        if(i < Lens)
        {
            cout <<i<<".";
            MemDescVec[i].show();
            return 0;
        }
        else
        {
            return -1;
        }
	}
};

#endif /* _MEMCHECK_H_ */