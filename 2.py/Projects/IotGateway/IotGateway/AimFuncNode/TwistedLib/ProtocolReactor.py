'''
Created on 2019-11-30
@author: Xu
'''

from twisted.internet.protocol import Protocol,ServerFactory
from twisted.internet import reactor,ssl
from twisted.internet.defer import DeferredLock
import twisted.internet.error as twistedError
import struct
import time
import threading
import logging
logging.basicConfig(filename='example.log',level=logging.INFO,format="%(asctime)s-%(name)s-%(levelname)s-%(message)s")

asscii_string = lambda s: ''.join(map(lambda c: "%02X " % ord(c), s))

class MsgProtocol(Protocol):
   
    connection_count    = 0                                   #连接总数
    countPendingCmd     = 0                                   #正在处理的命令数
    heartbeat           = 1
    count_connection    = 2
    
    def __init__(self):
        self.m_buffer               = ""                                                        #接收缓冲区
        self.lockBuffer             = DeferredLock()
        self.tmActivate             = time.time()                                               #最近活动时间
        self.lock_dictWaitResp      = threading.RLock()
        self.dictControlling        = {}                                                        #控制命令表
        self.cond_dictControlling   = threading.Condition()
        self.timer                  = reactor.callLater(self.heartbeat,self.timeout)            #超时定时器
        self.lockCmd                = threading.RLock()
        self.HeaderTagType          = -1                                                        #-1: 未定义, 0: 无, 1: 有
        self.rcv_alarm              = "False"
        self.role                   = ""
        
    def dataReceived(self, data):                                       #接收数据时调用
        Protocol.dataReceived(self, data) 
        self.m_buffer += data.decode('utf-8')                           #将新到数据放入接收缓冲区   
        self.lockBuffer.acquire().addCallback(self.AddDataAndDecode,self.m_buffer)
        
    def connectionMade(self):                                           #建立连接时调用
        ip=self.transport.getPeer().host
        print(ip)
        with self.factory.lockPendingCmd:
            MsgProtocol.connection_count += 1
            if(MsgProtocol.connection_count > self.count_connection):
                self.transport.loseConnection()
                print("close connection due to reaching connection limit.")

    def connectionLost(self, reason=twistedError.ConnectionDone):       #连接断开时调用
        try:
            self.timer.cancel()                              #取消超时定时器
        except Exception:
            pass
        with self.factory.lockPendingCmd:
            MsgProtocol.connection_count -= 1
        Protocol.connectionLost(self, reason=reason)         #调用基类函数        
        
     
    def RunCommand(self):
        with self.factory.lockPendingCmd:
            MsgProtocol.countPendingCmd=MsgProtocol.countPendingCmd-1
    
    def AddDataAndDecode(self,lock,data):                    #TCP流式分包
        self.tmActivate = time.time()
        print(self.tmActivate)
        print("data received in transport %d : %s (%s)" % (id(self.transport),asscii_string(data),data))
        lock.release()                                      #释放DeferredLock()占用
        
    def timeout(self):                                      #超时后取消
        self.transport.loseConnection()
 
class MsgProtocolFactory(ServerFactory):
    protocol = MsgProtocol
    
    def __init__(self):
        self.dictRelayer    = {}
        self.dictAccounts   = {}
        self.lockDict       = threading.RLock()
        self.SBMP_HEADERTAG = struct.pack("2B",0x01,0xBB)
        self.lockPendingCmd = threading.RLock()

    def GetAccountProtocol(self,relayer_id,client_id):
        with self.lockDict:
            if self.dictAccounts.has_key(relayer_id):
                for clientProtocol in self.dictAccounts[relayer_id]:
                    if clientProtocol.client_id==client_id:
                        return clientProtocol
        return None

class EchoProtocol(Protocol):
    def __init__(self):
        pass

    def dataReceived(self, data):
        Protocol.dataReceived(self, data) 
        print("data received: ", data,",",id(self.transport))
        self.transport.write(data)
        
    def connectionMade(self):
        ip=self.transport.getPeer().host
        print('connectionMade',ip)
    
    def connectionLost(self, reason=None):
        ip=self.transport.getPeer().host
        print('connectionLost',ip)
        
class EchoProtocolFactory(ServerFactory):
    protocol = EchoProtocol
    
    def __init__(self):
        pass

instance_MsgProtocolFactory = None
 
def Run(withListen=True):

    global instance_MsgProtocolFactory
    instance_MsgProtocolFactory = MsgProtocolFactory()

    if withListen:
        logging.info("listenTCP 9630...")
        reactor.listenTCP(9630,instance_MsgProtocolFactory)
        
        cert=None
        with open('server.pem') as keyAndCert:
            cert = ssl.PrivateCertificate.loadPEM(keyAndCert.read())
        reactor.listenSSL(9631, instance_MsgProtocolFactory, cert.options())
        logging.info("listenSSL 9631...")
        reactor.listenTCP(9632, EchoProtocolFactory())
        logging.info("listenTCP 9632...")

    try:
        logging.info("Reactor Running...")
        reactor.run()
    except:
        pass
    try:
        logging.info("Reactor stopping...")
        reactor.stop()
        import sys
        sys.exit(0)
    except:
        pass
    
def MsgProtocolTest():
    pro=MsgProtocolFactory().buildProtocol("127.0.0.1")
    pro.dataReceived("MsgProtocolTest")
    pro.dataReceived("MsgProtocolTest=============================")