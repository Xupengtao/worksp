/*
 * @Author: xpt 
 * @Date: 2019-12-17 18:55:39 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-17 18:56:14
 */

#include"csvFileStream.hpp"

struct _PdwType
{
    float No;
    float Rf;
    float Doa;
    float Pa;
    float Pw;
    float Toa;
    float Pri;
};

void showPdw(_PdwType& Pdw)
{
    COUT(Pdw.No, Pdw.Rf, Pdw.Doa, Pdw.Pa, Pdw.Pw, Pdw.Toa, Pdw.Pri);
}
void showPdwArray(vector<_PdwType>& CsvData)
{
    for(int i = 0; i < CsvData.size(); i++)
    {
        COUTSWIDTH(8, "No",     CsvData[i].No,
                      "Rf",     CsvData[i].Rf,
                      "Doa",    CsvData[i].Doa,
                      "Pa",     CsvData[i].Pa,
                      "Pw",     CsvData[i].Pw,
                      "Toa",    CsvData[i].Toa,
                      "Pri",    CsvData[i].Pri);
    }
}

int main(void)
{
    // CsvStream 例，可读取不规整数据
    _CsvStream<float> csvStream("/home/xpt/WorkSp/Data/pdw1_0.csv");
    csvStream.read();
    csvStream.show();
    // CsvFile 例，可读取规整数据，和CsvStream比速度较快一些
    _CsvFile<_PdwType> csv("/home/xpt/WorkSp/Data/pdw1_0.csv");
    csv.read();
    csv.registShowCsvDataFunc(showPdwArray); //注册打印 typedef void (*_pFunc_ShowCsvData)(vector<Ty>&)
    csv.showCsvData();
    cin.get();
    return 0;
}