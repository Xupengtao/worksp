/*
 * @Author: xpt 
 * @Date: 2019-12-17 22:43:29 
 * @Last Modified by: xpt
 * @Last Modified time: 2019-12-20 17:44:36
 */
 #include<iostream>
 #include<string>
 #include<stdlib.h>
 #include"../Tools/Macro.h"
 #include"../Tools/Timeit.h"
 #include"../cudaLib/cudaCommon.h"
 #include"../Tools/PdwStruct/PdwStruct.hpp"
 using namespace std;

#define UNROLL_NUMS 8

__constant__ int d_ArraySize, d_SharedSize, d_Order;

/* @Author: xpt
 * 1.__launch_bounds__(maxThreadsPerBlock, minBlockPerMultiprocessor) 
 * maxThreadsPerBlock指出了每个线程块可以包含的最大线程数，这个线程块由核函数来启动。
 * minBlockPerMultiprocessor是可选参数，指明了在每个SM中预期的最小的常驻线程块数量。
 * 2.__restrict__  只读缓存，可以使用内部函数__ldg用于代替标准指针解引用。
 * 3.共享内存和一级缓存共用，可以使用cudaDeviceSetCacheConfig配置大小。
 * 4.使用常量索引的局部数组变量，也可能分配在寄存器上，在编译时使用 -Xptxas -v, -abi=no
 * 可以输出寄存器的数量、共享内存的字节数以及每个线程所使用的常量内存的字节数。
 * 你还可以使用maxrregcount编译器选项(-maxrregcount=32)，来控制一个编译单元里所有核
 * 函数使用的寄存器的最大数量。
 * 5.#pragma unroll 编译时循环展开。
 * 6.还使用了防止线程束分化、显式障碍、内存栅栏、弱排序、内存填充、指令洗牌等技术。
 */
template<class Ty, int UnrollNums>
__global__ void
__launch_bounds__(256,2)
HistogramDiff(const Ty * __restrict__ inArray, Ty *OutArray)
{
    extern __shared__ Ty Diff[];                                                    //一阶导(x2-x1)/dx SharedSize+冗余Order 使用共享内存 建议？禁用一级Cache -dlcm=cg
    Ty DiffTmp[UnrollNums];                                                         //这个操作或许？可以使用寄存器，64个？256个？没有使用寄存器，大概率是在二级Cache上了。
    UINT InArrIdx           = UnrollNums*(blockIdx.x * blockDim.x + threadIdx.x);   //当前线程处理输入数组起始索引值
    UINT SmemIdx            = InArrIdx % d_SharedSize;                              //当前线程负责计算一级阶差的共享内存偏移
    UINT rearOrderThreadsNo = d_SharedSize - d_Order;                               //冗余Order数的一级阶差由Smem偏移线程打理
    #pragma unroll
    for(int i = 0; i < UnrollNums; i++)
    {
        DiffTmp[i]      = inArray[InArrIdx+i+1] - inArray[InArrIdx+i];
        Diff[SmemIdx+i] = DiffTmp[i];
        OutArray[InArrIdx+i] = DiffTmp[i];
    }
    if(threadIdx.x >= rearOrderThreadsNo/UnrollNums)                                //线程束不会分化，然而断言判断不会产生？尝试 -g 和 O3, $结果写回
    {
        #pragma unroll
        for(int i = 0; i < UnrollNums; i++)
        {
            Diff[SmemIdx+d_Order+i] = inArray[InArrIdx+d_Order+i+1] - inArray[InArrIdx+d_Order+i];
        }
    }
    __syncthreads();
    for(int i = 1; i < d_Order; i++)
    {
        #pragma unroll
        for(int j = 0; j < UnrollNums; j++)
        {
            DiffTmp[j] = DiffTmp[j] + Diff[SmemIdx+j+i];
            OutArray[i*d_ArraySize+InArrIdx+j] = DiffTmp[j];
        }
    }
}

template<class Ty>
void HistogramDiffOnHost(const Ty * inArray, Ty *OutArray, int ArraySize, int Difflens, int Order)
{
    for(int i = 0; i < Order; i++)
    {
        for(int j = 0; j < Difflens-Order-1; j++)
        {
            OutArray[i*ArraySize + j] = inArray[j+i+1] - inArray[j];
        }
    }
}

template<class Ty>
void SameCheckHistogram(const Ty * inArray0, const Ty * inArray1, const Ty * inArray2, int ArraySize, int Difflens, int Order)
{
    bool CheckTag = true;
    for(int i = 0; i < Order; i++)
    {
        for(int j = 0; j < Difflens-Order-1; j++)
        {
            if(inArray1[i*ArraySize + j] != inArray2[i*ArraySize + j])
            {
                COUT(i,j);
                inArray0[j].print();
                inArray0[j+i+1].print();
                inArray1[i*ArraySize + j].print();
                inArray2[i*ArraySize + j].print();
                CheckTag = false;
                break;
            }
        }
        if(CheckTag == false)
        {
            break;
        }
    }
}

template<class Ty>
void cHistogramDiff(int GridSize, int BlockSize, const Ty *d_pInArray, Ty *d_pOutArray, int SharedMemSize, int Order)
{
    COUTS("SharedMemSize: ", (SharedMemSize+Order) * sizeof(Ty));
    HistogramDiff<Ty, UNROLL_NUMS><<<GridSize, BlockSize, (SharedMemSize+Order) * sizeof(Ty)>>>(d_pInArray, d_pOutArray);
}

typedef _Pdw<> _PdwTy;

int main(int argc, char **argv)
{
    int dev = 0;
    cudaDeviceProp deviceProp;
    CHECK(cudaGetDeviceProperties(&deviceProp, dev));
    COUTS("starting reduction at ",argv[0]);
    COUTS("device no: ", dev, "device name: ", deviceProp.name);
    CHECK(cudaSetDevice(dev));
    CHECK(cudaDeviceSetCacheConfig(cudaFuncCachePreferShared));

    int Order       = 128;
    int blocksize   = 256;
    int SharedMems  = 1024;
    int size        = 1<<19;
    switch (argc)
    {
    case 4:
        size        = atoi(argv[3]);
    case 3:
        SharedMems  = atoi(argv[2]);
    case 2:
        Order       = atoi(argv[1]);
    case 1:
    default:
        blocksize = SharedMems/UNROLL_NUMS;
        COUT(Order, blocksize, SharedMems, size);
        break;
    }
    setupConstant(d_ArraySize, size);
    setupConstant(d_SharedSize, SharedMems);
    setupConstant(d_Order, Order);

    dim3 block (blocksize, 1);
    dim3 grid  ((size + block.x - 1) / block.x, 1);
    COUTS("grid: ", grid.x, "block: ", block.x);

    UINT InArrPdwbytes  = size * sizeof(_PdwTy);
    UINT OutArrPdwbytes = Order * InArrPdwbytes;

    _PdwTy *pInPdwArray;
    _PdwTy *pOutPdwArray;

    _Timeit Timeit(6);
    Timeit.Start(0, "Host cudaMallocHost allocation took(us) ");
    CHECK(cudaMallocHost ((_PdwTy **)&pInPdwArray, InArrPdwbytes));
    CHECK(cudaMallocHost ((_PdwTy **)&pOutPdwArray, OutArrPdwbytes));
    Timeit.End(0);

    for(int i = 0; i < size; i++)
    {
        pInPdwArray[i].random();
    }

    _PdwTy *d_pInPdwArray = NULL;
    _PdwTy *d_pOutPdwArray = NULL;

    CHECK(cudaMalloc((void **) &d_pInPdwArray, InArrPdwbytes));
    CHECK(cudaMalloc((void **) &d_pOutPdwArray, OutArrPdwbytes));

    CHECK(cudaMemcpy(d_pInPdwArray, pInPdwArray, InArrPdwbytes, cudaMemcpyHostToDevice));

    Timeit.Start(1, "HistogramDiff took(us) ");
    cHistogramDiff(grid.x/UNROLL_NUMS - 1, block.x, d_pInPdwArray, d_pOutPdwArray, SharedMems, Order);
    CHECK(cudaDeviceSynchronize());
    Timeit.End(1);
    CHECK(cudaMemcpy(pOutPdwArray, d_pOutPdwArray, OutArrPdwbytes, cudaMemcpyDeviceToHost));

    _PdwTy *pHostOutPdwArray = (_PdwTy *)malloc(OutArrPdwbytes);
    Timeit.Start(2, "HistogramDiffOnHost took(us) ");
    HistogramDiffOnHost(pInPdwArray, pHostOutPdwArray, size, UNROLL_NUMS * (grid.x/UNROLL_NUMS-1) * block.x, Order);
    Timeit.End(2);
    Timeit.Show(0,6);
    SameCheckHistogram(pInPdwArray, pOutPdwArray, pHostOutPdwArray, size, UNROLL_NUMS * (grid.x/UNROLL_NUMS-1) * block.x, Order);

    free(pHostOutPdwArray);
    CHECK(cudaFreeHost(pInPdwArray));
    CHECK(cudaFreeHost(pOutPdwArray));
    CHECK(cudaFree(d_pInPdwArray));
    CHECK(cudaFree(d_pOutPdwArray));
    cudaDeviceReset();
    return EXIT_SUCCESS;
}