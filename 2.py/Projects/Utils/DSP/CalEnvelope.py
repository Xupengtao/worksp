# -*- coding: utf-8 -*-
"""
Created on Wed Dec  4 21:28:16 2019

@author: Xu
"""

import numpy as np
import pylab as pl
from scipy import fftpack

class EnvelopeDetect(object):
    def __init__(self,nparr):
        self.FiltFactor = 100
        self.data = nparr
        self.RstData = None
        self.RevseRstData = None
    
    def SetFiltFactor(self,FiltFactor):
        self.FiltFactor = FiltFactor
        
    def InData(self):
        return self.data
        
    def Result(self):
        return self.RstData
    
    def ReverseResult(self):
        return self.RevseRstData
        
    def Filter(self,FiltFactor = None,Type = 'Default'):
        Threshold = 0
        FiltFactorTmp = 0
        if FiltFactor != None:
            FiltFactorTmp = FiltFactor
        else:
            FiltFactorTmp = self.FiltFactor
        FiltFactorTmp = FiltFactorTmp/(FiltFactorTmp + 1)
        def FilterItem(x):
            nonlocal Threshold
            if(x > Threshold):
                Threshold = x
            else:
                Threshold *= FiltFactorTmp
            return Threshold
        if Type == 'Default':
            self.RstData = np.frompyfunc(FilterItem,1,1)(self.data)
        elif Type == 'Reverse':
            self.RevseRstData = np.frompyfunc(FilterItem,1,1)(self.data[::-1])[::-1]
    
    def ReverseFilter(self,FiltFactor = None):
        self.Filter(FiltFactor,'Default')
        self.Filter(FiltFactor,'Reverse')
    
    def Hilbert(self):
        h = fftpack.hilbert(self.data)
        self.RstData = np.sqrt(self.data**2 + h**2)
    
    def Plot(self):
        pl.subplot()
        pl.plot(self.data, "g", label=u"Data")
        pl.plot(self.RstData, "r", linewidth=2, label=u"Envelop")
        if self.RevseRstData.any():
            pl.plot(self.RevseRstData, "b", linewidth=2, label=u"Envelop")
        pl.legend()
    
if __name__ == '__main__':      
    t = np.arange(0, 0.3, 1/20000.0)
    x = np.sin(2*np.pi*1000*t) * (np.sin(2*np.pi*20*t) + np.sin(2*np.pi*8*t) + 3.0)
    EnvlDetectCase = EnvelopeDetect(x)
    EnvlDetectCase.ReverseFilter(1000)
    EnvlDetectCase.Plot()