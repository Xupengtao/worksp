# -*- coding: utf-8 -*-
"""
Created on Mon May 28 21:05:44 2018

@author: wolai
"""
from time import time,localtime,strftime
import sys,os
from PyQt5 import QtWidgets
import threading 
from ui_TpaUrlui import Ui_TpaUrlui
from TPaUrl_Threading import TPaUrl
from TpaUrl_Sql import TPaUrl_Sql
from gethtmlpath import gethtmlpath
from filecontrol import rmvoidfolder,renamefolder

class Ui_TpaUrl(Ui_TpaUrlui):
    def __init__(self):
        self.log = os.getcwd()+'\\log.txt'
        self.logtype = ['url = ','urltype = ','type = ','htmlpath = ','baseurl = ','filepath = ','times = ',
                        'nextstr = ','filetype = ','filename = ','srchtmclass = ','baseurlopen = '
                        ,'baseurlnext = ','nexttype = ','genLink = ','db_name = ','collection = '
                        ,'modbkey = ','inserttype = ']
        app = QtWidgets.QApplication(sys.argv)
        self.MainWindow = QtWidgets.QMainWindow()
        self.setupUi(self.MainWindow)
        self.setupFunction()
        self.lineEdit_filepath.setText(os.getcwd()+'\\')
        self.MainWindow.show()
        sys.exit(app.exec_())
    
    def setupFunction(self):
        self.pushButton_run.clicked.connect(self.run)
        self.pushButton_writedb.clicked.connect(self.writedb)
        self.pushButton_find.clicked.connect(self.gethtmlpathstr)
        self.pushButton_save.clicked.connect(self.save)
        self.pushButton_load.clicked.connect(self.load)
        self.pushButton_rmvoidfolder.clicked.connect(self.rmvoidfolder)
        self.pushButton_renamefolder.clicked.connect(self.renamefolder)
        
    def run(self):   
        wlog = int(self.checkBox_writelog.checkState())
        if wlog == 2:
            self.writelog(self.log,'run')
        url = self.lineEdit_url.text()
        pagetype = self.lineEdit_type.text()
        htmlpath = self.lineEdit_htmlpath.text()
        baseurl = self.lineEdit_baseurl.text()
        filepath = self.lineEdit_filepath.text()
        times = int(self.lineEdit_times.text())
        urltype = self.comboBox_urltype.currentText()
        nextstr = self.lineEdit_nextstr.text()
        filetype = self.comboBox_filetype.currentText()
        filename = self.comboBox_filename.currentText()
        srchtmclass = self.comboBox_srchtmclass.currentText()
        
        baseurlopen = int(self.checkBox_open.checkState())
        baseurlnext = int(self.checkBox_next.checkState())
        baseurltype = [0,0]
        baseurltype[0] = baseurlopen
        baseurltype[1] = baseurlnext
        nexttype = int(self.checkBox_nexttype.checkState()) 
        genLink = int(self.checkBox_genLink.checkState()) 
        
        pagetype_list = pagetype.strip(',').split(',')
        htmlpath_list = htmlpath.strip(',').split(',')
        
        T_PaUrl = TPaUrl(url,urltype,pagetype_list,htmlpath_list,baseurl,
                         baseurltype,nexttype,nextstr,filetype,
                         filepath,filename,srchtmclass,genLink)
        T_PaUrl.show()
        t = threading.Thread(target=T_PaUrl.run,args=[times])
        t.start()
        
    def writedb(self):
        wlog = int(self.checkBox_writelog.checkState())
        if wlog == 2:
            self.writelog(self.log,'writedb')
        url = self.lineEdit_url.text()
        urltype = self.comboBox_urltype.currentText()
        pagetype = self.lineEdit_type.text()
        pagetype_list = pagetype.strip(',').split(',')
        htmlpath = self.lineEdit_htmlpath.text()
        htmlpath_list = htmlpath.strip(',').split(',')
        baseurl = self.lineEdit_baseurl.text()
        baseurlopen = int(self.checkBox_open.checkState())
        baseurlnext = int(self.checkBox_next.checkState())
        baseurltype = [0,0]
        baseurltype[0] = baseurlopen
        baseurltype[1] = baseurlnext
        nexttype = int(self.checkBox_nexttype.checkState()) 
        nextstr = self.lineEdit_nextstr.text()
        db_name = self.lineEdit_dbname.text()
        collection = self.lineEdit_collection.text()
        T_PaUrl_Sql = TPaUrl_Sql(url,urltype,pagetype_list,htmlpath_list,baseurl,
                                 baseurltype,nexttype,nextstr,db_name,collection)
        modbkey = self.lineEdit_modbkey.text()
        modbkey_list = modbkey.strip(',').split(',')
        inserttype = self.comboBox_inserttype.currentText()
        times = int(self.lineEdit_times.text())
        t = threading.Thread(target=T_PaUrl_Sql.run,args=[modbkey_list,inserttype,times])
        t.start()
        
    def gethtmlpathstr(self):
        url = self.lineEdit_url.text()
        htmlstr = self.lineEdit_htmlstr.text()
        classstr = self.lineEdit_classstr.text()
        htmlpathstr = gethtmlpath(url,htmlstr,classstr)
        self.lineEdit_bspath.setText(htmlpathstr)
    
    def rmvoidfolder(self):
        path = self.lineEdit_filepath.text()
        filesize = int(self.lineEdit_filesize.text())
        rmvoidfolder(path,filesize)
        
    def renamefolder(self):
        path = self.lineEdit_filepath.text()
        oldstr = self.lineEdit_oldstr.text()
        newstr = self.lineEdit_newstr.text()
        folderorfile = int(self.checkBox_folderorfile.checkState())
        if folderorfile == 0:
            renamefolder(path,oldstr,newstr,'folder')
        else:
            renamefolder(path,oldstr,newstr,'file')
    
    def save(self):
        filename = self.lineEdit_filepath.text()
        self.writelog(filename,'save','w')
        
    def load(self):
        filename = self.lineEdit_filepath.text()
        with open(filename,'r') as f:
            loglist = f.readlines()
        for i in range(len(loglist)):
            if "run Function:" in loglist[i]:
                startline =  i + 1
                break
        for i in range(len(self.logtype)):
            logline = self.modifylogline(loglist[startline+i],self.logtype[i])
            print(logline)
            if logline != '':
                if i == 0:
                    self.lineEdit_url.setText(logline)
                    continue
                elif i == 1:
                    self.comboBox_urltype.setCurrentIndex(int(logline))
                    continue
                elif i == 2:
                    self.lineEdit_type.setText(logline)
                    continue
                elif i == 3:
                    self.lineEdit_htmlpath.setText(logline)
                    continue
                elif i == 4:
                    self.lineEdit_baseurl.setText(logline)
                    continue
                elif i == 5:
                    self.lineEdit_filepath.setText(logline)
                    continue
                elif i == 6:
                    self.lineEdit_times.setText(logline)
                    continue
                elif i == 7:
                    self.lineEdit_nextstr.setText(logline)
                    continue
                elif i == 8:
                    self.comboBox_filetype.setCurrentIndex(int(logline))
                    continue
                elif i == 9:
                    self.comboBox_filename.setCurrentIndex(int(logline))
                    continue
                elif i == 10:
                    self.comboBox_srchtmclass.setCurrentIndex(int(logline))
                    continue
                elif i == 11:
                    if logline == '2':
                        self.checkBox_open.setChecked(True)
                    else:
                        self.checkBox_open.setChecked(False)
                    continue
                elif i == 12:
                    if logline == '2':
                        self.checkBox_next.setChecked(True)
                    else:
                        self.checkBox_next.setChecked(False)
                    continue
                elif i == 13:
                    if logline == '2':
                        self.checkBox_nexttype.setChecked(True)
                    else:
                        self.checkBox_nexttype.setChecked(False)
                    continue
                elif i == 14:
                    if logline == '2':
                        self.checkBox_genLink.setChecked(True)
                    else:
                        self.checkBox_genLink.setChecked(False)
                    continue
                elif i == 15:
                    self.lineEdit_dbname.setText(logline)
                    continue
                elif i == 16:
                    self.lineEdit_collection.setText(logline)
                    continue
                elif i == 17:
                    self.lineEdit_modbkey.setText(logline)
                    continue
                elif i == 18:
                    self.comboBox_inserttype.setCurrentIndex(int(logline))
                    continue
            else:
                continue
                
    def modifylogline(self,logline,findstr):
        if findstr in  logline:
            logline = logline.replace(findstr,'')
            logline = logline.replace('\n','')
            return logline
        else:
            return ''
                
    def writelog(self,file,func,optype = 'a'):
        with open(file,optype) as f:
            f.write('run Function:' + func + '*'*110
                    + 'date: '+strftime('%Y.%m.%d %H:%M',localtime(time()))+'\n')
            f.write('url = '+self.lineEdit_url.text()+'\n')
            f.write('urltype = '+str(self.comboBox_urltype.currentIndex())+'\n')
            f.write('type = '+self.lineEdit_type.text()+'\n')
            f.write('htmlpath = '+self.lineEdit_htmlpath.text()+'\n')
            f.write('baseurl = '+self.lineEdit_baseurl.text()+'\n')
            f.write('filepath = '+self.lineEdit_filepath.text()+'\n')
            f.write('times = '+self.lineEdit_times.text()+'\n')
            f.write('nextstr = '+self.lineEdit_nextstr.text()+'\n')
            f.write('filetype = '+str(self.comboBox_filetype.currentIndex())+'\n')
            f.write('filename = '+str(self.comboBox_filename.currentIndex())+'\n')
            f.write('srchtmclass = '+str(self.comboBox_srchtmclass.currentIndex())+'\n')
            f.write('baseurlopen = '+str(self.checkBox_open.checkState())+'\n')
            f.write('baseurlnext = '+str(self.checkBox_next.checkState())+'\n')
            f.write('nexttype = '+str(self.checkBox_nexttype.checkState())+'\n')
            f.write('genLink = '+str(self.checkBox_genLink.checkState())+'\n')
            f.write('db_name = '+self.lineEdit_dbname.text()+'\n')
            f.write('collection = '+self.lineEdit_collection.text()+'\n')
            f.write('modbkey = '+self.lineEdit_modbkey.text()+'\n')
            f.write('inserttype = '+str(self.comboBox_inserttype.currentIndex())+'\n')
            f.write('*'*157+'\n'+'\n')
        
#################################################################
        
if __name__ == "__main__":    
    ui = Ui_TpaUrl()       
    