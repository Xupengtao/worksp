/*
 * @Author: xpt 
 * @Date: 2019-12-23 19:35:02 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-26 19:23:45
 */

#ifndef _BINARY_STREAM_HPP
#define _BINARY_STREAM_HPP

#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cstring>
#include "../Error.h"
#include "../Macro.h"
#include "../XptType.h"
using namespace std;

enum _fileStatus
{
    closed  = 0,
    openned = 1,
    readend = 2
};

/*
 * @template参数: _FileMsgTy 文件结构体 _readToMsgTy 读取结构。
 * @使用这个类模版，需要重定义 _readToMsgTy 的 operator = 。
 */
template<typename _FileMsgTy, typename _readToMsgTy>
class _binaryFile
{
    typedef void (*_pFunc_ShowLine)(_FileMsgTy&);
    typedef void (*_pFunc_ShowData)(_readToMsgTy *, int, int);
private:
    ifstream         binfile;
    char             *fileHeader;
    UINT             fileHeaderSize;
    _readToMsgTy     *ReadData;
    UINT             ReadDataSize;
    _FileMsgTy       line;                      // 文件行缓存
    int              curLine;                   // 文件当前行
    int              ReadLines;                 // ReadData读取数据长度
    long             fileSize;                  // 文件大小
    int              fileLines;                 // 文件行数
    _fileStatus      fileStatus;                // 文件状态
    _pFunc_ShowLine  pShowLine;
    _pFunc_ShowData  pShowData;

public:
    _binaryFile(UINT ReadDataSize_ = 1000000)
                :ReadDataSize(ReadDataSize_), fileStatus(closed), pShowLine(NULL), pShowData(NULL)
    {
        fileHeaderSize  = _FileMsgTy::headerSize;
        fileHeader      = new char[fileHeaderSize];
        ReadData        = new _readToMsgTy[ReadDataSize];
    }
    _binaryFile(const string& filepath, UINT ReadDataSize_ = 1000000)
                :ReadDataSize(ReadDataSize_), fileStatus(closed), pShowLine(NULL), pShowData(NULL)
    {
        fileHeaderSize  = _FileMsgTy::headerSize;
        fileHeader      = new char[fileHeaderSize];
        ReadData        = new _readToMsgTy[ReadDataSize];
        open(filepath);
    }
    virtual ~_binaryFile()
    {
        close();
        delete[] fileHeader;
        delete[] ReadData;
    }
    int  GetFileLines() const {return fileLines;}
    void ReadDataResize(UINT ReadDataSize_)
    {
        ReadDataSize = ReadDataSize_;
        delete[] ReadData;
        ReadData = new _readToMsgTy[ReadDataSize];
    }
    int GetDataValidLines() const
    {
        return ReadLines;
    }
    _readToMsgTy *GetDataAddr() const
    {
        return ReadData;
    }
    void clear()
    {
        curLine     = 0; 
        ReadLines   = 0;
        fileLines   = 0; 
        fileSize    = 0;
    }
    void open(const string& filepath)
    {
        clear();
        if(fileStatus == closed)
        {
            binfile.open(filepath, ios::binary);                    // 打开文件
            if(!binfile.is_open())
            {
                ERRORMSG("读取文件错误!");
            }
            binfile.seekg(0, ios::end);
            fileSize = binfile.tellg();
            if(fileSize > fileHeaderSize)
            {
                fileSize -= fileHeaderSize;
                COUTS("文件路径 ", filepath, "文件大小(bytes) ", fileSize);
                fileLines = fileSize/sizeof(_FileMsgTy);
            }
            else
            {
                ERRORMSG("文件太小！！!");
            }
            binfile.seekg(0, ios::beg);
            binfile.read(fileHeader, fileHeaderSize);               // 将文件头读出
            fileStatus = openned;
            showStatus();
        }
        else
        {
            ERRORMSG("这个文件已经打开了...");
        }
    }
    void save(const string& save_path)
    {
        if(save_path.size() == 0)
        {
            ERRORMSG("保存文件路径 Null!");
            return;
        }
        ofstream svfile(save_path, ios::binary);
        if(!svfile.is_open())
        {
            ERRORMSG("保存文件无法打开!");
            return;
        }
        svfile.write(fileHeader, fileHeaderSize * sizeof(char));
        svfile.write((char *)ReadData, ReadDataSize * sizeof(_readToMsgTy));
        svfile.close();
    }
    void seekg(int seekst)
    {
        CUINT   FileMsgTySize = sizeof(_FileMsgTy);
        if((seekst >= 0) && (seekst < fileLines))
        {
            binfile.seekg(seekst * FileMsgTySize + fileHeaderSize, ios::beg);
            curLine = seekst;
            fileStatus = openned;
        }
    }
    void read(int seekst = -1)
    {
        CUINT   FileMsgTySize = sizeof(_FileMsgTy);
        UINT    i = 0, ReadSize = 0;
        if((seekst >= 0) && (seekst < fileLines))
        {
            binfile.seekg(seekst * FileMsgTySize + fileHeaderSize, ios::beg);
            curLine = seekst;
        }
        if(fileLines - curLine > ReadDataSize)                          // 文件剩余足够行_FileMsgTy
        {
            ReadSize = ReadDataSize;
        }
        else                                                            // 文件剩余数据长度不够ReadDataSize
        {
            ReadSize = fileLines - curLine;
            fileStatus = readend;
        }
        if((_FileMsgTy::lineHead != 0) && (_FileMsgTy::lineTail != 0))
        {
            for(i = 0; i < ReadSize; i++)
            {
                binfile.read((char *)&line, FileMsgTySize);
                if((line.head == _FileMsgTy::lineHead) && (line.tail == _FileMsgTy::lineTail))
                {
                    ReadData[i] = line;
                }
                else
                {
                    ERRORMSG("文件读取格式错误!");
                    COUT(i);
                    break;
                }
            }
        }
        else
        {
            for(i = 0; i < ReadSize; i++)
            {
                binfile.read((char *)&line, FileMsgTySize);
                ReadData[i] = line;
            }
        }
        curLine     = curLine + i;
        ReadLines   = i;
    }
    void AddRead(float AddTime_ms, CINT ToaUnit_ns)
    {
        CUINT   FileMsgTySize = sizeof(_FileMsgTy);
        UINT    i = 0;
        CINT    AddTime = CINT(AddTime_ms*(1000000/ToaUnit_ns));
        UINT    ToaSt = 0, ToaTmp;
        bool    ExceedTimeTag = false;
        for(i = 0; !ExceedTimeTag; i++)
        {
            if(i >= ReadDataSize)
            {
                break;
            }
            binfile.read((char *)&line, FileMsgTySize);
            if((line.head == _FileMsgTy::lineHead) && (line.tail == _FileMsgTy::lineTail))
            {
                ReadData[i] = line;
                if(ToaSt == 0)
                {
                    if(ReadData[i].ToaS != -1)
                    {
                        ToaSt = ReadData[i].Toa;
                    }
                }
                else
                {
                    if(ReadData[i].ToaS != -1)
                    {
                        ToaTmp = ReadData[i].Toa;
                        if(ToaTmp - ToaSt > AddTime)
                        {
                            ExceedTimeTag = true;
                        }
                    }
                }
            }
            else
            {
                fileStatus = readend;
                ERRORMSG("文件读取格式错误!");
                COUT(i);
                break;
            }
        }
        curLine     = curLine + i;
        ReadLines   = i;
    }
    bool isEOF()
    {
        return fileStatus == readend;
    }
    void close()
    {
        if(fileStatus != closed)
        {
            binfile.close();                                            // 关闭文件
            fileStatus = closed;
            clear();
        }
    }
    void registShowLineFunc(_pFunc_ShowLine pShowLine_)
    {
        pShowLine = pShowLine_;
    }
    void registShowDataFunc(_pFunc_ShowData pShowData_)
    {
        pShowData = pShowData_;
    }
    void showStatus() const
    {
        COUTSECBEG("binaryFile showStatus");
        COUTSWIDTH(18, "文件状态      ", fileStatus);
        COUTSWIDTH(18, "文件大小      ", fileSize);
        COUTSWIDTH(18, "文件头长度    ", fileHeaderSize);
        COUTSWIDTH(18, "文件行数      ", fileLines);
        COUTSWIDTH(18, "文件当前行    ", curLine);
        COUTSWIDTH(18, "缓冲数据长度  ", ReadDataSize);
        COUTSECEND("binaryFile showStatus");
    }
    void showFileHeader() const
    {
        for(int i = 0; i < fileHeaderSize; i++)
        {
            COUT(i, int(fileHeader[i]));
        }
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
    void showData(int st, int ed)
    {
        if(pShowData != NULL)
        {
            (*pShowData)(ReadData, st, ed);
        }
        else
        {
            ;
        }
    }
};

#endif // !_BINARY_STREAM_HPP