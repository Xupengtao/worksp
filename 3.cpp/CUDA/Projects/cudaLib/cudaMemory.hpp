/*
 * @Author: xpt 
 * @Date: 2019-12-11 14:21:53 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-11 14:27:50
 */

#ifndef _CUDA_MEMORY_HPP_
#define _CUDA_MEMORY_HPP_

#include <iostream>
using namespace std;
#include<complex>
#include<cuda_runtime.h>

enum _MallocType
{
    Host1D,
    Host2D,
    Gpu1D,
    Gpu2D
};

enum _dType
{
    CharType,
    ShortType,
    IntType,
    FloatType,
    stlComplexType,
    CuComplexType
};

struct _MallocUnit
{
    float *     addr;
    size_t      Lens;
    size_t      Col;
    size_t      pitch;
    _dType      dType;
    _MallocType MallocType;
    bool        MallocSign;
    _MallocUnit()
    {
        addr = NULL;
        dType = FloatType;
        MallocType = Host1D;
        MallocSign = false;
    };
};

class cu_memory
{
public:
    _MallocUnit *MallocUnitList;
    bool        InitSign;
    size_t      Lens;

    cu_memory()
    {
        MallocUnitList = NULL;
        InitSign = false;
        Lens = 0;
    }
    ~cu_memory()
    {
        AddrListFree();
        if(InitSign == true)
        {
            delete[] MallocUnitList;
        }
    }
    void SetLens(size_t Lens_)
    {
        if(InitSign == true)
        {
            delete[] MallocUnitList;
        }
        InitSign = true;
        Lens = Lens_;
        MallocUnitList = new _MallocUnit[Lens];
    }

    float* UnitMalloc(size_t Ind, _MallocType MallocType, size_t dTypeLens, _dType dType = FloatType, size_t Col = 0, size_t pitch = 0)
    {
        if(Ind >= Lens)
        {
            cout<<"预申请空间索引值超出预留最大值"<<endl;
            cout<<__FILE__<<"$"<<__LINE__<<endl;
            return NULL;
        }
        _MallocUnit & MallocUnit = MallocUnitList[Ind];
        MallocUnit.MallocType = MallocType;
        MallocUnit.dType = dType;
        switch(MallocUnit.dType)
        {
            case CharType:
                MallocUnit.Lens = dTypeLens;
                break;
            case ShortType:
                MallocUnit.Lens = dTypeLens * sizeof(short) / sizeof(char);
                break;
            case IntType:
                MallocUnit.Lens = dTypeLens * sizeof(int) / sizeof(char);
                break;
            case FloatType:
                MallocUnit.Lens = dTypeLens * sizeof(float) / sizeof(char);
                break;
            case stlComplexType:
                MallocUnit.Lens = dTypeLens * sizeof(complex<float>) / sizeof(char);
                break;
            case CuComplexType:
                MallocUnit.Lens = dTypeLens * sizeof(cufftComplex) / sizeof(char);
                break;
            default:
                MallocUnit.Lens = 0;
                break;
        }
        MallocUnit.Col = Col;
        MallocUnit.pitch = pitch;
        UnitMalloc(MallocUnit);
        return MallocUnit.addr;
    }

    cudaError_t UnitMalloc(_MallocUnit & MallocUnit)
    {
        cudaError_t cudaError = cudaErrorUnknown;
        if(MallocUnit.MallocSign == true)
        {
            cout<<"UnitMalloc Failed!"<<endl;
            return cudaError;
        }
        if(MallocUnit.Lens == 0)
        {
            cout<<"错误：数据长度设置为0或未设置!"<<endl;
            cudaError = cudaErrorUnknown;
        }
        else if(MallocUnit.MallocType == Host1D)
        {
            cudaError = cudaMallocHost((void**)(&MallocUnit.addr),MallocUnit.Lens* sizeof(char));
        }
        else if(MallocUnit.MallocType == Host2D)
        {
            cudaError = cudaMallocHost((void**)(&MallocUnit.addr), MallocUnit.Lens * sizeof(std::complex<char>));
        }
        else if(MallocUnit.MallocType == Gpu1D)
        {
            cudaError = cudaMalloc((void**)(&MallocUnit.addr), MallocUnit.Lens * sizeof(char));
        }
        else if(MallocUnit.MallocType == Gpu2D)
        {
            if((MallocUnit.pitch == 0) or (MallocUnit.Col == 0))
            {
                cout<<"错误：矩阵pitch或Col设置为0或未设置！"<<endl;
                cudaError = cudaErrorUnknown;
            }
            else
            {
                cudaError = cudaMallocPitch((void **) (&MallocUnit.addr), &MallocUnit.pitch,MallocUnit.Lens * sizeof(char), MallocUnit.Col);    
            }
        }
        if(cudaError == cudaSuccess)
        {
            MallocUnit.MallocSign = true;
            cout<<"UnitMalloc Success!"<<endl;
        }
        else
        {
            cout<<"UnitMalloc Failed!"<<endl;
        }
        return cudaError;
    }

    cudaError_t UnitFree(_MallocUnit & MallocUnit)
    {
        cudaError_t cudaError = cudaErrorUnknown;
        if(MallocUnit.MallocSign == false)
        {
            cout<<"MallocUnit.MallocSign is false!"<<endl;
            return cudaError;
        }
        MallocUnit.MallocSign = false;
        if((MallocUnit.MallocType == Host1D)||(MallocUnit.MallocType ==Host2D))
        {
            cudaError = cudaFreeHost(MallocUnit.addr);
        }
        else if ((MallocUnit.MallocType==Gpu1D)||(MallocUnit.MallocType==Gpu2D))
        {
            cudaError = cudaFree(MallocUnit.addr);
        }
        if(cudaError == cudaSuccess)
        {
            cout<<"UnitFree Success!"<<endl;
        }
        else
        {
            cout<<"UnitFree Failed!"<<endl;
        }
        return cudaError;
    }

    void AddrListFree()
    {
        cudaError_t cudaError;
        for(int i = Lens-1; i >= 0;i--)
        {
            cudaError = UnitFree(MallocUnitList[i]);
        }
    }

#define COUT(x) cout<<#x" = "<< x <<endl;

    void showUnit(size_t Ind) const
    {
        COUT(Ind);
        COUT(MallocUnitList[Ind].addr);
        COUT(MallocUnitList[Ind].Lens);
        COUT(MallocUnitList[Ind].dType);
        COUT(MallocUnitList[Ind].MallocType);
        COUT(MallocUnitList[Ind].MallocSign);
        COUT(MallocUnitList[Ind].pitch);
        COUT(MallocUnitList[Ind].Col);
    }

    void showMallocUnitList() const
    {
        for(int i = 0; i < Lens;i++)
        {
            showUnit(i);
        }
    }
};

#endif