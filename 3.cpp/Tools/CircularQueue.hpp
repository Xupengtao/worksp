/*
 * @Author: xpt 
 * @Date: 2020-04-21 16:37:04 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-04-21 18:45:56
 */
#ifndef _CIRCULARQUEUE_HPP
#define _CIRCULARQUEUE_HPP
#include "common.h"

template<typename _T>
struct _CircularQueue
{
    typedef void    (*_pFuncShow)(_T&);
    vector<_T>      Queue;
    UINT            Size;
    UINT            Capacity;
    UINT            StInd;
    _pFuncShow      pFuncShow;
public:
    _CircularQueue(UINT Capacity_):pFuncShow(NULL)
    {
        init(Capacity_);
    }
    virtual ~_CircularQueue()
    {
        pFuncShow = NULL;
    }
    inline void init(UINT Capacity_)
    {
        Capacity = Capacity_;
        Queue.resize(Capacity);
        clear();
    }
    inline void clear()
    {
        Size    = 0;
        StInd   = 0;
    }
    inline UINT GetSize() const {return Size;}
    inline _T& operator[](CUINT Ind)
    {
        if(Ind < Size)
        {
            if(StInd + Ind < Capacity)
            {
                return Queue[StInd + Ind];
            }
            else
            {
                return Queue[StInd + Ind - Capacity];
            }
        }
        else
        {
            ERRORMSG("Ind >= Size Error!");
            return Queue[0];
        }
    }
    inline void Push(_T& PushElem)
    {
        if(Size < Capacity)
        {
            Size++;
            (*this)[Size - 1] = PushElem;
        }
        else if(Size == Capacity)
        {
            Pop();
            Push(PushElem);
        }
        else
        {
            ERRORMSG("Size > Capacity Error!");
        }
    }
    inline _T Pop()
    {
        if(Size > 0)
        {
            _T rtn = (*this)[0];
            StInd = (StInd >= Capacity - 1) ? 0 : StInd + 1;
            Size--;
            return rtn;
        }
        else
        {
            ERRORMSG("Size == 0 Error!");
            return _T();
        }
    }
    void registShowFunc(_pFuncShow pFuncShow_)
    {
        pFuncShow = pFuncShow_;
    }
    void ShowStatus() const
    {
        COUTSECBEG("CircularQueue ShowStatus");
        COUT(Capacity, Size, StInd);
        COUTSECEND("CircularQueue ShowStatus");
    }
    void Show()
    {
        if(pFuncShow != NULL)
        {
            for(UINT i = 0; i < Size; i++)
            { 
                (*pFuncShow)((*this)[i]);
            }
        }
        else
        {
            ERRORMSG("pFuncShow == NULL");
        }
    }
};
#endif // _CIRCULARQUEUE_HPP