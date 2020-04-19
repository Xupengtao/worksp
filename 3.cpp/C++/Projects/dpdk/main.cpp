//
// Created by root on 19-3-9.
//

#include <iostream>
#include "jz_dpdk_api.h"

int main(int argc,char **argv)
{
    jz_dpdk_init(argc,argv);
    std::cout << "hello,world." << std::endl;

    return 0;
}
