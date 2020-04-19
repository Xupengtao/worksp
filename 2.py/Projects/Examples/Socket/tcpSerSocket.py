from socket import *
from time import ctime
HOST = '192.168.31.246'
PORT = 21567
BUFSIZE = 1024
ADDR = (HOST,PORT)

tcpSerSock = socket(AF_INET, SOCK_STREAM)
tcpSerSock.bind(ADDR)
tcpSerSock.listen(5)
while True:
    print("waiting for connection...")
    tcpCliSock, addr = tcpSerSock.accept()
    print("...connected from:",addr)
    while True:
        data = tcpCliSock.recv(BUFSIZE)
        if not data:
            break
        sendStr = '[%s] %s' % (ctime().encode('utf-8'),data)
        sendStr = sendStr.encode('utf-8')
        tcpCliSock.send(sendStr)
    tcpCliSock.close()
tcpSerSock.close()