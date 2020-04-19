# -*- coding: utf-8 -*-
"""
Created on Mon May 28 21:05:44 2018

@author: wolai
"""
import sys
from PyQt5 import QtWidgets
from project.spyder.Ui_TpaUrlui import Ui_TpaUrlui
from project.spyder.TPaUrl import TPaUrl

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
    
    def run(self):   
        url = self.lineEdit_url.text()
        pagetype = self.lineEdit_type.text()
        htmlpath = self.lineEdit_htmlpath.text()
        baseurl = self.lineEdit_baseurl.text()
        filetype = self.lineEdit_filetype.text()
        filepath = self.lineEdit_filepath.text()
        filename = self.lineEdit_filename.text()
        srchtmclass = self.lineEdit_srchtmclass.text()
        
        pagetype_list = pagetype.strip(',').split(',')
        htmlpath_list = htmlpath.strip(',').split(',')
        
        T_PaUrl = TPaUrl(url,pagetype_list,htmlpath_list,baseurl,filetype = filetype,
                         filepath = filepath,filename = filename,srchtmclass = srchtmclass)
        T_PaUrl.show()
        T_PaUrl.run()

#################################################################
        
if __name__ == "__main__":    
    ui = Ui_TpaUrl()       
    