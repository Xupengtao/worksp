# -*- coding: utf-8 -*-
"""
Created on Mon May 28 21:05:44 2018

@author: wolai
"""
import sys
from PyQt5 import QtWidgets
from Ui_TpaUrlui import Ui_TpaUrlui
from TPaUrl_Threading import TPaUrl
from filecontrol import *

class Ui_TpaUrl(Ui_TpaUrlui):
    def __init__(self):
        app = QtWidgets.QApplication(sys.argv)  
        self.MainWindow = QtWidgets.QMainWindow()
        self.setupUi(self.MainWindow)  
        self.setupFunction()                                 
        self.MainWindow.show()                        
        sys.exit(app.exec_())
    
    def setupFunction(self):
        self.pushButton_run.clicked.connect(self.run)
        self.pushButton_rmvoidfolder.clicked.connect(self.rmvoidfolder)
        self.pushButton_renamefolder.clicked.connect(self.renamefolder)
        
    def run(self):   
        url = self.lineEdit_url.text()
        pagetype = self.lineEdit_type.text()
        htmlpath = self.lineEdit_htmlpath.text()
        baseurl = self.lineEdit_baseurl.text()
        filepath = self.lineEdit_filepath.text()
        times = int(self.lineEdit_times.text())
        filetype = self.comboBox_filetype.currentText()
        filename = self.comboBox_filename.currentText()
        srchtmclass = self.comboBox_srchtmclass.currentText()
        
        pagetype_list = pagetype.strip(',').split(',')
        htmlpath_list = htmlpath.strip(',').split(',')
        
        T_PaUrl = TPaUrl(url,pagetype_list,htmlpath_list,baseurl,filetype = filetype,
                         filepath = filepath,filename = filename,srchtmclass = srchtmclass)
        T_PaUrl.show()
        T_PaUrl.run(times)
    
    def rmvoidfolder(self):
        path = self.lineEdit_filepath.text()
        filesize = int(self.lineEdit_filesize.text())
        rmvoidfolder(path,filesize)
        
    def renamefolder(self):
        path = self.lineEdit_filepath.text()
        oldstr = self.lineEdit_oldstr.text()
        newstr = self.lineEdit_newstr.text()
        renamefolder(path,oldstr,newstr)
        
#################################################################
        
if __name__ == "__main__":    
    ui = Ui_TpaUrl()       
    