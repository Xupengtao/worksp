/*
 * @Author: xpt 
 * @Date: 2019-12-23 18:43:26 
 * @Last Modified by:   xpt 
 * @Last Modified time: 2019-12-23 18:43:26 
 */

#include "../Timeit.h"

int main(int argc, char **argv)
{
    _Timeit Timeit(10);
    for(int i = 0; i < 10; i++)
    {
        Timeit.Start(i);
        Timeit.End(i);
    }
    Timeit.Show(0,10);
    cin.get();
    return 1;
}