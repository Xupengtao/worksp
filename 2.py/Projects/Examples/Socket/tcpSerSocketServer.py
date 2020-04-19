from socketserver import (TCPServer as TCP,StreamRequestHandler as SRH)
from time import ctime
HOST = '192.168.31.246'
PORT = 21567
ADDR = (HOST,PORT)

class MyRequestHandler(SRH):
    def handle(self):
        print('...connected from:',self.client_address)
        sendStr = '[%s] %s' % (ctime().encode('utf-8'),self.rfile.readline())
        sendStr = sendStr.encode('utf-8')
        self.wfile.write(sendStr)

tcpSer = TCP(ADDR,MyRequestHandler)
print('waiting for connection ...')
tcpSer.serve_forever()