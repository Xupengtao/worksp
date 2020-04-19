/*
 * @Author: xpt 
 * @Date: 2020-01-09 23:33:44 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-01-09 23:54:03
 */

#include <iostream>
// main OpenMP include header
#include <omp.h>
#include "../Tools/Macro.h"

int main()
{
	omp_set_num_threads(3);					//设置后续并行区域中并行执行的线程数
	COUTS("最多线程数", omp_get_max_threads());
	COUTS("CPU核心数", omp_get_num_procs());
	COUTS("活动线程数", omp_get_num_threads());
	COUTS("当前线程号", omp_get_thread_num());
	omp_set_dynamic(1);						//启用动态改变线程数量
	COUTS("是否支持动态改变线程数量", omp_get_dynamic());
	omp_set_dynamic(0);						//关闭动态改变线程数量
	COUTS("是否支持动态改变线程数量", omp_get_dynamic());

	return 0;
}
