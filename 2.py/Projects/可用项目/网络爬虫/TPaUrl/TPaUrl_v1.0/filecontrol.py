# -*- coding: utf-8 -*-
"""
Created on Thu May 31 19:50:38 2018

@author: wolai
"""

import os 
from os.path import join, getsize
import threading 

def rmvoidfolder(path,filesize_min = 0):
    Listdirs = os.listdir(path)
    for Listdir in Listdirs:
        pathdir = join(path,Listdir)
        if os.path.isfile(pathdir):
            filesize = getsize(pathdir)
            if filesize < filesize_min:
                os.remove(pathdir)
        elif os.path.isdir(pathdir):
            try:
                os.rmdir(pathdir)
            except:
                t= threading.Thread(target=rmvoidfolder,args=(pathdir,filesize_min))
                t.start()
    try:
        os.rmdir(path)
    except:
        print('folder %s has files!' % path)

def renamefolder(path,oldstr = '',newstr = '',type = 'folder'):
    Listdirs = os.listdir(path)
    for i in range(len(Listdirs)):
        Listdir = Listdirs[i]
        pathdir = join(path,Listdir)
        if os.path.isdir(pathdir):
            if oldstr in Listdirs[i]:
                os.rename(pathdir,join(path,pathdir.replace(oldstr,newstr)))
            else:
                t= threading.Thread(target=renamefolder,args=(pathdir,oldstr,newstr,type))
                t.start()
        elif (os.path.isfile(pathdir)) & (type == 'file'):
            if oldstr in Listdirs[i]:
                os.rename(pathdir,join(path,pathdir.replace(oldstr,newstr)))
                
def getdirsize(pathdir):  
    size = 0  
    for root, dirs, files in os.walk(pathdir):  
        size += sum([getsize(join(root, name)) for name in files])  
    return size  