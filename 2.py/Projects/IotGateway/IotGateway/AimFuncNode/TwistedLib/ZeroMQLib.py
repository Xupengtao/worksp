'''
Created on 2019-11-30
@author: Xu
'''

import zmq                                          #ZeroMQ包

context = zmq.Context()                             #初始化zmq上下文
socketSubscribe = context.socket(zmq.SUB)           #Subscriber对象
socketPublish=context.socket(zmq.PUB)               #Publisher对象

PORT_PUBSUB=5557

#将Subscriber连接到集群中的所有服务器地址，用来截收消息
socketSubscribe.connect("tcp://%s:%d" % (server1, PORT_PUBSUB))
socketSubscribe.connect("tcp://%s:%d" % (server2, PORT_PUBSUB))
socketSubscribe.connect("tcp://%s:%d" % (server3, PORT_PUBSUB))

#用Publisher绑定到本地端口，用来发送消息
socketPublish.bind("tcp://＊:%d" % (PORT_PUBSUB))

def ProcessMessage(head, from, body):
#此处省略处理逻辑，在需要时可调用如下函数向其他服务器发送消息
    socketPublisher.send_multipart(hear, from, body)
    while True:
    try:
        [head, from_filter, body]=socketSubscribe.recv_multipart()     #接收消息
        threads.deferToThread(ProcessMessage, head, from_filter, body)  #处理消息
    except:
        pass