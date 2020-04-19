# -*- coding: utf-8 -*-
"""
Created on Wed Oct 17 23:00:16 2018

@author: wolai
"""
from PyQt5 import QtCore
import time
import threading

class Job(QtCore.QThread):#注意这里使用的是qt自己本身的线程，而不能用python自己的线程
    signal2=QtCore.pyqtSignal(tuple)#因为这个信号槽要继承于Qobject，不然会发生没有emit，connect函数的报错，括号里面填写的是发射的数据类型
    def signal2emit(self,var:tuple):
        self.signal2.emit(var)#朝connect的函数发射一个tuple

    def __init__(self, *args, **kwargs):#这里自己实现
        super(Job, self).__init__(*args, **kwargs)
        self.__flag = threading.Event()     # 用于暂停线程的标识
        self.__flag.set()       # 设置为True
        self.__running = threading.Event()      # 用于停止线程的标识
        self.__running.set()      # 将running设置为True
        self.signal2.connect(setItem)#连接发射函数

    def run(self):
        global SystemTime,TimePice#声明要用的全局变量
        while self.__running.isSet():#如果被设置为了true就继续，false就终止了
            self.__flag.wait()      # 为True时立即返回, 为False时阻塞直到内部的标识位为True后返回
            ###################
            #这里写线程要干的事情#
            ###################
            time.sleep(1)

    def pause(self):
        self.__flag.clear()     # 设置为False, 让线程阻塞

    def resume(self):
        self.__flag.set()    # 设置为True, 让线程停止阻塞

    def stop(self):
        self.__flag.set()       # 将线程从暂停状态恢复, 如何已经暂停的话
        self.__running.clear()        # 设置为False  
        
def setItem(var:tuple):#用于向jcb表格填写内容
    i=var[0]#行
    j=var[1]#列
    s=var[2]#值
    table=MainWindow_ui.tableWidget_2
    if j != 6 :
        table.setItem(i,j,QtWidgets.QTableWidgetItem(str(s)))
    else :
        table.cellWidget(i,6).setValue(s)#这是个进度条来的
        
def Print_Job():
    rowcol=len(Jcb_Ready_List)+len(Jcb_Wait_List)
    print(rowcol)
    #MainWindow_ui.tableWidget_2.setRowCount(rowcol)
    for i in range(0,rowcol):
        MainWindow_ui.tableWidget_2.showRow(i)
    #for i in range(rowcol,100):
    #    MainWindow_ui.tableWidget_2.hideRow(i)
    for i in range(0,len(Jcb_Ready_List)):
        jcb=Jcb_Ready_List[i]
        table=MainWindow_ui.tableWidget_2
                                         #table.setItem(i,0,QtWidgets.QTableWidgetItem(str(jcb.Name)))
        MainThread.signal2emit((i,0,jcb.Name))
        #table.setItem(i,1,QtWidgets.QTableWidgetItem(str(jcb.Arrival_time)))
        MainThread.signal2emit((i,1,jcb.Arrival_time))
        #table.setItem(i,2,QtWidgets.QTableWidgetItem(str(jcb.Need_memory)))
        MainThread.signal2emit((i,2,jcb.Need_memory))
        #table.setItem(i,3,QtWidgets.QTableWidgetItem(str(jcb.Need_time)))
        MainThread.signal2emit((i,3,jcb.Need_time))
        #table.setItem(i,4,QtWidgets.QTableWidgetItem(str(jcb.Already_execution_time)))
        MainThread.signal2emit((i,4,jcb.Already_execution_time))
        #table.setItem(i,5,QtWidgets.QTableWidgetItem(str(jcb.Wait_time)))
        MainThread.signal2emit((i,5,jcb.Wait_time))
        #bar:QtWidgets.QProgressBar=MainWindow_ui.tableWidget_2.cellWidget(i,6)
        #bar.setValue(100-jcb.Need_time/(jcb.Already_execution_time+jcb.Need_time)*100)
        MainThread.signal2emit((i,6,100-jcb.Need_time/(jcb.Already_execution_time+jcb.Need_time)*100))