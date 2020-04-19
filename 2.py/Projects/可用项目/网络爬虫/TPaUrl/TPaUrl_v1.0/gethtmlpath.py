# -*- coding: utf-8 -*-
"""
Created on Mon May 28 21:05:44 2018

@author: wolai
"""
import requests
import re
from bs4 import BeautifulSoup
import bs4

def gethtmlpath(url,str,classstr = '',depth = 5):
    pathList = []
    htmlpath = ''
    bssoup = Bs4soup(url)
    gethtmlpathList(bssoup,str,pathList,classstr)
    if len(pathList) < depth:
        length = len(pathList)
    else:
        length = depth
        pathList = pathList[len(pathList)-length:]
    for i in range(length):
        htmlpath = htmlpath + pathList[i] + ' '
    htmlpath = htmlpath[:-1]
    return htmlpath
    
def gethtmlpathList(soup,str,pathList = [],classstr = ''):
    if classstr != '':
        soup = soup.find(class_ = classstr)
        name = soup.name
        attrs = soup.attrs
        if (' ' not in attrs['class']) & (len(attrs['class']) == 1):
            name = name + '[class="' + attrs['class'][0] + '"]'
        pathList.append(name)
    for bschild in soup.contents:
        if isinstance(bschild,(bs4.element.Tag)):
            if bschild.text != None: 
                if str in bschild.text:
                    name = bschild.name
                    attrs = bschild.attrs
                    if 'class' in attrs.keys():
                        if (' ' not in attrs['class']) & (len(attrs['class']) == 1):
                            name = name + '[class="' + attrs['class'][0] + '"]'
                    pathList.append(name)
                    gethtmlpathList(bschild,str,pathList)
                    break
    
def Bs4soup(url,type = 'post'): 
    user_agent = 'Mozilla/4.0 (compatible; MSIE 5.5; Windows NT)'  
    headers = { 'User-Agent' : user_agent }  
    encode = urlchasetinit(url,headers,type)
    if type == 'post':
        try:
            response = requests.post(url,headers = headers)
            response.encoding = encode
            soup = BeautifulSoup(response.text,'lxml')
        except:
            soup = -1
        return soup
    elif type == 'get':
        try:
            response = requests.get(url,headers = headers)
            response.encoding = encode
            soup = BeautifulSoup(response.text,'lxml')
        except:
            soup = -1
        return soup
        
def urlchasetinit(url,headers,type = 'post'):
    charset = r'charset="?(.*?)"+'
    charsetpattern = re.compile(charset,re.S)
    if type == 'post':
        try:
            response = requests.post(url,headers = headers)
            urlchasets = re.findall(charsetpattern,response.text)
            urlchaset = urlchasets[0]
        except:
            urlchaset = 'utf-8'
    elif type == 'get':
        try:
            response = requests.get(url,headers = headers)
            urlchasets = re.findall(charsetpattern,response.text)
            urlchaset = urlchasets[0]
        except:
            urlchaset = 'utf-8'
    return urlchaset