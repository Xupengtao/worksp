/*
 * @Author: xpt 
 * @Date: 2020-01-03 06:34:33 
 * @Last Modified by: xpt
 * @Last Modified time: 2020-01-03 06:48:45
 */

/* cat /proc/cpuinfo            查看cpu信息
 * ps
 * taskset -p ProcessId         查看进程当前运行在哪个cpu上
 * taskset -pc 1 ProcessId      指定进程运行在cpu1上
 * taskset -c 1 ./dgram_servr&  启动程序时绑定cpu
 */
#include <unistd.h>

int sysconf(_SC_NPROCESSORS_CONF);/* 返回系统可以使用的核数，但是其值会包括系统中禁用的核的数目，因 此该值并不代表当前系统中可用的核数 */
int sysconf(_SC_NPROCESSORS_ONLN);/* 返回值真正的代表了系统当前可用的核数 */

/* 以下两个函数与上述类似 */
#include <sys/sysinfo.h>

int get_nprocs_conf (void);/* 可用核数 */
int get_nprocs (void);/* 真正的反映了当前可用核数 */

#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <sched.h>

/* 设置进程号为pid的进程运行在mask所设定的CPU上
 * 第二个参数cpusetsize是mask所指定的数的长度
 * 通常设定为sizeof(cpu_set_t)

 * 如果pid的值为0,则表示指定的是当前进程 
 */
int sched_setaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask);

/* 获得pid所指示的进程的CPU位掩码,并将该掩码返回到mask所指向的结构中 */
int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask);

//绑定线程到cpu核上运行
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <pthread.h>

int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset);
int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t *cpuset);

Compile and link with -pthread.