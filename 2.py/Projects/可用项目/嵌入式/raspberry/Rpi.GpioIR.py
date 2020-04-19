#!/usr/bin/python
# -*- coding:utf-8 -*-

import RPi.GPIO as GPIO
import time

PIN = 18; 
GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN,GPIO.IN,GPIO.PUD_UP)
print('IRM Test Start ...')

try:
     
     while True:
          
          if GPIO.input(PIN) == 0:
               count = 0 
               while GPIO.input(PIN) == 0 and count < 2000:  #9ms
                    count += 1
                    time.sleep(0.00006)
               print('head 0:'+ str(count*60))
               
               count = 0               
               while GPIO.input(PIN) == 1 and count < 2000:  #4.5ms
                    count += 1
                    time.sleep(0.00006)
               print('head 1:'+ str(count*60))

               if count > 0:
                    flag = 1
               else:
                    flag = 0
                    
               if flag == 1:
                    for i in range(0,500):
                         count = 0 
                         while GPIO.input(PIN) == 0 and count < 2000:    #0.56ms
                              count += 1
                              time.sleep(0.00006)
                         print('command 0:'+ str(count*60))

                         if count > 1500:
                              break
                         
                         count = 0 
                         while GPIO.input(PIN) == 1 and count < 2000:   #0: 0.56mx
                              count += 1                               #1: 1.69ms
                              time.sleep(0.00006)
                         print('command 1:'+ str(count*60))
                         if count > 1500:
                              break
                    print('')
               
except KeyboardInterrupt:

    GPIO.cleanup();
