#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Nov 22 12:03:22 2019

@author: Xu
"""
import numpy as np
import Utils.PdUtilis as PdUtilis
import Utils.EnvelopeDetect as EnvDetect
import Utils.FilepathUtilis as FileUtilis

if __name__ == '__main__':
    FileUtilisCase = FileUtilis.FilepathUtils()
    filename = 'pdw1_0.csv'
    filepath = FileUtilisCase.FindFilepath(FileUtilisCase.Lpath(FileUtilisCase.Locpath, 2), filename)
    datatype = {'No':np.float64,        \
                        'Rf':np.float64,      \
                        'Doa':np.float64,   \
                        'Pa':np.float64,     \
                        'Pw':np.float64,     \
                        'Toa':np.float64,    \
                        'Pri':np.float64}
    PdUtilisCase = PdUtilis.PdUtilis(filepath+filename,list(datatype.keys()),datatype)
    PdUtilisCase.readcsvfile()
    arr = PdUtilisCase.DataFilter('Doa',0,5,'Pa')
    EnvelopeDetectCase = EnvDetect.EnvelopeDetect(arr)
    EnvelopeDetectCase.Hilbert()
    EnvelopeDetectCase.Plot()