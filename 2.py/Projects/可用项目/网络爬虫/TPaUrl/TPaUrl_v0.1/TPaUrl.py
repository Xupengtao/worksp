# -*- coding: utf-8 -*-
"""
Created on Sun May 27 16:49:02 2018

@author: wolai
"""

import urllib
import requests,os
import re
from bs4 import BeautifulSoup

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
        self.srchtmclass = srchtmclass
    
    def PaUrl(self,url,type,htmpath,baseurl='',*,filetype,filepath,filename):
        if baseurl == '':
            baseurl = url
        if 'open' in type:
            soup = self.Beautifulsoupurl(url)
            if soup != -1:
                Tags = soup.select(htmpath[type.index('open')])
                typetemp = type.copy()
                typetemp.remove('open')
                htmpathtemp = htmpath.copy()
                htmpathtemp.remove(htmpathtemp[type.index('open')])
                for Tag in Tags:
                    self.PaUrl(baseurl+Tag.get('href'),typetemp,htmpathtemp,baseurl,
                               filetype = filetype,filepath = filepath,filename = filename)
                if 'next' in type:
                    Tags = soup.select(htmpath[type.index('next')])
                    print(Tags[0].get('href'))
                    self.PaUrl(baseurl+Tags[0].get('href'),type,htmpath,baseurl,
                               filetype = filetype,filepath = filepath,filename = filename)
        elif 'down' in type:
            soup = self.Beautifulsoupurl(url)
            if soup != -1:
                folderpath = self.foldernamecreate(soup,filepath)
                Tags = soup.select(htmpath[type.index('down')])
                if not os.path.exists(folderpath):
                    os.mkdir(folderpath)
                    i = 1
                    prefilehref = ''
                    for Tag in Tags:
                        filehref = Tag.get(self.srchtmclass)
                        if (filehref != None) & (filehref != prefilehref):
                            if filename == 'auto':
                                self.reqdownsrc(Tag.get(self.srchtmclass),
                                                folderpath+'/'+str(i)+'.'+filetype)
                                i = i+1
                            elif filename == 'id':
                                self.reqdownsrc(Tag.get(self.srchtmclass),
                                                folderpath+'/'+Tag.get('id')
                                 +'.'+filetype)
                            prefilehref = filehref
                        
    def Beautifulsoupurl(self,url,type = 'post'):
        if type == 'post':
            try:
                response = requests.post(url)
                soup = BeautifulSoup(response.text,'lxml')
            except:
                print(url+'can`t open')
                soup = -1
            return soup
    
    def reqdownsrc(self,url,file_name,type = 'get'):
        if type == 'get':
            try:
                r = requests.get(url) 
                with open(file_name, "wb") as code:
                    code.write(r.content)
            except:
                print(url+'can`t down')
    
    def urllibdownsrc(self,url,file_name):
        binary_data = urllib.request.urlopen(url).read()
        temp_file = open(file_name, 'wb')
        temp_file.write(binary_data)
        temp_file.close()
        
    def foldernamecreate(self,soup,filepath):
        foldername = soup.title.string
        foldername = re.sub(self.rstr,"",foldername)
        folderpath = os.path.join(filepath,foldername)
        return folderpath
    
    def run(self):
        self.PaUrl(self.url,self.type,self.htmpath,self.baseurl,filetype = self.filetype,
                   filepath = self.filepath,filename = self.filename)
    
    def show(self):
        print(self.url)
        print(self.type)
        print(self.htmpath)
    
###################################################################################
          
url = 'http://taohuabt.cc/forum-56-7.html'
type = ['open','down','next']
htmpath = ['ul#waterfall li div a[class="z"]','td[class="t_f"] img','div#pgbtn a']
baseurl = 'http://taohuabt.cc/'
filetype = 'jpg'
filepath = 'E:\PyDownTest'
filename = 'auto'
srchtmclass = 'file'

Tp = TPaUrl(url,type,htmpath,baseurl,filetype = filetype,filepath = filepath,
            filename = filename,srchtmclass = srchtmclass)