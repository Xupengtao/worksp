#include "../PdwStruct/PdwStruct.hpp"
#include "../imageTools/BMPImage.h"
int main(int argc, char **argv)
{
    typedef _Pdw<int, int, int, int, int>   PdwTy;
    typedef _PdwMap2D<PdwTy, BGRA>          PdwMap2DTy;

    // PdwTy       Pdw;
    // PdwMap2DTy  PdwMap2D;
    // PdwMap2D.SetPaPara(0,10);
    // PdwMap2D.SetRfPara(0,10);
    // PdwMap2D.SetToaPara(0,10);
    // PdwMap2D.SetPwPara(0,10);
    // Pdw.Rf  = 100;
    // Pdw.Pw  = 200;
    // Pdw.Pa  = 500;
    // Pdw.Toa = 800;
    // Pdw.Doa = 300;
    // Pdw.print();
    // Pdw.map2D(PdwMap2D, 10);
    // COUT(PdwMap2D.x, PdwMap2D.y, int(PdwMap2D.Map2DUnit.b), int(PdwMap2D.Map2DUnit.g), int(PdwMap2D.Map2DUnit.r));

    typedef _MatPdwMatrix<PdwTy>            MatPdwMatrixTy;
    PdwTy Pdw;
    MatPdwMatrixTy MatPdwMatrix;
    MatPdwMatrix.init(100);
    for(UINT i = 0; i < 100; i++)
    {
        Pdw.Rf   = i;
        Pdw.Pw   = i;
        Pdw.Pa   = i;
        Pdw.Doa  = i;
        Pdw.Toa  = i;
        Pdw.ToaS = i;
        MatPdwMatrix[i].Push(Pdw);
    }
    MatPdwMatrix.Translation(90);
    for(UINT i = 0; i < 100; i++)
    {
        Pdw.Rf   = i;
        Pdw.Pw   = i;
        Pdw.Pa   = i;
        Pdw.Doa  = i;
        Pdw.Toa  = i;
        Pdw.ToaS = i;
        MatPdwMatrix[i].Push(Pdw);
    }
    for(UINT i = 0; i < 100; i++)
    {
        MatPdwMatrix[i].print(0,100);
    }
    vector<PdwTy> FilterArr;
    FilterArr.resize(0);
    MatPdwMatrix.Filter(FilterArr, 0, 100, 0, 100);
    PrintPdw(FilterArr, 0, 100);
}