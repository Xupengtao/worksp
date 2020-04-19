# -*- coding: utf-8 -*-
"""
Created on Mon Oct  8 17:23:09 2018

@author: xupengtao
"""

import sys
from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QFileDialog
from PayrollRegisterUI import Ui_MainWindow
from PayrollRegister import PayrollRegister

class Ui_PayrollRegister(Ui_MainWindow):
    def __init__(self):
        app = QtWidgets.QApplication(sys.argv)  
        self.MainWindow = QtWidgets.QMainWindow()
        self.setupUi(self.MainWindow)  
        self.tableWidget.setColumnCount(12)
        self.tableWidget.setRowCount(100)
        self.tableWidget.setHorizontalHeaderLabels(['姓名','基本工资','园龄工资','安全','保证金','兴趣班','请假','奖惩','应领','实领','签字','备注'])
        self.setupFunction()                                 
        self.MainWindow.show()
        self.init()                        
        sys.exit(app.exec_())
    
    def setupFunction(self):
        self.pushButton.clicked.connect(self.caculate)
        self.pushButton_2.clicked.connect(self.init)
        self.pushButton_3.clicked.connect(self.savefile)
        self.pushButton_4.clicked.connect(self.changefile)
        self.pushButton_5.clicked.connect(self.reset)
            
    def caculate(self):
        row = self.PayrollRegister.table.iloc[:,0].size
        col = self.PayrollRegister.table.iloc[0,:].size
        for i in range(row):
            for j in range(col):
                self.PayrollRegister.table.iloc[i,j] = self.tableWidget.item(i,j).text()
        self.PayrollRegister.caculate()
        for i in range(row):
            for j in range(col):
                newItem = QtWidgets.QTableWidgetItem(str(self.PayrollRegister.table.iloc[i,j]))
                self.tableWidget.setItem(i,j,newItem)
        self.status = 1
        
    def init(self,filepath = 'E:\Data\init.xlsx'):   
        self.filepath = filepath
        self.PayrollRegister = PayrollRegister(filepath)
        row = self.PayrollRegister.table.iloc[:,0].size
        col = self.PayrollRegister.table.iloc[0,:].size
        for i in range(100):
            for j in range(12):
                newItem = QtWidgets.QTableWidgetItem('')
                self.tableWidget.setItem(i,j,newItem)
        for i in range(row):
            for j in range(col):
                newItem = QtWidgets.QTableWidgetItem(str(self.PayrollRegister.table.iloc[i,j]))
                self.tableWidget.setItem(i,j,newItem)
        self.status = 0

    def savefile(self):
        if self.status == 1:
            self.PayrollRegister.table.to_excel(self.filepath,index=False)
      
    def changefile(self):
        linestr = self.lineEdit.text()
        if linestr == '':    
            init_file, filetype = QFileDialog.getOpenFileName(None, "选取工资表文件","E:","Text Files(*.xlsx)")
            self.init(init_file)
        
    def reset(self):
        self.init(self.filepath)
        
#################################################################
        
if __name__ == "__main__":    
    ui = Ui_PayrollRegister()       
    