#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 22 07:28:37 2019

@author: Xu
"""

import os
import numpy as np
import pandas as pd
import struct
import pylab as pl
from pandas import Series, DataFrame as df

class PdUtilis(object):
    #Default filetype: .csv...  more
    def __init__(self, filepath, label = None, datatype = None, IndexLabel = None):
        self.filepath = None
        self.label = None
        self.datatype = None
        self.IndexLabel = None
        self.data = None
        self.filestutus = 'close'
        self.resetfile(filepath, label, datatype, IndexLabel)
        
    def resetfile(self, filepath, label = None, datatype = None, IndexLabel = None):
        self.filepath = filepath
        self.label = label
        self.datatype = datatype
        self.IndexLabel = IndexLabel
    
    def readcsvfile(self):
        self.data = pd.read_csv(self.filepath, names = self.label, dtype = self.datatype, index_col = self.IndexLabel)
        self.filestutus = 'open'
        
    def DataFilter(self, label, Min = 0, Max = 0, outlabel = None):
        if Min == 0 and Max == 0:
            print('Para invalid!')
            return []
        if self.filestutus == 'close':
            print('File invalid!')
            return []
        Filter_Data = None
        try:
            if Max != 0:
                Filter_Data = self.data.loc[(self.data[label] >= Min) & (self.data[label] <= Max)]
            else:
                Filter_Data = self.data.loc[self.data[label] >= Min]
            if outlabel  != None:
                Filter_Data = Filter_Data[outlabel]
        except:
            Filter_Data = []
        return Filter_Data
        
if __name__ == '__main__':
    datapath = str(os.path.abspath(os.path.dirname(os.getcwd())+os.path.sep + '..'))+'/Data/'
    filename = 'pdw1_0.csv'
    datatype = {'No':np.float64, 'Rf':np.float64, 'Doa':np.float64, 'Pa':np.float64, 'Pw':np.float64, 'Toa':np.float64,'Pri':np.float64}
    PdUtilsCase = PdUtilis(datapath+filename,list(datatype.keys()),datatype)
    