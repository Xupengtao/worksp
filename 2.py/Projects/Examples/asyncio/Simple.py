# -*- coding: utf-8 -*-
"""
Created on Sun Nov 24 01:05:32 2019

@author: wolai
"""
#1、在python3.3之后新增了asyncio模块，可以帮我们检测IO（只能是网络IO），实现应用程序级别的切换
import asyncio
@asyncio.coroutine
def task(task_id,senconds):
    print('%s is start' %task_id)
    yield from asyncio.sleep(senconds) #只能检测网络IO,检测到IO后切换到其他任务执行
    print('%s is end' %task_id)

tasks=[task(task_id="任务1",senconds=3),task("任务2",2),task(task_id="任务3",senconds=1)]

loop=asyncio.get_event_loop()
loop.run_until_complete(asyncio.wait(tasks))
loop.close()

#2、但asyncio模块只能发tcp级别的请求，不能发http协议，因此，在我们需要发送http请求的时候，需要我们自定义http报头
import asyncio
import requests
import uuid
user_agent='Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.221 Safari/537.36 SE 2.X MetaSr 1.0'

def parse_page(host,res):
    print('%s 解析结果 %s' %(host,len(res)))
    with open('%s.html' %(uuid.uuid1()),'wb') as f:
        f.write(res)

@asyncio.coroutine
def get_page(host,port=80,url='/',callback=parse_page,ssl=False):
    print('下载 http://%s:%s%s' %(host,port,url))

    #步骤一（IO阻塞）：发起tcp链接，是阻塞操作，因此需要yield from
    if ssl:
        port=443
    recv,send=yield from asyncio.open_connection(host=host,port=443,ssl=ssl)

    # 步骤二：封装http协议的报头，因为asyncio模块只能封装并发送tcp包，因此这一步需要我们自己封装http协议的包
    request_headers="""GET %s HTTP/1.0\r\nHost: %s\r\nUser-agent: %s\r\n\r\n""" %(url,host,user_agent)
    # requset_headers="""POST %s HTTP/1.0\r\nHost: %s\r\n\r\nname=egon&password=123""" % (url, host,)
    request_headers=request_headers.encode('utf-8')

    # 步骤三（IO阻塞）：发送http请求包
    send.write(request_headers)
    yield from send.drain()

    # 步骤四（IO阻塞）：接收响应头
    while True:
        line=yield from recv.readline()
        if line == b'\r\n':
            break
        print('%s Response headers：%s' %(host,line))

    # 步骤五（IO阻塞）：接收响应体
    text=yield from recv.read()

    # 步骤六：执行回调函数
    callback(host,text)

    # 步骤七：关闭套接字
    send.close() #没有recv.close()方法，因为是四次挥手断链接，双向链接的两端，一端发完数据后执行send.close()另外一端就被动地断开


if __name__ == '__main__':
    tasks=[
        get_page('www.baidu.com',url='/s?wd=美女',ssl=True),
        get_page('www.cnblogs.com',url='/',ssl=True),
    ]

    loop=asyncio.get_event_loop()
    loop.run_until_complete(asyncio.wait(tasks))
    loop.close()

#3、自定义http报头多少有点麻烦，于是有了aiohttp模块，专门帮我们封装http报头，然后我们还需要用asyncio检测IO实现切换
import aiohttp
import asyncio

@asyncio.coroutine
def get_page(url):
    print('GET:%s' %url)
    response=yield from aiohttp.request('GET',url)

    data=yield from response.read()

    print(url,data)
    response.close()
    return 1

tasks=[
    get_page('https://www.python.org/doc'),
    get_page('https://www.cnblogs.com/linhaifeng'),
    get_page('https://www.openstack.org')
]

loop=asyncio.get_event_loop()
results=loop.run_until_complete(asyncio.gather(*tasks))
loop.close()

print('=====>',results) #[1, 1, 1]

#4、此外，还可以将requests.get函数传给asyncio，就能够被检测
import requests
import asyncio

@asyncio.coroutine
def get_page(func,*args):
    print('GET:%s' %args[0])
    loop=asyncio.get_event_loop()
    furture=loop.run_in_executor(None,func,*args)
    response=yield from furture

    print(response.url,len(response.text))
    return 1

tasks=[
    get_page(requests.get,'https://www.python.org/doc'),
    get_page(requests.get,'https://www.cnblogs.com/linhaifeng'),
    get_page(requests.get,'https://www.openstack.org')
]

loop=asyncio.get_event_loop()
results=loop.run_until_complete(asyncio.gather(*tasks))
loop.close()

print('=====>',results) #[1, 1, 1]

#5、还有之前在协程时介绍的gevent模块
from gevent import monkey;monkey.patch_all()
import gevent
import requests

def get_page(url):
    print('GET:%s' %url)
    response=requests.get(url)
    print(url,len(response.text))
    return 1

# g1=gevent.spawn(get_page,'https://www.python.org/doc')
# g2=gevent.spawn(get_page,'https://www.cnblogs.com/linhaifeng')
# g3=gevent.spawn(get_page,'https://www.openstack.org')
# gevent.joinall([g1,g2,g3,])
# print(g1.value,g2.value,g3.value) #拿到返回值


#协程池
from gevent.pool import Pool
pool=Pool(2)
g1=pool.spawn(get_page,'https://www.python.org/doc')
g2=pool.spawn(get_page,'https://www.cnblogs.com/linhaifeng')
g3=pool.spawn(get_page,'https://www.openstack.org')
gevent.joinall([g1,g2,g3,])
print(g1.value,g2.value,g3.value) #拿到返回值

#6、封装了gevent+requests模块的grequests模块
#pip3 install grequests

import grequests

request_list=[
    grequests.get('https://wwww.xxxx.org/doc1'),
    grequests.get('https://www.cnblogs.com/linhaifeng'),
    grequests.get('https://www.openstack.org')
]


##### 执行并获取响应列表 #####
# response_list = grequests.map(request_list)
# print(response_list)

##### 执行并获取响应列表（处理异常） #####
def exception_handler(request, exception):
    # print(request,exception)
    print("%s Request failed" %request.url)

response_list = grequests.map(request_list, exception_handler=exception_handler)
print(response_list)

#7、twisted：是一个网络框架，其中一个功能是发送异步请求，检测IO并自动切换，scrapy框架就是基于twisted编写的 ！！！
'''
#问题一：error: Microsoft Visual C++ 14.0 is required. Get it with "Microsoft Visual C++ Build Tools": http://landinghub.visualstudio.com/visual-cpp-build-tools
https://www.lfd.uci.edu/~gohlke/pythonlibs/#twisted
pip3 install C:\Users\Administrator\Downloads\Twisted-17.9.0-cp36-cp36m-win_amd64.whl
pip3 install twisted

#问题二：ModuleNotFoundError: No module named 'win32api'
https://sourceforge.net/projects/pywin32/files/pywin32/

#问题三：openssl
pip3 install pyopenssl
'''

#twisted基本用法
from twisted.web.client import getPage,defer
from twisted.internet import reactor

def all_done(arg):
    # print(arg)
    reactor.stop()

def callback(res):
    print(res)
    return 1

defer_list=[]
urls=[
    'http://www.baidu.com',
    'http://www.bing.com',
    'https://www.python.org',
]
for url in urls:
    obj=getPage(url.encode('utf=-8'),)
    obj.addCallback(callback)
    defer_list.append(obj)

defer.DeferredList(defer_list).addBoth(all_done)

reactor.run()




#twisted的getPage的详细用法
from twisted.internet import reactor
from twisted.web.client import getPage
import urllib.parse


def one_done(arg):
    print(arg)
    reactor.stop()

post_data = urllib.parse.urlencode({'check_data': 'adf'})
post_data = bytes(post_data, encoding='utf8')
headers = {b'Content-Type': b'application/x-www-form-urlencoded'}
response = getPage(bytes('http://dig.chouti.com/login', encoding='utf8'),
                   method=bytes('POST', encoding='utf8'),
                   postdata=post_data,
                   cookies={},
                   headers=headers)
response.addBoth(one_done)

reactor.run()