# -*- coding: utf-8 -*-
"""
Created on Wed Dec 26 22:29:13 2018

@author: wolai
"""

import requests
import re
from bs4 import BeautifulSoup
import threading
from Mongo.sTon_mongodb import sTon_mongodb

class TPaUrl_Sql:
    user_agent = 'Mozilla/4.0 (compatible; MSIE 5.5; Windows NT)'  
    headers = { 'User-Agent' : user_agent }  
    rstr = r"[\/\\\:\*\?\"\<\>\|]"
    charset = r'charset="?(.*?)"+'
    def __init__(self,url,urltype,type,htmpath,baseurl,baseurltype,nexttype,nextstr,db_name,collection):        #构造函数
        self.err = 0
        self.url = url
        self.urltype = urltype                                                                                      #url操作类型，默认为post
        self.urlchasetinit(url,urltype)
        self.type = type                                                                                            #type列表，open在前，record在后
        self.htmpath = htmpath                                                                                      #html路径列表，与type一一对应
        self.baseurl = baseurl                                                                                      #基地址url
        self.baseurltype = baseurltype                                                                              #基地址类型，2表示需要基地址
        self.nexttype = nexttype                                                                                    #下一页类型，为0则执行路径匹配，否则执行字符串搜索
        self.nextstr = nextstr                                                                                      #下一页字符串
        self.threads = []                                                                                           #线程列表
        self.mongodb = sTon_mongodb()                                                                               #连接mongodb
        self.mongodb.change_database(db_name)                                                                       #数据库db_name
        self.collection = collection                                                                                #写入collection名，为NULL则以第一个Record命名
    
    def urlchasetinit(self,url,type = 'post'):                                                                  #识别url编码
        charsetpattern = re.compile(self.charset,re.S)
        if type == 'post':
            try:
                response = requests.post(url,headers = self.headers)
                urlchasets = re.findall(charsetpattern,response.text)
                self.urlchaset = urlchasets[0]
            except:
                self.urlchaset = 'utf-8'
        elif type == 'get':
            try:
                response = requests.get(url,headers = self.headers)
                urlchasets = re.findall(charsetpattern,response.text)
                self.urlchaset = urlchasets[0]
            except:
                self.urlchaset = 'utf-8'
 
    def PaUrl(self,url,type,htmpath,modbkey,times = 0,
              threadtype = 0,collection = '',inserttype = 'page',recorddict = {}):                              #爬虫主程序
        soup = self.Beautifulsoupurl(url,self.urltype)
        if soup != -1: 
            if 'open' in type:                                                                                      #open：打开（循环）不录入数据库#open和next的深层绑定，使next的格式只可以运行一次#所以要么多层open，要么一层open+next 
                Tags = soup.select(htmpath[type.index('open')])
                typetemp = type.copy()
                typetemp.remove('open')
                htmpathtemp = htmpath.copy()
                htmpathtemp.remove(htmpathtemp[type.index('open')])
                for Tag in Tags:
                    if self.baseurltype[0] == 2:                                                                        #2表示需要基地址
                        self.PaUrl(self.baseurl+Tag.get('href'),typetemp.copy(),htmpathtemp.copy(),modbkey,times)       #modbkey表示写入数据库的dict的key值
                    else:
                        self.PaUrl(Tag.get('href'),typetemp.copy(),htmpathtemp.copy(),modbkey,times)
                if 'next' in type:
                    if self.nexttype == 0:                                                                              #0表示使用html路径搜索下一页
                        Tags = soup.select(htmpath[type.index('next')])
                    else:                                                                                               #表示使用下一页字符串
                        Tags = soup.find_all('a',text=self.nextstr)
                    if self.baseurltype[1] == 2:                                                                        #2表示需要基地址
                        nexturl = self.baseurl+Tags[0].get('href')
                    else:
                        nexturl = Tags[0].get('href')
                    print(url+' : '+str(times))
                    if times == 0:
                        self.PaUrl(nexturl,type,htmpath,modbkey,-1)                                                     #-1表示执行最后一次
                    elif times > 1:
                        self.PaUrl(nexturl,type,htmpath,modbkey,times-1)
            elif 'record' in type:                                                                                  #record：打开（循环）并录入recorddict，key值为对应modbkey
                Tags = soup.select(htmpath[type.index('record')])
                typetemp = type.copy()
                typetemp.remove('record')
                htmpathtemp = htmpath.copy()
                htmpathtemp.remove(htmpathtemp[type.index('record')])
                dictkey = modbkey[0]
                modbkeytemp = modbkey.copy()
                if threadtype == 0:                                                                                     #为0起线程
                    if (self.collection != '') | (collection != ''):  
                        modbkeytemp.remove(modbkeytemp[0])
#                    collectionlist = []                                                                                 #Debug
                    for Tag in Tags:
                        recorddicttemp = recorddict.copy()
                        if (self.collection == '')&(collection == ''):                                                  #使用当前url的text为collection名起线程
                            collectiontemp = Tag.text                                                                       #字符串类型不需要copy
                        elif self.collection != '':                                                                     #写入当前url的text至recorddict
                            collectiontemp = self.collection
                            recorddicttemp[dictkey] = Tag.text
                        else:
                            recorddicttemp[dictkey] = Tag.text
#                        collectionlist.append(collectiontemp)                                                           #Debug
                        if self.baseurltype[0] == 2:                                                                    #2表示需要基地址
                            nexturl = self.baseurl+Tag.get('href')
                            t = threading.Thread(target=self.PaUrl,args=(nexturl,typetemp.copy()                        #起线程并加入线程列表
                                                                         ,htmpathtemp.copy()
                                                                         ,modbkeytemp.copy(),times,1
                                                                         ,collectiontemp,inserttype
                                                                         ,recorddicttemp.copy()))
                            t.start()
                            self.threads.append(t)
                        else:
                            nexturl = Tag.get('href')
#                            self.PaUrl(nexturl,typetemp.copy(),htmpathtemp.copy()                                       #单线程运行
#                                       ,modbkeytemp.copy(),times,1,collectiontemp
#                                       ,inserttype,recorddicttemp.copy())
                            t = threading.Thread(target=self.PaUrl,args=(nexturl,typetemp.copy()                        #起线程并加入线程列表
                                                                         ,htmpathtemp.copy()
                                                                         ,modbkeytemp.copy(),times,1
                                                                         ,collectiontemp,inserttype
                                                                         ,recorddicttemp.copy()))
                            t.start()
                            self.threads.append(t)
                    for i in range(self.threads.__len__()):
                        self.threads[i].join()
                    print('self.threads = ',self.threads.__len__())                                                     #Debug
#                    print(collectionlist)                                                                               #Debug
                else:                                                                                                   #为1写入recorddict并打开（循环）
                    modbkeytemp.remove(modbkeytemp[0])
                    for Tag in Tags:
                        recorddicttemp = recorddict.copy()
                        recorddicttemp[dictkey] = Tag.text
                        if self.baseurltype[0] == 2:                                                                    #2表示需要基地址
                            nexturl = self.baseurl+Tag.get('href')
                            self.PaUrl(nexturl,typetemp.copy(),htmpathtemp.copy(),modbkeytemp.copy(),times,1
                                       ,collection,inserttype,recorddicttemp.copy())
                        else:
                            nexturl = Tag.get('href')
                            self.PaUrl(nexturl,typetemp.copy(),htmpathtemp.copy(),modbkeytemp.copy(),times,1
                                       ,collection,inserttype,recorddicttemp.copy())
            elif ('write' in type) | ('key' in type):                                                               #write：录入recorddict，key值为对应modbkey,此时url与insert相同
                if inserttype == 'page':
                    self.mongoUrlPageInsert(soup,type,htmpath,collection,modbkey,recorddict)                        #key：按页录入数据库，key值为对应modbkey
                else:
                    self.mongoUrlInsert(soup,type,htmpath,collection,modbkey,recorddict)                            #key：按条录入数据库，key值为对应modbkey
            return 1
        else:
            return 0
        
    def mongoUrlInsert(self,soup,type,htmpath,collection,modbkey,recorddict):                                   #按页写入数据库 一个key-value或write一个字典对内容
        writecount = 0
        keycount = 0
        valuecount = 0
        if 'write' in type:
            writecount = type.count('write')
        if 'key' in type:
            keycount = type.count('key')
        if 'value' in type:
            valuecount = type.count('value')
        if keycount != valuecount:
            print('key-value don`t mate!')
            return 0
        if writecount != 0:
            Tags = soup.select(htmpath[type.index('write')])
            urldictlen = len(Tags)                                                                                  #写入字典列表长度
            urldictlist = [recorddict.copy() for i in range(urldictlen)]                                                 #初始化写入字典列表
        else:
            Tags = soup.select(htmpath[type.index('key')])
            urldictlen = len(Tags)
            urldictlist = [recorddict.copy() for i in range(urldictlen)]
        if len(modbkey) < writecount:
            writecount = len(modbkey)
        for i in range(writecount):
            Tags = soup.select(htmpath[type.index('write')])
            htmpath.remove(htmpath[type.index('write')])
            type.remove('write')
            dictkey = modbkey[0]
            modbkey.remove(modbkey[0])
            Tagslen = len(Tags)
            if Tagslen > urldictlen:
                Tagslen = urldictlen
            for j in range(Tagslen):
                urldictlist[j][dictkey] = Tags[j].text
        for i in range(keycount):
            keyTags = soup.select(htmpath[type.index('key')])
            valueTags = soup.select(htmpath[type.index('value')])
            htmpath.remove(htmpath[type.index('key')])
            type.remove('key')
            htmpath.remove(htmpath[type.index('value')])
            type.remove('value')
            keyTagslen = len(keyTags)
            valueTagslen = len(valueTags)
            if keyTagslen > urldictlen:
                keyTagslen = urldictlen
            if keyTagslen > valueTagslen:
                keyTagslen = valueTagslen
            for j in range(keyTagslen):
                urldictlist[j][keyTags[j].text] = valueTags[j].text
        if urldictlen != 0:
            self.mongodb.insert_many(collection,urldictlist)
        else:
            print('urldictlen = 0')
        return 1
            
    def mongoUrlPageInsert(self,soup,type,htmpath,collection,modbkey,recorddict):                                   #按页写入数据库 一页一个字典对内容
        writecount = 0
        keycount = 0
        valuecount = 0
        if 'write' in type:
            writecount = type.count('write')
        if 'key' in type:
            keycount = type.count('key')
        if 'value' in type:
            valuecount = type.count('value')
        if keycount != valuecount:
            print('key-value don`t mate!')
            return 0
        if writecount != 0:
            Tags = soup.select(htmpath[type.index('write')])
            urldictlen = len(Tags)                                                                                  #写入字典列表长度
            urldict = recorddict.copy()                                                                                 #初始化写入字典
        else:
            Tags = soup.select(htmpath[type.index('key')])
            urldictlen = len(Tags)
            urldict = recorddict.copy()
        if len(modbkey) < writecount:
            writecount = len(modbkey)
        for i in range(writecount):
            Tags = soup.select(htmpath[type.index('write')])
            htmpath.remove(htmpath[type.index('write')])
            type.remove('write')
            dictkey = modbkey[0]
            modbkey.remove(modbkey[0])
            Tagslen = len(Tags)
            if Tagslen > urldictlen:
                Tagslen = urldictlen
            for j in range(Tagslen):
                urldict[dictkey] = Tags[j].text
        for i in range(keycount):
            keyTags = soup.select(htmpath[type.index('key')])
            valueTags = soup.select(htmpath[type.index('value')])
            htmpath.remove(htmpath[type.index('key')])
            type.remove('key')
            htmpath.remove(htmpath[type.index('value')])
            type.remove('value')
            keyTagslen = len(keyTags)
            valueTagslen = len(valueTags)
            if keyTagslen > urldictlen:
                keyTagslen = urldictlen
            if keyTagslen > valueTagslen:
                keyTagslen = valueTagslen
            for j in range(keyTagslen):
                urldict[keyTags[j].text] = valueTags[j].text
        if urldictlen != 0:
            self.mongodb.insert(collection,urldict)
            self.mongodb.save(collection,urldict)
        else:
            print('urldictlen = 0')                                                                                 #Debug
        return 1
                                
    def Beautifulsoupurl(self,url,type = 'post'):                                                               #获取urlhtml
        if type == 'post':
            try:
                response = requests.post(url,headers = self.headers)
                response.encoding = self.urlchaset
                soup = BeautifulSoup(response.text,'lxml')
            except:
                soup = -1
            return soup
        elif type == 'get':
            try:
                response = requests.get(url,headers = self.headers)
                response.encoding = self.urlchaset
                soup = BeautifulSoup(response.text,'lxml')
            except:
                soup = -1
            return soup
    
    def run(self,modbkey,inserttype,times = 0):
        self.PaUrl(self.url,self.type,self.htmpath,modbkey,times,0,self.collection,inserttype)