# -*- coding: utf-8 -*-
"""
Created on Wed Oct 17 01:14:42 2018

@author: wolai
"""
import pandas as pd

class CameraFind:
    def __init__(self,filepath):
        self.table = pd.read_excel(filepath)
    
#    def Merge(self):
        