'''
Created on 2019-11-30
@author: Xu
'''

from socket import *
import struct
import optparse

Data = None

def parse_args():
    usage = """
Run it like this:
        python ClientSocket.py port1 port2 (-d 213489213) (-s ediksagfja)...
"""
    parser = optparse.OptionParser(usage)
    parser.add_option("-d", "--data",   type="int",    dest="data", help="Send data to Port")
    parser.add_option("-s", "--str",    type="string", dest="str", help="Send str to Port")
    options, addresses = parser.parse_args()
    if not options and not addresses:
        print(parser.format_help())
        exit('ERROR!must support server and port parameters!')
    if not addresses:
        print(parser.format_help())
        parser.exit()
    if options:
        global Data
        if options.data != None:
            Data = options.data
        else:
            Data = options.str

    def parse_address(addr):
        if ':' not in addr:
            host = '127.0.0.1'
            port = addr
        else:
            host, port = addr.split(':', 1)
        if not port.isdigit():
            parser.error('Ports must be integers.')
        return host, int(port)

    return map(parse_address, addresses)

class ClientSocket():
    def __init__(self,Host,Port):
        self.addr       = (Host,Port)
        self.BufSize    = 1024
        self.tcpCliSock = None

    def Connect(self):
        self.tcpCliSock = socket(AF_INET, SOCK_STREAM)
        self.tcpCliSock.connect(self.addr)
    
    def SendData(self,data):
        self.tcpCliSock.send(data)
        data = self.tcpCliSock.recv(self.BufSize)
        if not data:
            return
        print(data)
    
    def CloseConnect(self):
        self.tcpCliSock.close()

if __name__ == '__main__':
    addresses = parse_args()
    CliSockets = [ClientSocket(*addr) for addr in addresses]
    for CliSocket in CliSockets:
        CliSocket.Connect()
        if Data == None:
            CliSocket.SendData(struct.pack('2B',0x01,0x02))
        else:
            if(type(Data) == str):
                Data = Data.encode('utf-8')
                CliSocket.SendData(Data)
            else:
                Data = str(Data).encode('utf-8')
                CliSocket.SendData(Data)
        CliSocket.CloseConnect()