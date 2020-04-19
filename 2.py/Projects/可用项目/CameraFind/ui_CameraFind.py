# -*- coding: utf-8 -*-
"""
Created on Mon Oct  8 17:23:09 2018

@author: xupengtao
"""

import sys,os
import pandas as pd
from PyQt5 import QtWidgets
from PyQt5.QtGui import QFont,QColor
from PyQt5.QtWidgets import QFileDialog
from ui_CameraFindui import Ui_MainWindow
from ping import threadping,mutiping,mutiarpping

class Ui_CameraFind(Ui_MainWindow):
    def __init__(self):
        self.ipset = ['192.168.31.1','192.168.31.200']
        self.iplist = []
        self.ipmacdict = {}
        app = QtWidgets.QApplication(sys.argv)  
        self.MainWindow = QtWidgets.QMainWindow()
        self.setupUi(self.MainWindow)  
        self.tableWidget.setColumnCount(3)
        self.tableWidget.setRowCount(100)
        self.tableWidget.setSortingEnabled(False)
        self.tableWidget.setHorizontalHeaderLabels(['IP地址','MAC','摄像头位置'])
        self.setupFunction()
        self.MainWindow.show()
        self.reset()
        self.lineEdit_runstatus.setText('欢迎使用!')
        sys.exit(app.exec_())
    
    def setupFunction(self):
        self.pushButton_camerafind.clicked.connect(self.camerafind)
        self.pushButton_merge.clicked.connect(self.merge)
        self.pushButton_dispnoconn.clicked.connect(self.dispnoconn)
        self.pushButton_dispchange.clicked.connect(self.dispchange)
        self.pushButton_save.clicked.connect(self.save)
        self.pushButton_load.clicked.connect(self.load)
        self.pushButton_reset.clicked.connect(self.reset)
        self.pushButton_saveasinit.clicked.connect(self.saveasinit)
          
    def camerafind(self):
        self.iplist = []
        self.ipmacdict = {}
        threadping(self.ipset[0],self.ipset[1],self.iplist,100)
        mutiarpping(self.iplist,self.ipmacdict)
        self.tWidgetclear()
        i = 0
        maclist = []
        newiplist = []
        initiplist = []
        self.iplist = []
        for ip in self.ipmacdict.keys():
            self.iplist.append(ip)
            maclist.append(self.ipmacdict[ip])
        ipmacDframe = pd.DataFrame({'IP地址':self.iplist, 'MAC':maclist})
        if not self.CameraFind.empty:
            row = self.CameraFind.iloc[:,0].size
            for j in range(row):
                initiplist.append(self.CameraFind.iloc[j,0])
        newips = ipmacDframe.IP地址.isin(initiplist).ne(True)
        for j in range(newips.size):
            if newips[j] == True:
                newiplist.append(ipmacDframe.iloc[j,0])
                for k in range(2):
                    self.tWidgetinsert(i,k,ipmacDframe.iloc[j,k],QColor(255,255,0))
                i = i+1
        ipsortdict = {}      
        for key in self.ipmacdict.keys():
            ipsortdict[int(key[key.rfind('.')+1:])] = key
        for key in sorted(ipsortdict):
            if ipsortdict[key] not in newiplist:
                self.tWidgetinsert(i,0,ipsortdict[key])
                self.tWidgetinsert(i,1,self.ipmacdict[ipsortdict[key]])
                i = i+1
#        for key,values in self.ipmacdict.items():
#            if key not in newiplist:
#                self.tWidgetinsert(i,0,key)
#                self.tWidgetinsert(i,1,values)
#                i = i+1
        if i != 0:
            self.lineEdit_runstatus.setText('扫描完毕!')
        else:
            self.lineEdit_runstatus.setText('未检测到设备!')

    def merge(self):
        self.tWidgetclear()
        self.tWidgetshow(self.CameraFind)
        lostseries = self.CameraFind.IP地址.isin(self.iplist).ne(True)
        for i in range(lostseries.size):
            if lostseries[i] == True:
                for j in range(3):
                    self.tWidgetinsert(i,j,self.tableWidget.item(i,j).text(),QColor(0,60,10))
        if self.CameraFind.empty:
            self.lineEdit_runstatus.setText('没有数据!')
            return 0
        row = self.CameraFind.iloc[:,0].size
        for ip in self.iplist:
            for i in range(row):
                if ip == self.CameraFind.iloc[i,0]:
                    if self.ipmacdict[ip] != self.CameraFind.iloc[i,1]:
                        for j in range(3):
                            self.tWidgetinsert(i,j,self.tableWidget.item(i,j).text(),QColor(60,60,180))
        self.lineEdit_runstatus.setText('对比完毕!')
        
    def dispnoconn(self):
        lostseries = self.CameraFind.IP地址.isin(self.iplist)
        lostdict = self.CameraFind[lostseries.ne(True)]
        self.tWidgetclear()
        if lostdict.empty:
            self.lineEdit_runstatus.setText('状态正常!')
            return 0
        self.tWidgetshow(lostdict)
        self.lineEdit_runstatus.setText('<-未连接设备')

    def dispchange(self):
        connseries = self.CameraFind.IP地址.isin(self.iplist)
        conncamera = self.CameraFind[connseries]
        maclist = []
        for ip in self.iplist:
            maclist.append(self.ipmacdict[ip])
        mapdataframe = pd.DataFrame({'IP地址':self.iplist, 'MAC':maclist})
        dispdataframe = pd.merge(mapdataframe,conncamera,on='IP地址',how='inner')
        self.tWidgetclear()
        if dispdataframe.empty:
            self.lineEdit_runstatus.setText('状态正常!')
            return 0
        dispdataframe = dispdataframe.fillna('')
        row = dispdataframe.iloc[:,0].size
        j = 0
        for i in range(row):
            if dispdataframe.iloc[i,1] != dispdataframe.iloc[i,2]:
                self.lineEdit_runstatus.setText('<-已修改设备')
                self.tWidgetinsert(j,0,str(dispdataframe.iloc[i,0]))
                self.tWidgetinsert(j,1,str(dispdataframe.iloc[i,1]))
                self.tWidgetinsert(j,2,str(dispdataframe.iloc[i,3]))
                j = j+1

    def save(self):
        positionlist = []
        iplist = []
        maclist = []
        for i in range(100):
            if self.tableWidget.item(i,0).text() != '':
                print(i)
                iplist.append(self.tableWidget.item(i,0).text())
                maclist.append(self.tableWidget.item(i,1).text())
                positionlist.append(self.tableWidget.item(i,2).text())
        
        self.CameraFind = pd.DataFrame({'IP地址':iplist, 'MAC':maclist, '摄像头位置':positionlist})
        init_file, filetype = QFileDialog.getOpenFileName(None, "保存文件至",os.getcwd(),"Text Files(*.xlsx)")
        self.CameraFind.to_excel(init_file,index=False)
        self.lineEdit_runstatus.setText('保存成功!')
        
    def load(self): 
        init_file, filetype = QFileDialog.getOpenFileName(None, "选择加载文件",os.getcwd(),"Text Files(*.xlsx)")
        self.reset(init_file)
        self.lineEdit_runstatus.setText('读取成功!')
    
    def tWidgetinsert(self,i,j,strval,qcolor = QColor(255,255,255),fonttype = 'Times New Roman',pointsize = 12):
        newItem = QtWidgets.QTableWidgetItem(strval)
        newItem.setTextAlignment(0x0004|0x0080)
        newItem.setBackground(qcolor)       
        font = QFont()
        font.setFamily(fonttype)
        font.setPointSize(pointsize)
        newItem.setFont(font)
        self.tableWidget.setItem(i,j,newItem)
    
    def tWidgetshow(self,shdatafarame):
        self.tWidgetclear()
        if shdatafarame.empty:
            return 0
        row = shdatafarame.iloc[:,0].size
        col = shdatafarame.iloc[0,:].size
        shdatafarame = shdatafarame.fillna('')
        for i in range(row):
            for j in range(col):
                self.tWidgetinsert(i,j,str(shdatafarame.iloc[i,j]))

    def tWidgetclear(self):
        for i in range(100):
            for j in range(3):
                newItem = QtWidgets.QTableWidgetItem('')
                self.tableWidget.setItem(i,j,newItem)
        
    def reset(self,filepath = os.getcwd()+'\\init.xlsx'):   
        self.filepath = filepath
        self.CameraFind = pd.read_excel(filepath)
        self.iplist = []
        self.ipmacdict = {}
        self.tWidgetclear()
        self.tWidgetshow(self.CameraFind)
        self.lineEdit_runstatus.setText('重置成功!')
        return 1

    def saveasinit(self):
        positionlist = []
        iplist = []
        maclist = []
        for i in range(100):
            if self.tableWidget.item(i,0).text() != '':
                print(i)
                iplist.append(self.tableWidget.item(i,0).text())
                maclist.append(self.tableWidget.item(i,1).text())
                positionlist.append(self.tableWidget.item(i,2).text())
        
        self.CameraFind = pd.DataFrame({'IP地址':iplist, 'MAC':maclist, '摄像头位置':positionlist})
        self.CameraFind.to_excel(os.getcwd()+'\\init.xlsx',index=False)
        self.lineEdit_runstatus.setText('保存重置文件成功!')
            
#################################################################
        
if __name__ == "__main__":    
    ui = Ui_CameraFind()       
    