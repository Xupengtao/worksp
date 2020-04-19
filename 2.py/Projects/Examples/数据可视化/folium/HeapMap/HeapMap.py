# -*- coding: utf-8 -*-
"""
Created on Sat Dec 23 20:19:47 2017

@author: 周文青
"""

import numpy as np
import pandas as pd
import seaborn as sns
import folium
import webbrowser
from folium.plugins import HeatMap

posi=pd.read_excel("2015Cities-CHINA.xlsx")

num = 10

lat = np.array(posi["lat"][0:num])                        # 获取维度之维度值
lon = np.array(posi["lon"][0:num])                        # 获取经度值
pop = np.array(posi["pop"][0:num],dtype=float)    # 获取人口数，转化为numpy浮点型
gdp = np.array(posi["GDP"][0:num],dtype=float)    # 获取人口数，转化为numpy浮点型

data1 = [[lat[i],lon[i],pop[i]] for i in range(num)]    #将数据制作成[lats,lons,weights]的形式

map_osm = folium.Map(location=[35,110],zoom_start=5)    #绘制Map，开始缩放程度是5倍
HeatMap(data1).add_to(map_osm)  # 将热力图添加到前面建立的map里

file_path = r"Hm.html"
map_osm.save(file_path)     # 保存为html文件

webbrowser.open(file_path)  # 默认浏览器打开