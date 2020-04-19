# -*- coding: utf-8 -*-
"""
Created on Wed Nov  8 21:32:20 2017

@author: Armani
"""
import time
from datetime import datetime
from Arduino_serial import Arduino_serial as Arserial
from pymongo import MongoClient

class autocontrol(object):
    temperature = 0
    temperature_low = 18
    temperature_high = 23
    
    def __init__(self):
        self.date_str = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        self.Arse = Arserial('/dev/ttyUSB0',9600,1)
        self.enable = 1
        
    def gree_control(self):
        self.temperature = self.Arse.send('19\r\n')
        while self.temperature == '':
            self.temperature = self.Arse.send('19\r\n')
            time.sleep(0.5)
#        print(self.temperature)
        try:
            if (int(self.temperature) <= self.temperature_low):
                self.Arse.send('3\r\n')
                time.sleep(1)
                return 1
            elif (int(self.temperature) >= self.temperature_high):
                self.Arse.send('4\r\n')
                time.sleep(1)
                return 0
            else: return -1
        except:
            print("temperature's data error!")
    
    def datedelta_hours(self):
        date1 = datetime.strptime(self.date_str,'%Y-%m-%d %H:%M:%S')
        date2 = datetime.now()
        delta = date2-date1
        deltahour = int(delta.seconds/3600)
        date2 = date2.strftime('%Y-%m-%d %H:%M:%S')
        return deltahour, date2
    
    def datedelta_minutes(self):
        date1 = datetime.strptime(self.date_str,'%Y-%m-%d %H:%M:%S')
        date2 = datetime.now()
        delta = date2-date1
        deltahour = int(delta.seconds/60)
        date2 = date2.strftime('%Y-%m-%d %H:%M:%S')
        return deltahour, date2

#*****************************************************************************#

autoctr = autocontrol()
conn = MongoClient('localhost', 27017)
db = conn.gree_control
opentime_data = db.opentime
tem = db.temperature
flag = 0
try:
    while autoctr.enable == 1:
        deltahours, date = autoctr.datedelta_hours()
        if deltahours >= 1:
            autoctr.date_str = date
            gree_status = autoctr.gree_control()
            if gree_status:
                flag = 1
                data = {
                        "date:":date,
                        "status:":"open"
                        }
                opentime_data.insert_one(data)
            elif gree_status == 0:
                flag = 0
                data = {
                        "date:":date,
                        "status:":"close"
                        }
                opentime_data.insert_one(data)
            data = {
                    "date":date,
                    "temperature:":autoctr.temperature
                    }
            tem.insert_one(data)
        if flag == 1:
            deltaminutes, date = autoctr.datedelta_minutes()
            if deltaminutes >= 30:
                flag = 0
                autoctr.Arse.send('4\r\n')
                time.sleep(1)
                data = {
                        "date:":date,
                        "status:":"close"
                        }
                opentime_data.insert_one(data)
        time.sleep(1)
except:
    print("error!")
    data = {
            "date:":date,
            "status:":"program exist error!"
            }
    opentime_data.insert_one(data)