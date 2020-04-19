/*
 * @Author: xpt 
 * @Date: 2019-12-26 14:43:10 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-26 19:01:52
 */

#ifndef _PDWTOBMP_HPP
#define _PDWTOBMP_HPP

#include "../PdwStruct/PdwStruct.hpp"
#include "../imageTools/BMPImage.h"
#include "../fileStream/binaryFileStream.hpp"
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
#include "../Timeit.h"

template<typename _RfTy, typename _PwTy, typename _PaTy, typename _ToaTy, typename _DoaTy>
_PdwArrayDesc<_Pdw<_RfTy, _PwTy, _PaTy, _ToaTy, _DoaTy>> _Pdw<_RfTy, _PwTy, _PaTy, _ToaTy, _DoaTy>::PdwArrayDesc;

enum _ImageBackup
{
    Black = 0, White = 1
};

template <typename _FileMsgTy, typename _PdwType, typename _BmpType>
class _PdwToBmp
{
private:
    enum _ImageChannel{Rch = 0, Gch = 1, Bch = 2, Ach = 3};
	typedef _PdwToBmp<_FileMsgTy, _PdwType, _BmpType>	_ThisType;
    typedef _binaryFile<_FileMsgTy, _PdwType>           binaryFileType;
    typedef _PdwMap2D<_PdwType, _BmpType>               PdwMap2DType;
    typedef _PdwArrayDesc<_PdwType>                     PdwArrayDescType;

    PdwMap2DType    PdwMap2D;
    binaryFileType  binaryFile;
    BMPImage        image;                                              // col_count是图像的宽，row_count是图像的高
    vector<vector<_PdwType> >   PdwArray;
    vector<vector<UINT> >       PdwArrayIndex;
    _Timeit         Timeit;
    UINT            PdwSize;
    _PdwType*       PdwAddr;
    UINT            RowCount;                                           // 数组行数
    UINT            ColCount;                                           // 数组列数
    _ImageBackup    ImageBackup;

public:
    _PdwToBmp(const char* filepath, UINT fileHeaderSize_, UINT ReadDataSize_, UINT RowCount_, UINT ColCount_)
            :binaryFile(filepath, fileHeaderSize_, ReadDataSize_),
             image(ColCount_, RowCount_, sizeof(_BmpType)*8),Timeit(10),
             RowCount(RowCount_), ColCount(ColCount_), ImageBackup(Black)
    {
        binaryFile.registShowDataFunc(PrintPdw<_PdwType>);
        PdwAddr = binaryFile.GetDataAddr();
        PdwArray.resize(ColCount);
        PdwArrayIndex.resize(RowCount);
        for(int i = 0; i < RowCount; i++)
        {
            PdwArray[i].resize(0);
            PdwArrayIndex[i].resize(ColCount);
            for(int j = 0; j < ColCount; j++)
            {
                PdwArrayIndex[i][j] = 0;
            }
        }
    }
    virtual ~_PdwToBmp()
    {
        Timeit.Show();
        binaryFile.close();
    }
    _ThisType& ImageResize(UINT RowCount_, UINT ColCount_)
    {
        RowCount = RowCount_;
        ColCount = ColCount_;
        image.resize(ColCount, RowCount);
        PdwArray.resize(ColCount);
        PdwArrayIndex.resize(RowCount);
        for(int i = 0; i < RowCount; i++)
        {
            PdwArray[i].resize(0);
            PdwArrayIndex[i].resize(ColCount);
            for(int j = 0; j < ColCount; j++)
            {
                PdwArrayIndex[i][j] = 0;
            }
        }
        return *this;
    }
    _ThisType& ReadDataResize(UINT ReadDataSize_)
    {
        binaryFile.ReadDataResize(ReadDataSize_);
        PdwAddr = binaryFile.GetDataAddr();
        return *this;
    }
    _ThisType& SetImageBackup(_ImageBackup ImageBackup_)
    {
        ImageBackup = ImageBackup_;
        return *this;
    }
    _ThisType& ReadData(int seekst = -1)
    {
        Timeit.Start(0, "读取文件，共计(us): ");
        _PdwType::PdwArrayDescClear();                          //读取之前清零
        binaryFile.read(seekst);
        PdwSize = binaryFile.GetDataValidLines();
        showPdwArrayDesc();
        Timeit.End(0);
        return *this;
    }
    _ThisType& FilterData(int Type, int FilterintervalL, int FilterintervalR)
    {
        switch(Type)
        {
        case 0:                     // PA
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Pa < FilterintervalL) || (PdwAddr[i].Pa > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        case 1:                     // RF
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Rf < FilterintervalL) || (PdwAddr[i].Rf > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        case 2:                     // PW
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Pw < FilterintervalL) || (PdwAddr[i].Pw > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        case 3:                     // TOA
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Toa < FilterintervalL) || (PdwAddr[i].Toa > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        case 4:                     // DOA
            for(int i = 0; i < PdwSize; i++)
            {
                if((PdwAddr[i].Doa < FilterintervalL) || (PdwAddr[i].Doa > FilterintervalR))
                {
                    PdwAddr[i].ToaS = -1;
                }
            }
            break;
        default:
            break;
        }
        return *this;
    }
    _ThisType& SetPdwArrayDesc(UINT PaMin = 0, UINT PaMax = 0,
                               UINT RfMin = 0, UINT RfMax = 0,
                               UINT PwMin = 0, UINT PwMax = 0)
    {
        _PdwType::PdwArrayDesc.PaMin = (PaMin == 0) ? _PdwType::PdwArrayDesc.PaMin : PaMin;
        _PdwType::PdwArrayDesc.PaMax = (PaMax == 0) ? _PdwType::PdwArrayDesc.PaMax : PaMax;
        _PdwType::PdwArrayDesc.RfMin = (RfMin == 0) ? _PdwType::PdwArrayDesc.RfMin : RfMin;
        _PdwType::PdwArrayDesc.RfMax = (RfMax == 0) ? _PdwType::PdwArrayDesc.RfMax : RfMax;
        _PdwType::PdwArrayDesc.PwMin = (PwMin == 0) ? _PdwType::PdwArrayDesc.PwMin : PwMin;
        _PdwType::PdwArrayDesc.PwMax = (PwMax == 0) ? _PdwType::PdwArrayDesc.PwMax : PwMax;
        return *this;
    }
    _ThisType& SavePdwData(const char* save_path)
    {
        Timeit.Start(1, "保存文件，共计(us): ");
        binaryFile.save(save_path);
        Timeit.End(1);
        return *this;
    }
    _ThisType& ImageClear(BMPImage &BGRABmp)
    {
        switch(ImageBackup)
        {
        case Black:
            BGRABmp.clear(0);
            break;
        case White:
            BGRABmp.clear(0xFF);
            break;
        default:
            break;
        }
        if((sizeof(_BmpType) == 4) && (ImageBackup != Black))
        {
            int width   = BGRABmp.width();
            int height  = BGRABmp.height();
            for(int Row = 0; Row < height; Row++)
            {
                for(int Col = 0; Col < width; Col++)
                {
                    (BGRABmp.at<_BmpType>(Row, Col)).a = 0;
                }
            }
        }
        return *this;
    }
    _ThisType& PdwArrayClear()
    {
        PdwArray.resize(ColCount);
        PdwArrayIndex.resize(RowCount);
        for(int i = 0; i < RowCount; i++)
        {
            PdwArray[i].resize(0);
            PdwArrayIndex[i].resize(ColCount);
            for(int j = 0; j < ColCount; j++)
            {
                PdwArrayIndex[i][j] = 0;
            }
        }
        return *this;
    }
    _ThisType& GenBGRAImageAndPdwArray(UINT Threshold, UINT PixelMin = 0,
                                       const float DoaNormalUnit = 1.0,
                                       UINT PaMin = 0, float PaNormalUnit = 0,
                                       UINT RfMin = 0, float RfNormalUnit = 0,
                                       UINT PwMin = 0, float PwNormalUnit = 0)
    {
        ImageClear(image);
        PdwArrayClear();
        PdwSize = binaryFile.GetDataValidLines();
        UINT PaRange = _PdwType::PdwArrayDesc.PaMax - _PdwType::PdwArrayDesc.PaMin;
        UINT RfRange = _PdwType::PdwArrayDesc.RfMax - _PdwType::PdwArrayDesc.RfMin;
        UINT PwRange = _PdwType::PdwArrayDesc.PwMax - _PdwType::PdwArrayDesc.PwMin;
        if(PaNormalUnit == 0)
        {
            PaMin        = _PdwType::PdwArrayDesc.PaMin;
            PaNormalUnit = float(PaRange/256.0);
        }
        if(RfNormalUnit == 0)
        {
            RfMin        = _PdwType::PdwArrayDesc.RfMin;
            RfNormalUnit = float(RfRange/256.0);
        }
        if(PwNormalUnit == 0)
        {
            PwMin        = _PdwType::PdwArrayDesc.PwMin;
            PwNormalUnit = float(PwRange/256.0);
        }
        UINT ToaNormalUnit;
        if(_PdwType::PdwArrayDesc.ToaReverseTimes == 0)
        {
            ToaNormalUnit = (PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(ColCount-1);
        }
        else
        {
            if(PdwAddr[0].Toa > PdwAddr[PdwSize - 1].Toa)
            {
                ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(ColCount-1)) + 
                                (_PdwType::PdwArrayDesc.ToaReverseTimes - 1) * (0x100000000/(ColCount-1));
            }
            else
            {
                ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(ColCount-1)) + 
                                 _PdwType::PdwArrayDesc.ToaReverseTimes * (0x100000000/(ColCount-1));
            }
        }
        PdwMap2D.SetPaPara(PaMin, PaNormalUnit);
        PdwMap2D.SetRfPara(RfMin, RfNormalUnit);
        PdwMap2D.SetPwPara(PwMin, PwNormalUnit);
        PdwMap2D.SetToaPara(PdwAddr[0].Toa, ToaNormalUnit);
        Timeit.Start(7, "生成图像GenBGRAImageAndPdwArray，共计(us): ");
        _BmpType BmpPixelTmp;
        for(int i = 0; i < PdwSize; i++)
        {
            if(PdwAddr[i].ToaS != -1)
            {
                PdwAddr[i].map2D(PdwMap2D, DoaNormalUnit);
                if((PdwMap2D.x < RowCount) && (PdwMap2D.y < ColCount))
                {
                    BmpPixelTmp = image.at<_BmpType>(PdwMap2D.x, PdwMap2D.y);
                    BmpPixelTmp.a = (BmpPixelTmp.a < 0xFF) ? BmpPixelTmp.a+1 : BmpPixelTmp.a;
                    PdwArray[PdwMap2D.y].push_back(PdwAddr[i]);
                    PdwArrayIndex[PdwMap2D.x][PdwMap2D.y]++;
                    if(BmpPixelTmp.a > Threshold)
                    {
                        PdwMap2D.Map2DUnit.r = (PdwMap2D.Map2DUnit.r > PixelMin) ? PdwMap2D.Map2DUnit.r : PixelMin;
                        PdwMap2D.Map2DUnit.b = (PdwMap2D.Map2DUnit.b > PixelMin) ? PdwMap2D.Map2DUnit.b : PixelMin;
                        PdwMap2D.Map2DUnit.g = (PdwMap2D.Map2DUnit.g > PixelMin) ? PdwMap2D.Map2DUnit.g : PixelMin;
                        PdwMap2D.Map2DUnit.a = BmpPixelTmp.a;
                        image.at<_BmpType>(PdwMap2D.x, PdwMap2D.y) = PdwMap2D.Map2DUnit;
                    }
                    else
                    {
                        image.at<_BmpType>(PdwMap2D.x, PdwMap2D.y) = BmpPixelTmp;
                    }
                }
                else
                {
                    ERRORMSG("Index out of range! -- More");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
            else
            {
                // COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                // COUT(PdwAddr[i-1].ToaS);
            }
        }
        Timeit.End(7);
        return *this;
    }
    _ThisType& GenImage(const float DoaNormalUnit = 1.0,
                        UINT PaMin = 0, float PaNormalUnit = 0,
                        UINT RfMin = 0, float RfNormalUnit = 0,
                        UINT PwMin = 0, float PwNormalUnit = 0)
    {
        ImageClear(image);
        PdwSize = binaryFile.GetDataValidLines();
        UINT PaRange = _PdwType::PdwArrayDesc.PaMax - _PdwType::PdwArrayDesc.PaMin;
        UINT RfRange = _PdwType::PdwArrayDesc.RfMax - _PdwType::PdwArrayDesc.RfMin;
        UINT PwRange = _PdwType::PdwArrayDesc.PwMax - _PdwType::PdwArrayDesc.PwMin;
        if(PaNormalUnit == 0)
        {
            PaMin        = _PdwType::PdwArrayDesc.PaMin;
            PaNormalUnit = float(PaRange/256.0);
        }
        if(RfNormalUnit == 0)
        {
            RfMin        = _PdwType::PdwArrayDesc.RfMin;
            RfNormalUnit = float(RfRange/256.0);
        }
        if(PwNormalUnit == 0)
        {
            PwMin        = _PdwType::PdwArrayDesc.PwMin;
            PwNormalUnit = float(PwRange/256.0);
        }
        UINT ToaNormalUnit;
        if(_PdwType::PdwArrayDesc.ToaReverseTimes == 0)
        {
            ToaNormalUnit = (PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(ColCount-1);
        }
        else
        {
            if(PdwAddr[0].Toa > PdwAddr[PdwSize - 1].Toa)
            {
                ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(ColCount-1)) + 
                                (_PdwType::PdwArrayDesc.ToaReverseTimes - 1) * (0x100000000/(ColCount-1));
            }
            else
            {
                ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(ColCount-1)) + 
                                 _PdwType::PdwArrayDesc.ToaReverseTimes * (0x100000000/(ColCount-1));
            }
        }
        PdwMap2D.SetPaPara(PaMin,PaNormalUnit);
        PdwMap2D.SetRfPara(RfMin,RfNormalUnit);
        PdwMap2D.SetPwPara(PwMin,PwNormalUnit);
        PdwMap2D.SetToaPara(PdwAddr[0].Toa, ToaNormalUnit);
        Timeit.Start(2, "生成图像GenImage，共计(us): ");
        for(int i = 0; i < PdwSize; i++)
        {
            if(PdwAddr[i].ToaS != -1)
            {
                PdwAddr[i].map2D(PdwMap2D, DoaNormalUnit);
                if((PdwMap2D.x < RowCount) && (PdwMap2D.y < ColCount))
                {
                    image.at<_BmpType>(PdwMap2D.x, PdwMap2D.y) = PdwMap2D.Map2DUnit;
                }
                else
                {
                    ERRORMSG("Index out of range! -- More");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
            else
            {
                // COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                // COUT(PdwAddr[i-1].ToaS);
            }
        }
        Timeit.End(2);
        return *this;
    }
    _ThisType& GenBGRAImage(UINT Threshold, UINT PixelMin = 0,
                            const float DoaNormalUnit = 1.0,
                            UINT PaMin = 0, float PaNormalUnit = 0,
                            UINT RfMin = 0, float RfNormalUnit = 0,
                            UINT PwMin = 0, float PwNormalUnit = 0)
    {
        ImageClear(image);
        PdwSize = binaryFile.GetDataValidLines();
        UINT PaRange = _PdwType::PdwArrayDesc.PaMax - _PdwType::PdwArrayDesc.PaMin;
        UINT RfRange = _PdwType::PdwArrayDesc.RfMax - _PdwType::PdwArrayDesc.RfMin;
        UINT PwRange = _PdwType::PdwArrayDesc.PwMax - _PdwType::PdwArrayDesc.PwMin;
        if(PaNormalUnit == 0)
        {
            PaMin        = _PdwType::PdwArrayDesc.PaMin;
            PaNormalUnit = float(PaRange/256.0);
        }
        if(RfNormalUnit == 0)
        {
            RfMin        = _PdwType::PdwArrayDesc.RfMin;
            RfNormalUnit = float(RfRange/256.0);
        }
        if(PwNormalUnit == 0)
        {
            PwMin        = _PdwType::PdwArrayDesc.PwMin;
            PwNormalUnit = float(PwRange/256.0);
        }
        UINT ToaNormalUnit;
        if(_PdwType::PdwArrayDesc.ToaReverseTimes == 0)
        {
            ToaNormalUnit = (PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(ColCount-1);
        }
        else
        {
            if(PdwAddr[0].Toa > PdwAddr[PdwSize - 1].Toa)
            {
                ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(ColCount-1)) + 
                                (_PdwType::PdwArrayDesc.ToaReverseTimes - 1) * (0x100000000/(ColCount-1));
            }
            else
            {
                ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(ColCount-1)) + 
                                 _PdwType::PdwArrayDesc.ToaReverseTimes * (0x100000000/(ColCount-1));
            }
        }
        PdwMap2D.SetPaPara(PaMin, PaNormalUnit);
        PdwMap2D.SetRfPara(RfMin, RfNormalUnit);
        PdwMap2D.SetPwPara(PwMin, PwNormalUnit);
        PdwMap2D.SetToaPara(PdwAddr[0].Toa, ToaNormalUnit);
        Timeit.Start(3, "生成图像GenBGRAImage，共计(us): ");
        _BmpType BmpPixelTmp;
        for(int i = 0; i < PdwSize; i++)
        {
            if(PdwAddr[i].ToaS != -1)
            {
                PdwAddr[i].map2D(PdwMap2D, DoaNormalUnit);
                if((PdwMap2D.x < RowCount) && (PdwMap2D.y < ColCount))
                {
                    BmpPixelTmp = image.at<_BmpType>(PdwMap2D.x, PdwMap2D.y);
                    BmpPixelTmp.a = (BmpPixelTmp.a < 0xFF) ? BmpPixelTmp.a+1 : BmpPixelTmp.a;
                    if(BmpPixelTmp.a > Threshold)
                    {
                        PdwMap2D.Map2DUnit.r = (PdwMap2D.Map2DUnit.r > PixelMin) ? PdwMap2D.Map2DUnit.r : PixelMin;
                        PdwMap2D.Map2DUnit.b = (PdwMap2D.Map2DUnit.b > PixelMin) ? PdwMap2D.Map2DUnit.b : PixelMin;
                        PdwMap2D.Map2DUnit.g = (PdwMap2D.Map2DUnit.g > PixelMin) ? PdwMap2D.Map2DUnit.g : PixelMin;
                        PdwMap2D.Map2DUnit.a = BmpPixelTmp.a;
                        image.at<_BmpType>(PdwMap2D.x, PdwMap2D.y) = PdwMap2D.Map2DUnit;
                    }
                    else
                    {
                        image.at<_BmpType>(PdwMap2D.x, PdwMap2D.y) = BmpPixelTmp;
                    }
                }
                else
                {
                    ERRORMSG("Index out of range! -- More");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
            else
            {
                // COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                // COUT(PdwAddr[i-1].ToaS);
            }
        }
        Timeit.End(3);
        return *this;
    }
    _ThisType& SaveBGRAPolarImage(const char* savePath, UINT Threshold, 
                                  UINT PixelMin = 0,
                                  const float DoaUnit = 1.0, 
                                  UINT R0 = 100, UINT R1 = 2000,
                                  UINT PaMin = 0, float PaNormalUnit = 0,
                                  UINT RfMin = 0, float RfNormalUnit = 0,
                                  UINT PwMin = 0, float PwNormalUnit = 0)
    {
        BMPImage PolarImage(2*R1, 2*R1, 32);
        ImageClear(PolarImage);
        PdwSize = binaryFile.GetDataValidLines();
        UINT PaRange = _PdwType::PdwArrayDesc.PaMax - _PdwType::PdwArrayDesc.PaMin;
        UINT RfRange = _PdwType::PdwArrayDesc.RfMax - _PdwType::PdwArrayDesc.RfMin;
        UINT PwRange = _PdwType::PdwArrayDesc.PwMax - _PdwType::PdwArrayDesc.PwMin;
        if(PaNormalUnit == 0)
        {
            PaMin        = _PdwType::PdwArrayDesc.PaMin;
            PaNormalUnit = float(PaRange/256.0);
        }
        if(RfNormalUnit == 0)
        {
            RfMin        = _PdwType::PdwArrayDesc.RfMin;
            RfNormalUnit = float(RfRange/256.0);
        }
        if(PwNormalUnit == 0)
        {
            PwMin        = _PdwType::PdwArrayDesc.PwMin;
            PwNormalUnit = float(PwRange/256.0);
        }
        UINT ToaNormalUnit;
        if(_PdwType::PdwArrayDesc.ToaReverseTimes == 0)
        {
            ToaNormalUnit = (PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(R1 - R0 - 1);
        }
        else
        {
            if(PdwAddr[0].Toa > PdwAddr[PdwSize - 1].Toa)
            {
                ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(R1 - R0 - 1)) + 
                                (_PdwType::PdwArrayDesc.ToaReverseTimes - 1) * (0x100000000/(R1 - R0 - 1));
            }
            else
            {
                ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/(R1 - R0 - 1)) + 
                                 _PdwType::PdwArrayDesc.ToaReverseTimes * (0x100000000/(R1 - R0 - 1));
            }
        }
        PdwMap2D.SetPaPara(PaMin, PaNormalUnit);
        PdwMap2D.SetRfPara(RfMin, RfNormalUnit);
        PdwMap2D.SetPwPara(PwMin, PwNormalUnit);
        PdwMap2D.SetToaPara(PdwAddr[0].Toa, ToaNormalUnit);
        Timeit.Start(4, "生成图像SaveBGRAPolarImage，共计(us): ");
        _BmpType BmpPixelTmp;
        for(int i = 0; i < PdwSize; i++)
        {
            if(PdwAddr[i].ToaS != -1)
            {
                PdwAddr[i].mapPolar2D(PdwMap2D, DoaUnit, R0, R1);
                if((PdwMap2D.x < 2*R1) && (PdwMap2D.y < 2*R1))
                {
                    BmpPixelTmp = PolarImage.at<_BmpType>(PdwMap2D.x, PdwMap2D.y);
                    BmpPixelTmp.a = (BmpPixelTmp.a < 0xFF) ? BmpPixelTmp.a+1 : BmpPixelTmp.a;
                    if(BmpPixelTmp.a > Threshold)
                    {
                        PdwMap2D.Map2DUnit.r = (PdwMap2D.Map2DUnit.r > PixelMin) ? PdwMap2D.Map2DUnit.r : PixelMin;
                        PdwMap2D.Map2DUnit.b = (PdwMap2D.Map2DUnit.b > PixelMin) ? PdwMap2D.Map2DUnit.b : PixelMin;
                        PdwMap2D.Map2DUnit.g = (PdwMap2D.Map2DUnit.g > PixelMin) ? PdwMap2D.Map2DUnit.g : PixelMin;
                        PdwMap2D.Map2DUnit.a = BmpPixelTmp.a;
                        PolarImage.at<_BmpType>(PdwMap2D.x, PdwMap2D.y) = PdwMap2D.Map2DUnit;
                    }
                    else
                    {
                        PolarImage.at<_BmpType>(PdwMap2D.x, PdwMap2D.y) = BmpPixelTmp;
                    }
                }
                else
                {
                    ERRORMSG("Index out of range! -- More");
                    COUTSTR("       ");
                    COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                    break;
                }
            }
            else
            {
                // COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
                // COUT(PdwAddr[i-1].ToaS);
            }
        }
        Timeit.End(4);
        PolarImage.save(savePath);
        string savePathstr = chartostring(savePath);
        UINT pos=savePathstr.find_last_of('/');
        std::string filepath(savePathstr.substr(0, pos+1));
        std::string filename(savePathstr.substr(pos+1));
        std::string DensitysavePath = filepath + "Density_" + filename;
        std::string PasavePath = filepath + "Pa_" + filename;
        std::string RfsavePath = filepath + "Rf_" + filename;
        std::string PwsavePath = filepath + "Pw_" + filename;
        CCHAR* DensityImagesavePath = stringtochar(DensitysavePath);
        CCHAR* PaImagesavePath = stringtochar(PasavePath);
        CCHAR* RfImagesavePath = stringtochar(RfsavePath);
        CCHAR* PwImagesavePath = stringtochar(PwsavePath);
        SaveDensityImage(DensityImagesavePath, PolarImage);
        SaveChannelImage(PaImagesavePath, PolarImage, Rch);
        SaveChannelImage(PwImagesavePath, PolarImage, Gch);
        SaveChannelImage(RfImagesavePath, PolarImage, Bch);
        return *this;
    }
    _ThisType& SaveChannelImage(const char* savePath, BMPImage &BGRABmp, _ImageChannel ImageChannel)
    {
        int width   = BGRABmp.width();
        int height  = BGRABmp.height();
        BMPImage ChannelImage(width, height, 8);
        _BmpType BmpPixelTmp;
        if(sizeof(_BmpType) != 4)
        {
            ERRORMSG("");
        }
        UCHAR *pBmpPixelChannelTmp = NULL;
        switch(ImageChannel)
        {
        case Rch:
            pBmpPixelChannelTmp = &BmpPixelTmp.r;
            break;
        case Gch:
            pBmpPixelChannelTmp = &BmpPixelTmp.g;
            break;
        case Bch:
            pBmpPixelChannelTmp = &BmpPixelTmp.b;
            break;
        case Ach:
            pBmpPixelChannelTmp = &BmpPixelTmp.a;
            break;
        default:
            break;
        }
        Timeit.Start(5, "生成图像SaveChannelImage，共计(us): ");
        for(int Row = 0; Row < height; Row++)
        {
            for(int Col = 0; Col < width; Col++)
            {
                BmpPixelTmp = BGRABmp.at<_BmpType>(Row, Col);
                ChannelImage.at<UCHAR>(Row, Col) = *pBmpPixelChannelTmp;
            }
        }
        Timeit.End(5);
        ChannelImage.save(savePath);
        return *this;
    }
    _ThisType& SaveDensityImage(const char* savePath)
    {
        SaveDensityImage(savePath, image);
        return *this;
    }
    _ThisType& SaveDensityImage(const char* savePath, BMPImage &BGRABmp)
    {
        int width   = BGRABmp.width();
        int height  = BGRABmp.height();
        BMPImage DensityImage(width, height, 8);
        _BmpType BmpPixelTmp;
        if(sizeof(_BmpType) != 4)
        {
            ERRORMSG("");
        }
        Timeit.Start(6, "生成图像SaveDensityImage，共计(us): ");
        for(int Row = 0; Row < height; Row++)
        {
            for(int Col = 0; Col < width; Col++)
            {
                BmpPixelTmp = BGRABmp.at<_BmpType>(Row, Col);
                if(ImageBackup == Black)
                {
                    DensityImage.at<UCHAR>(Row, Col) = BmpPixelTmp.a;
                }
                else if(ImageBackup == White)
                {
                    DensityImage.at<UCHAR>(Row, Col) = 255 - BmpPixelTmp.a;
                }
            }
        }
        Timeit.End(6);
        DensityImage.save(savePath);
        return *this;
    }
    _ThisType& SaveImage(const char* save_path)
    {
        image.save(save_path);
        return *this;
    }
    _ThisType& SaveVideo(string save_path, int seekst = 0,
                         int interval = 10000, UINT ReadDataSize_ = 100000, int readSize = 6000000)
    {
        ReadDataResize(ReadDataSize_);
        int SizeTemp = 0;
        Mat src;
        VideoWriter writer;
        int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
        double fps = 10.0;
        while(1)
        {
            ReadData(seekst + SizeTemp);
            if(PdwSize == ReadDataSize_)
            {
                SizeTemp += interval;
            }
            else
            {
                break;
            }
            if(SizeTemp >= readSize)
            {
                break;
            }
            SaveBGRAPolarImage("/home/admin/WorkSp/Analysis/Video/videofps.bmp", 1, 0, 3.1415926/512, 100, 600);
            src = imread("/home/admin/WorkSp/Analysis/Video/videofps.bmp", IMREAD_COLOR);
            if(!src.data)
            {
                ERRORMSG("读取图片错误!");
                break;
            }
            if (!writer.isOpened())
            {
                writer.open(save_path, codec, fps, src.size(), true);
                if (!writer.isOpened())
                {
                    ERRORMSG("Could not open the output video file for write");
                    break;
                }
            }
            writer.write(src);
        }
        return *this;
    }
    _ThisType& showData(int st, int ed)
    {
        binaryFile.showData(st, ed);
        return *this;
    }
    _ThisType& showPdwArray()
    {
        for(int i = 0; i < ColCount; i++)
        {
            int PdwSize = PdwArray[i].size();
            for(int j = 0; j < PdwSize; j++)
            {
                PdwArray[i][j].print();
            }
            int PdwArrayIndexSum = 0;
            for(int j = 0; j < RowCount; j++)
            {
                PdwArrayIndexSum += PdwArrayIndex[j][i];
            }
            COUT(i, PdwSize, PdwArrayIndexSum);
        }
        return *this;
    }
    _ThisType& showPdwArrayDesc()
    {
        _PdwType::PdwArrayDesc.print();
        return *this;
    }
    _ThisType& showFileStatus()
    {
        binaryFile.showStatus();
        return *this;
    }
};

#endif // !_PDWTOBMP_HPP