#!/usr/bin/env python

import ZmqSocket
import time

ZmqSock = ZmqSocket._ZmqSocket(ZmqSocket._CommProtocol.ZmqProcess,  #ZmqThread ZmqProcess ZmqTcpIp
                          "ipcexap.ipc",
                          ZmqSocket._ZmqType.ZmqRep,                #ZmqRep-ZmqReq ZmqPub-ZmqSub ZmqPush-ZmqPull
                          ZmqSocket._ZmqBindType.ZmqBind)           #ZmqBind-ZmqConnect

while 1 :
    time.sleep(1)
    ZmqSock.SendMore("aabbcc");
    ZmqSock.SendMore("ddeeff");
    ZmqSock.Send(" ");
    RecvMsg = ZmqSock.Recv()
    print(RecvMsg)

# SendTaskSock = ZmqSocket._ZmqSocket(ZmqSocket._CommProtocol.ZmqTcpIp,
#                           "192.168.1.21:6010",
#                           ZmqSocket._ZmqType.ZmqPush,
#                           ZmqSocket._ZmqBindType.ZmqBind)
# SendBegSock = ZmqSocket._ZmqSocket(ZmqSocket._CommProtocol.ZmqTcpIp,
#                           "192.168.1.21:6020",
#                           ZmqSocket._ZmqType.ZmqPush,
#                           ZmqSocket._ZmqBindType.ZmqConnect)
# SendBegSock.Send("10")                          
# i = 0
# j = 0
# while j < 10 :
#     time.sleep(1)
#     SendTaskSock.Send(str(i) + " " + str(i+1) + " " + str(i+2))
#     i = i+1
#     j = j+1

# RecvRsltSock = ZmqSocket._ZmqSocket(ZmqSocket._CommProtocol.ZmqTcpIp,
#                           "192.168.1.21:6020",
#                           ZmqSocket._ZmqType.ZmqPull,
#                           ZmqSocket._ZmqBindType.ZmqBind)
# RecvMsg = RecvRsltSock.Recv()
# TaskNum = int(RecvMsg)
# j = 0
# while j < TaskNum :
#     RecvMsg = RecvRsltSock.Recv()
#     print(RecvMsg)
#     j = j+1