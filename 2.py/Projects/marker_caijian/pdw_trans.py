# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import struct


file_path = 'F:\\wlh\\PDW\\'
file_trans_path = 'F:\\wlh\\PDW\\trans\\'
file_name = 'Task20190727163654_ch2_20190727163654'
file = open(file_path+file_name,'rb')
#rf,doa,pa,pw,toa = [],[],[],[],[]
RF,DOA,PA,PW,TOA = [],[],[],[],[]

while 1:
    try:
        (head,flag,bak1,pdwcount_l,pdwcount_h,toal,toah,rf,bw,doa,tr_flag,\
     pa,pwl,pwh,add1_0,add1_1,add1_2,add1_3,add1_4,add1_5,seccnt1,seccnth,\
     add2_0,add2_1,add2_2,add2_3,add2_4,add2_5,add2_6,add2_7,bak2,tail)=\
     struct.unpack('H'*32,file.read(64))
    except:
        break
    if head != 0xaa55 or tail != 0x55aa:continue
    RF.append(rf)
    DOA.append(doa)
    PA.append(pa)
    PW.append(pwl+(pwh<<16))
    TOA.append(toal+(toah<<16))

f1 = open(file_trans_path+file_name,'wb+')    
for i in range(len(RF)):
    data = struct.pack('I4H',TOA[i],RF[i],DOA[i],PA[i],PW[i])
    f1.write(data)
    
file.close()
f1.close()
    

