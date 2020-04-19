/*
 * @Author: admin 
 * @Date: 2019-12-26 18:33:23 
 * @Last Modified by: admin
 * @Last Modified time: 2019-12-26 19:56:10
 */

#include "../PdwStruct/PdwToBmp.hpp"

typedef _Pdw<UINT, UINT, UINT, UINT, UINT>      PdwType;
const float Pi = 3.1415926;

int main(int argc, char **argv)
{
    std::string filestr("/home/admin/WorkSp/Analysis/文件32.bmp");
    CCHAR* filepath = stringtochar(filestr);
   //  _PdwToBmp<_fileMsg, PdwType, BGRA> PdwToBmp1("/home/admin/WorkSp/Data/文件32.data", 20, 1000000, 1024, 4096);
   //  PdwToBmp1.ImageResize(1024, 1024)
   //           .ReadDataResize(26000000)
   //           .ReadData(0x55AA, 0xAA55)
   //           .SetImageBackup(White)
   //           .SetPdwArrayDesc(0, 0, 0, 0, 0, 255000)
   //           .GenImage()
   //           .GenBGRAImage(1, 0)
   //           .SaveBGRAPolarImage("/home/admin/WorkSp/Analysis/Polar.bmp", 1, 0, Pi/512, 100, 1000)
   //           .SaveDensityImage("/home/admin/WorkSp/Analysis/Density.bmp")
   //           .SaveImage(filepath)
   //           .SaveVideo("/home/admin/WorkSp/Analysis/文件32.avi", 0x55AA, 0xAA55, 100, 10000, 1000000, 25000000);
    _PdwToBmp<_xyg_z, PdwType, BGRA> PdwToBmp2("/home/admin/WorkSp/Data/z", 0, 6553600, 1024, 1024);
    PdwToBmp2.ReadData(100)
             .showFileStatus()
          //    .showData(0,100)
             .ReadData(101)
             .showFileStatus()
          //    .showData(0,100)
             .FilterData(1, 10000, 40000)
          //    .FilterData(4, 450, 515)
             .SetImageBackup(White)
             .GenImage()
             .GenBGRAImage(1, 0)
             .GenBGRAImageAndPdwArray(10, 0)
             .showPdwArray()
             .SaveBGRAPolarImage("/home/admin/WorkSp/Analysis/Polar_z_1.bmp", 1, 0, Pi/512, 100, 1000)
             .SaveDensityImage("/home/admin/WorkSp/Analysis/Density_z.bmp")
             .SaveImage("/home/admin/WorkSp/Analysis/z.bmp");
          //    .ReadData(0xAA55, 0x55AA)
          //    .SaveBGRAPolarImage("/home/admin/WorkSp/Analysis/Polar_z_2.bmp", 1, 0, Pi/512, 100, 1000)
          //    .SavePdwData("/home/admin/WorkSp/Analysis/new.data")
          //    .SaveVideo("/home/admin/WorkSp/Analysis/z.avi", 0xAA55, 0x55AA, 100);
    return 1;
}