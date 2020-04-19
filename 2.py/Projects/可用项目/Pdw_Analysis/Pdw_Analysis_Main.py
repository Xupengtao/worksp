# -*- coding: utf-8 -*-
"""
Created on Mon Oct  8 17:23:09 2018

@author: xupengtao
"""

import sys
from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QFileDialog
from Pdw_AnalysisUI import Ui_MainWindow
from Pdw_Analysis import Pdw_Analysis

class Ui_PdwAnalysis(Ui_MainWindow):
    def __init__(self):
        app = QtWidgets.QApplication(sys.argv)  
        self.MainWindow = QtWidgets.QMainWindow()
        self.setupUi(self.MainWindow)  
        self.tableWidget.setColumnCount(8)
        self.tableWidget.setRowCount(100)
        self.tableWidget.setHorizontalHeaderLabels(['PRI','PRI_Frq','RF','RF_Frq','PW','PW_Frq','PA','PA_Frq'])
        self.setupFunction()                                 
        self.MainWindow.show()                        
        sys.exit(app.exec_())
    
    def setupFunction(self):
        self.pushButton.clicked.connect(self.openfile)
        self.pushButton_2.clicked.connect(self.init)
        self.pushButton_3.clicked.connect(self.analysis)
        self.pushButton_4.clicked.connect(self.writesingle)
        self.pushButton_5.clicked.connect(self.writeall)
        
    def init(self):   
        init_file, filetype = QFileDialog.getOpenFileName(None, "选取初始化参数文件","E:","Text Files(*.csv)")
        print(init_file)
        self.Pdw_Analysis = Pdw_Analysis(init_file)
            
    def openfile(self):
        file, filetype = QFileDialog.getOpenFileName(None, "选取数据文件","E:\Data","Text Files(*.data)")
        self.Pdw_Analysis.file_init(file)

    def analysis(self):
        RadarSource = self.lineEdit.text()
        if RadarSource == '':
            RadarSource = 600
        elif int(RadarSource) < 600:
            RadarSource = int(RadarSource)
        self.Pdw_Analysis.file_read()    
        self.Pdw_Analysis.RadarSourceSelect(RadarSource)
        self.Pdw_Analysis.Para_Analysis()
        row = self.Pdw_Analysis.Analysis.iloc[:,0].size
        for i in range(100):
            for j in range(8):
                newItem = QtWidgets.QTableWidgetItem('')
                self.tableWidget.setItem(i,j,newItem)
        for i in range(row):
            for j in range(8):
                if(self.Pdw_Analysis.Analysis.iloc[i,j] != 0):
                    newItem = QtWidgets.QTableWidgetItem('%.2f'%(self.Pdw_Analysis.Analysis.iloc[i,j]))
                    self.tableWidget.setItem(i,j,newItem)
      
    def writesingle(self):
        self.Pdw_Analysis.file_write(0)
        newItem = QtWidgets.QTableWidgetItem('Single writed!')
        self.tableWidget.setItem(0,0,newItem)
        
    def writeall(self):
        self.Pdw_Analysis.file_init(self.Pdw_Analysis.filepath)
        RadarSource = self.lineEdit.text()
        if RadarSource == '':
            RadarSource = 600
        elif int(RadarSource) < 600:
            RadarSource = int(RadarSource)
        file_status = 0
        count = 0
        while file_status != -1:
            try:
                if count == 0:
                    file_status = self.Pdw_Analysis.file_read()    
                    self.Pdw_Analysis.RadarSourceSelect(RadarSource)
                    self.Pdw_Analysis.Para_Analysis(1)
                    self.Pdw_Analysis.file_write(1)
                    count = 1
                else:
                    file_status = self.Pdw_Analysis.file_read()    
                    self.Pdw_Analysis.RadarSourceSelect(RadarSource)
                    self.Pdw_Analysis.Para_Analysis(1)
                    self.Pdw_Analysis.file_write(2)
            except:
                break
        newItem = QtWidgets.QTableWidgetItem('All writed!')
        self.tableWidget.setItem(0,0,newItem)
                
        
#################################################################
        
if __name__ == "__main__":    
    ui = Ui_PdwAnalysis()       
    