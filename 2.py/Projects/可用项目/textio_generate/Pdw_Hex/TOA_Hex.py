# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

def TOA_Hex(num):
    for i in range(len(num)-1):
        PRI_unit = 0.08
        PDW_PRI  = round((int(num[i+1],16)-int(num[i],16))*PRI_unit)
        print(PDW_PRI)
 
#   1
#num = ['1D27','2768','2D50','3232','3791','47BA','5284','57E3','5DCB','62AD'] 
#Hex(num)
#print('\n')       
#num = ['3D2E','4316','4D57','533F','5821','5D80','684A','6DA9','7DD2','83BA']        
#Hex(num)

#   2
#num = ['33d6','3ea0','5434','5efe','69c8','7492','7f5c','8a26','94f0','9fba'] 
#Hex(num)
#print('\n')       
#num = ['5f0ac','5fb76','60640','61bd4','6269e','63c32','646fc','651c6','65c90','6675a']        
#Hex(num)

#   3
#num = ['81CE','8C98','9762','A22C','ACF6','C28A','CD54','D81E','E2E8','EDB2']        
#Hex(num)

#   4
#num = ['5300','585F','5E47','6329','6888','6E70','7352','78B1','7E99','837B']        
#Hex(num)

#   5
#num = ['8F91','94F0','9AD8','9FBA','A519','AB01','AFE3','BB2A','C00C','C56B']        
#Hex(num)

#   6
#num = ['A812','B2DC','B83B','BE23','C305','C864','CE4C','D32E','D88D','DE75']        
#Hex(num)

#   7
#num = ['9287','9769','9CC8','A792','B2D9','B7BB','BD1A','C302','C7E4','CD43']        
#Hex(num)

#   8
num = ['1253','1e88','284c','3348','4941','543d','6072','6a36','8167','9627']        
Hex(num)