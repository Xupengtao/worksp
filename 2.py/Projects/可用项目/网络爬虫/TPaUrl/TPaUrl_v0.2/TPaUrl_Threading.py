# -*- coding: utf-8 -*-
"""
Created on Wed May 30 21:51:00 2018

@author: wolai
"""

import urllib
import requests,os
import re
from bs4 import BeautifulSoup
import threading 
  
class TPaUrl:
    rstr = r"[\/\\\:\*\?\"\<\>\|]"
    def __init__(self,url,type,htmpath,baseurl='',*,filetype,filepath,filename,srchtmclass):
        self.err = 0
        self.url = url
        self.type = type
        self.htmpath = htmpath
        self.baseurl = baseurl
        self.filetype = filetype
        self.filepath = filepath
        self.filename = filename
        self.folderpathdefault = os.path.join(filepath,'None')
        self.srchtmclass = srchtmclass
        if not os.path.exists(self.folderpathdefault):
            os.mkdir(self.folderpathdefault)
    
    def PaUrl(self,url,type,htmpath,times = 0):
        if 'open' in type:
            soup = self.Beautifulsoupurl(url)
            if soup != -1:
                Tags = soup.select(htmpath[type.index('open')])
                typetemp = type.copy()
                typetemp.remove('open')
                htmpathtemp = htmpath.copy()
                htmpathtemp.remove(htmpathtemp[type.index('open')])
                for Tag in Tags:
                    self.PaUrl(self.baseurl+Tag.get('href'),typetemp,htmpathtemp,times)
                if 'next' in type:
                    Tags = soup.select(htmpath[type.index('next')])
                    print(url + str(times))
                    if times == 0:
                        self.PaUrl(self.baseurl+Tags[0].get('href'),type,htmpath,times)
                    elif times > 1:
                        self.PaUrl(self.baseurl+Tags[0].get('href'),type,htmpath,times-1)
                    else:
                        return 1
                    
        elif 'down' in type:
            soup = self.Beautifulsoupurl(url)
            if soup != -1:
                folderpath = self.foldernamecreate(soup,self.filepath)
                Tags = soup.select(htmpath[type.index('down')])
                if not os.path.exists(folderpath):
                    os.mkdir(folderpath)
                    t = threading.Thread(target=self.UrlListDown,args=(Tags,folderpath))
                    t.start()
                elif folderpath == self.folderpathdefault:
                    t = threading.Thread(target=self.UrlListDown,args=(Tags,folderpath))
                    t.start()
                    
    def UrlListDown(self,Tags,folderpath):
        i = 1
        prefilehref = ''
        for Tag in Tags:
            filehref = Tag.get(self.srchtmclass)
            if (filehref != None) & (filehref != prefilehref):
                if self.filename == 'auto':
                    self.reqdownsrc(Tag.get(self.srchtmclass),
                                    folderpath+'/'+str(i)+'.'+self.filetype)
                    i = i+1
                elif self.filename == 'id':
                    self.reqdownsrc(Tag.get(self.srchtmclass),
                                    folderpath+'/'+Tag.get('id')
                     +'.'+self.filetype)
                prefilehref = filehref
                        
    def Beautifulsoupurl(self,url,type = 'post'):
        if type == 'post':
            try:
                response = requests.post(url)
                soup = BeautifulSoup(response.text,'lxml')
            except:
                soup = -1
            return soup
    
    def reqdownsrc(self,url,file_name,type = 'get'):
        if type == 'get':
            try:
                r = requests.get(url) 
                with open(file_name, "wb") as code:
                    code.write(r.content)
            except:
                return -1
    
    def urllibdownsrc(self,url,file_name):
        binary_data = urllib.request.urlopen(url).read()
        temp_file = open(file_name, 'wb')
        temp_file.write(binary_data)
        temp_file.close()
        
    def foldernamecreate(self,soup,filepath):
        try:
            foldername = soup.title.string
            foldername = re.sub(self.rstr,"",foldername)
            folderpath = os.path.join(filepath,foldername)
        except:
            folderpath = self.folderpathdefault
        finally:
            return folderpath
    
    def run(self,times = 0):
        self.PaUrl(self.url,self.type,self.htmpath,times)
    
    def show(self):
        print(self.url)
        print(self.type)
        print(self.htmpath)
   
###################################################################################
     
#url = ['http://taohuabt.cc/forum-42-1.html',
#       'http://taohuabt.cc/forum-56-1.html',
#       'http://taohuabt.cc/forum-57-1.html',
#       'http://taohuabt.cc/forum-221-100.html',
#       'http://taohuabt.cc/forum-239-1.html']
#type = ['open','down','next']
#htmpath = ['ul#waterfall li div a[class="z"]','td[class="t_f"] img','div#pgbtn a']
#baseurl = 'http://taohuabt.cc/'
#filetype = 'jpg'
#filepath = ['E:\PyDownTest\网上自拍',
#            'E:\PyDownTest\人体写真',
#            'E:\PyDownTest\街头抓拍',
#            'E:\PyDownTest\欧美图片',
#            'E:\PyDownTest\AV美图']
#filename = 'auto'
#srchtmclass = 'file'
#times = [977,1078,7,1600,226]
#
#for i in range(5):
#    Tp = TPaUrl(url[i],type,htmpath,baseurl,filetype = filetype,filepath = filepath[i],
#            filename = filename,srchtmclass = srchtmclass)
#    t = threading.Thread(target=Tp.run,args=(times[i],))
#    t.start()