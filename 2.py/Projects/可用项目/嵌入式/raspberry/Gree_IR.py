# -*- coding: utf-8 -*-
"""
Created on Sun Dec 17 14:14:37 2017

@author: wolai
"""

def Gree_IR(mode,temp,open,**kw):
    mode_dic = {"自动":"000","制冷":"001","加湿":"010","送风":"011","制热":"100"}
    gree_IR_tmp = []
    mode_tmp = hex(int(str(open)+mode_dic[mode],2))[2:]
    gree_IR_tmp.append('sendGree({0}, 8);'.format('0x3'+mode_tmp))
    temp_tmp = hex(temp-16)[2:]
    gree_IR_tmp.append('sendGree({0}, 8);'.format('0x0'+temp_tmp))
    gree_IR_tmp.append('sendGree(0x20, 8);')  
    gree_IR_tmp.append('sendGree(0x50, 8);')
    gree_IR_tmp.append('sendGree(0x02, 3);')
    gree_IR_tmp.append('irsend.mark(560);')
    gree_IR_tmp.append('irsend.space(10000);')
    gree_IR_tmp.append('irsend.space(10000);')
    gree_IR_tmp.append('sendGree(0x01, 8);')
    gree_IR_tmp.append('sendGree(0x21, 8);')
    gree_IR_tmp.append('sendGree(0x00, 8);')
    check_bit = hex(int(mode_dic[mode])+temp-11-open)[2:]
    if len(check_bit) == 2:
        check_bit = check_bit[1:]
    gree_IR_tmp.append('sendGree({}, 8);'.format('0x'+check_bit+'0'))
    gree_IR_tmp.append('irsend.mark(560);')
    gree_IR_tmp.append('irsend.space(0);')   
    return gree_IR_tmp

for i in Gree_IR('自动',16,0):
    print(i)