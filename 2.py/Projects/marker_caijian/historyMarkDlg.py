#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Mar 13 16:16:21 2020

@author: admin
"""

import sys
import numpy as np
import pandas as pd

import time
import hashlib
import os
import traceback
import struct
from datetime import datetime

from PyQt5.QtWidgets import (QApplication, QWidget, QToolTip,QColorDialog,
     QPushButton, QMainWindow, QAction, QDockWidget, QMessageBox,
     QFileDialog, QGridLayout, QLabel, QHBoxLayout,QVBoxLayout,
     QLineEdit, QTableView, QInputDialog, QRadioButton,QHeaderView,
     QTabWidget, QSizePolicy, QCheckBox,QMenu,QAbstractItemView)

from guidata.qt.QtGui import QSplitter, QListWidget,QCursor,QToolBar,QDialog,\
        QIcon
from guidata.qt.QtCore import QSize, QT_VERSION_STR, PYQT_VERSION_STR, Qt


from guiqwt.tools import (RectangleTool, EllipseTool, HRangeTool, PlaceAxesTool,
                          MultiLineTool, FreeFormTool, SegmentTool, CircleTool,
                          AnnotatedRectangleTool, AnnotatedEllipseTool,
                          AnnotatedSegmentTool, AnnotatedCircleTool, LabelTool,
                          AnnotatedPointTool,
                          VCursorTool, HCursorTool, XCursorTool,
                          ObliqueRectangleTool, AnnotatedObliqueRectangleTool)


import guiqwt
from guiqwt.baseplot import BasePlot
from guiqwt.transitional import QwtLinearColorMap, QwtInterval,\
        QwtPlotCurve,QwtPlotItem
from guiqwt.config import _
from guiqwt.plot import ImageWidget,CurveDialog,PlotManager
from guiqwt.curve import CurvePlot, PlotItemList
from guiqwt.colormap import get_colormap_list, get_cmap, build_icon_from_cmap
from guiqwt.builder import make
import qwt

# QComboBox,QVBoxLayout, 
from PyQt5.QtGui import (QStandardItemModel,QStandardItem,QScreen,\
                         QBrush,  QColor ,  QFont 
    )
from PyQt5.QtCore import pyqtSignal,QObject,Qt

#sys.path.append('D:\\py\\xuexiyige\\ui\\net\\')
#sys.path.append('D:\\py\\xuexiyige\\statistics\\')
from analysis import Ui_Marker as ui
import xml.etree.ElementTree as ET

from guiqwt.shapes import Marker
from guiqwt.config import CONF, _
from guiqwt.styles import GridParam
from guiqwt.curve import GridItem
import popwindow

import historyMark


class histdlg(historyMark.Ui_Dialog,QDialog):
    def __init__(self):
        super().__init__()
        self.init()
    def init(self):
        self.setupUi(self)
        self.changeWgt()
        
        self.btn_chooseFile.clicked.connect(self.chooseFile)
        self.setWindowModality(Qt.ApplicationModal)
#        self.pdwWgt.show()
#        self.show()
    def chooseFile(self):
        fname = QFileDialog.getOpenFileName(self,'Open file',os.curdir,\
                                                ';;data(*.data)')        
        if(fname[0] == ''):
            return
        if fname == -1:
            return
        self._OpenPdw(fname[0])
        os.chdir(os.path.split(fname[0])[0])#改变当前目录
        
    def _OpenPdw(self,name):
        fname = [name,0]
        os.chdir(os.path.split(fname[0])[0])#改变当前目录

        filename = os.path.split(name)[1]
        self.filename = filename.split('.')[0]

        self.first_flag = 1
        self.tick = 0
        self.MarkProgress = 0
        self.le_filename.setText(name)
        
    def changeWgt(self):
        self.pdwWgt = popwindow.PdwDrawWgt(4,1,4)
        self.le_filename.deleteLater()
        self.le_filename = popwindow.LineEdit()
        self.gridLayout.addWidget(self.le_filename,0,3)
        self.gridLayout.addWidget(self.pdwWgt,5,0,1,7)
        self.tableView_trgt.deleteLater()
        self.tableView_trgt = popwindow.tableview()
        self.gridLayout.addWidget(self.tableView_trgt,2,0,1,3)
        
        
if __name__=='__main__':

    app = QApplication(sys.argv)

    ex=histdlg()

    sys.exit(app.exec_())