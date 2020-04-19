/*
 * @Author: xpt 
 * @Date: 2019-12-17 14:23:43 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-23 00:24:25
 */

#ifndef CSV_FILE_STREAM_
#define CSV_FILE_STREAM_

#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include"../Macro.h"
using namespace std;

template<class Ty>
inline Ty strToType(const string& str)
{
    istringstream strstream(str);
    Ty num;
    strstream >> num;
    return num;
}

template<class Ty>
class _CsvStream
{
private:
    ifstream    csvfile;
    string      line;
    string      elem;
    vector<Ty>  CsvData;
    char        sep;

public:
    _CsvStream(const char* filepath, char sep_ = ','):line(""),sep(sep_)
    {
        csvfile.open(filepath, ios::in);
        if(!csvfile.is_open())
        {
            COUTSTR("read file error: ");COUTFILE;COUTLINE;COUTSTR(filepath);COUTEND;
        }
    }
    virtual ~_CsvStream()
    {
        ;
    }
    bool read()
    {
        CsvData.clear();
        while (getline(csvfile, line))
        {
            istringstream sin(line);
            while(getline(sin, elem, sep))
            {
                CsvData.push_back(strToType<Ty>(elem));
            }
        }
    }
    void show()
    {
        for(int i = 0; i < CsvData.size(); i++)
        {
            COUT(CsvData[i]);
        }
    }
};

#define CSV_FSCANF2(fp,Ty)                      fscanf(fp, "%f",                    ((float *)&Ty))
#define CSV_FSCANF3(fp,Ty,N1)                   fscanf(fp, "%f,%f",                 ((float *)&Ty),((float *)&Ty)+1)
#define CSV_FSCANF4(fp,Ty,N1,N2)                fscanf(fp, "%f,%f,%f",              ((float *)&Ty),((float *)&Ty)+1,((float *)&Ty)+2)
#define CSV_FSCANF5(fp,Ty,N1,N2,N3)             fscanf(fp, "%f,%f,%f,%f",           ((float *)&Ty),((float *)&Ty)+1,((float *)&Ty)+2,((float *)&Ty)+3)
#define CSV_FSCANF6(fp,Ty,N1,N2,N3,N4)          fscanf(fp, "%f,%f,%f,%f,%f",        ((float *)&Ty),((float *)&Ty)+1,((float *)&Ty)+2,((float *)&Ty)+3,((float *)&Ty)+4)
#define CSV_FSCANF7(fp,Ty,N1,N2,N3,N4,N5)       fscanf(fp, "%f,%f,%f,%f,%f,%f",     ((float *)&Ty),((float *)&Ty)+1,((float *)&Ty)+2,((float *)&Ty)+3,((float *)&Ty)+4,((float *)&Ty)+5)
#define CSV_FSCANF8(fp,Ty,N1,N2,N3,N4,N5,N6)    fscanf(fp, "%f,%f,%f,%f,%f,%f,%f",  ((float *)&Ty),((float *)&Ty)+1,((float *)&Ty)+2,((float *)&Ty)+3,((float *)&Ty)+4,((float *)&Ty)+5,((float *)&Ty)+6)
#define CSV_FSCANF(...)     INVOKE_VAR_MACRO(CSV_FSCANF,__VA_ARGS__)

enum _CsvReadType
{
    ReadLine = 0,
    ReadFile = 1
};

template<class Ty, int ElemNums>
int CsvRead(int type, FILE *fp, Ty& Ty_, vector<Ty>& CsvData,int readlinenums = 1)
{
    int LineNums = 0;
    switch (type)
    {
    case ReadLine:
        switch(ElemNums)
        {
            case 1:
                while (readlinenums != 0)
                {
                    CSV_FSCANF(fp,Ty_);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    readlinenums--;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 2:
                while (readlinenums != 0)
                {
                    CSV_FSCANF(fp,Ty_,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    readlinenums--;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 3:
                while (readlinenums != 0)
                {
                    CSV_FSCANF(fp,Ty_,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    readlinenums--;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 4:
                while (readlinenums != 0)
                {
                    CSV_FSCANF(fp,Ty_,1,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    readlinenums--;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 5:
                while (readlinenums != 0)
                {
                    CSV_FSCANF(fp,Ty_,1,1,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    readlinenums--;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 6:
                while (readlinenums != 0)
                {
                    CSV_FSCANF(fp,Ty_,1,1,1,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    readlinenums--;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 7:
                while (readlinenums != 0)
                {
                    CSV_FSCANF(fp,Ty_,1,1,1,1,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    readlinenums--;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            default:
                return -1;
        }
    case ReadFile:
        switch(ElemNums)
        {
            case 1:
                while (1)
                {
                    CSV_FSCANF(fp,Ty_);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 2:
                while (1)
                {
                    CSV_FSCANF(fp,Ty_,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 3:
                while (1)
                {
                    CSV_FSCANF(fp,Ty_,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 4:
                while (1)
                {
                    CSV_FSCANF(fp,Ty_,1,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 5:
                while (1)
                {
                    CSV_FSCANF(fp,Ty_,1,1,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 6:
                while (1)
                {
                    CSV_FSCANF(fp,Ty_,1,1,1,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            case 7:
                while (1)
                {
                    CSV_FSCANF(fp,Ty_,1,1,1,1,1,1);
                    CsvData.push_back(Ty_);
                    LineNums++;
                    if(feof(fp))
                    {
                        break;
                    }
                }
                return LineNums;
            default:
                return -1;
        }
    default:
        return -2;
    }
}

template<class Ty>
class _CsvFile
{
    typedef void (*_pFunc_ShowLine)(Ty&);
    typedef void (*_pFunc_ShowCsvData)(vector<Ty>&);
private:
    FILE        *fp;
    Ty          line;
    vector<Ty>  CsvData;
    int         lineNums;
    _pFunc_ShowLine     pShowLine;
    _pFunc_ShowCsvData  pShowCsvData;

public:
    _CsvFile(const char* filepath):lineNums(0),pShowLine(NULL),pShowCsvData(NULL)
    {
        fp = fopen(filepath, "rb");
    }
    virtual ~_CsvFile()
    {
        fclose(fp);
    }
    bool read()
    {
        CsvData.clear();
        lineNums = CsvRead<Ty,sizeof(Ty)/sizeof(float)>(ReadFile, fp, line, CsvData);
        if(lineNums > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    bool readline(int linenums)
    {
        CsvData.clear();
        lineNums = CsvRead<Ty,sizeof(Ty)/sizeof(float)>(ReadLine, fp, line, CsvData, linenums);
        if(lineNums > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    void registShowLineFunc(_pFunc_ShowLine pShowLine_)
    {
        pShowLine = pShowLine_;
    }
    void registShowCsvDataFunc(_pFunc_ShowCsvData pShowCsvData_)
    {
        pShowCsvData = pShowCsvData_;
    }
    void showLine()
    {
        if(pShowLine != NULL)
        {
            (*pShowLine)(line);
        }
        else
        {
            ;
        }
    }
    void showCsvData()
    {
        if(pShowCsvData != NULL)
        {
            (*pShowCsvData)(CsvData);
        }
        else
        {
            ;
        }
    }
};

#endif // !CSV_FILE_STREAM_