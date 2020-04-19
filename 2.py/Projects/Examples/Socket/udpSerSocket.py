from socket import *
from time import ctime
HOST = '192.168.31.246'
PORT = 21567
BUFSIZE = 1024
ADDR = (HOST,PORT)

udpSerSock = socket(AF_INET, SOCK_DGRAM)
udpSerSock.bind(ADDR)
while True:
    print("waiting for message...")
    data, addr = udpSerSock.recvfrom(BUFSIZE)
    sendStr = '[%s] %s' % (ctime().encode('utf-8'),data)
    sendStr = sendStr.encode('utf-8')
    udpSerSock.sendto(sendStr,addr)
    print("...received from and returned to:",addr)
udpSerSock.close()