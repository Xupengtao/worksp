/*
 * @Author: xpt 
 * @Date: 2019-12-23 00:17:00 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-23 17:44:55
 */

#ifndef _TIMEIT_H
#define _TIMEIT_H

#include "XptType.h"
#include <sys/time.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include "Macro.h"
using namespace std;

enum _TimeitUnitStatus
{
    Idle    = 0,
    Work    = 1,
    Elapsed = 2
};

struct _TimeitUnit
{
    double              start;
    double              end;
    double              elapsed;
    string              TimeitMsg;
    _TimeitUnitStatus   status;

    _TimeitUnit()
    {
        clear();
    }
    ~_TimeitUnit() {}
    void clear()
    {
        start       = 0.0;
        end         = 0.0;
        elapsed     = 0.0;
        TimeitMsg   = "";
        status      = Idle; 
    }
    double seconds()
    {
        struct timeval tp;
        struct timezone tzp;
        int i = gettimeofday(&tp, &tzp);
        return ((double)tp.tv_sec + (double)tp.tv_usec * 1.e-6);
    }
    bool Start(const std::string & TimitMsg_)
    {
        if(status != Work)
        {
            start = seconds();
            status = Work;
            TimeitMsg = TimitMsg_;
            return true;
        }
        else
        {
            COUTSTREND("Timeit is already work!");
            return false;
        }
    }
    bool End()
    {
        if(status == Work)
        {
            end = seconds();
            elapsed = end - start;
            status = Elapsed;
            return true;
        }
        else
        {
            COUTSTREND("Timeit is not work!");
            return false;
        }
    }
    void showElapsed()
    {
        if(status == Elapsed)
        {
            COUTS(TimeitMsg, elapsed * 1000000.0);
        }
        else
        {
            return;
        }
    }
};

class _Timeit
{
private:
    UINT size;
    vector<_TimeitUnit> TimeArray;
public:
    _Timeit(UINT size_):size(size_)
    {
        TimeArray.resize(size);
    }
    virtual ~_Timeit()
    {
        ;
    }
    void Start(int TimeNo = 0, std::string TimitMsg_ = "")
    {
        if(TimeNo < size)
        {
            if(TimitMsg_ == "")
            {
                TimitMsg_ = tostring(TimeNo);
            }
            TimeArray[TimeNo].Start(TimitMsg_);
        }
        else
        {
            COUTSTREND("TimeNo Out of Range!");
        }
    }
    void End(int TimeNo = 0)
    {
        if(TimeNo < size)
        {
            TimeArray[TimeNo].End();
        }
        else
        {
            COUTSTREND("TimeNo Out of Range!");
        }
    }
    void Show(int TimeNoSt = 0, int TimeNoEd = 0)
    {
        if(TimeNoSt >= size)
        {
            COUTSTREND("TimeNo Out of Range!");
            return;
        }
        if(TimeNoEd == 0)
        {
            for(int i = TimeNoSt; i < size; i++)
            {
                TimeArray[i].showElapsed();
            }
        }
        else
        {
            if(TimeNoEd >= size)
            {
                TimeNoEd = size;
            }
            for(int i = TimeNoSt; i < TimeNoEd; i++)
            {
                TimeArray[i].showElapsed();
            }
        }
    }
};

#endif