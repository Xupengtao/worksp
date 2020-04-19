# -*- coding: utf-8 -*-
"""
Created on Mon Oct 30 11:11:10 2017

@author: xupengtao
"""
global num
num = 1

def PDW_HEX(RF, PRI, PW, DOA,path = 'E:/Workspace/14m148/FPGA/SIM_PDW_OED45/vibrate_mod/vibrate.vhd'):
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
    
    with open(path) as f_in:
        vhd_t = []
        for line in f_in.readlines():
            vhd_t.append(line)
    vhd_t[91] = 'PDW_RF<=  X"'+'0'*(4-len(str(PDW['RF'])[2:]))+str(PDW['RF'])[2:]+'";\n'
    vhd_t[92] = 'RAM1(0)<=  X"'+'0'*(8-len(str(PDW['PRI'])[2:]))+str(PDW['PRI'])[2:]+'";\n'
    vhd_t[93] = 'PDW_PW <= X"'+'0'*(4-len(str(PDW['PW'])[2:]))+str(PDW['PW'])[2:]+'";\n'
    vhd_t[94] = 'PDW_DOA<= "00"& X"'+'0'*(2-len(str(PDW['DOA'])[2:]))+str(PDW['DOA'])[2:]+'";\n'
    
    with open(path[:len(path)-4]+'_'+str(num)+'.vhd','w') as f_out:
        f_out.write('--'+str(num)+'_RF_'+str(RF)+'_PRI_'+str(PRI)+'_PW_'+str(PW)+'_DOA_'+str(DOA)+'\n')
        f_out.writelines(vhd_t)
    num = num+1
    return
    
PDW_HEX(2400,100,1,0)