# -*- coding: utf-8 -*-
"""
Created on Tue Apr  7 19:40:13 2020

@author: qinchanghai
"""

from PyQt5.QtWidgets import (QApplication, QWidget, QToolTip,
     QPushButton, QMainWindow, QAction, QDockWidget, QMessageBox,
     QFileDialog, QGridLayout, QLabel, QHBoxLayout,QVBoxLayout,
     QLineEdit, QTableView, QInputDialog, QRadioButton,QProgressBar,
     QTabWidget, QSizePolicy, QCheckBox,QMenu,QComboBox)

from guidata.qt.QtGui import QSplitter, QListWidget,QCursor,QToolBar,QDialog,\
        QIcon,QDoubleValidator,QListWidgetItem,QPalette,QPixmap,QBrush,QFont\
        
from guidata.qt.QtCore import QSize, QT_VERSION_STR, PYQT_VERSION_STR, \
        Qt,Signal, pyqtSignal,QObject

from guiqwt.colormap import get_colormap_list, get_cmap, build_icon_from_cmap

from guiqwt.tools import (RectangleTool, EllipseTool, HRangeTool, PlaceAxesTool,
                          MultiLineTool, FreeFormTool, SegmentTool, CircleTool,
                          AnnotatedRectangleTool, AnnotatedEllipseTool,
                          AnnotatedSegmentTool, AnnotatedCircleTool, LabelTool,
                          AnnotatedPointTool,ColormapTool,
                          VCursorTool, HCursorTool, XCursorTool,
                          ObliqueRectangleTool, AnnotatedObliqueRectangleTool)


import guiqwt
from guiqwt.baseplot import BasePlot
from guiqwt.transitional import QwtLinearColorMap, QwtInterval,\
        QwtPlotCurve,QwtPlotItem

from guiqwt.plot import ImageWidget,CurveDialog,PlotManager
from guiqwt.curve import CurvePlot,  ErrorBarCurveItem,\
        CurveItem#PlotItemList,

from guiqwt.builder import make

# QComboBox,QVBoxLayout, 
from PyQt5.QtGui import (QStandardItemModel,QStandardItem,QScreen\
                         
    )

from marker import UI as ui_main
#from mw1 import Ui_Marker as ui
import sys

class LoginWnd(QMainWindow):
    def __init__(self,parent=None):
        super().__init__()
        self.setupUI()
        
    def setupUI(self):
#        lot = QGridLayout()
        self.setWindowTitle('登陆系统')
        self.desktop = QApplication.desktop()
        self.screenRect = self.desktop.screenGeometry()
        self.height = int(self.screenRect.height()*0.75)
        self.width = int(self.screenRect.width()*0.75)
        print('window size:',self.height,self.width)
        self.resize(self.width,self.height)
        
        wgt = QWidget()
        label0 = QLabel('电子情报分析管理系统',wgt)
        font = QFont()
        font.setFamily("宋体")
        font.setPointSize(45)
        font.setBold(True)
        font.setWeight(75)
        label0.setFont(font)
        label0.move(1920/8*3-350,200)
        label1 = QLabel('用户名',wgt)
        label1.move(1920/8*3-50,600)
        label2 = QLabel('密码',wgt)
        label2.move(1920/8*3-50,620)
        self.le_user = QLineEdit('admin',wgt)
        self.le_passwd =QLineEdit('123456',wgt)
        self.le_passwd.setEchoMode(QLineEdit.Password)
        self.le_user.move(1920/8*3+50,600)
        self.le_passwd.move(1920/8*3+50,620)
        self.btn_login = QPushButton('登陆',wgt)
        self.btn_login.move(1920/8*3,640)
        
        self.setCentralWidget(wgt)
        self.btn_login.clicked.connect(self.login_func)
        
        pix = QPixmap('D:\lkc备份\J6A_1\J6A_1\chenlin\chenlin_LB_20141028\FPGA\com_input_FIFO_wave0.jpg')
        pal = QPalette()
        
        pal.setBrush(QPalette.Background,QBrush(pix))
        self.setPalette(pal)
        self.show()
        
    def login_func(self):
        user = self.le_user.text()
        passwd = self.le_passwd.text()
        if user==''and passwd=='':
            QMessageBox.warning(self,'error',"输入用户名及密码!",QMessageBox.Ok)
            return
        if user=='admin'and passwd=='123456':
            self.close()
            ui_main()
        else:
            QMessageBox.warning(self,'warning',"用户名或密码错误!",QMessageBox.Ok)
            
            
            
if __name__=='__main__':

    app = QApplication(sys.argv)

    ex = LoginWnd()
    
    sys.exit(app.exec_())