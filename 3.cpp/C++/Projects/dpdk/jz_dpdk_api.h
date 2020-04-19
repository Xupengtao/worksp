//
// Created by root on 19-3-9.
//

#ifndef DPDK_JZ_DPDK_API_H
#define DPDK_JZ_DPDK_API_H


/********************************************************************
	Copyright (C), 2001-2017, XXX Co., Ltd.
	File Name	:	jz_dpdk_api.h
	Version		: 	Initial Draft
	Author		:
	Created		:	2017.06.19
	Last Modified:	2017.06.19
	Description	:
封装dpdk抓包接口
源代码实现的功能：
1、可通过配置文件进行配置多个队列收取数据包，并且保证数据包的同源同宿（或者负载均衡，二者取其一，具体方法须修改网卡驱动程序）
2、实时显示收到总包数、Gbps、pps。
 https://blog.csdn.net/oceanperfect/article/details/75226855
*********************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <rte_config.h>
#include <rte_eal.h>
#include <rte_ethdev.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
*   rx_number：从配置文件中读取的线程数，用于调用者绑定cpu以及开启多线程
*/
extern int rx_number;

/*
*   描述: 初始化函数，对端口、队列、大页内存等配置。
*   参数: 无。
*   返回值: 初始化成功返回0，失败返回-1。
*/
int  jz_dpdk_init(int argc,char **argv);

/*
*   描述: 接收包。
*   参数: uint8_t : 端口号
          uint16_t: 队列号
          struct rte_mbuf * ：存储接收到包的数组指针，一次性返回多个数据包。
*         uint16_t ： 一次接收包的个数，需要根据包长以及配置的单个页大小来设定。
*                     如：页大小为2M，设定接收包大概是1500字节，那么设置32.
*   返回值:返回收到包的个数，如果为-1，则说明收包失败。
*/
int jz_dpdk_recv_pkts(uint8_t port_id,uint16_t queue_id,struct rte_mbuf *pkts[],const uint16_t nb_pkts);

/*
*   描述: 释放dpdk收包内存，注意：此接口必须在收包成功后调用。
*   参数：struct rte_mbuf * ：存储接收到包的数组指针。
*         uint16_t ：收到包的个数，与接口 jz_dpdk_recv_pkt 配合使用。
*   返回值: 无。
*/
void jz_dpdk_free(struct rte_mbuf *pkts[], uint16_t nb_pkts);

/*
*   描述: 返回当前收包状态，如pps,总包数。
*   参数: 无。
*   返回值: 无。
*/
void jz_dpdk_current_stat(void);
#ifdef __cplusplus
};
#endif

#endif //DPDK_JZ_DPDK_API_H
