#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 22 11:40:43 2019

@author: Xu
"""

import os
from os.path import join,getsize
import threading

class FilepathUtils(object):
    def __init__(self):
         self.Locpath = os.getcwd()
         
    def Lpath(self,path,lev = 1):
        if os.path.isfile(path) or os.path.isdir(path):
            for i in range(0,lev):
                path = str(os.path.abspath(path +os.path.sep + '..'))
            return path + '/'
        else:
            return None
    
    def GetFileFolder(self,filepath):
        if os.path.isfile(filepath):
            return os.path.dirname(filepath)
        else:
            return None
        
    def RmVoidFolder(self,path,fileminsize):
        Listdirs = os.Listdir(path)
        for Listdir in Listdirs:
            pathdir = join(path,Listdir)
            if os.path.isfile(pathdir):
                filesize = getsize(pathdir)
                if filesize < fileminsize:
                    os.remove(pathdir)
                elif os.path.isdir(pathdir):
                    try:
                        os.rmdir(pathdir)
                    except:
                        t = threading.Thread(target = self.rmvoidfolder,args = (pathdir,fileminsize))
                        t.start()
            try:
                os.rmdir(path)
            except:
                print('folder %s has files!' % path)
                
    def RenameFolder(self, path, oldstr = '', newstr = ''):
        Listdirs = os.Listdir(path)
        for Listdir in Listdirs:
            pathdir = join(path,Listdir)
            if os.path.isdir(pathdir):
                if oldstr in Listdir:
                    os.rename(pathdir,join(path,pathdir.replace(oldstr,newstr)))
                else:
                    t = threading.Thread(target = self.renamefolder,args = (pathdir,oldstr,newstr))
                    t.start()
            elif os.path.isfile(pathdir):
                break
    
    def GetDirSize(self,pathdir):
        size = 0
        for root, dirs, files in os.walk(pathdir):
            size += sum([getsize(join(root, name)) for name in files])
        return size
    
    def FindFilepath(self,path,filename):
        rtn = ''
        if os.path.isdir(path):
            for root, dirs, files in os.walk(path):
                if filename in files:
                    return root + '/'
            return rtn
        else:
            return rtn