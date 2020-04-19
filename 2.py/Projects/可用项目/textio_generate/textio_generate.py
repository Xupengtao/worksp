# -*- coding: utf-8 -*-
"""
Created on Thu Dec  7 15:59:17 2017

@author: xupengtao
"""
from PDW_D2H import PDW_D2H as D2H
def textio_generate(row,path,*vectors,**types):
    vectors_tmp = list(range(len(vectors)))
    set_list = []
    for key in types:
        if key != 'dec':
            set_list = list(set(types[key]) or set(set_list))
    types['dec'] = list(set(vectors_tmp) - set(set_list))
    if 'dec' in types:
        for i in types['dec']:
            vectors_tmp[i] = []
            for j in range(row):
                s_val = int(hex(2**vectors[i]-j-1),16)
                if s_val > 0:
                    vector_tmp = bin(s_val)[2:]
                    vectors_tmp[i].append('0'*(vectors[i]-len(vector_tmp)) + vector_tmp) 
                else:
                    vectors_tmp[i].append('0'*vectors[i])
    if 'inc' in types:
        for i in types['inc']:
            vectors_tmp[i] = []
            for j in range(row):
                s_val = int(hex(j),16)
                if s_val < 2**vectors[i]:
                    vector_tmp = bin(s_val)[2:]
                    vectors_tmp[i].append('0'*(vectors[i]-len(vector_tmp)) + vector_tmp) 
                else:
                    vectors_tmp[i].append('0'*vectors[i])
    if 'turn' in types:
        for i in types['turn']:
            vectors_tmp[i] = []
            for j in range(row):
                if j%2 == 1:
                    vectors_tmp[i].append('1'*vectors[i])
                else:
                    vectors_tmp[i].append('0'*vectors[i])
    if 'constant' in types:
        for cons_i in types['constant']:
            vectors_tmp[cons_i[0]] = []
            for j in range(row):
                vector_tmp = bin(int(cons_i[1],16))[2:]
                vectors_tmp[cons_i[0]].append('0'*(vectors[cons_i[0]]-len(vector_tmp)) + vector_tmp)
    
    with open(path,'w') as f_out:
        for i in range(row):
            str = ''
            if i < row-1:
                for vector in vectors_tmp:
                    str = str + vector[i] + '   '
                str = str + '\n'
            else:
                for vector in vectors_tmp:
                    str = str + vector[i] + '   '
            f_out.write(str)    
            
###################################################################################################
vhd_t = D2H(2000,1000,1,0)
textio_generate(100000,'vibrate_textio_in.txt',16,32,16,10,1,constant = [(0,vhd_t[0]),(1,vhd_t[1]),(2,vhd_t[2]),(3,vhd_t[3])],turn = [4])
