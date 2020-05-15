# -*- coding: utf-8 -*-
"""
Created on Wed Nov  6 23:27:35 2019

@author: wolai
"""

import numpy as np
import pandas as pd
#基本柱状图绘制
data_1 = pd.read_excel('./20180630空气质量指数.xlsx',sheetname=0)#读取数据
data_1.head()

#各城市AQI指数
from pyecharts.charts import Bar

x = data_1.columns.tolist()
y = data_1.loc['AQI'].tolist()

bar = Bar('各城市AQI', '2018年6月30日') #参数分别为标题和副标题
bar.add('AQI', x, y,mark_point=['min'],mark_line=['average'],label_color = ['#9932CC'])
bar.render('./参考案例HTML/各城市AQI柱状图.html')
bar