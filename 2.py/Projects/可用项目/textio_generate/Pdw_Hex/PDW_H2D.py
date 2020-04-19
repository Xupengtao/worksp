# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""
def PDW_HTI(RF='0',PRI='0',PW='0',DOA='0'):
    RF_unit = 1
    PRI_unit = 0.08
    PW_unit = 0.02
    DOA_unit = 0.35
    PDW = {'RF': 0, 'PRI': 0, 'PW': 0, 'DOA': 0}
    PDW['RF'] = int(RF,16)*RF_unit
    PDW['PRI']  = int(PRI,16)*PRI_unit
    PDW['PW'] = int(PW,16)*PW_unit
    PDW['DOA'] = int(DOA,16)*DOA_unit
    print(PDW)
    return PDW
    
#PDW_HTI(RF='176f',PRI='10',PW='003c',DOA='01e')
#PDW_HTI(RF='3a98',PRI='10',PW='00c8',DOA='180')
#PDW_HTI(RF='0f9e',PRI='10',PW='028a',DOA='0')
#PDW_HTI(RF='176f',PRI='10',PW='0041',DOA='006')
#PDW_HTI(RF='1d4b',PRI='10',PW='09c4',DOA='00e')
PDW_HTI(RF='2edf',PRI='0',PW='0028',DOA='01a')