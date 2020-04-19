# -*- coding: utf-8 -*-
"""
Created on Tue May 29 21:23:01 2018

@author: wolai
"""
  
import threading  
import random  
import time  
  
#生产者和消费者  
  
MONEY = 0  
gLock = threading.Lock()  
def procuder():  
    while True:  
        global MONEY  
        random_money = random.randint(10,100)  
        gLock.acquire();  
        MONEY += random_money  
        gLock.release()  
        print('生产者%s--生产了%d'%(threading.currentThread,random_money))  
        time.sleep(0.5)  
  
  
def customer():  
    while True:  
        global MONEY  
        random_money = random.randint(10,100)  
        if MONEY < random_money:  
            print('余额不足，欲消费:%d,但是仓库剩余：%d'%(random_money,MONEY) )  
        else:  
            gLock.acquire()  
            MONEY -= random_money  
            gLock.release()  
            print('消费者%s--消费了%d'%(threading.currentThread,random_money))  
  
        time.sleep(0.5)  
  
  
def p_c_test():  
    for x in range(3):  
        th = threading.Thread(target=procuder)  
        th.start()  
    for x in range(3):  
        th = threading.Thread(target=customer)  
        th.start()  
  
  
if __name__ =="__main__":  
    p_c_test()  