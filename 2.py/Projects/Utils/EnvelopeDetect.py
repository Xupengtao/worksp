#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 22 07:05:45 2019

@author: Xu
"""

import numpy as np
import pylab as pl
from scipy import fftpack

class EnvelopeDetect(object):
    def __init__(self,nparr):
        self.FilterFactor = 100
        self.data = nparr
        self.RstData = None
        self.RevseRstData =None
    
    def SetFilterFactor(self,FilterFactor):
        self.FilterFactor = FilterFactor
        
    def InData(self):
        return self.data
    
    def Result(self):
        return self.RstData
    
    def ReverseResult(self):
        return self.ReverseResult
    
    def Filter(self,FilterFactor = None, Type = 'Default'):
        Threshold = 0
        FliterFactorTmp = 0
        if FilterFactor == None:
            FliterFactorTmp = self.FilterFactor
        else:
            FliterFactorTmp = FilterFactor
            FliterFactorTmp /= FliterFactorTmp + 1
        def FliterItem(x):
            nonlocal Threshold
            if(x > Threshold):
                Threshold = x
            else:
                Threshold *= FliterFactorTmp
            return Threshold
        if Type == 'Default':
            self.RstData = np.frompyfunc(FliterItem,1,1)(self.data)
        elif Type == 'Reverse':
            self.RevseRstData = np.frompyfunc(FliterItem,1,1)(self.data[::-1])[::-1]
            
    def ReverseFilter(self,FilterFactor = None):
        self.Filter(FilterFactor,'Default')
        self.Filter(FilterFactor,'Reverse')
        
    def Hilbert(self):
        h = fftpack.hilbert(self.data)
        self.RstData = np.sqrt(self.data**2 + h**2)
    #@delete
    def Plot(self): 
        pl.subplot()
        pl.plot(self.data,'g',label = u'data')
        pl.plot(self.RstData,'r',linewidth = 2, label = u'Envelop')
        if self.RevseRstData != None:
            pl.plot(self.RevseRstData,'b',linewidth = 2, label = u'Reverse')
        pl.legend()
        
if __name__ == '__main__':
    t = np.arange(0,0.3,1/20000.0)
    x = np.sin(2*np.pi*1000*t) * (np.sin(2*np.pi*20*t) + np.sin(2*np.pi*8*t) + 2.0)
    EnvlDectCase = EnvelopeDetect(x)
    #EnvlDectCase.Hilbert()
    EnvlDectCase.ReverseFilter(1000)
    EnvlDectCase.Plot()
   