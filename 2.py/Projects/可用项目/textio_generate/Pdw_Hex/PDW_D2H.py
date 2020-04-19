# -*- coding: utf-8 -*-
"""
Created on Mon Oct 30 11:11:10 2017

@author: xupengtao
"""
def PDW_D2H(RF, PRI, PW, DOA):
    global num
    RF_unit = 1
    PRI_unit = 0.08
    PW_unit = 0.02
    DOA_unit = 0.35
    
    PDW = {'RF': 0, 'PRI': 0, 'PW': 0, 'DOA': 0}
    PDW['RF'] = hex(round(RF/RF_unit))
    PDW['PRI']  = hex(round(PRI/PRI_unit)-1)
    PDW['PW'] = hex(round(PW/PW_unit))
    PDW['DOA'] = hex(round(DOA/DOA_unit))
    vhd_t = list(range(4))
    vhd_t[0] = str(PDW['RF'])[2:]
    vhd_t[1] = str(PDW['PRI'])[2:]
    vhd_t[2] = str(PDW['PW'])[2:]
    vhd_t[3] = str(PDW['DOA'])[2:]
    return vhd_t