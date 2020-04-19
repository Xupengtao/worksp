from twisted.internet import protocol,reactor
from time import ctime
PORT = 21567

class TSServProtocol(protocol.Protocol):
    def connectionMade(self):
        clnt = self.clnt = self.transport.getPeer().host
        print('...connected from:',clnt)
    def dataReceived(self,data):
        sendStr = '[%s] %s' % (ctime().encode('utf-8'),data)
        sendStr = sendStr.encode('utf-8')
        self.transport.write(sendStr)

factory = protocol.Factory()
factory.protocol = TSServProtocol
print('waiting for connection...')
reactor.listenTCP(PORT,factory)
reactor.run()