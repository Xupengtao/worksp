'''
Created on 2019-11-30
@author: Xu
'''

from twisted.internet.protocol import Protocol,ServerFactory
from twisted.internet import reactor,threads,ssl
from twisted.internet.defer import DeferredLock
import twisted.internet.error as twistedError
import struct
import Command
import logging,time
from DB import SBDB
import threading
from Utils import Util,Config
from SBPS import InternalMessage

class SBProtocol(Protocol):
   
    connection_count=0                                                      #连接总数
    countPendingCmd=0                                                       #正在处理的命令数
    
    def __init__(self):
        self.m_buffer               = ""                                                        #接收缓冲区
        self.lockBuffer             = DeferredLock()
        self.tmActivate             = time.time()                                               #最近活动时间
        self.dictWaitResp           = {}                                                        #待反馈命令表
        self.lock_dictWaitResp      = threading.RLock()
        self.dictControlling        = {}                                                        #控制命令表
        self.cond_dictControlling   = threading.Condition()
        self.timer                  = reactor.callLater(Config.time_heartbeat,self.timeout)     #超时定时器
        self.lockCmd                = threading.RLock()
        self.HeaderTagType          = -1                                                        #-1: 未定义, 0: 无, 1: 有
        self.rcv_alarm              = "False"
        self.role                   = ""
        
    def dataReceived(self, data):                                       #接收数据时调用
        Protocol.dataReceived(self, data)    
        self.lockBuffer.acquire().addCallback(self.AddDataAndDecode,data)
        
    def connectionMade(self):                                           #建立连接时调用
        ip=self.transport.getPeer().host
        print(ip)
        with self.factory.lockPendingCmd:
            SBProtocol.connection_count += 1
            if(SBProtocol.connection_count > Config.count_connection):
                self.transport.loseConnection()
                print("close connection due to reaching connection limit.")

    def connectionLost(self, reason=twistedError.ConnectionDone):       #连接断开时调用
        try:
            self.timer.cancel()                               #取消超时定时器
        except Exception:
            pass
        self.releaseFromDict()                               #取消保存SBProtocol对象
        with self.factory.lockPendingCmd:
            SBProtocol.connection_count -= 1
        Protocol.connectionLost(self, reason=reason)          #调用基类函数        
        
     
    def RunCommand(self,command):
        with self.factory.lockPendingCmd:
            SBProtocol.countPendingCmd=SBProtocol.countPendingCmd-1
        command.Run()
        
    def AddDataAndDecode(self,lock,data):                      #TCP流式分包
        print("data received in transport %d : %s (%s)" % (id(self.transport),Util.asscii_string(data),data))
        self.m_buffer+=data                                                         #将新到数据放入接收缓冲区
        while len(self.m_buffer)>=Command.BaseCommand.CBaseCommand.HEAD_LEN:        #如果已经收到了完整的消息头，则尝试解包
            self.m_buffer,command,=self.Decode(self.m_buffer)
            if command == None:
                break
            if SBProtocol.countPendingCmd < Config.count_connection/100:
                threads.deferToThread(self.RunCommand,command) 
                with self.factory.lockPendingCmd:
                    SBProtocol.countPendingCmd=SBProtocol.countPendingCmd+1  
            else:
                try:                    
                    cmd_resp=command.GetResp()
                    cmd_resp.SetErrorCode(Command.BaseCommand.CS_SERVERBUSY)
                    cmd_resp.Send()
                except:
                    pass          
        lock.release()                                                              #释放DeferredLock()占用
        
    def timeout(self):                                      #超时后取消
        if self.role!=Command.BaseCommand.PV_ROLE_INTERNAL:
            self.transport.loseConnection()
    
    def releaseFromDict(self):                              #取消在factory中保存的SBProtocol对象
        with self.factory.lockDict:
            if 'role' not in dir(self): return
            if self.role == Command.BaseCommand.PV_ROLE_RELAYER:
                if self.factory.dictRelayer.has_key(self.relayer_id):
                    if self.factory.dictRelayer[self.relayer_id]==self:
                        self.factory.dictRelayer.pop(self.relayer_id)
            elif self.role == Command.BaseCommand.PV_ROLE_HUMAN:
                for relayerId in SBDB.GetRelayerIDsByAccountId(self.account_id):
                    if self.factory.dictAccounts.has_key(relayerId):
                        listAccount=self.factory.dictAccounts[relayerId]
                        if self in listAccount:
                            listAccount.remove(self)
                            if len(listAccount)<=0:
                                self.factory.dictAccounts.pop(relayerId)
 
class SBProtocolFactory(ServerFactory):
    protocol = SBProtocol
    
    def __init__(self):
        self.lockDict=DeferredLock()
        self.dictRelayer={}                     #key:relayerid,value:SBProtocol
        self.dictAccounts={}                    #key:relayerid,value:array of SBProtocol
        self.lockDict=threading.RLock()
        self.SBMP_HEADERTAG=struct.pack("2B",0x01,0xBB)
        
        self.lockPendingCmd=threading.RLock()

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
    
    def connectionLost(self, reason=None):
        ip=self.transport.getPeer().host
        
class EchoProtocolFactory(ServerFactory):
    protocol = EchoProtocol
    
    def __init__(self):
        pass

instance_SBProtocolFactory=None   
bReactorStopped=False
 
def Run(withListen=True):
    
    global instance_SBProtocolFactory,bReactorStopped
    instance_SBProtocolFactory=SBProtocolFactory()
    
    InternalMessage.protocolInternal=SBProtocol()
    InternalMessage.protocolInternal.role="internal"
    InternalMessage.protocolInternal.factory=instance_SBProtocolFactory
    threading.Thread(target=InternalMessage.Run).start()

    if withListen:
        reactor.listenTCP(9630,instance_SBProtocolFactory) #@UndefinedVariable
        
        cert=None
        with open('server.pem') as keyAndCert:
            cert = ssl.PrivateCertificate.loadPEM(keyAndCert.read())
        reactor.listenSSL(9631, instance_SBProtocolFactory, cert.options())
        reactor.listenTCP(9632,EchoProtocolFactory()) #@UndefinedVariable

    try:
        reactor.run() #@UndefinedVariable
    except:
        pass
    InternalMessage.Stop()
    bReactorStopped=True
    try:
        #reactor.stop()
        import sys
        sys.exit(0)
    except:
        pass
    
if __name__ == '__main__':
    pro=SBProtocolFactory().buildProtocol("127.0.0.1")
    pro.dataReceived("abc__")
    pro.dataReceived("abc=============================")
    print type(12)(34)
