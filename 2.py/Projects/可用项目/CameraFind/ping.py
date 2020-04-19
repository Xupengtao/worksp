# -*- coding: utf-8 -*-
"""
Created on Mon Dec 31 02:34:45 2018

@author: wolai
"""

import re
import time
import struct
import socket
import select
import threading 
import subprocess

def chesksum(data):
    """
    校验
    """
    n = len(data)
    m = n % 2
    sum = 0 
    for i in range(0, n - m ,2):
        sum += (data[i]) + ((data[i+1]) << 8)#传入data以每两个字节（十六进制）通过ord转十进制，第一字节在低位，第二个字节在高位
    if m:
        sum += (data[-1])
    #将高于16位与低16位相加
    sum = (sum >> 16) + (sum & 0xffff)
    sum += (sum >> 16) #如果还有高于16位，将继续与低16位相加
    answer = ~sum & 0xffff
    #主机字节序转网络字节序列（参考小端序转大端序）
    answer = answer >> 8 | (answer << 8 & 0xff00)
    return answer 

    '''
    连接套接字,并将数据发送到套接字
    '''
def raw_socket(dst_addr,imcp_packet):
    rawsocket = socket.socket(socket.AF_INET,socket.SOCK_RAW,socket.getprotobyname("icmp"))
    send_request_ping_time = time.time()
    #send data to the socket
    rawsocket.sendto(imcp_packet,(dst_addr,80))
    return send_request_ping_time,rawsocket,dst_addr

    '''
    request ping
    '''
def request_ping(data_type,data_code,data_checksum,data_ID,data_Sequence,payload_body):
    #把字节打包成二进制数据
    imcp_packet = struct.pack('>BBHHH32s',data_type,data_code,data_checksum,data_ID,data_Sequence,payload_body)
    icmp_chesksum = chesksum(imcp_packet)#获取校验和
    imcp_packet = struct.pack('>BBHHH32s',data_type,data_code,icmp_chesksum,data_ID,data_Sequence,payload_body)
    return imcp_packet
    '''
    reply ping
    '''
def reply_ping(send_request_ping_time,rawsocket,data_Sequence,timeout = 2):
    while True:
        started_select = time.time()
        what_ready = select.select([rawsocket], [], [], timeout)
        wait_for_time = (time.time() - started_select)
        if what_ready[0] == []:  # Timeout
            return -1
        time_received = time.time()
        received_packet, addr = rawsocket.recvfrom(1024)
        icmpHeader = received_packet[20:28]
        type, code, checksum, packet_id, sequence = struct.unpack(
            ">BBHHH", icmpHeader
        )
        if type == 0 and sequence == data_Sequence:
            return time_received - send_request_ping_time
        timeout = timeout - wait_for_time
        if timeout <= 0:
            return -1

    '''
    实现 ping 主机/ip
    '''
def ping(host):
    data_type = 8 # ICMP Echo Request
    data_code = 0 # must be zero
    data_checksum = 0 # "...with value 0 substituted for this field..."
    data_ID = 0 #Identifier
    data_Sequence = 1 #Sequence number
    payload_body = b'abcdefghijklmnopqrstuvwabcdefghi' #data
    dst_addr = socket.gethostbyname(host)#将主机名转ipv4地址格式，返回以ipv4地址格式的字符串，如果主机名称是ipv4地址，则它将保持不变
    print("正在 Ping {0} [{1}] 具有 32 字节的数据:".format(host,dst_addr))
    for i in range(0,4):
        icmp_packet = request_ping(data_type,data_code,data_checksum,data_ID,data_Sequence + i,payload_body)
        send_request_ping_time,rawsocket,addr = raw_socket(dst_addr,icmp_packet)
        times = reply_ping(send_request_ping_time,rawsocket,data_Sequence + i)
        if times > 0:
            print("来自 {0} 的回复: 字节=32 时间={1}ms".format(addr,int(times*1000)))
            time.sleep(0.7)
        else:
            print("请求超时。")

def ping_rtn(host,iplist):
    data_type = 8 # ICMP Echo Request
    data_code = 0 # must be zero
    data_checksum = 0 # "...with value 0 substituted for this field..."
    data_ID = 0 #Identifier
    data_Sequence = 1 #Sequence number
    payload_body = b'abcdefghijklmnopqrstuvwabcdefghi' #data
    dst_addr = socket.gethostbyname(host)#将主机名转ipv4地址格式，返回以ipv4地址格式的字符串，如果主机名称是ipv4地址，则它将保持不变
    for i in range(0,4):
        icmp_packet = request_ping(data_type,data_code,data_checksum,data_ID,data_Sequence + i,payload_body)
        send_request_ping_time,rawsocket,addr = raw_socket(dst_addr,icmp_packet)
        times = reply_ping(send_request_ping_time,rawsocket,data_Sequence + i)
        if times > 0:
            time.sleep(0.1)
            iplist.append(host)
            break
            
def arping_scan(ip,arpdict):
    p = subprocess.Popen("arp -a %s" % ip,shell = True,stdout = subprocess.PIPE)
    out = p.stdout.read()
    result = out.split()
    pattern = re.compile(r'.*-.*-.*-.*-.*-.*')
    macaddr = None
    for item in result:
        if re.search(pattern,str(item)):
            macaddr = str(item)[2:-1]
            arpdict[ip] = macaddr
           
def rangeping(ipbegin,ipend,iplist):
    ipbegint = int(ipbegin[ipbegin.rfind('.')+1:])
    ipendint = int(ipend[ipend.rfind('.')+1:])
    ipbegintseg = ipbegin[:ipbegin.rfind('.')]
    ipendseg = ipend[:ipend.rfind('.')]
    ip_start = min(ipbegint,ipendint)
    ip_end = max(ipbegint,ipendint)
    if ipbegintseg != ipendseg:
        return 
    for ip in range(ip_start,ip_end+1):
        ping_rtn(ipbegintseg + '.' + str(ip),iplist)
        print(iplist)

def mutiping(ipbegin,ipend,iplist):
    ipbegint = int(ipbegin[ipbegin.rfind('.')+1:])
    ipendint = int(ipend[ipend.rfind('.')+1:])
    ipbegintseg = ipbegin[:ipbegin.rfind('.')]
    ipendseg = ipend[:ipend.rfind('.')]
    ip_start = min(ipbegint,ipendint)
    ip_end = max(ipbegint,ipendint)
    threads = []
    if ipbegintseg != ipendseg:
        return 
    for ip in range(ip_start,ip_end+1):
        t = threading.Thread(target=ping_rtn,args=(ipbegintseg + '.' + str(ip),iplist))
        t.start()
        threads.append(t)
    for i in range(threads.__len__()):
        threads[i].join()
        
def threadping(ipbegin,ipend,iplist,threadnum = 10):
    ipbegint = int(ipbegin[ipbegin.rfind('.')+1:])
    ipendint = int(ipend[ipend.rfind('.')+1:])
    ipbegintseg = ipbegin[:ipbegin.rfind('.')]
    ipendseg = ipend[:ipend.rfind('.')]
    ip_start = min(ipbegint,ipendint)
    ip_end = max(ipbegint,ipendint)
    threads = []
    if ipbegintseg != ipendseg:
        return 
    for ip in range(ip_start,ip_end+1):
        t = threading.Thread(target=ping_rtn,args=(ipbegintseg + '.' + str(ip),iplist))
        t.start()
        threads.append(t)
        if len(threads) == threadnum:
            for i in range(threadnum):
                threads[i].join()
            threads = []

def threadarpping(iplist,arpdict,threadnum = 10):
    threads = []
    for ip in iplist:
        t = threading.Thread(target=arping_scan,args=(ip,arpdict))
        t.start()
        threads.append(t)
        if len(threads) == threadnum:
            for i in range(threadnum):
                threads[i].join()
            threads = []

def mutiarpping(iplist,arpdict):
    threads = []
    for ip in iplist:
        t = threading.Thread(target=arping_scan,args=(ip,arpdict))
        t.start()
        threads.append(t)
    for i in range(threads.__len__()):
        threads[i].join()
    return 1