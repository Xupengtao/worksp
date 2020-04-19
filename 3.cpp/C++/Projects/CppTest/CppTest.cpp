/*
 * @Author: xpt 
 * @Date: 2020-04-19 14:36:35 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-04-19 14:38:19
 */
#include"../Tools/Error.h"
#include"../Tools/Macro.h"
#include"../Tools/XptType.h"

UINT GetRCCLNum(UINT Num)
{
    UINT i;
    for(i = 0; Num > 1; i++)
    {
        Num = Num>>1;
    }
    return i;
}

class _CaseA
{
    int a;
public:
    _CaseA(int m):a(m){};
};

int main()
{
    // COUT(GetRCCLNum(0x1000000));
    // COUT(sizeof(UINT));
    // _CaseA *CaseA = new _CaseA[2]{(1), (2)};

    /**
     * 20200419 测试无符号数溢出减法
     */
    UCHAR a = 1, b = 250;
    COUT(abs(a - b));
    return 1;
}