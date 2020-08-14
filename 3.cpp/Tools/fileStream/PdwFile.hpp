/*
 * @Author: xpt 
 * @Date: 2020-08-12 17:59:04 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-08-13 15:40:12
 */

#ifndef _PDWFILE_HPP
#define _PDWFILE_HPP

#include <string>
#include <vector>
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
class _PdwFile
{
    typedef void (*_pFunc_ShowLine)(_FileMsgTy&);
    typedef void (*_pFunc_ShowData)(_readToMsgTy *, int, int);
private:
    ifstream         binfile;
    vector<>char             *fileHeader;
    UINT             fileHeaderSize;
    _readToMsgTy     *ReadData;
    UINT             ReadDataSize;
    _FileMsgTy       line;                      // 文件行缓存
    long             fileSize;                  // 文件大小
    int              fileLines;                 // 文件行数
    _fileStatus      fileStatus;                // 文件状态
    _pFunc_ShowLine  pShowLine;
    _pFunc_ShowData  pShowData;
public:
    _PdwFile():ReadDataSize(1), fileStatus(closed), pShowLine(NULL), pShowData(NULL)
    {
        fileHeaderSize  = _FileMsgTy::headerSize;
        fileHeader      = new char[fileHeaderSize];
        ReadData        = new _readToMsgTy[ReadDataSize];
    }
    virtual ~_PdwFile()
    {
        close();
        delete[] fileHeader;
        delete[] ReadData;
    }
    _readToMsgTy *GetDataAddr() const{return ReadData;}
    int     GetFileLines()      const{return fileLines;}
    int     GetReadDataSize()   const{return ReadDataSize;}
    void    clear(){ReadDataSize = 0; fileLines = 0; fileSize = 0;}
    void    ReadDataResize(UINT ReadDataSize_)
    {
        delete[] ReadData;
        ReadDataSize = ReadDataSize_;
        ReadData = new _readToMsgTy[ReadDataSize];
    }
    void    open(const string& filepath)
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
                ReadDataResize(fileLines);
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
    void    save(const string& save_path)
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
        svfile.write((char *)ReadData, ReadDataSize * sizeof(_readToMsgTy));
        svfile.close();
    }
    void    read()
    {
        CUINT   FileMsgTySize = sizeof(_FileMsgTy);
        UINT    i = 0;
        if((_FileMsgTy::lineHead != 0) && (_FileMsgTy::lineTail != 0))
        {
            for(i = 0; i < ReadDataSize; i++)
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
                }
            }
        }
        else
        {
            for(i = 0; i < ReadDataSize; i++)
            {
                binfile.read((char *)&line, FileMsgTySize);
                ReadData[i] = line;
            }
        }
    }
    void    close()
    {
        if(fileStatus != closed)
        {
            binfile.close();                                            // 关闭文件
            fileStatus = closed;
            clear();
        }
    }
    void    registShowLineFunc(_pFunc_ShowLine pShowLine_){pShowLine = pShowLine_;}
    void    registShowDataFunc(_pFunc_ShowData pShowData_){pShowData = pShowData_;}
    void    showStatus() const
    {
        COUTSECBEG("binaryFile showStatus");
        COUTSWIDTH(18, "文件状态      ", fileStatus);
        COUTSWIDTH(18, "文件大小      ", fileSize);
        COUTSWIDTH(18, "文件头长度    ", fileHeaderSize);
        COUTSWIDTH(18, "文件行数      ", fileLines);
        COUTSWIDTH(18, "缓冲数据长度  ", ReadDataSize);
        COUTSECEND("binaryFile showStatus");
    }
    void    showFileHeader() const
    {
        for(int i = 0; i < fileHeaderSize; i++)
        {
            COUT(i, int(fileHeader[i]));
        }
    }
    void    showLine()
    {
        if(pShowLine != NULL)
        {
            (*pShowLine)(line);
        }
    }
    void    showData(int st, int ed)
    {
        if(pShowData != NULL)
        {
            (*pShowData)(ReadData, st, ed);
        }
    }
};

#endif // !_PDWFILE_HPP