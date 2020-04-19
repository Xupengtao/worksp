#!/usr/bin/python
# -*- coding:utf-8 -*-


import serial
import time
class Arduino_serial(object):
    
    def __init__(self,drive,Baud_rate,timeout):
        self.drive = drive
        self.Baud_rate = Baud_rate
        self.timeout = timeout
        self.ser = serial.Serial(self.drive,self.Baud_rate,timeout = self.timeout)
    
    def send(self,str):
        self.ser.write(str.encode())
        time.sleep(0.1)
        response = self.ser.readall()
        return response