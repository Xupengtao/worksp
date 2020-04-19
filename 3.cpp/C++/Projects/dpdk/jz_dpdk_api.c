//
// Created by root on 19-3-9.
//


#include "jz_dpdk_api.h"
//#include "ini/iniparser.h"

/**************************宏********************************/
#define NUM_MBUFS_ 8191
#define MBUF_CACHE_SIZE_ 512
#define RX_RING_SIZE_ 128
#define TX_RING_SIZE_ 512

/**************************全局变量**************************/
int rx_number  = 0;
uint64_t total_pkts[512] = {0};
uint64_t last_total_pkts[512] = {0};
uint64_t total_pkts_bytes[512] = {0};
uint64_t last_total_pkts_bytes[512] = {0};
uint64_t last_time = 0;
static const struct rte_eth_conf port_conf_default =
        {
                .rxmode = {
                        .max_rx_pkt_len = ETHER_MAX_LEN,
                        .mq_mode = ETH_MQ_RX_RSS
                },
                .rx_adv_conf = {
                        .rss_conf = {
                                .rss_key = NULL,
                                .rss_hf = ETH_RSS_IP,
                        }
                }
        };

/**************************函数声明*************************/
static int  config_init(const char *filename);
static int  port_init(int port,struct rte_mempool *mbuf_pool);
static void recv_pkt_init(void);

/**************************接口实现*************************/
int jz_dpdk_init(int argc,char **argv)
{
    const char *filename = "conf.ini";

    // 0.配置文件读取
    rx_number = config_init(filename);
    if( rx_number < 1)
    {
        printf("错误：读取配置文件[conf.ini] 失败！\n");
        return -1;
    }

    // 1. DPDK EAL初始化
    if( rte_eal_init(argc,argv) < 0 )
    {
        printf("错误：EAL初始化失败！\n");
        return -1;
    }
    else
    {
        printf("EAL初始化成功。\n");
    }

    // 2. 检测至少有一个端口（即网口）
    if( rte_eth_dev_count() != 1)
    {
        printf("错误：只能有一个端口（网口）。\n");
        return -1;
    }

    // 3. 内存池的创建
    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create("JZ_MBUF_POOL",
                                                            NUM_MBUFS_ * rte_eth_dev_count(), MBUF_CACHE_SIZE_, 0,
                                                            RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if( NULL == mbuf_pool )
    {
        printf("错误：创建内存池失败。\n");
        return -1;
    }
    else
    {
        printf("创建内存池成功。\n");
    }

    // 4. 对端口进行初始化
    int port = 0;
    for (; port < rte_eth_dev_count(); port++)
    {
        if( port_init(port, mbuf_pool) < 0)
        {
            printf("错误：端口初始化失败。\n");
            return -1;
        }
        else
        {
            printf("端口初始化成功。\n");
        }
    }

    recv_pkt_init();
    printf("\n恭喜：jz_dpdk 初始化成功！\n");

    return 0;
}

int jz_dpdk_recv_pkts(uint8_t port_id,uint16_t queue_id,struct rte_mbuf *pkts[],const uint16_t nb_pkts)
{
    uint16_t nb_rx = rte_eth_rx_burst(port_id, queue_id, pkts, nb_pkts);

    if( nb_rx > 0)
    {
        total_pkts[queue_id] += nb_rx;
        uint16_t i;

        for ( i = 0; i < nb_rx; i++)
        {
            total_pkts_bytes[queue_id] += pkts[i]->pkt_len + 24;
        }
    }

    return nb_rx;
}

void jz_dpdk_free(struct rte_mbuf *pkts[], uint16_t nb_pkts)
{
    uint16_t i;

    for ( i = 0; i < nb_pkts; i++)
    {
        rte_pktmbuf_free(pkts[i]);
    }
}

void jz_dpdk_current_stat(void)
{
    uint64_t now = time(NULL);
    uint64_t total_pps = 0;
    uint64_t total_pkts_all = 0;
    float total_gbps = 0.0;
    uint16_t i = 0;

    printf("---------------------------------------------------------\n");
    for(; i < rx_number; i++)
    {
        uint64_t pps = (total_pkts[i] - last_total_pkts[i]) / (now - last_time);
        float gbps = (float)((total_pkts_bytes[i] - last_total_pkts_bytes[i]) * 8) / ((now - last_time) * 1000000000);

        last_total_pkts_bytes[i] = total_pkts_bytes[i];
        last_total_pkts[i] = total_pkts[i];
        total_pps += pps;
        total_gbps += gbps;
        total_pkts_all += total_pkts[i];

        printf("Rx[%d] rate: [current %lu pps/%0.3f Gbps][total: %lu pkts]\n",i,pps,gbps,total_pkts[i]);
    }
    printf("Rx[All] rate: [current %lu pps/%0.3f Gbps][total: %lu pkts]\n",total_pps,total_gbps,total_pkts_all);

    last_time = now;
}

int config_init(const char *filename)
{
    int thread_number = 0;
    /*
    dictionary *ini = iniparser_load(filename);

    if( NULL == ini)
    {

        return thread_number;
    }

    thread_number = iniparser_getint(ini, "config:thread_number",-1);
     */
    thread_number = 2;
    printf("成功配置了 %d 个线程用于接收数据包.\n",thread_number);

    return thread_number;
}

int port_init(int port,struct rte_mempool *mbuf_pool)
{
    struct rte_eth_conf port_conf = port_conf_default;
    const uint16_t rx_rings = rx_number, tx_rings = rx_number;
    int retval = 0;
    uint16_t q = 0;
    struct rte_eth_link link;

    retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
    if (retval != 0)
    {
        printf("ERROR: %d , %s ,%d ",retval,__FUNCTION__,__LINE__);
        return retval;
    }

    /* 初始化队列. */
    for (q = 0; q < rx_rings; q++)
    {
        retval = rte_eth_rx_queue_setup(port, q, RX_RING_SIZE_,
                                        rte_eth_dev_socket_id(port), NULL, mbuf_pool);
        if (retval < 0)
        {
            printf("ERROR: %d , %s ,%d ",retval,__FUNCTION__,__LINE__);
            return retval;
        }
    }

    for (q = 0; q < tx_rings; q++)
    {
        retval = rte_eth_tx_queue_setup(port, q, TX_RING_SIZE_,
                                        rte_eth_dev_socket_id(port), NULL);
        if (retval < 0)
        {
            printf("ERROR: %d , %s ,%d ",retval,__FUNCTION__,__LINE__);
            return retval;
        }
    }

    /* 启动端口 */
    retval = rte_eth_dev_start(port);
    if (retval < 0)
    {
        printf("ERROR: %d , %s ,%d ",retval,__FUNCTION__,__LINE__);
        return retval;
    }
    /* 开启混杂模式 */
    rte_eth_promiscuous_enable(port);

    /* 查看端口状态 */
    rte_eth_link_get(port, &link);
    if (link.link_status)
    {
        printf("Port %d Link Up - speed %u Mbps - %s\n",port,
               (uint32_t) link.link_speed,
               (link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
               ("full-duplex") : ("half-duplex\n"));
    }
    else
    {
        printf("Port %d Link Down\n",port);
    }

    return 0;
}

void recv_pkt_init(void)
{
    uint8_t port;

    for (port = 0; port < rte_eth_dev_count(); port++)
    {
        if (rte_eth_dev_socket_id(port) > 0 && rte_eth_dev_socket_id(port) != (int)rte_socket_id())
        {
            printf("WARNING, port %u is on remote NUMA node to "
                   "polling thread.\n\tPerformance will "
                   "not be optimal.\n", port);
        }
    }
    printf("Core %u forwarding packets. [Ctrl+C to quit]\n",rte_lcore_id());
}


