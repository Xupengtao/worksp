/*
 * @Author: xpt 
 * @Date: 2019-12-26 14:41:58 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-26 19:20:56
 */

#include "../Tools/PdwStruct/PdwStruct.hpp"
#include "../Tools/imageTools/BMPImage.h"
#include "../Tools/fileStream/binaryFileStream.hpp"
#include "../Tools/Timeit.h"

typedef _Pdw<UINT, UINT, UINT, UINT, UINT>      PdwType;
typedef _PdwArrayDesc<PdwType>                  PdwArrayDescType;
typedef _PdwMap2D<PdwType, BGR>                 PdwMap2DTy;

template<typename _RfTy, typename _PwTy, typename _PaTy, typename _ToaTy, typename _DoaTy>
_PdwArrayDesc<_Pdw<_RfTy, _PwTy, _PaTy, _ToaTy, _DoaTy>> _Pdw<_RfTy, _PwTy, _PaTy, _ToaTy, _DoaTy>::PdwArrayDesc;

int main(int argc, char **argv)
{
    _binaryFile<_fileMsg, PdwType> binaryFile("/home/admin/WorkSp/Data/文件32.data", 20, 26000000);
    _Timeit Timeit(3);
    Timeit.Start(0, "Read Times elapsed(us): ");
    PdwType::PdwArrayDescClear();
    binaryFile.read(0x55AA, 0xAA55);
    Timeit.End(0);
    PdwType::PdwArrayDesc.print();
    binaryFile.showStatus();
    binaryFile.close();
    int PdwSize         = binaryFile.GetDataValidLines();
    PdwType * PdwAddr   = binaryFile.GetDataAddr();
    UINT row_count = 1024;                            //数组行数
    UINT col_count = 4096;                            //数组列数
    PdwMap2DTy  PdwMap2D;
    UINT PaMin      = PdwType::PdwArrayDesc.PaMin;
    UINT PaRange    = PdwType::PdwArrayDesc.PaMax - PdwType::PdwArrayDesc.PaMin;
    UINT RfMin      = PdwType::PdwArrayDesc.RfMin;
    UINT RfRange    = PdwType::PdwArrayDesc.RfMax - PdwType::PdwArrayDesc.RfMin;
    UINT PwMin      = PdwType::PdwArrayDesc.PwMin;
    UINT PwRange    = PdwType::PdwArrayDesc.PwMax - PdwType::PdwArrayDesc.PwMin;
    float PaNormalUnit   = float(PaRange/256.0);
    float RfNormalUnit   = float(RfRange/256.0);
    float PwNormalUnit   = float(PwRange/256.0);
    PdwMap2D.SetPaPara(PaMin,PaNormalUnit);
    PdwMap2D.SetRfPara(RfMin,RfNormalUnit);
    PdwMap2D.SetPwPara(PwMin,PwNormalUnit);
    UINT ToaNormalUnit;
    if(PdwType::PdwArrayDesc.ToaReverseTimes == 0)
    {
        ToaNormalUnit = (PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/col_count;
    }
    else
    {
        if(PdwAddr[0].Toa > PdwAddr[PdwSize - 1].Toa)
        {
            ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/col_count) + 
                            (PdwType::PdwArrayDesc.ToaReverseTimes - 1) * (0x100000000/col_count);
        }
        else
        {
            ToaNormalUnit = ((PdwAddr[PdwSize - 1].Toa - PdwAddr[0].Toa)/col_count) + 
                            PdwType::PdwArrayDesc.ToaReverseTimes * (0x100000000/col_count);
        }
    }
    PdwMap2D.SetToaPara(PdwAddr[0].Toa, ToaNormalUnit);
    BMPImage image(col_count, row_count, 24);           // col_count是图像的宽，row_count是图像的高，创建大小和数组维数相同，纯黑24位BMP图像
    binaryFile.registShowDataFunc(PrintPdw<PdwType>);
    Timeit.Start(1, "elapsed(us): ");
    for(int i = 0; i < PdwSize; i++)
    {
        if(PdwAddr[i].ToaS != -1)
        {
            PdwAddr[i].map2D(PdwMap2D, 1.0);
            if((PdwMap2D.x < row_count) && (PdwMap2D.y < col_count))
            {
                image.at<BGR>(PdwMap2D.x, PdwMap2D.y) = PdwMap2D.Map2DUnit;
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
            COUT(i, PdwMap2D.x, PdwMap2D.y, PdwAddr[i-1].Toa, PdwAddr[i].Toa, PdwAddr[i+1].Toa);
            COUT(PdwAddr[i-1].ToaS);
        }
    }
    Timeit.End(1);
    Timeit.Show();
    image.save("./save2DArray.bmp");
    return 1;
}