/*
 * @Author: xpt 
 * @Date: 2020-01-09 23:30:48 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-01-09 23:31:11
 * @descript: try compiling with and without -fopenmp compiler flag
 */

#include <iostream>

int main()
{
#ifdef _OPENMP
	std::cout << "Hello, OpenMP!" << std::endl;
#else
	std::cout << "OpenMP is not enabled." << std::endl;
#endif
	return 0;
}