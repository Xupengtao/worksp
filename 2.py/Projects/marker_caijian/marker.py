# -*- coding: utf-8 -*-
"""
Created on Wed Jan  3 16:43:57 2018

@author: qinchanghai
"""

import sys
import numpy as np
import pandas as pd

import time
import hashlib
import os
import traceback
import struct
from   datetime import datetime

from PyQt5.QtWidgets import (QApplication, QWidget, QToolTip,QColorDialog,
     QPushButton, QMainWindow, QAction, QDockWidget, QMessageBox,
     QFileDialog, QGridLayout, QLabel, QHBoxLayout,QVBoxLayout,QStackedLayout,
     QLineEdit, QTableView, QInputDialog, QRadioButton,QHeaderView,
     QTabWidget, QSizePolicy, QCheckBox,QMenu,QAbstractItemView)
from guidata.qt.QtGui import QCursor,QToolBar, QIcon
from guidata.qt.QtCore import QSize, Qt
import guiqwt
from guiqwt.transitional import QwtInterval
from guiqwt.plot import PlotManager
from guiqwt.colormap import get_cmap, build_icon_from_cmap
from guiqwt.builder import make

from PyQt5.QtGui import (QStandardItemModel, QStandardItem, QBrush, QColor)
from PyQt5.QtCore import pyqtSignal

from analysis import Ui_Marker as ui

import popwindow

timestp = datetime(2018,1,1,0,0,0)
windows = 1
t1=time.time()

file_info_index=['filename','md5','time']
xygrpt_index = ['批号','模式代码','方位','幅度','脉宽类型','脉宽',\
        '脉宽最大值','脉宽最小值','载频类型','载频1',\
        '载频2','载频3','载频4','载频5','载频6',\
        '载频7','载频8','载频9','载频10','载频11',\
        '载频12','载频13','载频14','载频15','载频16',\
        '载频17','载频18','载频19','载频20','载频21',\
        '载频22','载频23','载频24','载频25','载频26',\
        '载频27','载频28','载频29','载频30','载频31',\
        '载频32','载频最大值','载频最小值','载频QRF','载频FAR',\
        '重频类型','重复间隔1','重复间隔2','重复间隔3','重复间隔4',\
        '重复间隔5','重复间隔6','重复间隔7','重复间隔8','重复间隔9',\
        '重复间隔10','重复间隔11','重复间隔12','重复间隔13','重复间隔14',\
        '重复间隔15','重复间隔16','重频最大值','重频最小值','重频FPI']

rownum = -1

class DataSource():
    xd = 'xd'
    struct = 'struct'
    xyg_c = 'xxg'
    xyg_z = 'xxg_z'
    x5_dgn = 'X5_DGN'
    
class ReadMode():
    time = 'time'
    plsnum = 'plsnum'
    
def calc_hash(file):
    try:
        str(file)
    except:
        return -1
    return hashlib.md5(file).hexdigest()
        
class tablevue(QTableView):
    click_sig = pyqtSignal(int)
    def __init__(self,parent=None):
        super(tablevue,self).__init__(parent)
        self.setContextMenuPolicy(Qt.CustomContextMenu)
        self.customContextMenuRequested.connect(self.showMenu)
        self.contextMenu = QMenu(self)
        self.m_draw_action = self.contextMenu.addAction('draw')
        
    def showMenu(self,pos):
        self.contextMenu.exec_(QCursor.pos())

    def mousePressEvent(self,event):
        global rownum
        pos = event.pos()
        item = self.indexAt(pos)
        if item:
            print('at %d %d'%(item.row(),item.column()))
        try:
            rownum = self.model().item(item.row(),0).text()
            rownum = int(rownum)
            self.click_sig.emit(rownum)
            print(rownum)
        except:
            traceback.print_exc()
            return
        QTableView.mousePressEvent(self,event)

import psutil
def judgeprocess(processname):
    if windows:
        try:
            print('tasklist|findstr'+processname)
            process = len(os.popen('tasklist | findstr '+processname).readlines())
            print(process)
            if process >=1:
                return 1
            else:
                return 0
        except:
            print('error')
            return 0
    else:
        pl = psutil.pids()
        for pid in pl:
            if psutil.Process(pid).name() == processname:
                print('%s is running'%processname)
                os.system('kill %d'%pid)
                print('%d killed'%pid)
                return 1
        print('%s not found'%processname)
        return 0
        
class UI(QMainWindow,ui):
    def __init__(self,parent=None):
        super(QMainWindow,self).__init__(parent)
        self.setupUi(self)
        self.initUI()
        
    def test(self):
        QMessageBox.warning(self,'warning','head!')
        self.file_name_le.setText('test ok!')

    def initUI(self):
        global g_edp_sys
        self.path = os.curdir
        self.init_style()
        self.init_state = 1
        self.cur_tick = 0
        self.first_flag = 1
        self.autoSaveFlag = 1
        self.fullsee = popwindow.Draw_Wgt_C(4,1,4)
        self.fullsee.setObjectName('customWidget')
        self.fullsee.setStyleSheet(self.styleSheet)
    ##  部件初始化区    
        self.prg_Slider.setMaximum(100)
        self.prg_Slider.setMinimum(1)
        self.prg_Slider.setSingleStep(1)
        self.tot_LE.setText(str(self.prg_Slider.maximum()))
        self.tot_LE.setEnabled(0)
        self.changeWgt()
#        self.init_histmark()
        self.init_PDWFilter()
        ###绘图区
        self.init_draw()
        ###编码设置
        if windows:
            self.encode = 'gbk'
        else:
            self.encode = 'utf-8'
#        self.init_backprocess()
        self.Origin_Data_wnd = popwindow.OriginDataParseWnd()
        self.Origin_Data_wnd.setObjectName('centerWidget')
        self.Origin_Data_wnd.setStyleSheet(self.styleSheet)
        self.Origin_Data_wnd.resize(QSize(960,540))
        self.matrix_wnd = popwindow.MatrixSearchWnd()
        self.matrix_wnd.setObjectName('mainWindow')
        self.matrix_wnd.setStyleSheet(self.styleSheet)
        self.matrix_data_df = pd.DataFrame()
        self.init_connect()
        self.init_data()
        self.init_le()
#        sp.cvar.g_var.trgt.nTrgt = 0
        self.tick = 0
        self.runFlag = 1
#        self.setWindowOpacity(0.95)
#        self.setWindowFlags(Qt.FramelessWindowHint)
#        self.setAttribute(Qt.WA_TranslucentBackground)
#        sp.cvar.g_var.DebugModeFlag = 0
        self.ChooseReadTime()
        self.ChooseStruct()

        self.info_dock.setObjectName('centerWidget')
        self.draw_dock.setObjectName('centerWidget')
        self.info_dock.setStyleSheet(self.styleSheet)
        self.draw_dock.setStyleSheet(self.styleSheet)
        self.splitDockWidget(self.info_dock,self.draw_dock,Qt.Horizontal)
        self.desktop = QApplication.desktop()
        self.screenRect = self.desktop.screenGeometry()
        self.height = int(self.screenRect.height()*0.75)
        self.width = int(self.screenRect.width()*0.75)
        print('window size:',self.height,self.width)
        self.resize(self.width,self.height)
        self.setStyleSheet(self.styleSheet)
        self.init_toolbar()
        self.Origin_Data_wnd.show()
#        self.show()

    def changeWgt(self):
        #更换显示部件
        self.edw_tableView.deleteLater()
        self.edw_tableView = tablevue(self.tab)
        self.verticalLayout_4.addWidget(self.edw_tableView)
        
        self.res_tableView.deleteLater()
        self.res_tableView = tablevue(self.tab)
        self.verticalLayout.addWidget(self.res_tableView)
        
        self.file_name_le.deleteLater()
        currentdir = os.path.dirname(os.path.realpath(__file__))
        name = os.path.join(currentdir,'pdw_data.data')
        self.file_name_le = popwindow.LineEdit(name)
#        self._OpenPdw(name)
        self.file_name_le.dropEvent = self.dropEvent
        self.gridLayout_6.addWidget(self.file_name_le,1,3,1,3)
        
    def init_style(self):
        currentdir = os.path.dirname(os.path.realpath(__file__))
        name = os.path.join(currentdir,'white.qss')
        with open(name,encoding='utf-8') as file:
            style = file.readlines()
            style =''.join(style)#.strip('\\n').strip('\n')
        self.styleSheet = style
        self.setObjectName('mainWindow')
        
    def init_data(self):
        #数据初始化区
        self.pdw_df = pd.DataFrame(columns=['RF','DOA','PA','PW','TOA'])
        self.pdw_trans_df = pd.DataFrame(columns=['RF','DOA','PA','PW','TOA'])
        self.mark_pdw_df = pd.DataFrame(columns=['RF','DOA', 'PA','PW','TOA','FLAG','TRGT_NO'])
        self.edw_df = pd.DataFrame(columns=xygrpt_index)
        self.ori_pdw_df = pd.DataFrame()
        self.ori_flt_pdw_df = pd.DataFrame()
        self.flt_pdw_df = pd.DataFrame()
        self.flt_edw_df = pd.DataFrame()
        self.cur_pdw_df = pd.DataFrame()
        self.colormap = guiqwt.colormap.get_cmap('spectral')
#        self.colormap.rgb(QwtInterval(0,255),value)
        
    def init_backprocess(self):
         ####启动服务
        currentdir = os.path.dirname(os.path.realpath(__file__))
        import subprocess
        if windows:
            if judgeprocess('q.exe'):
                cmd = 'taskkill /f /im q.exe'
                os.system(cmd)
            cmd = os.path.join(currentdir,'q.exe')
        else:
            if judgeprocess('q'):
                cmd = 'killall q'#os.path.join(currentdir,'q')
                os.system(cmd)
            cmd = os.path.join(currentdir,'q')
        subprocess.Popen(cmd,shell=1)

        os.chdir(currentdir)
        
    def init_PDWFilter(self):
        self.pdwFilter = popwindow.Pdw_Filter()
        self.pdwFilter.setStyleSheet(self.styleSheet)
        self.pdwFilter.cb = ['RF','DOA','PW','PA']
        self.pdwFilter.reset_filter()
        self.pdwFilter.le_min[0].setText('380')
        self.pdwFilter.le_min[1].setText('0')
        self.pdwFilter.le_min[2].setText('0')
        self.pdwFilter.le_min[3].setText('0')
        self.pdwFilter.le_max[0].setText('40000')
        self.pdwFilter.le_max[1].setText('360')
        self.pdwFilter.le_max[2].setText('1000')
        self.pdwFilter.le_max[3].setText('1000')
        
    def init_le(self):
        self.st_rf_min, self.st_rf_max = 380, 40000
        self.st_doa_min, self.st_doa_max = 0, 360
        self.st_pa_min, self.st_pa_max = 0, 255
        self.st_pw_min ,self.st_pw_max = 0, 1024
        self.st_rf_min_le.setText(str(self.st_rf_min))
        self.st_rf_max_le.setText(str(self.st_rf_max))
        self.st_doa_min_le.setText(str(self.st_doa_min))
        self.st_doa_max_le.setText(str(self.st_doa_max))
        self.st_pa_min_le.setText(str(self.st_pa_min))
        self.st_pa_max_le.setText(str(self.st_pa_max))
        self.st_pw_min_le.setText(str(self.st_pw_min))
        self.st_pw_max_le.setText(str(self.st_pw_max))
        self.prg_LE.setText('0')
        
    def init_toolbar(self):
        self.toolbar = QToolBar()
        self.addToolBar(self.toolbar)
        self.stopaction = QAction(self)
        self.toolbar.addAction(self.stopaction)
        self.stopaction.setIcon(QIcon('start.png'))
        self.stopaction.triggered.connect(self.setFlag)
        self.stopaction.setToolTip('连续处理')
        SingleStepAction = QAction(self)
        self.toolbar.addAction(SingleStepAction)
        SingleStepAction.setIcon(QIcon('ARW03RT.ICO'))
        SingleStepAction.setToolTip('单步处理')
        ReAnalyseAction = QAction(self)
        self.toolbar.addAction(ReAnalyseAction)
        ReAnalyseAction.setIcon(QIcon('Reset_24.png'))
        ReAnalyseAction.setToolTip('重新分析')
        self.ColorMapAction = QAction(self)
        self.cmtool = popwindow.colormapTool(self.manager)
        self.toolbar.addAction(self.ColorMapAction)
        self.ColorMapAction.setIcon(build_icon_from_cmap(get_cmap("spectral"),
                                                width=16, height=16))
        self.ColorMapAction.setToolTip('设置绘图颜色映射')
        self.ColorMapAction.triggered.connect(self.showColorMapOption)
        self.cmtool.menu.triggered.connect(self.setColorMap)
        self.drawBgColorAction.triggered.connect(self.setDrawBgColor)
        self.tbBgColorAction.triggered.connect(self.setTableBgColor)
        
    def showColorMapOption(self):
        self.cmtool.menu.exec_(QCursor.pos())
        
    def setColorMap(self, action):
        self.colormap = guiqwt.colormap.get_cmap(action.text())
        self.statusbar.showMessage('current ColorMap:'+action.text())
        self.ColorMapAction.setIcon(build_icon_from_cmap(get_cmap(action.text()),
                                                 width=16, height=16))
        
    def setDrawBgColor(self):
        col = QColorDialog.getColor() 
        if col.isValid(): 
            for i in self.dkw2.cp:
                i.setCanvasBackground(QColor(col.name()))

    def setTableBgColor(self):
        col = QColorDialog.getColor() 
        if col.isValid():
            for tableview in [self.edw_tableView,self.res_tableView,self.pdw_tableView]:
                self.setTableviewColor( tableview, QColor(col.name()))
        
    def setFlag(self):
        if self.runFlag == 0:
            self.runFlag = 1
            self.stopaction.setIcon(QIcon('stop.png'))
        else:
            self.runFlag = 0
            self.stopaction.setIcon(QIcon('start.png'))
            return
        self.SequenceProcess()

    def init_draw(self):
        self.dkw2 = popwindow.Draw_Wgt_C(4,1,4,itemlist=1)
        self.draw_dock.setWidget(self.dkw2)
        self.manager = PlotManager(self)
        self.dkw2.cp[0].set_titles(title='TOA-RF',xlabel='TOA',ylabel='RF')
        self.dkw2.cp[1].set_titles(title='TOA-PA',xlabel='TOA',ylabel='PA')
        self.dkw2.cp[2].set_titles(title='TOA-DOA',xlabel='TOA',ylabel='DOA')
        self.dkw2.cp[3].set_titles(title='TOA-PW',xlabel='TOA',ylabel='PW')
        self.cp1 = self.dkw2.cp[0]
        self.cp2 = self.dkw2.cp[1]
        self.cp3 = self.dkw2.cp[2]
        self.cp4 = self.dkw2.cp[3]
        self.curve1 = make.curve(np.arange(100), np.random.rand(100),\
                    linewidth=3, title='My curve',curvestyle = 'NoCurve')
        self.curve2 = make.curve(np.arange(100), np.random.rand(100), \
                    linewidth=3,title='My curve1',curvestyle = 'NoCurve')
        self.curve3 = make.curve(np.arange(100), np.random.rand(100), \
                            title='My curve1',curvestyle = 'NoCurve')
        self.curve4 = make.curve(np.arange(100), np.random.rand(100), \
                            title='My curve1',curvestyle = 'NoCurve')
        self.cp1.add_item(self.curve1)
        self.cp2.add_item(self.curve2)
        self.cp3.add_item(self.curve3)
        self.cp4.add_item(self.curve4)
        
    def St_Para_Set(self):
        self.st_doa_max = int(self.st_doa_max_le.text())
        self.st_doa_min = int(self.st_doa_min_le.text())
        self.st_pa_min = int(self.st_pa_min_le.text())
        self.st_pa_max = int(self.st_pa_max_le.text())
        self.st_pw_max = int(self.st_pw_max_le.text())
        self.st_pw_min = int(self.st_pw_min_le.text())
        self.st_rf_max = int(self.st_rf_max_le.text())
        self.st_rf_min = int(self.st_rf_min_le.text())
        
    def init_connect(self):
        ##按钮连接
        self.choose_file_btn.clicked.connect(self.OpenPdw)
        self.filter_wnd_btn.clicked.connect(self.pdwFilter.show)
        self.close_file_btn.clicked.connect(self.CloseFile)
        ##事件连接
        self.edw_tableView.click_sig.connect(self.show_edw)
        self.res_tableView.click_sig.connect(self.show_res)
        self.prg_Slider.valueChanged.connect(self.slider2le)
        self.prg_LE.textEdited.connect(self.le2slider)
    ##信号调试区
        self.draw_btn.clicked.connect(self.Show_Full)
        self.draw_hist_btn.clicked.connect(self.draw_all_hist)
        self.matrix_wnd.save_btn.clicked.connect(self.save_maxtrix_search_pdw)
        self.edw_tableView.m_draw_action.triggered.connect(self.DetailShow)
        m_draw_pdw_info = self.edw_tableView.contextMenu.addAction('绘制单部信号')
        m_draw_pdw_info.triggered.connect(self.draw_pdw_info)
        m_save_one_pdw = self.edw_tableView.contextMenu.addAction('保存单部信号')
        m_save_one_pdw.triggered.connect(self.save_one_pdw)
        m_hist_pdw = self.edw_tableView.contextMenu.addAction('绘制本部信号直方图')
        m_hist_pdw.triggered.connect(self.draw_one_hist)
        
        m_hist_pdw_res = self.res_tableView.contextMenu.addAction('绘制本部信号直方图')
        m_hist_pdw_res.triggered.connect(self.draw_one_hist_res)
        
        self.pdwFilter.reset_file_btn.clicked.connect(self.ResetFile)
        self.actionXxg.triggered.connect(self.ChooseXxg)
        self.actionXxg_z.triggered.connect(self.ChooseXxgZ)
        self.actionXd.triggered.connect(self.ChooseXd)
        self.actionX5_DGN.triggered.connect(self.ChooseX5DGN)
        self.actionPlsnum.triggered.connect(self.ChooseReadNum)
        self.actionTime.triggered.connect(self.ChooseReadTime)
        self.actionOrigin_dataparse.triggered.connect(self.Origin_Data_wnd.show)
        
        self.Origin_Data_wnd.ParseBtn.clicked.connect(self.OriParse)
        self.Origin_Data_wnd.SaveBtn.clicked.connect(self.OriSave)
        self.Origin_Data_wnd.DrawBtn.clicked.connect(self.OriDraw)
        self.Origin_Data_wnd.filterbox.filterbtn.clicked.connect(self.OriFilter)
        self.Origin_Data_wnd.DrawMoreWnd.okBtn.clicked.connect(self.OriSDraw)
        
    def OpenTgt(self):
        fname = QFileDialog.getOpenFileName(self,'Open file',os.curdir,\
                                                'trgt(*.xlsx);;(*.csv);;all(*.*)')
        if(fname[0] == ''):
            return
        if fname == -1:
            return
        filename = fname[0]
        suffix = filename.split('.')[-1]
        if suffix == 'xlsx':
            print('xlsx file!')
            f = pd.read_excel
        elif suffix == 'csv':
            print('csv file!')
            f = pd.read_csv
        else:
            return
        self.trgtList = f(filename)
        self.update(self.histMark.tableView_trgt,self.trgtList,ctc = 1)
        
    def pdwWgtDraw(self, pdw, wgt, num = 4):
        #### pdw绘制到pdwwgt
        for i in range(len(wgt.cp)):
            self.histMark.pdwWgt.cp[i].del_all_items()
        _pdw = pdw[pdw.FLAG == 0]
        cv = make.curve(_pdw.TOA,_pdw.RF,color='g',curvestyle='Dots',\
                        linewidth = 3)
        wgt.cp[0].add_item(cv)
        cv = make.curve(_pdw.TOA,_pdw.PA,color='g',curvestyle='Dots',\
                        linewidth = 3)
        wgt.cp[1].add_item(cv)
        cv = make.curve(_pdw.TOA,_pdw.DOA,color='g',curvestyle='Dots', linewidth = 3)
        wgt.cp[2].add_item(cv)
        cv = make.curve(_pdw.TOA,_pdw.PW,color='g',curvestyle='Dots', linewidth = 3)
        wgt.cp[3].add_item(cv)
        colors = []
        typenum = max(pdw.FLAG)
        print('color nums:',typenum)
        for i in range(typenum):
            _pdw = pdw[pdw.FLAG == i+1]
            colors.append(self.colormap.rgb(QwtInterval(0,255), i*(int(255/typenum+2))))
            cv = make.curve(_pdw.TOA,_pdw.RF,color=colors[i],curvestyle='Dots', linewidth = 3)
            wgt.cp[0].add_item(cv)
            cv = make.curve(_pdw.TOA,_pdw.PA,color=colors[i],curvestyle='Dots', linewidth = 3)
            wgt.cp[1].add_item(cv)
            cv = make.curve(_pdw.TOA,_pdw.DOA,color=colors[i],curvestyle='Dots', linewidth = 3)
            wgt.cp[2].add_item(cv)
            cv = make.curve(_pdw.TOA,_pdw.PW,color=colors[i],curvestyle='Dots', linewidth = 3)
            wgt.cp[3].add_item(cv)
        for i in range(len(wgt.cp)):
            self.histMark.pdwWgt.cp[i].do_autoscale(replot=1)
        self.histMark.pdwWgt.cp[0].do_autoscale(replot=1)

    def CloseFile(self):
        try:
            self.f2.close()
            self.statusbar.showMessage('关闭文件!')
        except:
            return
            
    def ChooseStruct(self):
        self.DataSource = 'struct'
        self.actionStruct.setChecked(1)
        self.statusBar().showMessage('Data Source struct!')
        self.actionXxg.setChecked(0)
    def ChooseXxg(self):
        self.DataSource = 'xxg'
        self.actionXxg.setChecked(1)
        self.statusBar().showMessage('Data Source xxg!')
        self.actionStruct.setChecked(0)
    def ChooseXxgZ(self):
        self.DataSource = 'xxg_z'
        self.actionXxg.setChecked(1)
        self.statusBar().showMessage('Data Source xxg_z!')
        self.actionStruct.setChecked(0)
    def ChooseXd(self):
        self.DataSource = 'xd'
        self.statusBar().showMessage('Data Source xd!')
    def ChooseX5DGN(self):
        self.DataSource = 'X5_DGN'
        self.actionX5_DGN.setChecked(1)
        self.statusBar().showMessage('Data Source X5_DGN!')
        self.actionStruct.setChecked(0)
    def ChooseReadTime(self):
        self.ReadMode = ReadMode.time
        self.unit_label.setText('ms')
        self.pdw_num_le.setPlaceholderText('请输入时间/us:(输入小于10ms)')
        self.statusBar().showMessage('按时间读取脉冲!')
    def ChooseReadNum(self):
        self.ReadMode = ReadMode.plsnum
        self.unit_label.setText('个')
        self.pdw_num_le.setPlaceholderText('请输入脉冲数')
        self.statusBar().showMessage('按脉冲数读取!')
    
    def parse_xyg_z(self,name):
        file = open(name,'rb')
        length = os.stat(name)[6]
        RF,DOA,PA,PW,TOA,PRI,TOA_all = [],[],[],[],[],[],[]
        FLAG1,FLAG2,FLAG3,FLAG4,FLAG5,FLAG6 = [],[],[],[],[],[]
        FLAG04,FLAG7,FLAG11,FLAG12,FLAG13,FLAG14,FLAG15 = [],[],[],[],[],[],[]
        SecCntCyc,BANDNUM,TIME,BW,SHIP_ANGLE,SENSITIVITY = [],[],[],[],[],[]
        BENZHEN,DIREC_P = [],[]
        pdw_col = ['RF/MHz','DOA/deg','PA/dBm','PW/us','TOA/us','PRI/us',\
                    'DOAflag','TOAflag','CWflag','Watchflag','Jamflag','PWflag',\
                    'FUBANFlag','rfSt','DoaSt','ToaInvalid','PaInvalid','rfProt','ProPulse',\
                    'TOA_all/us','SecCntCyc/us','bandnum','TIME',\
                    '带宽','航向角','灵敏度','HESM本振码','方位指向']
        self.ori_pdw_df = pd.DataFrame()
        prog_flag,firstflag = 0,0
        self.statusBar().showMessage('解析开始!',5)
        turn_no,last_toa = 0,0
        err_num1,err_num2 = 0,0
        while 1:
            try:
                (head,flag,bak1,pdwcount_l,pdwcount_h,toal,toah,rf,bw,doa,tr_flag,\
             pa,pwl,pwh,ship_angle,add1_1,add1_2,sensitivity,benzhen,direc_p,seccnt1,seccnth,\
             add2_0,add2_1,add2_2,add2_3,add2_4,add2_5,add2_6,add2_7,bak2,tail)=\
             struct.unpack('H'*32,file.read(64))
            except:
                file.close()
                break
                
            if head != 0xaa55 or tail != 0x55aa:
                err_num1 += 1
                continue
            new_toa = (toal+(toah<<16))*80/1000
            if new_toa == 0:
                err_num2 += 1
            DOA.append('%.2f'%(doa/1024*360))
            PW.append('%.2f'%((pwl+(pwh<<16))*20/1000))
            yy,mm,dd = ((bak1&0xfff0)>>4),(bak1&0xf),(pdwcount_l&0xff00)>>8
            hh,m,ss = (pdwcount_l&0xff),(pdwcount_h&0xff00)>>8,pdwcount_h&0xff
            try:
                _time = datetime(yy,mm,dd,hh,m,ss)
            except:
                _time = datetime(2019,1,1,0,0,0)
            TOA.append('%.2f'%new_toa)
            
            if firstflag == 0:
                PRI.append(0)
                firstflag = 1
                print(add2_4,add2_5,add2_6,add2_7)
            else:
                if (last_toa <= new_toa) or ((last_toa > new_toa) & (last_toa-new_toa < 0xefffffff/12.5)):
                    PRI.append('%.2f'%(new_toa-last_toa))
                else:
                    PRI.append('%.2f'%(0xffffffff/12.5 + new_toa - last_toa))
                    turn_no += 1
            TOA_all.append('%.2f'%(new_toa+0xffffffff/12.5*turn_no))
            last_toa = new_toa
            bandnum = flag & 0x0f
            if bandnum <= 5:
                RF.append(rf/2)
            elif bandnum == 6:
                RF.append(34000-rf/2)
            else:
                RF.append(30000+rf/2)

            SecCntCyc.append((add2_4+add2_5*(2**16)+add2_6*(2**32)+add2_7*(2**48))*80/1000)
            BANDNUM.append(bandnum)
            BW.append(bw/2)
            TIME.append(_time)

            if bandnum == 1: ##0.38-2
                DOAflag = (flag & 0x40)>>6
                TOAflag = (flag & 0x10)>>4
                CWflag  = (flag & 0x20)>>5
                Watchflag =(flag & 0x100)>>8
                Jamflag   = (flag & 0x200)>>9
                PWflag  = (flag & 0x400)>>10
                pa     =  pa/2
            elif bandnum == 5:
                DOAflag = (flag & 0x40)>>6
                TOAflag = (flag & 0x10)>>4
                CWflag  = (flag & 0x20)>>5
                Watchflag =(flag & 0x100)>>8
                Jamflag   = (flag & 0x200)>>9
                PWflag  = (flag & 0x400)>>10
                pa     =  pa/2
            else :
                DOAflag = (flag & 0x40)>>6
                TOAflag = (flag & 0x800)>>10
                CWflag  = (flag & 0x20)>>5
                Watchflag =(flag & 0x100)>>8
                Jamflag   = (flag & 0x200)>>9
                PWflag  = (flag & 0x400)>>10
                pa     =  pa/3
                
            flag04 = (flag & 0x10)>>4#rfSt
            flag7 = (flag & 0x80)>>7#fuban
            flag11 = (flag & 0x800)>>11#doaSt
            flag12 = (flag & 0x1000)>>12#ToaInvalid
            flag13 = (flag & 0x2000)>>13#PaInvalid
            flag14 = (flag & 0x4000)>>14#rfProt
            flag15 = (flag & 0x8000)>>15#ProPulse
            
            FLAG04.append(flag04)
            FLAG7.append(flag7)
            FLAG11.append(flag11)
            FLAG12.append(flag12)
            FLAG13.append(flag13)
            FLAG14.append(flag14)
            FLAG15.append(flag15)

            FLAG1.append(int(DOAflag))
            FLAG2.append(int(TOAflag))
            FLAG3.append(int(CWflag))
            FLAG4.append(int(Watchflag))
            FLAG5.append(int(Jamflag))
            FLAG6.append(int(PWflag))
            PA.append(int(pa))
            SHIP_ANGLE.append(ship_angle/1024*360)
            SENSITIVITY.append(sensitivity)
            BENZHEN.append(benzhen)
            DIREC_P.append(direc_p)
            progress = (file.tell()/length)*100
            if ( progress % 10) < 2:
                if( prog_flag == 0):
                    self.statusBar().showMessage('进度%.2f'%progress)
                    self.Origin_Data_wnd.ProgBar.setValue(progress)
                    prog_flag = 1
            else:
                prog_flag = 0
            QApplication.processEvents()
        dst = [RF,DOA,PA,PW,TOA,PRI,FLAG1,FLAG2,FLAG3,FLAG4,FLAG5,FLAG6,\
               FLAG04,FLAG7,FLAG11,FLAG12,FLAG13,FLAG14,FLAG15,\
               TOA_all,SecCntCyc,BANDNUM,TIME,BW,SHIP_ANGLE,SENSITIVITY,BENZHEN,DIREC_P]
        print('error num:',err_num1,err_num2)
        for idx,i in enumerate(pdw_col):
            self.ori_pdw_df[i] = dst[idx]
        
    def OriParse(self):
        file_name = self.Origin_Data_wnd.le_filename.text()#self.file_name_le.text()
        fmt_choose = self.Origin_Data_wnd.ChooseSrcCb.currentText()
        if file_name == '':
            return
        if fmt_choose == 'xyg-z':
            self.parse_xyg_z(file_name)
            self.ori_pdw_df[['PW/us','PA/dBm','DOA/deg','RF/MHz','TOA/us','PRI/us','TOA_all/us','SecCntCyc/us']] = \
            self.ori_pdw_df[['PW/us','PA/dBm','DOA/deg','RF/MHz','TOA/us','PRI/us','TOA_all/us','SecCntCyc/us']].apply(pd.to_numeric)
            self.statusBar().showMessage('解析结束!',5)
            self.Origin_Data_wnd.ProgBar.setValue(100)
            self.update(self.Origin_Data_wnd.tableView,self.ori_pdw_df,0)
            print('parse end')
        elif fmt_choose == 'xyg_z_tpedw':
            self.parse_xyg_tpedw(file_name)
        else:
            return
        self.filename = file_name
        self.Origin_Data_wnd.DrawMoreWnd.sdefine_x_cb.clear()
        self.Origin_Data_wnd.DrawMoreWnd.sdefine_y_cb.clear()
        for i in  self.ori_pdw_df.columns:
            self.Origin_Data_wnd.DrawMoreWnd.sdefine_x_cb.addItem(i)
            self.Origin_Data_wnd.DrawMoreWnd.sdefine_y_cb.addItem(i)
        self.Origin_Data_wnd.SaveMoreWnd.checkbox = []

        self.Origin_Data_wnd.SaveMoreWnd.lot.setParent(None)
        count = self.Origin_Data_wnd.SaveMoreWnd.lot.count()
#        print(count)
        for i in range(count):
            item = self.Origin_Data_wnd.SaveMoreWnd.lot.takeAt(count-i-1)
            self.Origin_Data_wnd.SaveMoreWnd.lot.removeWidget(item.widget())
            item.widget().deleteLater()
#            self.Origin_Data_wnd.SaveMoreWnd.lot = None
#        print(self.Origin_Data_wnd.SaveMoreWnd.wndlayout.children())
        self.Origin_Data_wnd.SaveMoreWnd.wndlayout.removeItem(self.Origin_Data_wnd.SaveMoreWnd.lot)
        self.Origin_Data_wnd.SaveMoreWnd.lot.deleteLater()
#        print(self.Origin_Data_wnd.SaveMoreWnd.wndlayout.children())
        self.Origin_Data_wnd.SaveMoreWnd.lot = QGridLayout()
        self.Origin_Data_wnd.SaveMoreWnd.wndlayout.addLayout(self.Origin_Data_wnd.SaveMoreWnd.lot,2,1,1,2)
        for idx,i in  enumerate(self.ori_pdw_df.columns):
            check = QCheckBox(i)
            self.Origin_Data_wnd.SaveMoreWnd.checkbox.append(check)
            if idx < 6: check.setChecked(1)
            self.Origin_Data_wnd.SaveMoreWnd.lot.addWidget(QLabel(i),idx+1,1)
            self.Origin_Data_wnd.SaveMoreWnd.lot.addWidget(check,idx+1,2)
#        self.Origin_Data_wnd.SaveMoreWnd.InitConnect()
         
    def parse_xyg_tpedw(self, filename):
        
        with open(filename,'r') as f:
            buffer = f.readlines()
        progress = 0
        length = len(buffer)
        col = ['batch','systick','refer','class_type','merge_type','rank1',\
               'rank2','rank3','rank4','rank5','nStary','level','nRaw',\
               'rf_type','rf_min','rf_max','rf_n','pri_type','pri_min',\
               'pri_max','pri_n','doa_mode','pw_mode','pw_type','pa_max',\
               'isDP2','trgt_no','DBno','cMode','DataBaseMatch','boxnum_knw',\
               'isTR']
        src = [[] for i in range( len(col) )]
#        dst = [[] for i in range(len(col))]
        while((progress+32) <= length):
#            edw = struct.unpack('<32I',buffer[progress:progress+32])
            for i in range( len(col) ):
                src[i].append(int(buffer[progress+i]))
            progress += 32
        self.ori_pdw_df = pd.DataFrame()
        for idx,i in enumerate(col):
            self.ori_pdw_df[i] = src[idx]
#        self.ori_pdw_df = edw_df
        self.update(self.Origin_Data_wnd.tableView,self.ori_pdw_df,1)
        
    def OriSave(self):
        try:
            da = self.filename#now.strftime('%b_%d_%H_%M')
        except:
            return
        if self.Origin_Data_wnd.save_flt_cb.isChecked():
            df = self.ori_flt_pdw_df
            if self.Origin_Data_wnd.filterbox.flt_rf_cb.isChecked():
                rf_max,rf_min = self.Origin_Data_wnd.filterbox.rf_most_le.text(),\
                        self.Origin_Data_wnd.filterbox.rf_least_le.text()
                da += '_RF'+rf_min+'to'+rf_max
            if self.Origin_Data_wnd.filterbox.flt_doa_cb.isChecked():
                doa_max,doa_min = self.Origin_Data_wnd.filterbox.doa_most_le.text(),\
                        self.Origin_Data_wnd.filterbox.doa_least_le.text()
                da += '_DOA'+doa_min+'to'+doa_max
            if self.Origin_Data_wnd.filterbox.flt_pw_cb.isChecked():
                pw_max,pw_min = self.Origin_Data_wnd.filterbox.pw_most_le.text(),\
                        self.Origin_Data_wnd.filterbox.pw_least_le.text()
                da += '_PW'+pw_min+'to'+pw_max
            if self.Origin_Data_wnd.filterbox.flt_pa_cb.isChecked():
                pa_max,pa_min = self.Origin_Data_wnd.filterbox.pa_most_le.text(),\
                        self.Origin_Data_wnd.filterbox.pa_least_le.text()
                da += '_PA'+pa_min+'to'+pa_max
        else:
            try:
                df = self.ori_pdw_df
            except:
                return
        save_list = []
        for i in self.Origin_Data_wnd.SaveMoreWnd.checkbox:
            if i.isChecked():
                save_list.append(i.text())
        try:
            df = df[save_list]
        except KeyError:
            return
        try:
            fname = QFileDialog.getSaveFileName(self.Origin_Data_wnd,\
            'Save pdw file',da,'tables(*.csv);;tables(*.xlsx)')
        except:
            return
        if fname[0]=='' :return
        self.SaveFile(df, fname)
        
    def OriSDraw(self):
        ori_sdraw_wgt = popwindow.Draw_Wgt_C(1,1)
        ori_sdraw_wgt.setWindowTitle('自定义绘制')
        x = self.Origin_Data_wnd.DrawMoreWnd.sdefine_x_cb.currentText()
        y = self.Origin_Data_wnd.DrawMoreWnd.sdefine_y_cb.currentText()
        if self.Origin_Data_wnd.draw_flt_cb.isChecked():
            df = self.ori_flt_pdw_df
        else:
            try:
                df = self.ori_pdw_df
            except:
                return
        try:
            df[[x]] = df[[x]].apply(pd.to_numeric)
            df[[y]] = df[[y]].apply(pd.to_numeric)
        except:
            return
        if x == y:
            ori_sdraw_wgt.cp[0].set_titles(title='NUM-'+x,\
                                              xlabel='NUM',ylabel=x)
            cv = make.curve(np.arange(len(df[x])),df[x],color='b',curvestyle = 'Dots', linewidth = 3)
            ori_sdraw_wgt.cp[0].add_item(cv)
            ori_sdraw_wgt.show()
        else:
            ori_sdraw_wgt.cp[0].set_titles(title=x+'-'+y, xlabel=x, ylabel=y)
            cv = make.curve(df[x],df[y],color='b',curvestyle = 'Dots', linewidth = 3)
            ori_sdraw_wgt.cp[0].add_item(cv)
            ori_sdraw_wgt.show()
        
    def OriDraw(self):
        self.ori_draw_wgt = popwindow.Draw_Wgt_C(3,1,3)
        self.ori_draw_wgt2 =popwindow.Draw_Wgt_C(2,1,2)
        self.ori_draw_wgt.setObjectName('customWidget')
        self.ori_draw_wgt.setStyleSheet(self.styleSheet)
        self.ori_draw_wgt2.setObjectName('customWidget')
        self.ori_draw_wgt2.setStyleSheet(self.styleSheet)
        self.ori_draw_wgt.setWindowTitle('全景图1')
        self.ori_draw_wgt2.setWindowTitle('全景图2')
        if self.Origin_Data_wnd.draw_flt_cb.isChecked():
            try:
                df = self.ori_flt_pdw_df
                df['TOA/us']
            except:
                return
        else:
            try:
                df = self.ori_pdw_df
                df['TOA/us']
            except:
                return
        df['idx'] = np.arange(df.shape[0])
        label = self.Origin_Data_wnd.DrawMoreWnd.flag_cb.currentText()
        _df1 = df.where(df[label]==0).dropna().reset_index(drop=1)
        _df2 = df.where(df[label]==1).dropna().reset_index(drop=1)
        xlabel_cb = self.Origin_Data_wnd.DrawMoreWnd.xlabel_cb.currentText()
        print (xlabel_cb)
        if xlabel_cb == 'NUM':
            _df1 = df.where(df[label]==0).dropna().reset_index(drop=1)
            _df2 = df.where(df[label]==1).dropna().reset_index(drop=1)
            self.ori_draw_wgt.cp[0].set_titles(title ='RF',xlabel='NUM',ylabel='RF/MHz')
            self.ori_draw_wgt.cp[1].set_titles(title ='PA',xlabel='NUM',ylabel='PA/dBm')
            self.ori_draw_wgt.cp[2].set_titles(title ='DOA',xlabel='NUM',ylabel='DOA/deg')
            self.ori_draw_wgt2.cp[0].set_titles(title ='PRI',xlabel='NUM',ylabel='PRI/us')
            self.ori_draw_wgt2.cp[1].set_titles(title ='PW',xlabel='NUM',ylabel='PW/us')
            xlabel1 =  _df1['idx']
            xlabel2 =  _df2['idx']
            print(' xlabel_sp_cb')
        else:
            xlabel1 = _df1[xlabel_cb]
            xlabel2 = _df2[xlabel_cb]
            self.ori_draw_wgt.cp[0].set_titles(title ='TOA-RF',xlabel='TOA/us',ylabel='RF/MHz')
            self.ori_draw_wgt.cp[1].set_titles(title ='TOA-PA',xlabel='TOA/us',ylabel='PA/dBm')
            self.ori_draw_wgt.cp[2].set_titles(title ='TOA-DOA',xlabel='TOA/us',ylabel='DOA/deg')
            self.ori_draw_wgt2.cp[0].set_titles(title ='TOA-PRI',xlabel='TOA/us',ylabel='PRI/us')
            self.ori_draw_wgt2.cp[1].set_titles(title ='TOA-PW',xlabel='TOA/us',ylabel='PW/us')
            print(' not xlabel_sp_cb')
        if not self.Origin_Data_wnd.DrawMoreWnd.flag_draw_cb.isChecked():
            color1 = 'b'
            marker = None
            print(' b')
        else:
            color1 = 'r'
            marker = None#'Ellipse'
            print(' r')
            
        cv11 = make.curve(xlabel1,_df1['RF/MHz'],title='flag=0',color='b',curvestyle = 'Dots', linewidth = 3,marker=marker)
        cv12 = make.curve(xlabel2,_df2['RF/MHz'],title='flag=1',color=color1,curvestyle = 'Dots', linewidth = 3,marker=marker)
        cv21 = make.curve(xlabel1,_df1['PA/dBm'],title='flag=0',color='b',curvestyle = 'Dots', linewidth = 3)
        cv22 = make.curve(xlabel2,_df2['PA/dBm'],title='flag=1',color=color1,curvestyle = 'Dots', linewidth = 3,marker=marker)
        cv31 = make.curve(xlabel1,_df1['DOA/deg'],title='flag=0',color='b',curvestyle = 'Dots', linewidth = 3)
        cv32 = make.curve(xlabel2,_df2['DOA/deg'],title='flag=1',color=color1,curvestyle = 'Dots', linewidth = 3,marker=marker)
        cv41 = make.curve(xlabel1,_df1['PRI/us'],title='flag=0',color='b',curvestyle = 'Dots', linewidth = 3)
        cv42 = make.curve(xlabel2,_df2['PRI/us'],title='flag=1',color=color1,curvestyle = 'Dots', linewidth = 3,marker=marker)
        cv51 = make.curve(xlabel1,_df1['PW/us'],title='flag=0',color='b',curvestyle = 'Dots', linewidth = 3)
        cv52 = make.curve(xlabel2,_df2['PW/us'],title='flag=1',color=color1,curvestyle = 'Dots', linewidth = 3,marker=marker)
        cv = [cv11,cv21,cv31,cv41,cv51]
        cv2 = [cv12,cv22,cv32,cv42,cv52]
        
        if not self.Origin_Data_wnd.DrawMoreWnd.onelayout_cb.isChecked():
            for i in range(3):
                self.ori_draw_wgt.cp[i].del_all_items()
                self.ori_draw_wgt.cp[i].add_item(cv[i])
                self.ori_draw_wgt.cp[i].add_item(cv2[i])
            self.ori_draw_wgt.show()
            for i in range(2):
                self.ori_draw_wgt2.cp[i].del_all_items()
                try:
                    self.ori_draw_wgt2.cp[i].add_item(cv[i+3])
                    self.ori_draw_wgt2.cp[i].add_item(cv2[i+3])
                except:
                    continue
            self.ori_draw_wgt2.show()
        else:
            self.ori_draw_wgt3 = popwindow.Draw_Wgt_C(3,2,5)
            self.ori_draw_wgt3.setObjectName('customWidget')
            self.ori_draw_wgt3.setWindowTitle('全景图')
            if xlabel_cb == 'NUM':
                self.ori_draw_wgt3.cp[0].set_titles(title ='RF',xlabel='NUM',ylabel='RF/MHz')
                self.ori_draw_wgt3.cp[1].set_titles(title ='PA',xlabel='NUM',ylabel='PA/dBm')
                self.ori_draw_wgt3.cp[2].set_titles(title ='DOA',xlabel='NUM',ylabel='DOA/deg')
                self.ori_draw_wgt3.cp[3].set_titles(title ='PRI',xlabel='NUM',ylabel='PRI/us')
                self.ori_draw_wgt3.cp[4].set_titles(title ='PW',xlabel='NUM',ylabel='PW/us')
            else:
                self.ori_draw_wgt3.cp[0].set_titles(title ='TOA-RF',xlabel='TOA/us',ylabel='RF/MHz')
                self.ori_draw_wgt3.cp[1].set_titles(title ='TOA-PA',xlabel='TOA/us',ylabel='PA/dBm')
                self.ori_draw_wgt3.cp[2].set_titles(title ='TOA-DOA',xlabel='TOA/us',ylabel='DOA/deg')
                self.ori_draw_wgt3.cp[3].set_titles(title ='TOA-PRI',xlabel='TOA/us',ylabel='PRI/us')
                self.ori_draw_wgt3.cp[4].set_titles(title ='TOA-PW',xlabel='TOA/us',ylabel='PW/us')
            for i in range(5):
                self.ori_draw_wgt3.cp[i].del_all_items()
                self.ori_draw_wgt3.cp[i].add_item(cv[i])
                self.ori_draw_wgt3.cp[i].add_item(cv2[i])
                if self.Origin_Data_wnd.DrawMoreWnd.flag_draw_cb.isChecked():
                    legend = make.legend()
                    self.ori_draw_wgt3.cp[i].add_item(legend)
            
            self.ori_draw_wgt3.show()
            
        if self.Origin_Data_wnd.DrawMoreWnd.hist_cb.isChecked():
            self.ori_draw_wgt4 = popwindow.Draw_Wgt_C(2,1,0)
            self.ori_draw_wgt4.setObjectName('customWidget')
            self.ori_draw_wgt4.setWindowTitle('直方图')
            self.ori_draw_wgt4.cp[0].set_titles(title ='RF-Hist',xlabel='RF',ylabel='NUM')
            self.ori_draw_wgt4.cp[1].set_titles(title ='DOA-Hist',xlabel='DOA',ylabel='NUM')
            bins = int(max(df['RF/MHz'])-min(df['RF/MHz']+1))//2+1
            print('bins',bins)
            cv1 = make.histogram(df['RF/MHz'],bins = bins)
            cv2 = make.histogram(df['DOA/deg'],bins = range(0,360,4))
            self.ori_draw_wgt4.cp[0].del_all_items()
            self.ori_draw_wgt4.cp[1].del_all_items()
            self.ori_draw_wgt4.cp[0].add_item(cv1)
            self.ori_draw_wgt4.cp[1].add_item(cv2)
            self.ori_draw_wgt4.show()
             
    def OriFilter(self):
        try:
            self.ori_pdw_df
        except AttributeError:
            return
        if self.ori_pdw_df.shape[0] == 0:
            return
        rf_min = self.Origin_Data_wnd.filterbox.rf_limit[0]
        rf_max = self.Origin_Data_wnd.filterbox.rf_limit[1]
        pw_min = self.Origin_Data_wnd.filterbox.pw_limit[0]
        pw_max = self.Origin_Data_wnd.filterbox.pw_limit[1]
        doa_min = self.Origin_Data_wnd.filterbox.doa_limit[0]
        doa_max = self.Origin_Data_wnd.filterbox.doa_limit[1]
        pa_min = self.Origin_Data_wnd.filterbox.pa_limit[0]
        pa_max = self.Origin_Data_wnd.filterbox.pa_limit[1]

        rf_en = self.Origin_Data_wnd.filterbox.flt_rf_cb.isChecked()
        pw_en = self.Origin_Data_wnd.filterbox.flt_pw_cb.isChecked()
        doa_en = self.Origin_Data_wnd.filterbox.flt_doa_cb.isChecked()
        pa_en = self.Origin_Data_wnd.filterbox.flt_pa_cb.isChecked()
        if (
            ( (rf_min > rf_max)&rf_en )|( (pw_min > pw_max)&pw_en )
            |
            ( (doa_min > doa_max)&doa_en )|( (pa_min > pa_max)&pa_en )
            ):
            return
        if self.Origin_Data_wnd.filterbox.filinre.isChecked():
            df = self.ori_flt_pdw_df
            if df.shape[0] == 0: return
        else:
            df = self.ori_pdw_df
        self.ori_flt_pdw_df = df
        if rf_en == 1:
            self.ori_flt_pdw_df = self.ori_flt_pdw_df.where(
             (rf_min <= self.ori_flt_pdw_df['RF/MHz'])&(self.ori_flt_pdw_df['RF/MHz'] <= rf_max) )
        if pw_en == 1:
            self.ori_flt_pdw_df = self.ori_flt_pdw_df.where(
            (pw_min <= self.ori_flt_pdw_df['PW/us'])&(self.ori_flt_pdw_df['PW/us']<= pw_max) )
        if doa_en ==1:
            self.ori_flt_pdw_df = self.ori_flt_pdw_df.where(
             (doa_min <= self.ori_flt_pdw_df['DOA/deg'])&(self.ori_flt_pdw_df['DOA/deg'] <= doa_max))
        if pa_en ==1:
            self.ori_flt_pdw_df = self.ori_flt_pdw_df.where(
            (pa_min <= self.ori_flt_pdw_df['PA/dBm'])&(self.ori_flt_pdw_df['PA/dBm'] <= pa_max))

        self.ori_flt_pdw_df = self.ori_flt_pdw_df.dropna()
        length = self.ori_flt_pdw_df.shape[0]
        print('数值滤波脉冲数:',length)
        if length == 0:
            return
        self.ori_flt_pdw_df = self.ori_flt_pdw_df.reset_index(drop=1)
        time_set = self.Origin_Data_wnd.filterbox.flt_time_le.text()
        try:
            time_set = float(time_set)
        except:
            self.Origin_Data_wnd.filterbox.flt_time_le.setText('设置为数字!!')
            return
        df = self.ori_flt_pdw_df
        if time_set == 0:
            pass
        else:
            rf_ch = [0 for i in range(20000)]
            idxes = []
            if self.Origin_Data_wnd.filterbox.flt_mode_comb.currentIndex()==0:
                for i in range(min(length,10000)):
                    rf = int((df.ix[i]['RF/MHz'])*2)
                    toa = df.ix[i]['TOA/us']
                    idx = rf_ch[rf>>3]
                    if i==0:
                        rf_ch[idx] = i
                        continue
                    if self.Calc_timediff(df.ix[idx]['TOA/us']+\
                                df.ix[idx]['PW/us'],toa)<time_set:
                        rf_ch[rf>>3] = i
                        continue
                    if rf-(rf>>3) >=3:
                        idx = rf_ch[(rf>>3)+1]
                        if self.Calc_timediff(df.ix[idx]['TOA/us']+\
                                df.ix[idx]['PW/us'],toa)<time_set:
                            rf_ch[rf>>3] = i
                            print(i,11)
                            continue
                        else:
                            print(i,12)
                            idxes.append(i)
                    elif rf-(rf>>3) <=1:
                        idx = rf_ch[(rf>>3)-1]
                        if self.Calc_timediff(df.ix[idx]['TOA/us']+\
                                    df.ix[idx]['PW/us'],toa)<time_set:
                            rf_ch[rf>>3] = i
                            print(i,21)
                            continue
                        else:
                            print(i,22)
                            idxes.append(i)
                    rf_ch[rf>>3] = i
            else:
                doatmp = [-1 for i in range(20000)]
                for i in range(length):
                    rf = int((df.ix[i]['RF/MHz'])*2)
                    doa = df.ix[i]['DOA/deg']
                    toa = df.ix[i]['TOA/us']
                    idx = rf_ch[rf>>3]
                    if idx == 0:
                        idxes.append(i)
                        doatmp[idx] = doa
                    else:
                        if self.Calc_timediff(df.ix[idx]['TOA/us']+\
                            df.ix[idx]['PW/us'],toa)<time_set and\
                             self.Calc_doadiff(doa,\
                                    doatmp[idx])>10/1024*360 :
                            rf_ch[rf>>3] = i
                            continue
                        else:
                            idxes.append(i)
                            doatmp[idx] = doa
                    rf_ch[rf>>3] = i
            print('length:',len(idxes),time_set)
            self.ori_flt_pdw_df = df.ix[idxes]
        self.ori_flt_pdw_df = self.ori_flt_pdw_df.reset_index(drop=1)
        PRI = self.TOA2PRI(self.ori_flt_pdw_df['TOA/us'])
        self.ori_flt_pdw_df['PRI/us'] = PRI
        self.ori_flt_pdw_df[['PW/us','PA/dBm','DOA/deg','RF/MHz','TOA/us','PRI/us','TOA_all/us','SecCntCyc/us']] = \
            self.ori_flt_pdw_df[['PW/us','PA/dBm','DOA/deg','RF/MHz','TOA/us','PRI/us','TOA_all/us','SecCntCyc/us']].apply(pd.to_numeric)
        self.update(self.Origin_Data_wnd.tableView,self.ori_flt_pdw_df,0)

    def TOA2PRI(self,toa):
        try:
            toa = np.array(toa)
        except:
            return
        firstflag,last_toa = 0,0
        PRI = []
        length = len(toa)
        for i in range(length):
            new_toa = toa[i]
            if firstflag == 0:
                PRI.append(0)
                firstflag = 1
            else:
                if (last_toa <= new_toa) or\
                ((last_toa>new_toa)&(last_toa-new_toa<0xefffffff/12.5)):
                    PRI.append('%.2f'%(new_toa-last_toa))
                else:
                    PRI.append('%.2f'%(0xffffffff/12.5+new_toa-last_toa))
            last_toa = new_toa
            
        PRI = np.array(PRI).astype('float')
        return PRI
        
    def Calc_timediff(self,t1,t2):
        if (t1 <= t2) or ((t1>t2)&(t1-t2<0xefffffff/12.5)):
            return(t2-t1)
        else:
            return((0xffffffff/12.5+t2-t1))
           
    def Calc_doadiff(self,t1,t2):
        if abs(t1 - t2) <= 180 :
            return abs(t2-t1)
        else:
            return(360-abs(t2-t1))

    def Show_Full(self):
        try:
            pdw = self.mark_pdw_df
        except:
            traceback.print_exc()
            return
        if pdw.shape[0] == 0:
            print('null mark_pdw')
            return
        self.fullsee.setWindowTitle('全景显示图')
        self.fullsee.cp[0].set_titles(title = 'TOA-RF',xlabel='TOA',ylabel='RF')
        self.fullsee.cp[1].set_titles(title = 'TOA-PA',xlabel='TOA',ylabel='PA')
        self.fullsee.cp[2].set_titles(title = 'TOA-DOA',xlabel='TOA',ylabel='DOA')
        self.fullsee.cp[3].set_titles(title = 'TOA-PW',xlabel='TOA',ylabel='PW')
        for i in range(4):
            self.fullsee.cp[i].del_all_items()
        cv1 = make.curve(pdw.TOA,pdw.RF,color='r',curvestyle='Dots',linewidth=3)
        cv2 = make.curve(pdw.TOA,pdw.PA,color='g',curvestyle='Sticks',linewidth=3)
        cv3 = make.curve(pdw.TOA,pdw.DOA,color='b',curvestyle='Dots',linewidth=3)
        cv4 = make.curve(pdw.TOA,pdw.PW,color='b',curvestyle='Dots',linewidth=3)
        self.fullsee.cp[0].add_item(cv1)
        self.fullsee.cp[1].add_item(cv2)
        self.fullsee.cp[2].add_item(cv3)
        self.fullsee.cp[3].add_item(cv4)
        self.fullsee.show()
        
    def DetailShow(self):
        global rownum
        try:
            pdw = self._pdw_df
        except:
            return
        _pdw = pdw[pdw.FLAG == 0]
        self.cp1.del_all_items()
        self.cp2.del_all_items()
        self.cp3.del_all_items()
        self.cp4.del_all_items()
        self.curve1 = make.curve(_pdw.TOA,_pdw.RF,title='0',color='g',\
                             linewidth=3, curvestyle='Dots')
        self.curve2 = make.curve(_pdw.TOA,_pdw.PA,title='0',color='g',\
                             linewidth=3, curvestyle='Dots')
        self.curve3 = make.curve(_pdw.TOA,_pdw.DOA,title='0',color='g',\
                             linewidth=3, curvestyle='Dots')
        self.curve4 = make.curve(_pdw.TOA,_pdw.PW,title='0',color='g',\
                             linewidth=3, curvestyle='Dots')
        res_num = self.edw_df.shape[0]
        print('resnum:',res_num)
        pdws = [0 for i in range(res_num)]
        self.colormap.rgb(QwtInterval(0,255),255)
        colors = [0 for i in range(res_num)]
        for i in range(res_num):
            colors[i] = self.colormap.rgb(QwtInterval(0,255),i*(int(255/res_num))+0.1)
        curve1,curve2,curve3,curve4 = [],[],[],[]
        for idx,i in enumerate(pdws):
            i = pdw[pdw.FLAG == idx+1]
            curve1.append(make.curve(i.TOA,i.RF,color=colors[idx],\
                             title=str(idx+1),linewidth=3,curvestyle='Dots'))
            curve2.append(make.curve(i.TOA,i.PA,color=colors[idx],\
                             title=str(idx+1),linewidth=3, curvestyle='Dots'))
            curve3.append(make.curve(i.TOA,i.DOA,color=colors[idx],\
                             title=str(idx+1),linewidth=3, curvestyle='Dots'))#Sticks,
            curve4.append(make.curve(i.TOA,i.PW,color=colors[idx],\
                            title=str(idx+1),linewidth=3, curvestyle='Dots'))
        for i in range(res_num):
            self.cp1.add_item(curve1[res_num -1 - i])
            self.cp2.add_item(curve2[res_num -1 - i])
            self.cp3.add_item(curve3[res_num -1 - i])
            self.cp4.add_item(curve4[res_num -1 - i])
        self.cp1.add_item(self.curve1)
        self.cp2.add_item(self.curve2)
        self.cp3.add_item(self.curve3)
        self.cp4.add_item(self.curve4)
        self.cp1.do_autoscale()
        self.cp2.do_autoscale()
        self.cp3.do_autoscale()
        self.cp4.do_autoscale()
        self.cp1.do_autoscale(replot=1)
        return
        
    def OpenPdw(self):
        fname = QFileDialog.getOpenFileName(self,'Open file',os.curdir,\
                                                ';;data(*.data)')        
        if(fname[0] == ''):
#            self.log_pte.appendPlainText('directory error!')
            return
        if fname == -1:
            self.statusbar.showMessage('open fail!')
            return
        self._OpenPdw(fname[0])
        os.chdir(os.path.split(fname[0])[0])#改变当前目录
        
    def _OpenPdw(self,name):
        fname = [name,0]
        os.chdir(os.path.split(fname[0])[0])#改变当前目录
        self.pdw_df = pd.DataFrame()
        _,filename = os.path.split(name)
        self.filename = filename.split('.')[0]
        self.statusbar.showMessage('PDW文件名: '+fname[0])

        self.first_flag = 1
        self.tick = 0
        
#        self.statusbar.showMessage('file opened!')
        ##显示界面交互    
        self.file_name_le.setText(name)
        ##重置trgt
        self.ResetTrgt()
        path = self.file_name_le.text()
        self.tot_LE.setText(str(os.stat(path)[6]))
        self.prg_Slider.setMaximum(int(os.stat(path)[6]))
        
    def re_draw_hist(self):
        try:
            df = self.re_pdw_df
        except AttributeError:
            return
        if self._pdw_df.shape[0]==0:
            return
            
        pdw_wgt = popwindow.Draw_Wgt_C(4,1)
        pdw_wgt.setObjectName('customWidget')
        pdw_wgt.setStyleSheet(self.styleSheet)
        pdw_wgt.setWindowTitle('直方图')
        pdw_wgt.cp[0].set_titles(title ='RF-Hist',xlabel='RF',ylabel='NUM')
        pdw_wgt.cp[1].set_titles(title ='DOA-Hist',xlabel='DOA',ylabel='NUM')
        pdw_wgt.cp[2].set_titles(title ='PA-Hist',xlabel='PA',ylabel='NUM')
        pdw_wgt.cp[3].set_titles(title ='PW-Hist',xlabel='PW',ylabel='NUM')
        maxx,minn = int(max(df['RF'])+5),int(min(df['RF'])-5)
        cv1 = make.histogram(df['RF'],bins = range(minn,maxx,1))
        cv2 = make.histogram(df['DOA'],bins = range(0,360,4))
        cv3 = make.histogram(df['PA'],bins = range(-2,200,5))
        cv4 = make.histogram(df['PW'],bins = range(-2,20,1))
        pdw_wgt.cp[0].add_item(cv1)
        pdw_wgt.cp[1].add_item(cv2)
        pdw_wgt.cp[2].add_item(cv3)
        pdw_wgt.cp[3].add_item(cv4)
        pdw_wgt.show()
        
    def SequenceProcess(self):
        ret = QMessageBox.question(self,'Message',
        "选择模式，Yes为按节拍处理，No为自动处理",
                                  # '固定节拍','连续处理','jjj')
            QMessageBox.Yes|QMessageBox.No,QMessageBox.Yes)
        path = self.path+'/'+self.filename
        try:
            os.mkdir(path)
        except FileExistsError:
            pass
        if ret == QMessageBox.Yes:
            
            text,ok = QInputDialog.getText(self,'输入节拍数','输入节拍数:')
            if ( ok & (text != '')):
                num = (int)(text)
            if not ok:
                return
            num1 = (int)(self.pdw_num_le.text())
            
            for i in range(num):
                self.SignalProcess(pdw_num = num1)
                QApplication.processEvents()
                if self.runFlag ==0:
                    break
                time.sleep(1)
                if self.autoSaveFlag:
                    name = path+'/'+'_pdw_'+ str(self.tick)+'.csv'
                    self._pdw_df.FLAG+=self.tick*100
                    self._pdw_df.to_csv(name)
                    name = path+'/'+'_res_'+ str(self.tick)+'.csv'
                    self.res_df['批号']+=self.tick*100
                    self.res_df.to_csv(name)
        else:
            while( int(self.prg_LE.text()) < int(self.tot_LE.text()) ):
                self.SignalProcess(pdw_num = 10000)
                QApplication.processEvents()
                if self.runFlag ==0:
                    break
                if self.autoSaveFlag:
                    name = path+'/'+'pdw_'+ str(self.tick)+'.csv'
                    self._pdw_df.FLAG+=self.tick*100
                    self._pdw_df.to_csv(name)
                    name = path+'/'+'res_'+ str(self.tick)+'.csv'
                    self.res_df['批号']+=self.tick*100
                    self.res_df.to_csv(name)
                time.sleep(1)

    def MarkRateShow(self):
        pdw = self._pdw_df
        try:
            pdw.FLAG
        except:
            self.curMarkRate_le.setText('')
            return
        rate = '%.2f'%(pdw.where(pdw.FLAG != 0).dropna().shape[0]/\
                pdw.shape[0]*100)
        self.curMarkRate_le.setText(rate)
        pdw = self.mark_pdw_df
        try:
            pdw.FLAG
        except:
            self.curMarkRate_le.setText('')
            return
        rate = '%.2f'%(pdw.where(pdw.FLAG != 0).dropna().shape[0]/\
                pdw.shape[0]*100)
        self.AllMarkRate_le.setText(rate)
        
    def edw_show(self,edw_df,flag,type0):
        if edw_df.shape[0] < flag:
            print('number < flag')
            return
        info = edw_df.ix[flag -1]
        rfs,pris,pws = [],[],[]
        pihao = info['批号']
        doa = info['方位']
        pw = info['脉宽']
        pa = info['幅度']
        rft = info['载频类型']
        rf_max = info['载频最大值']
        rf_min = info['载频最小值']
        for i in range(32):
            rfs.append(info['载频'+str(i+1)])
        rf_mid = rfs[0]
        prit = info['重频类型']
        pri_max,pri_min = info['重频最大值'],info['重频最小值']
        for i in range(16):
            pris.append(info['重复间隔'+str(i+1)])
        pri_mid = pris[0]
        pwt = info['脉宽类型']
        pw_max,pw_min = info['脉宽最大值'],info['脉宽最小值']
        pw_mid = pws = info['脉宽']
        self.pihao_LE.setText(str(pihao))
        self.doa_LE.setText(str(doa))
        self.pw_LE.setText(str(pw))
        self.pa_LE.setText(str(pa))
        self.rft_LE.setText(str(rft))
        self.rf_max_LE.setText(str(rf_max))
        self.rf_min_LE.setText(str(rf_min))
        self.rf_mid_LE.setText(str(rf_mid))
        self.prit_LE.setText(str(prit))
        self.pri_max_LE.setText(str(pri_max))
        self.pri_min_LE.setText(str(pri_min))
        self.pri_mid_LE.setText(str(pri_mid))
        self.pw_max_LE.setText(str(pw_max))
        self.pw_min_LE.setText(str(pw_min))
        self.pwt_LE.setText(str(pwt))
        self.pw_mid_LE.setText(str(pw_mid))
        
        for i in range(32):
            self.rf_listWidget.takeItem(0)
            self.pri_listWidget.takeItem(0)
        for i in range(32):
            self.rf_listWidget.addItem(str(rfs[i]))
        for i in range(16):
            self.pri_listWidget.addItem(str(pris[i]))
        self.pw_listWidget.takeItem(0)
        self.pw_listWidget.addItem(str(pws))
        
        self.cp1.del_all_items()
        self.cp2.del_all_items()
        self.cp3.del_all_items()
        self.cp4.del_all_items()
        
        if type0 == 'res':
            pdw = self._pdw_df
            _pdw = pdw[pdw.FLAG != flag]      
        else:
            pdw = self.mark_pdw_df
            _pdw = pdw[pdw.TRGT_NO != flag]
        _pdw[['TOA','PA']] = _pdw[['TOA','PA']].astype('int')
        pdw[['TOA','PA']] = pdw[['TOA','PA']].astype('int')
        self.curve1 = make.curve(_pdw.TOA.astype('int'),_pdw.RF,color='g',\
                             linewidth=3, curvestyle='Dots')
        self.curve2 = make.curve(_pdw.TOA.astype('int'),_pdw.PA,color='g',\
                             linewidth=3, curvestyle='Dots')
        self.curve3 = make.curve(_pdw.TOA.astype('int'),_pdw.DOA,color='g',\
                             linewidth=3, curvestyle='Dots')
        self.curve4 = make.curve(_pdw.TOA.astype('int'),_pdw.PW,color='g',\
                             linewidth=3, curvestyle='Dots')

        self.cp1.add_item(self.curve1)
        self.cp2.add_item(self.curve2)
        self.cp3.add_item(self.curve3)
        self.cp4.add_item(self.curve4)
        if type0 == 'res':
            i = pdw[pdw.FLAG == flag]
        else:
            i = pdw[pdw.TRGT_NO == flag]
        self.cp1.add_item(make.curve(i.TOA,i.RF,color='r',\
                             linewidth=3, curvestyle='Dots'))
        self.cp2.add_item(make.curve(i.TOA,i.PA,color='r',\
                             linewidth=3, curvestyle='Dots'))
        self.cp3.add_item(make.curve(i.TOA,i.DOA,color='r',\
                             linewidth=3, curvestyle='Dots'))
        self.cp4.add_item(make.curve(i.TOA,i.PW,color='r',\
                             linewidth=3, curvestyle='Dots'))
        self.cp1.do_autoscale()
        self.cp2.do_autoscale()
        self.cp3.do_autoscale()
        self.cp4.do_autoscale()
        self.cp1.do_autoscale(replot=1)
        
    def show_res(self):
        global rownum
        if rownum<0:return
        self.edw_show(self.res_df,rownum,'res')
        
    def show_edw(self):
        global rownum
        if rownum<0:return
        self.edw_show(self.edw_df,rownum,'edw')
        
    def SaveEdw(self):
        now = datetime.now()
        df = self.edw_df
        da = now.strftime('%b_%d_%H_%M')
        da = 'edw_'+da
        try:
            fname = QFileDialog.getSaveFileName(self,'Save edw file',da,\
            'tables(*.csv);;tables(*.xlsx)')
        except:
            return
        if fname[0]=='' :return
        self.SaveFile(df, fname)
        
    def SaveRes(self):
        now = datetime.now()
        df = self.res_df
        da = now.strftime('%b_%d_%H_%M')
        da = 'res_'+da
        try:
            fname = QFileDialog.getSaveFileName(self,'Save res file',da,\
            'tables(*.csv);;tables(*.xlsx)')
        except:
            return
        if fname[0]=='' :return
        self.SaveFile(df, fname[0])
        
    def SaveTmpPDW(self):
        now = datetime.now()
        df = self.re_pdw_df
        da = now.strftime('%b_%d_%H_%M')
        da = 'pdw_'+da
        try:
            fname = QFileDialog.getSaveFileName(self.res_wgt,'Save tmp pdw file',da,\
            'tables(*.csv);;tables(*.xlsx)')
        except:
            return
        if fname[0]=='' :return
        self.SaveFile(df, fname)
        
    def SaveTmpRES(self):
        now = datetime.now()
        df = self.res_df
        da = now.strftime('%b_%d_%H_%M')
        da = 'res_'+da
        try:
            fname = QFileDialog.getSaveFileName(self.res_wgt,'Save tmp res file',da,\
            'tables(*.csv);;tables(*.xlsx)')
        except:
            return
        if fname[0]=='' :return
        self.SaveFile(df, fname)

    def SaveMarkedPdw(self):
        now = datetime.now()
        try:
            df = self._pdw_df
        except:
            return
        da = now.strftime('%b_%d_%H_%M')
        da = 'pdw_'+da
        try:
            fname = QFileDialog.getSaveFileName(self,'Save pdw file',da,\
            'tables(*.csv);;tables(*.xlsx)')
        except:
            return
        if fname[0]=='' :return
        self.SaveFile(df, fname)
        
    def SaveAllMarkedPdw(self):
        now = datetime.now()
        try:
            df = self.mark_pdw_df
        except:
            return
        da = now.strftime('%b_%d_%H_%M')
        da = 'marked_pdw_'+da
        try:
            fname = QFileDialog.getSaveFileName(self,'Save pdw file',da,\
            'tables(*.csv);;tables(*.xlsx)')
        except:
            return
        if fname[0]=='' :return
        self.SaveFile(df, fname)
        
    def SaveFile(self, df, fname):
        global windows
        func = pd.DataFrame.to_csv
        if windows:
            print(fname[0])
            name,suffix = os.path.splitext(fname[0])
            iname = name
        else:
            name = fname[0]
            iname = fname[0]
            suffix = fname[1].split('.')[1][:-1]
        if df.shape[0]<=1000000:
            if suffix =='.csv':
                func = pd.DataFrame.to_csv
            elif suffix =='.xlsx' :
                func = pd.DataFrame.to_excel
            self.statusbar.showMessage('保存文件中...')
            try:
                func(df,name+suffix)
            except:
                traceback.print_exc()
                return
        else:
            filenum = int(df.shape[0]/1000000)
            if suffix =='csv':
                func = pd.DataFrame.to_csv
            elif suffix =='xlsx':
                func = pd.DataFrame.to_excel
            self.statusbar.showMessage('保存文件中...')
            for i in range(filenum):
                name = iname+'_'+str(i)
                try:
                    func(df.ix[i*1000000:((i+1)*1000000-1)],name+'.'+suffix)
                except:
                    traceback.print_exc()
                    return
            name = iname+'_'+str(i+1)
            try:
                func(df.ix[(i+1)*1000000:],name+'.'+suffix)
            except:
                traceback.print_exc()
                return
        self.statusbar.showMessage('数据保存成功！')

    def Update_preview(self,content,part=0):
        if part == 0:
            to_be = [self.timespan_all_le,self.pls_density_all_le,\
                     self.pls_num_all_le,self.pls_valid_rate_all_le]
        else:
            to_be = [self.timespan_cur_le,self.pls_density_cur_le,\
                     self.pls_num_cur_le,self.pls_valid_rate_cur_le]
        for i,j in zip(to_be,content):
            i.setText(j)
            
            
    def setTableviewColor(self, tableview, color):
        model = tableview.model()
        if model is None:return
        rowcount = model.rowCount()
        col = model.columnCount()
        for j in range(rowcount):
            sitem = model.verticalHeaderItem(j)
            sitem.setBackground(QBrush(QColor(color)))
        for j in range(col):
            sitem = model.horizontalHeaderItem(j)
            sitem.setBackground(QBrush(QColor(color)))
        for i in range(rowcount):
            for j in range(col):
                model.item(i,j).setBackground(QBrush(QColor(color)))

    def update(self, tableview, df,ctc = 1,maxLength = 100):
        self.statusbar.showMessage('更新显示中updating tableview!')
        complete_flag  = 1
        t1 = time.time()
        
        halfLength = int(maxLength/2)
        
        if df.shape[0]<=0:return
        model = QStandardItemModel()
        if len(df)<= maxLength:
            rowcount = df.shape[0]
        else:
            rowcount = maxLength
            complete_flag = 0
            self.statusbar.showMessage('显示部分结果!')
        model.setRowCount(rowcount)
        for i in range(rowcount):
            model.setVerticalHeaderItem(i,(QStandardItem)(str(i+1)))
        if len(df) > maxLength:
            model.setVerticalHeaderItem(halfLength,(QStandardItem)('...'))
            for i in range(halfLength+1,maxLength):    
                model.setVerticalHeaderItem(i,\
                                (QStandardItem)(str(df.shape[0]-maxLength+i)))
        colname = df.columns.tolist()
        model.setColumnCount(len(colname))
        for i in range(len(colname)):
            model.setHorizontalHeaderItem(i,(QStandardItem)(colname[i]))
        colcount = df.shape[1]
        for j in range(colcount):
            try:
                item = df.ix[0][j]
                item = float(item)
                item = max(df.ix[:][colname[j]].astype('float'))
                int_num = len(str(item).split('.')[0])
                int_flag = 0
                if item.is_integer():
                    int_flag = 1
                else:
                    int_flag = 0
            except (ValueError,TypeError):
                int_num = -1
            QApplication.processEvents()
            for i in range(rowcount):
                item = df.ix[i][j]
                if int_num >= 0:
                    if int_flag == 1:
                        try:
                            item = ('%0'+str(int_num)+'.0f')%(int(float(item)))
                        except:
                            traceback.print_exc()
                            print('##########',item,int_num)
                    else:
                        item = ('%0'+str(int_num)+'.2f')%(float(item))
                        num = int_num - len(item.split('.')[0])
                        item = ''.join(num*'0')+item
                else:
                    item = str(item)
                    
                if item == 'nan':item = ''
                if complete_flag == 0 and i== 50:
                    item = '...'
                elif complete_flag == 0 and i > 50:
                    item = str(df.ix[df.shape[0]+i-100][j])
                    if int_num >= 0:
                        if int_flag == 1:
                            try:
                                item = ('%0'+str(int_num)+'.0f')%(int(float(item)))
                            except:
                                traceback.print_exc()
                                print('##########',item,int_num)
                        else:
                            item = ('%0'+str(int_num)+'.2f')%(float(item))
                            num = int_num - len(item.split('.')[0])
                            item = ''.join(num*'0')+item
                    else:
                        item = str(item)
                sitem = QStandardItem(item)
                model.setItem(i,j,sitem)
                model.item(i,j).setTextAlignment(Qt.AlignCenter)
        t2 = time.time()
        tableview.setEditTriggers(QAbstractItemView.NoEditTriggers)
        tableview.setSelectionBehavior(QAbstractItemView.SelectRows)
        tableview.setSortingEnabled(1)
        tableview.setModel(model)
        if ctc == 1:
            tableview.resizeColumnsToContents()
        elif ctc == 2:
            tableview.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.statusbar.showMessage('更新列表显示完成!时间：%f'%(t2-t1))

    def slider2le(self):
        value = self.prg_Slider.value()
        self.prg_LE.setText(str(value))
        
    def le2slider(self):
        text = self.prg_LE.text()
        try:
            int(text)
        except:
            return
        text = int(text)
        self.prg_Slider.setValue(text)
        
    def dragEnterEvent(self,event):
        event.accept()  
        
    def dropEvent(self,event):
        global windows
        st = str(event.mimeData().urls())
        if windows:
            st = st.replace("[PyQt5.QtCore.QUrl('file:///",'')
        else:
            st = st.replace("[PyQt5.QtCore.QUrl('file://",'')
        st = st.replace('),','')  
        st = st.replace("')]",'')
        if windows:
            st = st.replace('/','\\')
        self._OpenPdw(st)
            
            
    def read_pdw_edw(self):
        self._pdw_df = pd.read_csv('D:\py\pdw_May_14_14_561.csv')
        self._pdw_df.drop('Unnamed: 0',axis=1,inplace=1)
        self.update(self.pdw_tableView,self._pdw_df)
        self.edw_df = pd.read_csv('D:\py\edw_May_14_14_531.csv',encoding='gbk')
        self.edw_df.drop('Unnamed: 0',axis=1,inplace=1)
        self.update(self.edw_tableView,self.edw_df)
    
    def draw_pdw_info(self):
        global rownum
        if rownum ==-1:return
        try:
            self._pdw_df
        except AttributeError:
            return
        if self._pdw_df.shape[0]==0:
            return
        pdw_wgt = popwindow.Draw_Wgt_C(4,1,4)
        pdw_wgt.setObjectName('customWidget')
        pdw_wgt.setStyleSheet(self.styleSheet)
        pdw_wgt.setWindowTitle('参数图')
        names = ['RF','PA','DOA','PRI']
        for idx,i in enumerate(pdw_wgt.cp):
            i.set_titles(title='TOA-'+names[idx],xlabel='TOA',ylabel=names[idx])
        pdw = self._pdw_df[self._pdw_df.TRGT_NO == self.edw_df.loc[rownum-1,'批号']]
#        cd = CurveDialog(edit=True, toolbar = 1,wintitle='参数图')
        cv = make.curve(pdw.TOA,pdw.RF,title='TOA-RF',color='r',\
                        linewidth=3,curvestyle='Dots')
        cv2 = make.curve(pdw.TOA,pdw.PA,title='TOA-PA',color='g',\
                        linewidth=3,curvestyle='Dots')#curvestyle='Sticks')
        cv3 =  make.curve(pdw.TOA,pdw.DOA,title='TOA-DOA',color='g',\
                        linewidth=3,  curvestyle='Dots')
        cv4 = make.curve(pdw.TOA[:-1],pdw.TOA.diff().dropna(),title='TOA-PRI',color='g',\
                        linewidth=3,  curvestyle='Dots')
        pdw_wgt.cp[0].del_all_items()
        pdw_wgt.cp[1].del_all_items()
        pdw_wgt.cp[2].del_all_items()
        pdw_wgt.cp[3].del_all_items()
        pdw_wgt.cp[0].add_item(cv)  
        pdw_wgt.cp[1].add_item(cv2)
        pdw_wgt.cp[2].add_item(cv3)
        pdw_wgt.cp[3].add_item(cv4)
        pdw_wgt.show()

    def draw_all_hist(self):
        try:
            df = self._pdw_df
        except AttributeError:
            return
        if self._pdw_df.shape[0]==0:
            return
            
        pdw_wgt = popwindow.Draw_Wgt_C(4,1)
        pdw_wgt.setObjectName('customWidget')
        pdw_wgt.setStyleSheet(self.styleSheet)
        pdw_wgt.setWindowTitle('直方图')
        pdw_wgt.cp[0].set_titles(title ='RF-Hist',xlabel='RF',ylabel='NUM')
        pdw_wgt.cp[1].set_titles(title ='DOA-Hist',xlabel='DOA',ylabel='NUM')
        pdw_wgt.cp[2].set_titles(title ='PA-Hist',xlabel='PA',ylabel='NUM')
        pdw_wgt.cp[3].set_titles(title ='PW-Hist',xlabel='PW',ylabel='NUM')
        bins = int(max(df['RF'])-min(df['RF']+1))//2+1
        cv1 = make.histogram(df['RF'],bins = bins)
        cv2 = make.histogram(df['DOA'],bins = range(0,360,4))
        bins = int(max(df['PA'])-min(df['PA']+1))+1
        cv3 = make.histogram(df['PA'],bins = bins)
        bins = int(max(df['PW'])-min(df['PW']+1))*4+1
        cv4 = make.histogram(df['PW'],bins = bins)
        pdw_wgt.cp[0].add_item(cv1)
        pdw_wgt.cp[1].add_item(cv2)
        pdw_wgt.cp[2].add_item(cv3)
        pdw_wgt.cp[3].add_item(cv4)
        pdw_wgt.show()

    def draw_one_hist(self):
        if rownum ==-1:return
        try:
            df = self._pdw_df
        except AttributeError:
            return
        if self._pdw_df.shape[0]==0:
            return
            
        pdw_wgt = popwindow.Draw_Wgt_C(4,1)
        pdw_wgt.setObjectName('customWidget')
        pdw_wgt.setStyleSheet(self.styleSheet)
        pdw_wgt.setWindowTitle('直方图')
        df = df[df.TRGT_NO == self.edw_df.loc[rownum-1,'批号']]
        pdw_wgt.cp[0].set_titles(title ='RF-Hist',xlabel='RF',ylabel='NUM')
        pdw_wgt.cp[1].set_titles(title ='DOA-Hist',xlabel='DOA',ylabel='NUM')
        pdw_wgt.cp[2].set_titles(title ='PA-Hist',xlabel='PA',ylabel='NUM')
        pdw_wgt.cp[3].set_titles(title ='PW-Hist',xlabel='PW',ylabel='NUM')
        maxx,minn = int(max(df['RF'])+5),int(min(df['RF'])-5)
        cv1 = make.histogram(df['RF'],bins = range(minn,maxx,1))
        cv2 = make.histogram(df['DOA'],bins = range(0,360,4))
        cv3 = make.histogram(df['PA'],bins = range(-2,200,5))
        cv4 = make.histogram(df['PW'],bins = range(-2,20,1))
        pdw_wgt.cp[0].add_item(cv1)
        pdw_wgt.cp[1].add_item(cv2)
        pdw_wgt.cp[2].add_item(cv3)
        pdw_wgt.cp[3].add_item(cv4)
        pdw_wgt.show()
        
    def draw_one_hist_res(self):
        if rownum ==-1:return
        try:
            df = self._pdw_df
        except AttributeError:
            return
        if self._pdw_df.shape[0]==0:
            return
            
        pdw_wgt = popwindow.Draw_Wgt_C(4,1)
        pdw_wgt.setStyleSheet(self.styleSheet)
        pdw_wgt.setWindowTitle('直方图')
        df = df[df.FLAG == self.res_df.loc[rownum-1,'批号']]
        pdw_wgt.cp[0].set_titles(title ='RF-Hist',xlabel='RF',ylabel='NUM')
        pdw_wgt.cp[1].set_titles(title ='DOA-Hist',xlabel='DOA',ylabel='NUM')
        pdw_wgt.cp[2].set_titles(title ='PA-Hist',xlabel='PA',ylabel='NUM')
        pdw_wgt.cp[3].set_titles(title ='PW-Hist',xlabel='PW',ylabel='NUM')
        maxx,minn = int(max(df['RF'])+5),int(min(df['RF'])-5)
        cv1 = make.histogram(df['RF'],bins = range(minn,maxx,1))
        cv2 = make.histogram(df['DOA'],bins = range(0,360,4))
        cv3 = make.histogram(df['PA'],bins = range(-2,200,5))
        cv4 = make.histogram(df['PW'],bins = range(-2,20,1))
        pdw_wgt.cp[0].add_item(cv1)
        pdw_wgt.cp[1].add_item(cv2)
        pdw_wgt.cp[2].add_item(cv3)
        pdw_wgt.cp[3].add_item(cv4)
        pdw_wgt.show()

    def save_one_pdw(self):
        if rownum ==-1:return
        try:
            self._pdw_df
        except AttributeError:
            return
        if self._pdw_df.shape[0]==0:
            return
        pdw = self._pdw_df[self._pdw_df.TRGT_NO == \
                           self.edw_df.loc[rownum-1,'批号']]
        now = datetime.now()
        da = now.strftime('%b_%d_%H_%M')
        da = 'pdw_'+da
        try:
            fname = QFileDialog.getSaveFileName(self,'Save pdw file',da,\
            'tables(*.csv);;tables(*.xlsx)')
        except:
            return
        if fname[0]=='' :return
        self.SaveFile(pdw,fname)
        
    def save_maxtrix_search_pdw(self):
        pdw = self.matrix_data_df
        if pdw.shape[0] == 0:
            return
        now = datetime.now()
        da = now.strftime('%b_%d_%H_%M')
        da = 'pdw_'+da
        try:
            fname = QFileDialog.getSaveFileName(self.matrix_wnd,\
                'Save pdw file',da,'tables(*.csv);;tables(*.xlsx)')
        except:
            return
        if fname[0]=='' :return
        self.SaveFile(pdw,fname)

    def TransPdw(self):
        try:
            self.f2
        except:
            try:
                self.f2 = open(self.file_name_le.text(),'rb')
            except:
                return
        try:
            rf_min = float(self.pdwFilter.le_min[0].text())
            rf_max = float(self.pdwFilter.le_max[0].text())
            doa_min =float(self.pdwFilter.le_min[1].text())
            doa_max = float(self.pdwFilter.le_max[1].text())
            pw_min = float(self.pdwFilter.le_min[2].text())
            pw_max = float(self.pdwFilter.le_max[2].text())
            pa_min = float(self.pdwFilter.le_min[3].text())
            pa_max = float(self.pdwFilter.le_max[3].text())
            pls_num = int(self.pdwFilter.pls_num_le.text())
            pls_time = float(self.pdwFilter.pls_time_le.text())
        except ValueError:
            return
        self.pdw_trans_df = pd.DataFrame()
        rf_c = self.pdwFilter.check[0].isChecked()
        doa_c = self.pdwFilter.check[1].isChecked()
        pw_c = self.pdwFilter.check[2].isChecked()
        pa_c =  self.pdwFilter.check[3].isChecked()
        if( rf_c | doa_c | pw_c | pa_c):
            pass
        else:
            return

        num ,time ,last_toa,first_flag= 0, 0, 0, 1
        RF,DOA,PW,PA,TOA = [],[],[],[],[]
        while( (num < pls_num) & (time < pls_time)):
            buffer = self.f2.read(12)
            if len(buffer)<12:break
            toa,rf,doa,pa,pw = struct.unpack('<IHHHH',buffer)
            rf,doa = rf/2, doa/1024*360
            if rf_c == 1:
                if ( (rf <= rf_max) & (rf >= rf_min) )==0:
                    continue
            if doa_c == 1:
                if ( (doa <= doa_max) & (doa >= doa_min) )==0:
                    continue
            if pw_c == 1:
                if ((pw <= pw_max) & (pw >= pw_min) )==0:
                    continue
            if pa_c == 1:
                if ((pa <= pa_max) & (pa >= pa_min) )==0:
                    continue
            num += 1
            time += self.time_diff(last_toa,toa)
            if first_flag: 
                time = 0
                first_flag = 0
            RF.append(rf)
            DOA.append(doa)
            PA.append(pa)
            PW.append(pw)
            TOA.append(toa)
            last_toa = toa
        self.pdw_trans_df['RF'] = RF
        self.pdw_trans_df['DOA'] = DOA
        self.pdw_trans_df['PW'] = PW
        self.pdw_trans_df['PA'] = PA
        self.pdw_trans_df['TOA'] = TOA
        self.trans_wgt = QWidget()
        self.trans_wgt.setWindowTitle('PDW传送')
        grid = QGridLayout()
        self.trans_wgt.setLayout(grid)
        tableview = QTableView()
        grid.addWidget(tableview,1,1)
        self.update(tableview,self.pdw_trans_df,ctc=2)
        self.trans_wgt.resize(QSize(500,600))
        self.trans_wgt.show()

    def time_diff(self,TOA_S,TOA_N):

        if TOA_N > TOA_S:
            TOA_D = TOA_N - TOA_S
        else:
            TOA_D = 0xffffffff - (TOA_S - TOA_N)
        return TOA_D

    def ResetFile(self):
        self.f2.seek(20,0)
        
if __name__=='__main__':
    app = QApplication(sys.argv)
    ex=UI()
    sys.exit(app.exec_())