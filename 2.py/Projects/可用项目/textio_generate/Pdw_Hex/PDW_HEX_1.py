# -*- coding: utf-8 -*-
"""
Created on Mon Dec  4 09:23:08 2017

@author: xupengtao
"""

global num
num = 0

def PDW_HEX(RF, PRI, PW, DOA, vhd_t = []):
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
    vhd_t.append('PDW_RF('+str(num%6)+')<=  X"'+'0'*(4-len(str(PDW['RF'])[2:]))+str(PDW['RF'])[2:]+'";')
    vhd_t.append('PRI('+str(num%6)+')<=  X"'+'0'*(8-len(str(PDW['PRI'])[2:]))+str(PDW['PRI'])[2:]+'";')
    vhd_t.append('PDW_PW('+str(num%6)+') <= X"'+'0'*(4-len(str(PDW['PW'])[2:]))+str(PDW['PW'])[2:]+'";')
    vhd_t.append('PDW_DOA('+str(num%6)+')<= "00"& X"'+'0'*(2-len(str(PDW['DOA'])[2:]))+str(PDW['DOA'])[2:]+'";')
    num = num+1
    return vhd_t
vhd_t = PDW_HEX(2000,1000,1,0)
vhd_t = PDW_HEX(4000,1010,1,0,vhd_t)
vhd_t = PDW_HEX(6000,1020,1,0,vhd_t)
vhd_t = PDW_HEX(8000,1030,1,0,vhd_t)
vhd_t = PDW_HEX(12000,1040,1,0,vhd_t)
vhd_t = PDW_HEX(18000,1050,1,0,vhd_t)
for i in vhd_t:
    print(i)