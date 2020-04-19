# -*- coding: utf-8 -*-
"""
Created on Thu Nov  7 23:32:08 2019

@author: wolai
"""

class Solution(object):
    def myAtoi(self, str):
        """
        :type str: str
        :rtype: int
        """
        negtag = 0
        rty= 0
        for i in str:
            if negtag == 0:
                if i == '-':
                    negtag = -1
                elif i == '+':
                    negtag = 1
                elif((i >= '0') and (i <= '9')):
                    negtag = 1
                    if i != '0':
                         rty = int(i)
                elif i != ' ':
                    break
            elif((i >= '0') and (i <= '9')):
                rty = rty*10 + int(i)
            else:
                break
        rty = negtag*rty
        if rty >= 2**31 - 1:
            return 2**31 - 1
        elif rty <= -1*2**31:
            return -1*2**31
        else:
            return rty
        
str = '68'
solu = Solution()
print(solu.myAtoi(str))