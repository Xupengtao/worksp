# -*- coding: utf-8 -*-
"""
Created on Thu Mar 14 15:14:56 2019

@author: xupengtao
"""
import re
regpress = re.compile(r'\s*\w*?:{0,1}\s*(\w+)\s+([\w\[\]\:\+]+\s?[\w\[\]\:\+]*)\s*,{0,1}\s*([\w\[\]\:\+]*\s?[\w\[\]\:\+]*);{0,1}',re.I)
tstr1 = ' AGL:     MOV    ES:[SI],AX                       ;;'
tstr2 = ' JK9:     MOV    ES:BYTE PTR[0],64H               ;'
tstr3 = '          MOV    ES:WORD PTR[BX+4],AX             ;'
tstr4 = '          PUSH   AX                               '
tstrs = 'PUSH   AX'
tstrd = 'MOV    ES,AX'
match = regpress.match(tstr1)
if match:
    print('tstr1')
    print(match.group(1))
    print(match.group(2))
    print(match.group(3))
match = regpress.match(tstr2)
if match:
    print('tstr2')
    print(match.group(1))
    print(match.group(2))
    print(match.group(3))
match = regpress.match(tstr3)
if match:
    print('tstr3')
    print(match.group(1))
    print(match.group(2))
    print(match.group(3))
match = regpress.match(tstr4)
if match:
    print('tstr4')
    print(match.group(1))
    print(match.group(2))
    print(match.group(3))
match = regpress.match(tstrs)
if match:
    print('tstrs')
    print(match.group(1))
    print(match.group(2))
    print(match.group(3))
match = regpress.match(tstrd)
if match:
    print('tstrd')
    print(match.group(1))
    print(match.group(2))
    print(match.group(3))
