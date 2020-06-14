/*
 * @Author: xpt 
 * @Date: 2019-12-23 00:20:26 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-05-19 21:13:05
 */

#ifndef XPT_TYPE
#define XPT_TYPE

#include <iostream>
#include <sstream>
#include <assert.h>
#include <string>
#include "Macro.h"
using namespace std;

typedef unsigned char           UCHAR;
typedef unsigned short          USHORT;
typedef unsigned int            UINT;
typedef unsigned long long      ULONGLONG;
typedef const char              CCHAR;
typedef const short             CSHORT;
typedef const int               CINT;
typedef const unsigned char     CUCHAR;
typedef const unsigned short    CUSHORT;
typedef const unsigned int      CUINT;

template<typename T>                                //大小端转换
T EndianSwap(const T &v)
{
    T r;
    int size = sizeof(T);
    CUCHAR *cvi = (CUCHAR *)&v;
    UCHAR  *cri = (UCHAR *)&r;
    cri = cri + size - 1;
    for(int i = 0; i < size; i++)
    {
        *cri = *cvi;
        ++cvi;
        --cri;
    }
    return r;
}
template<>
inline USHORT EndianSwap(const USHORT &v)           //USHORT大小端转换 模版全特化
{
    assert(sizeof(USHORT) == 2);
    return ((v & 0xff) << 8) | (v >> 8);
}
template<>
inline UINT EndianSwap(const UINT &v)               //UINT大小端转换 模版全特化
{
    assert(sizeof(UINT) == 4);
    return (v >> 24                 |
            ((v & 0x00ff0000) >> 8) |
            ((v & 0x0000ff00) << 8) |
            (v << 24));
}
template<>
inline ULONGLONG EndianSwap(const ULONGLONG &v)     //ULONGLONG大小端转换 模版全特化
{
    assert(sizeof(ULONGLONG) == 8);
    return (v >> 56                 |
            ((v & 0x00ff000000000000) >> 40) |
            ((v & 0x0000ff0000000000) >> 24) |
            ((v & 0x000000ff00000000) >> 8)  |
            ((v & 0x00000000ff000000) << 8)  |
            ((v & 0x0000000000ff0000) << 24) |
            ((v & 0x000000000000ff00) << 40) |
            (v << 56));
}

template<class T>
inline string tostring(T t)
{
    stringstream ToString;
    ToString<<t;
    string rtnStr;
    ToString>>rtnStr;
    return rtnStr;
}

template<class T>
inline T stringto(string& str)
{
    T rtnT;
    stringstream StringTo;
    StringTo<<str;
    StringTo>>rtnT;
    return rtnT;
}

inline string chartostring(const char *ch)
{
    string rtnstr(ch);
    return rtnstr;
}
inline CCHAR* stringtochar(string& str)
{
    CCHAR* rtnch = str.c_str();
    return rtnch;
}

#endif // !XPT_TYPE
