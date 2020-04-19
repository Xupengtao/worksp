from socket import *
from time import ctime

class ServerSocket():
    def __init__(self,Host,Port):
        self.addr       = (Host,Port)
        self.BufSize    = 1024
        self.tcpSerSock = None

    def ListenPort(self):
        self.tcpSerSock = socket(AF_INET, SOCK_STREAM)
        self.tcpSerSock.bind(self.addr)
        self.tcpSerSock.listen(5)
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

    def CloseServer(self):
        self.tcpSerSock.close()