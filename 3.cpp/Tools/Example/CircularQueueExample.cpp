/*
 * @Author: xpt 
 * @Date: 2020-04-21 17:44:48 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-04-21 18:46:32
 */

#include "../CircularQueue.hpp"

void uintShow(int& showUint)
{
    cout<<showUint<<endl;
}

int main(int argc, char** args)
{
    _CircularQueue<int> CirQue(10);
    for(int i = 0; i < 2; i++)
    {
        CirQue.Push(i);
        CirQue.Push(i);
        CirQue.Pop();
    }
    CirQue.Pop();
    CirQue.Pop();
    int i = CirQue.Pop();
    COUT(i);
    CirQue.registShowFunc(uintShow);
    CirQue.Show();
    CirQue.ShowStatus();
}