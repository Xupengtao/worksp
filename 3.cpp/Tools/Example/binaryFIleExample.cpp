/*
 * @Author: xpt 
 * @Date: 2019-12-24 15:01:48 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-24 17:17:38
 */

#include "../fileStream/binaryFileStream.hpp"
#include "../PdwStruct/PdwStruct.hpp"
#include "../Timeit.h"

template<typename _RfTy, typename _PwTy, typename _PaTy, typename _ToaTy, typename _DoaTy>
_PdwArrayDesc<_Pdw<_RfTy, _PwTy, _PaTy, _ToaTy, _DoaTy>> _Pdw<_RfTy, _PwTy, _PaTy, _ToaTy, _DoaTy>::PdwArrayDesc;

typedef _Pdw<UINT, UINT, UINT, UINT, UINT> PdwType;

int main(int argc, char **argv)
{
    _binaryFile<_fileMsg, PdwType> binaryFile("/home/admin/WorkSp/Data/文件32.data", 20, 1000000);
    binaryFile.showFileHeader();
    binaryFile.showStatus();
    _Timeit Timeit(3);
    Timeit.Start(0, "Read Times elapsed(us): ");
    binaryFile.read();
    Timeit.End(0);
    binaryFile.registShowDataFunc(PrintPdw<PdwType>);
    binaryFile.showData(0,10000);
    binaryFile.showStatus();
    binaryFile.close();
    Timeit.Show();
    return 1;
}