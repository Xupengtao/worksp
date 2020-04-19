from socket import *

HOST = '192.168.31.246'
PORT = 21567
BUFSIZE = 1024
ADDR = (HOST,PORT)
udpCliSock = socket(AF_INET,SOCK_DGRAM)
while True:
    data = input('> ')
    if not data:
        break
    udpCliSock.sendto(data,ADDR)
    data,ADDR = udpCliSock.recvfrom(BUFSIZE)
    if not data:
        break
    print(data.decode('utf-8'))
udpCliSock.close()