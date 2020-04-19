'''
Created on 2019-11-30
@author: Xu
'''

from Utils import PlatformIdentify
if PlatformIdentify.isWindows():
    from twisted.internet import iocpreactor
    iocpreactor.install()
else:
    from twisted.internet import epollreactor
    epollreactor.install()
    
import TwistedLib.ProtocolReactor as ProtocolReactor

if __name__ == '__main__':
    print("Relayer Server starting...")
    ProtocolReactor.Run()