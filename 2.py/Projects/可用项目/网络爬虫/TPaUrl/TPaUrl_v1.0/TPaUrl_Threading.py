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
    user_agent = 'Mozilla/4.0 (compatible; MSIE 5.5; Windows NT)'  
    headers = { 'User-Agent' : user_agent }  
    rstr = r"[\/\\\:\*\?\"\<\>\|]"
    charset = r'charset="?(.*?)"+'
    
    def __init__(self,url,urltype,type,htmpath,baseurl,baseurltype,nexttype,nextstr,filetype,filepath,filename,srchtmclass,genLink):
        self.err = 0
        self.url = url
        self.urltype = urltype
        self.urlchasetinit(url,urltype)
        self.type = type
        self.htmpath = htmpath
        self.baseurl = baseurl
        self.baseurltype = baseurltype
        self.nexttype = nexttype
        self.nextstr = nextstr
        self.filetype = filetype
        self.filepath = filepath
        self.filename = filename
        self.folderpathdefault = os.path.join(filepath,'None')
        self.srchtmclass = srchtmclass
        self.genLink = genLink
        self.prefilehref = ''
        self.prelinkhref = ''
        self.threads = []
        self.Links = []
        if not os.path.exists(self.folderpathdefault):
            os.mkdir(self.folderpathdefault)
    
    def PaUrl(self,url,type,htmpath,times = 0):
        if self.genLink == 0:
            if 'open' in type:
                soup = self.Beautifulsoupurl(url,self.urltype)
                if soup != -1:
                    Tags = soup.select(htmpath[type.index('open')])
                    typetemp = type.copy()
                    typetemp.remove('open')
                    htmpathtemp = htmpath.copy()
                    htmpathtemp.remove(htmpathtemp[type.index('open')])
                    for Tag in Tags:
                        if self.baseurltype[0] == 2:
                            self.PaUrl(self.baseurl+Tag.get('href'),typetemp,htmpathtemp,times)
                        else:
                            self.PaUrl(Tag.get('href'),typetemp,htmpathtemp,times)
                    if 'next' in type:
                        if self.nexttype == 0:
                            Tags = soup.select(htmpath[type.index('next')])
                        else:
                            Tags = soup.find_all('a',text=self.nextstr)
                        if self.baseurltype[1] == 2:
                            nexturl = self.baseurl+Tags[0].get('href')
                        else:
                            nexturl = Tags[0].get('href')
                        print(url+' : '+str(times))
                        if times == 0:
                            self.PaUrl(nexturl,type,htmpath,times)
                        elif times > 1:
                            self.PaUrl(nexturl,type,htmpath,times-1)
                        else:
                            return 1
            elif 'down' in type:
                soup = self.Beautifulsoupurl(url,self.urltype)
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
        else:
            if 'open' in type:
                soup = self.Beautifulsoupurl(url,self.urltype)
                if soup != -1:
                    Tags = soup.select(htmpath[type.index('open')])
                    typetemp = type.copy()
                    typetemp.remove('open')
                    htmpathtemp = htmpath.copy()
                    htmpathtemp.remove(htmpathtemp[type.index('open')])
                    for Tag in Tags:
                        linkhref = Tag.get('href')
                        if (linkhref != self.prelinkhref):
                            if self.baseurltype[0] == 2:
                                t = threading.Thread(target=self.PaUrl,args=(self.baseurl+linkhref,typetemp,htmpathtemp,times))
                                t.start()
                                self.threads.append(t)
                            else:
                                t = threading.Thread(target=self.PaUrl,args=(linkhref,typetemp,htmpathtemp,times))
                                t.start()
                                self.threads.append(t)
                            self.prelinkhref = linkhref
                            
                    for i in range(self.threads.__len__()):
                        self.threads[i].join()
                    with open(self.filepath+'/'+ 'url' +'.'+'txt','a') as fp:
                        for i in range(self.Links.__len__()):
                            if self.Links[i] != '':
                                fp.write(self.Links[i]+'\n')
                            else:
                                print(i)
                                break
                    self.threads = []
                    self.Links = []
                    if 'next' in type:
                        if self.nexttype == 0:
                            Tags = soup.select(htmpath[type.index('next')])
                        else:
                            Tags = soup.find_all('a',text=self.nextstr)
                        if self.baseurltype[1] == 2:
                            nexturl = self.baseurl+Tags[0].get('href')
                        else:
                            nexturl = Tags[0].get('href')
                        print(url+' : '+str(times))
                        if times == 0:
                            self.PaUrl(nexturl,type,htmpath,times)
                        elif times > 1:
                            self.PaUrl(nexturl,type,htmpath,times-1)
                        else:
                            return 1
            elif 'down' in type:
                soup = self.Beautifulsoupurl(url,self.urltype)
                if soup != -1:
                    folderpath = self.foldernamecreate(soup,self.filepath)
                    Tags = soup.select(htmpath[type.index('down')])
                    if not os.path.exists(self.filepath):
                        os.mkdir(self.filepath)
                    self.UrlListDown(Tags,self.filepath)
        
    def urlchasetinit(self,url,type = 'post'):
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
                    
    def UrlListDown(self,Tags,folderpath):
        if self.genLink == 0:
            i = 1
            for Tag in Tags:
                filehref = Tag.get(self.srchtmclass)
                if (filehref == None) & (self.filetype != 'jpg'):
                    filehref = Tag.text
                if (filehref != None) & (filehref != self.prefilehref) & (self.filetype == 'jpg'):
                    if self.filename == 'auto':
                        self.reqdownsrc(Tag.get(self.srchtmclass),
                                        folderpath+'/'+str(i)+'.'+self.filetype)
                        i = i+1
                    elif self.filename == 'id':
                        self.reqdownsrc(Tag.get(self.srchtmclass),
                                        folderpath+'/'+Tag.get('id')
                         +'.'+self.filetype)
                else:
                    if self.filename == 'auto':
                        self.reqdownsrc(Tag.text,
                                        folderpath+'/'+str(i)+'.'+self.filetype)
                        i = i+1
                    elif self.filename == 'id':
                        self.reqdownsrc(Tag.text,
                                        folderpath+'/'+Tag.get('id')
                         +'.'+self.filetype)
                self.prefilehref = filehref
        else:
            for Tag in Tags:
                filehref = Tag.text
                if (self.filetype != 'jpg'):
                    self.Links.append(filehref)
                        
    def Beautifulsoupurl(self,url,type = 'post'):
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
    
    def reqdownsrc(self,url,file_name,type = 'get'):
        if type == 'get':
            try:
                r = requests.get(url,headers = self.headers) 
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