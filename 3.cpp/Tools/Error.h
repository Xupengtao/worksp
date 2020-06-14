/*
 * Macro.h
 *
 *  Created on: Jul 1, 2019
 *      Author: xupengtao
 */

#ifndef ERROR_H_
#define ERROR_H_

#include "Macro.h"
#include <stdexcept>
using namespace std;

#define _OK 0
#define _ERROR -1

/**
 * condition为false时抛出错误，错误信息为error_message
 */
#define ERRORMSG(errmsg) errorMsg(__FILE__, __func__, __LINE__, errmsg)

inline void errorMsg(const std::string &filePath, const std::string &function, int line, const std::string &info)
{
    //获取文件名
    unsigned long long int pos=filePath.find_last_of('/');
    std::string filename(filePath.substr(pos+1));
    std::string err = "发现错误! --> 文件名 " + filename+" 函数名 "+function+" (文件行数: "+std::to_string(line)+"):  "+info;
    COUTSTREND(err);
}

#define ASSERT(condition,error_message) ((condition)?0:(assertion(__FILE__, __func__, __LINE__, error_message),-1))

inline void assertion(const std::string &filePath, const std::string &function, int line, const std::string &info)
{
    //获取文件名
    unsigned long long int pos=filePath.find_last_of('/');
    std::string filename(filePath.substr(pos+1));
    std::string err = filename+" "+function+" "+std::to_string(line)+">>  "+info;
    //抛出错误
    throw std::runtime_error(err);
}

#endif /* ERROR_H_ */
