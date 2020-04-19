# -*- coding: utf-8 -*-
"""
Created on Wed Oct 17 01:14:42 2018

@author: wolai
"""
import pandas as pd

class PayrollRegister:
    def __init__(self,filepath):
        self.table = pd.read_excel(filepath)
    
    def caculate(self):
        self.table['应领'] = self.table['应领'].astype('int')
        self.table['基本工资'] = self.table['基本工资'].astype('int')
        self.table['园龄工资'] = self.table['园龄工资'].astype('int')
        self.table['安全'] = self.table['安全'].astype('int')
        self.table['保证金'] = self.table['保证金'].astype('int')
        self.table['请假'] = self.table['请假'].astype('int')
        self.table['奖惩'] = self.table['奖惩'].astype('int')
        self.table['应领'] = self.table['基本工资']+self.table['园龄工资']+self.table['安全']+self.table['保证金']+self.table['请假']+self.table['奖惩']