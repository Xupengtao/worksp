/*
 * @Author: xpt 
 * @Date: 2020-08-13 10:47:27 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-08-13 10:54:41
 */

#include"../fileStream/PdwFile.hpp"
#include"../PdwStruct/PdwStruct.hpp"

typedef _Pdw<UINT, UINT, UINT, UINT, UINT>      PdwType;

std::string filepath("/home/admin/WorkSp/Data/");

int main(int argc, char **argv)
{
    _PdwFile<_xyg_z, PdwType> PdwFile;
    PdwFile.open(filepath + "Task20180811105114_ch3_20180811105114");
    PdwFile.read();
    PdwFile.registShowDataFunc(PrintPdw<PdwType>);
    PdwFile.showData(0, 100);
    PdwFile.close();
}