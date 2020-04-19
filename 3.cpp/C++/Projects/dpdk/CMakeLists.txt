cmake_minimum_required(VERSION 3.13)
project(dpdk)

add_definitions(-D_GNU_SOURCE
        -DRTE_MACHINE_CPUFLAG_SSE
        -DRTE_MACHINE_CPUFLAG_SSE2
        -DRTE_MACHINE_CPUFLAG_SSE3
        -DRTE_MACHINE_CPUFLAG_SSSE3
        -DRTE_MACHINE_CPUFLAG_SSE4_1
        -DRTE_MACHINE_CPUFLAG_SSE4_2
        -DRTE_MACHINE_CPUFLAG_AES
        -DRTE_MACHINE_CPUFLAG_PCLMULQDQ
        -DRTE_MACHINE_CPUFLAG_AVX
        #-DRTE_MACHINE_CPUFLAG_RDRAND
        -DRTE_MACHINE_CPUFLAG_FSGSBASE
        -DRTE_MACHINE_CPUFLAG_F16C
        -DRTE_MACHINE_CPUFLAG_AVX2
        )
#set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-msse4.2 -mavx2")
# 程序输出文件名
set(APP "hello")
# DPDK源码路径
set(DPDK_BUILD_PATH "/root/soft/dpdk-19.02/build")
# DPDK库路径
set(DPDK_LIB_DIR "${DPDK_BUILD_PATH}/lib")


set(STDAFX_PCH_H ${DPDK_PATH}/include/dpdk/rte_config.h)

include_directories(
        ${DPDK_BUILD_PATH}/include/dpdk
)
link_directories(
        ${DPDK_BUILD_PATH}/lib
        /usr/lib64
)

add_executable(${APP} helloworld.c)
add_executable(api jz_dpdk_api.h jz_dpdk_api.c main.cpp)

target_link_libraries(${APP}
        rte_flow_classify
        -Wl,--whole-archive
        rte_pipeline
        -Wl,--no-whole-archive
        -Wl,--whole-archive
        rte_table
        -Wl,--no-whole-archive
        -Wl,--whole-archive
        rte_port
        -Wl,--no-whole-archive
        rte_pdump
        rte_distributor
        rte_ip_frag
        rte_meter
        rte_lpm
        -Wl,--whole-archive
        rte_acl
        -Wl,--no-whole-archive
        rte_jobstats
        rte_metrics
        rte_bitratestats
        rte_latencystats
        rte_power
        rte_efd
        rte_bpf
        rte_ipsec
        -Wl,--whole-archive
        rte_cfgfile
        rte_gro
        rte_gso
        rte_hash
        rte_member
        rte_vhost
        rte_kvargs
        rte_mbuf
        rte_net
        rte_ethdev
        rte_bbdev
        rte_cryptodev
        rte_security
        rte_compressdev
        rte_eventdev
        rte_rawdev
        rte_timer
        rte_mempool
        rte_mempool_ring
        rte_ring
        rte_pci
        rte_eal
        rte_cmdline
        rte_reorder
        rte_sched
        rte_kni
        rte_common_cpt
        rte_common_octeontx
        rte_common_dpaax
        rte_bus_pci
        rte_bus_vdev
        rte_bus_dpaa
        rte_bus_fslmc
        rte_mempool_bucket
        rte_mempool_stack
        rte_mempool_dpaa
        rte_mempool_dpaa2
        rte_pmd_af_packet
        rte_pmd_ark
        rte_pmd_atlantic
        rte_pmd_avf
        rte_pmd_avp
        rte_pmd_axgbe
        rte_pmd_bnxt
        rte_pmd_bond
        rte_pmd_cxgbe
        rte_pmd_dpaa
        rte_pmd_dpaa2
        rte_pmd_e1000
        rte_pmd_ena
        rte_pmd_enetc
        rte_pmd_enic
        rte_pmd_fm10k
        rte_pmd_failsafe
        rte_pmd_i40e
        rte_pmd_ice
        rte_pmd_ixgbe
        rte_pmd_kni
        rte_pmd_lio
        rte_pmd_nfp
        rte_pmd_null
        rte_pmd_qede
        rte_pmd_ring
        rte_pmd_softnic
        rte_pmd_sfc_efx
        rte_pmd_tap
        rte_pmd_thunderx_nicvf
        rte_pmd_vdev_netvsc
        rte_pmd_virtio
        rte_pmd_vhost
        rte_pmd_ifc
        rte_pmd_vmxnet3_uio
        rte_bus_vmbus
        rte_pmd_netvsc
        rte_pmd_bbdev_null
        rte_pmd_null_crypto
        rte_pmd_octeontx_crypto
        rte_pmd_crypto_scheduler
        rte_pmd_dpaa2_sec
        rte_pmd_dpaa_sec
        rte_pmd_caam_jr
        rte_pmd_virtio_crypto
        rte_pmd_octeontx_zip
        rte_pmd_qat
        rte_pmd_skeleton_event
        rte_pmd_sw_event
        rte_pmd_dsw_event
        rte_pmd_octeontx_ssovf
        rte_pmd_dpaa_event
        rte_pmd_dpaa2_event
        rte_mempool_octeontx
        rte_pmd_octeontx
        rte_pmd_opdl_event
        rte_pmd_skeleton_rawdev
        rte_pmd_dpaa2_cmdif
        rte_pmd_dpaa2_qdma
        rte_bus_ifpga
        rte_pmd_ifpga_rawdev
        -Wl,--no-whole-archive
        pthread
        rt
        m
        numa
        dl

        )
