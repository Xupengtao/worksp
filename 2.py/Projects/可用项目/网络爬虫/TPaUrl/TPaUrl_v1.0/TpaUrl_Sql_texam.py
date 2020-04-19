# -*- coding: utf-8 -*-
"""
Created on Wed Dec 26 23:57:30 2018

@author: wolai
"""

from TpaUrl_Sql import TPaUrl_Sql

#url = 'https://www.anjuke.com/fangjia/nanjing2018/'
#urltype = 'get'
#type = ['record','key','value']
#htmpath = ['div div[class="items"] span[class="elem-l"] div[class="sub-items"] a'
#           ,'div ul li a[class="nostyle"] b'
#           ,'div ul li a[class="nostyle"] span']
#baseurl = ''
#baseurltype = [0,0]
#nexttype = 0
#nextstr = ''
#db_name = 'myDB'
#collection = ''
#modbkey = ['区域']
#
#Tpaurl = TPaUrl_Sql(url,urltype,type,htmpath,baseurl,baseurltype,nexttype,nextstr,db_name,collection)
#Tpaurl.run(modbkey,0)


url = 'https://www.anjuke.com/fangjia/quanguo2018/'
urltype = 'get'
type = ['record','record','record','record','record','key','value']
htmpath = ['div[class="main-content"] div div[class="items"] span[class="elem-l"] a'    #省份（collection`s nm）
           ,'div[class="main-content"] div a'                                           #时间
           ,'div div[class="items"] span[class="elem-l"] div[class="sub-items"] a'      #城市
           ,'div[class="main-content"] div div[class="items"] span[class="elem-l"] a'   #区域
           ,'div div[class="items"] span[class="elem-l"] div[class="sub-items"] a'      #位置
           ,'div ul li a[class="nostyle"] b'
           ,'div ul li a[class="nostyle"] span']
baseurl = ''
baseurltype = [0,0]
nexttype = 0
nextstr = ''
db_name = '2018年全国房价'
collection = ''
modbkey = ['时间','城市','区域','位置']
inserttype = 'page'

Tpaurl = TPaUrl_Sql(url,urltype,type,htmpath,baseurl,baseurltype,nexttype,nextstr,db_name,collection)
Tpaurl.run(modbkey,inserttype,0)