# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'MainWindow1.ui'
#
# Created by: PyQt5 UI code generator 5.6
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_Marker(object):
    def setupUi(self, Marker):
        Marker.setObjectName("Marker")
        Marker.resize(1708, 1061)
        self.centralwidget = QtWidgets.QWidget(Marker)
        self.centralwidget.setObjectName("centralwidget")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.centralwidget)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        Marker.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(Marker)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1708, 21))
        self.menubar.setObjectName("menubar")
        self.menu = QtWidgets.QMenu(self.menubar)
        self.menu.setObjectName("menu")
        self.menuDataSource = QtWidgets.QMenu(self.menu)
        self.menuDataSource.setObjectName("menuDataSource")
        self.menu_4 = QtWidgets.QMenu(self.menu)
        self.menu_4.setObjectName("menu_4")
        self.menu_2 = QtWidgets.QMenu(self.menubar)
        self.menu_2.setObjectName("menu_2")
        self.menu_3 = QtWidgets.QMenu(self.menu_2)
        self.menu_3.setObjectName("menu_3")
        self.menu_5 = QtWidgets.QMenu(self.menubar)
        self.menu_5.setObjectName("menu_5")
        Marker.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(Marker)
        self.statusbar.setObjectName("statusbar")
        Marker.setStatusBar(self.statusbar)
        self.info_dock = QtWidgets.QDockWidget(Marker)
        self.info_dock.setBaseSize(QtCore.QSize(180, 360))
        self.info_dock.setAutoFillBackground(False)
        self.info_dock.setStyleSheet("QDockWidget{\n"
"background:White;\n"
"border:none;\n"
"border-radius:10 px;\n"
"}")
        self.info_dock.setObjectName("info_dock")
        self.dockWidgetContents = QtWidgets.QWidget()
        self.dockWidgetContents.setStyleSheet("")
        self.dockWidgetContents.setObjectName("dockWidgetContents")
        self.gridLayout_4 = QtWidgets.QGridLayout(self.dockWidgetContents)
        self.gridLayout_4.setContentsMargins(0, 0, 0, 0)
        self.gridLayout_4.setObjectName("gridLayout_4")
        self.gridLayout_15 = QtWidgets.QGridLayout()
        self.gridLayout_15.setObjectName("gridLayout_15")
        self.groupBox = QtWidgets.QGroupBox(self.dockWidgetContents)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox.sizePolicy().hasHeightForWidth())
        self.groupBox.setSizePolicy(sizePolicy)
        self.groupBox.setObjectName("groupBox")
        self.gridLayout_5 = QtWidgets.QGridLayout(self.groupBox)
        self.gridLayout_5.setObjectName("gridLayout_5")
        self.gridLayout_6 = QtWidgets.QGridLayout()
        self.gridLayout_6.setObjectName("gridLayout_6")
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.prg_LE = QtWidgets.QLineEdit(self.groupBox)
        self.prg_LE.setStyleSheet("")
        self.prg_LE.setObjectName("prg_LE")
        self.horizontalLayout_2.addWidget(self.prg_LE)
        self.label = QtWidgets.QLabel(self.groupBox)
        self.label.setObjectName("label")
        self.horizontalLayout_2.addWidget(self.label)
        self.tot_LE = QtWidgets.QLineEdit(self.groupBox)
        self.tot_LE.setStyleSheet("")
        self.tot_LE.setObjectName("tot_LE")
        self.horizontalLayout_2.addWidget(self.tot_LE)
        self.gridLayout_6.addLayout(self.horizontalLayout_2, 13, 1, 1, 6)
        self.label_28 = QtWidgets.QLabel(self.groupBox)
        self.label_28.setObjectName("label_28")
        self.gridLayout_6.addWidget(self.label_28, 5, 1, 1, 1)
        self.prg_Slider = QtWidgets.QSlider(self.groupBox)
        self.prg_Slider.setOrientation(QtCore.Qt.Horizontal)
        self.prg_Slider.setObjectName("prg_Slider")
        self.gridLayout_6.addWidget(self.prg_Slider, 12, 1, 1, 6)
        self.label_23 = QtWidgets.QLabel(self.groupBox)
        self.label_23.setObjectName("label_23")
        self.gridLayout_6.addWidget(self.label_23, 6, 1, 1, 1)
        self.label_24 = QtWidgets.QLabel(self.groupBox)
        self.label_24.setObjectName("label_24")
        self.gridLayout_6.addWidget(self.label_24, 4, 6, 1, 1)
        self.label_26 = QtWidgets.QLabel(self.groupBox)
        self.label_26.setObjectName("label_26")
        self.gridLayout_6.addWidget(self.label_26, 4, 4, 1, 1)
        self.st_pa_max_le = QtWidgets.QLineEdit(self.groupBox)
        self.st_pa_max_le.setStyleSheet("")
        self.st_pa_max_le.setObjectName("st_pa_max_le")
        self.gridLayout_6.addWidget(self.st_pa_max_le, 6, 5, 1, 1)
        self.st_pw_max_le = QtWidgets.QLineEdit(self.groupBox)
        self.st_pw_max_le.setStyleSheet("")
        self.st_pw_max_le.setObjectName("st_pw_max_le")
        self.gridLayout_6.addWidget(self.st_pw_max_le, 5, 5, 1, 1)
        self.label_29 = QtWidgets.QLabel(self.groupBox)
        self.label_29.setObjectName("label_29")
        self.gridLayout_6.addWidget(self.label_29, 5, 4, 1, 1)
        self.label_30 = QtWidgets.QLabel(self.groupBox)
        self.label_30.setObjectName("label_30")
        self.gridLayout_6.addWidget(self.label_30, 5, 6, 1, 1)
        self.st_doa_max_le = QtWidgets.QLineEdit(self.groupBox)
        self.st_doa_max_le.setStyleSheet("")
        self.st_doa_max_le.setObjectName("st_doa_max_le")
        self.gridLayout_6.addWidget(self.st_doa_max_le, 4, 5, 1, 1)
        self.label_25 = QtWidgets.QLabel(self.groupBox)
        self.label_25.setObjectName("label_25")
        self.gridLayout_6.addWidget(self.label_25, 6, 6, 1, 1)
        self.label_22 = QtWidgets.QLabel(self.groupBox)
        self.label_22.setObjectName("label_22")
        self.gridLayout_6.addWidget(self.label_22, 4, 1, 1, 1)
        self.label_27 = QtWidgets.QLabel(self.groupBox)
        self.label_27.setObjectName("label_27")
        self.gridLayout_6.addWidget(self.label_27, 6, 4, 1, 1)
        self.label_20 = QtWidgets.QLabel(self.groupBox)
        self.label_20.setObjectName("label_20")
        self.gridLayout_6.addWidget(self.label_20, 3, 6, 1, 1)
        self.horizontalLayout_3 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.st_read_btn = QtWidgets.QPushButton(self.groupBox)
        self.st_read_btn.setStyleSheet("")
        self.st_read_btn.setObjectName("st_read_btn")
        self.horizontalLayout_3.addWidget(self.st_read_btn)
        self.once_read_btn = QtWidgets.QPushButton(self.groupBox)
        self.once_read_btn.setStyleSheet("")
        self.once_read_btn.setObjectName("once_read_btn")
        self.horizontalLayout_3.addWidget(self.once_read_btn)
        self.close_file_btn = QtWidgets.QPushButton(self.groupBox)
        self.close_file_btn.setStyleSheet("")
        self.close_file_btn.setObjectName("close_file_btn")
        self.horizontalLayout_3.addWidget(self.close_file_btn)
        self.gridLayout_6.addLayout(self.horizontalLayout_3, 10, 1, 1, 6)
        self.st_rf_max_le = QtWidgets.QLineEdit(self.groupBox)
        self.st_rf_max_le.setStyleSheet("")
        self.st_rf_max_le.setObjectName("st_rf_max_le")
        self.gridLayout_6.addWidget(self.st_rf_max_le, 3, 5, 1, 1)
        self.label_21 = QtWidgets.QLabel(self.groupBox)
        self.label_21.setObjectName("label_21")
        self.gridLayout_6.addWidget(self.label_21, 3, 4, 1, 1)
        self.choose_file_btn = QtWidgets.QPushButton(self.groupBox)
        self.choose_file_btn.setStyleSheet("")
        self.choose_file_btn.setObjectName("choose_file_btn")
        self.gridLayout_6.addWidget(self.choose_file_btn, 1, 1, 1, 1)
        self.label_15 = QtWidgets.QLabel(self.groupBox)
        self.label_15.setObjectName("label_15")
        self.gridLayout_6.addWidget(self.label_15, 1, 2, 1, 1)
        self.file_name_le = QtWidgets.QLineEdit(self.groupBox)
        self.file_name_le.setStyleSheet("")
        self.file_name_le.setText("")
        self.file_name_le.setClearButtonEnabled(True)
        self.file_name_le.setObjectName("file_name_le")
        self.gridLayout_6.addWidget(self.file_name_le, 1, 3, 1, 4)
        self.label_19 = QtWidgets.QLabel(self.groupBox)
        self.label_19.setObjectName("label_19")
        self.gridLayout_6.addWidget(self.label_19, 3, 1, 1, 1)
        self.st_rf_min_le = QtWidgets.QLineEdit(self.groupBox)
        self.st_rf_min_le.setStyleSheet("")
        self.st_rf_min_le.setObjectName("st_rf_min_le")
        self.gridLayout_6.addWidget(self.st_rf_min_le, 3, 2, 1, 2)
        self.st_doa_min_le = QtWidgets.QLineEdit(self.groupBox)
        self.st_doa_min_le.setStyleSheet("")
        self.st_doa_min_le.setObjectName("st_doa_min_le")
        self.gridLayout_6.addWidget(self.st_doa_min_le, 4, 2, 1, 2)
        self.st_pw_min_le = QtWidgets.QLineEdit(self.groupBox)
        self.st_pw_min_le.setStyleSheet("")
        self.st_pw_min_le.setObjectName("st_pw_min_le")
        self.gridLayout_6.addWidget(self.st_pw_min_le, 5, 2, 1, 2)
        self.st_pa_min_le = QtWidgets.QLineEdit(self.groupBox)
        self.st_pa_min_le.setStyleSheet("")
        self.st_pa_min_le.setObjectName("st_pa_min_le")
        self.gridLayout_6.addWidget(self.st_pa_min_le, 6, 2, 1, 2)
        self.st_para_set_btn = QtWidgets.QPushButton(self.groupBox)
        self.st_para_set_btn.setStyleSheet("")
        self.st_para_set_btn.setObjectName("st_para_set_btn")
        self.gridLayout_6.addWidget(self.st_para_set_btn, 7, 2, 1, 4)
        self.label_6 = QtWidgets.QLabel(self.groupBox)
        self.label_6.setObjectName("label_6")
        self.gridLayout_6.addWidget(self.label_6, 8, 1, 1, 1)
        self.pdw_num_le = QtWidgets.QLineEdit(self.groupBox)
        self.pdw_num_le.setObjectName("pdw_num_le")
        self.gridLayout_6.addWidget(self.pdw_num_le, 8, 2, 1, 4)
        self.unit_label = QtWidgets.QLabel(self.groupBox)
        self.unit_label.setObjectName("unit_label")
        self.gridLayout_6.addWidget(self.unit_label, 8, 6, 1, 1)
        self.gridLayout_5.addLayout(self.gridLayout_6, 0, 1, 1, 1)
        self.gridLayout_15.addWidget(self.groupBox, 0, 0, 1, 1)
        self.groupBox_3 = QtWidgets.QGroupBox(self.dockWidgetContents)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_3.sizePolicy().hasHeightForWidth())
        self.groupBox_3.setSizePolicy(sizePolicy)
        self.groupBox_3.setObjectName("groupBox_3")
        self.gridLayout_13 = QtWidgets.QGridLayout(self.groupBox_3)
        self.gridLayout_13.setObjectName("gridLayout_13")
        self.groupBox_8 = QtWidgets.QGroupBox(self.groupBox_3)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_8.sizePolicy().hasHeightForWidth())
        self.groupBox_8.setSizePolicy(sizePolicy)
        self.groupBox_8.setObjectName("groupBox_8")
        self.gridLayout_17 = QtWidgets.QGridLayout(self.groupBox_8)
        self.gridLayout_17.setObjectName("gridLayout_17")
        self.save_res_btn = QtWidgets.QPushButton(self.groupBox_8)
        self.save_res_btn.setStyleSheet("")
        self.save_res_btn.setObjectName("save_res_btn")
        self.gridLayout_17.addWidget(self.save_res_btn, 4, 1, 1, 1)
        self.draw_btn = QtWidgets.QPushButton(self.groupBox_8)
        self.draw_btn.setStyleSheet("")
        self.draw_btn.setObjectName("draw_btn")
        self.gridLayout_17.addWidget(self.draw_btn, 5, 1, 1, 1)
        self.save_all_flag_btn = QtWidgets.QPushButton(self.groupBox_8)
        self.save_all_flag_btn.setStyleSheet("")
        self.save_all_flag_btn.setObjectName("save_all_flag_btn")
        self.gridLayout_17.addWidget(self.save_all_flag_btn, 2, 1, 1, 1)
        self.save_flag_f_btn = QtWidgets.QPushButton(self.groupBox_8)
        self.save_flag_f_btn.setStyleSheet("")
        self.save_flag_f_btn.setObjectName("save_flag_f_btn")
        self.gridLayout_17.addWidget(self.save_flag_f_btn, 0, 1, 1, 1)
        self.draw_hist_btn = QtWidgets.QPushButton(self.groupBox_8)
        self.draw_hist_btn.setObjectName("draw_hist_btn")
        self.gridLayout_17.addWidget(self.draw_hist_btn, 6, 1, 1, 1)
        self.gridLayout_13.addWidget(self.groupBox_8, 0, 2, 1, 1)
        self.groupBox_9 = QtWidgets.QGroupBox(self.groupBox_3)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox_9.sizePolicy().hasHeightForWidth())
        self.groupBox_9.setSizePolicy(sizePolicy)
        self.groupBox_9.setObjectName("groupBox_9")
        self.gridLayout_19 = QtWidgets.QGridLayout(self.groupBox_9)
        self.gridLayout_19.setObjectName("gridLayout_19")
        self.trgt_choose_btn = QtWidgets.QPushButton(self.groupBox_9)
        self.trgt_choose_btn.setObjectName("trgt_choose_btn")
        self.gridLayout_19.addWidget(self.trgt_choose_btn, 1, 1, 1, 1)
        self.filter_wnd_btn = QtWidgets.QPushButton(self.groupBox_9)
        self.filter_wnd_btn.setStyleSheet("")
        self.filter_wnd_btn.setObjectName("filter_wnd_btn")
        self.gridLayout_19.addWidget(self.filter_wnd_btn, 3, 1, 1, 1)
        self.reset_trgt_btn = QtWidgets.QPushButton(self.groupBox_9)
        self.reset_trgt_btn.setObjectName("reset_trgt_btn")
        self.gridLayout_19.addWidget(self.reset_trgt_btn, 0, 1, 1, 1)
        self.set_prog_btn = QtWidgets.QPushButton(self.groupBox_9)
        self.set_prog_btn.setObjectName("set_prog_btn")
        self.gridLayout_19.addWidget(self.set_prog_btn, 2, 1, 1, 1)
        self.gridLayout_13.addWidget(self.groupBox_9, 0, 0, 1, 1)
        self.groupBox_7 = QtWidgets.QGroupBox(self.groupBox_3)
        self.groupBox_7.setObjectName("groupBox_7")
        self.gridLayout_16 = QtWidgets.QGridLayout(self.groupBox_7)
        self.gridLayout_16.setObjectName("gridLayout_16")
        self.AllMarkRate_le = QtWidgets.QLineEdit(self.groupBox_7)
        self.AllMarkRate_le.setObjectName("AllMarkRate_le")
        self.gridLayout_16.addWidget(self.AllMarkRate_le, 0, 4, 1, 1)
        self.curMarkRate_le = QtWidgets.QLineEdit(self.groupBox_7)
        self.curMarkRate_le.setObjectName("curMarkRate_le")
        self.gridLayout_16.addWidget(self.curMarkRate_le, 0, 1, 1, 1)
        self.label_3 = QtWidgets.QLabel(self.groupBox_7)
        self.label_3.setObjectName("label_3")
        self.gridLayout_16.addWidget(self.label_3, 0, 3, 1, 1)
        self.label_2 = QtWidgets.QLabel(self.groupBox_7)
        self.label_2.setObjectName("label_2")
        self.gridLayout_16.addWidget(self.label_2, 0, 0, 1, 1)
        self.label_4 = QtWidgets.QLabel(self.groupBox_7)
        self.label_4.setObjectName("label_4")
        self.gridLayout_16.addWidget(self.label_4, 0, 2, 1, 1)
        self.label_5 = QtWidgets.QLabel(self.groupBox_7)
        self.label_5.setObjectName("label_5")
        self.gridLayout_16.addWidget(self.label_5, 0, 5, 1, 1)
        self.gridLayout_13.addWidget(self.groupBox_7, 1, 0, 1, 3)
        self.gridLayout_15.addWidget(self.groupBox_3, 0, 1, 1, 1)
        self.gridLayout_4.addLayout(self.gridLayout_15, 0, 0, 1, 1)
        self.table_groupBox = QtWidgets.QGroupBox(self.dockWidgetContents)
        self.table_groupBox.setObjectName("table_groupBox")
        self.gridLayout_18 = QtWidgets.QGridLayout(self.table_groupBox)
        self.gridLayout_18.setObjectName("gridLayout_18")
        self.gridLayout_14 = QtWidgets.QGridLayout()
        self.gridLayout_14.setObjectName("gridLayout_14")
        self.res_tab = QtWidgets.QTabWidget(self.table_groupBox)
        self.res_tab.setObjectName("res_tab")
        self.tab = QtWidgets.QWidget()
        self.tab.setObjectName("tab")
        self.verticalLayout_4 = QtWidgets.QVBoxLayout(self.tab)
        self.verticalLayout_4.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_4.setObjectName("verticalLayout_4")
        self.edw_tableView = QtWidgets.QTableView(self.tab)
        self.edw_tableView.setObjectName("edw_tableView")
        self.verticalLayout_4.addWidget(self.edw_tableView)
        self.res_tab.addTab(self.tab, "")
        self.tab_3 = QtWidgets.QWidget()
        self.tab_3.setObjectName("tab_3")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.tab_3)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.res_tableView = QtWidgets.QTableView(self.tab_3)
        self.res_tableView.setObjectName("res_tableView")
        self.verticalLayout.addWidget(self.res_tableView)
        self.res_tab.addTab(self.tab_3, "")
        self.tab_2 = QtWidgets.QWidget()
        self.tab_2.setObjectName("tab_2")
        self.verticalLayout_5 = QtWidgets.QVBoxLayout(self.tab_2)
        self.verticalLayout_5.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_5.setObjectName("verticalLayout_5")
        self.pdw_tableView = QtWidgets.QTableView(self.tab_2)
        self.pdw_tableView.setObjectName("pdw_tableView")
        self.verticalLayout_5.addWidget(self.pdw_tableView)
        self.res_tab.addTab(self.tab_2, "")
        self.gridLayout_14.addWidget(self.res_tab, 0, 0, 1, 1)
        self.gridLayout_18.addLayout(self.gridLayout_14, 0, 0, 1, 1)
        self.gridLayout_4.addWidget(self.table_groupBox, 1, 0, 1, 1)
        self.detail_gB = QtWidgets.QGroupBox(self.dockWidgetContents)
        self.detail_gB.setObjectName("detail_gB")
        self.gridLayout_2 = QtWidgets.QGridLayout(self.detail_gB)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.groupBox_4 = QtWidgets.QGroupBox(self.detail_gB)
        self.groupBox_4.setObjectName("groupBox_4")
        self.gridLayout_3 = QtWidgets.QGridLayout(self.groupBox_4)
        self.gridLayout_3.setObjectName("gridLayout_3")
        self.gridLayout_8 = QtWidgets.QGridLayout()
        self.gridLayout_8.setObjectName("gridLayout_8")
        self.Label_15 = QtWidgets.QLabel(self.groupBox_4)
        self.Label_15.setObjectName("Label_15")
        self.gridLayout_8.addWidget(self.Label_15, 3, 0, 1, 1)
        self.rf_listWidget = QtWidgets.QListWidget(self.groupBox_4)
        self.rf_listWidget.setObjectName("rf_listWidget")
        self.gridLayout_8.addWidget(self.rf_listWidget, 0, 2, 4, 1)
        self.Label_16 = QtWidgets.QLabel(self.groupBox_4)
        self.Label_16.setObjectName("Label_16")
        self.gridLayout_8.addWidget(self.Label_16, 2, 0, 1, 1)
        self.Label_12 = QtWidgets.QLabel(self.groupBox_4)
        self.Label_12.setObjectName("Label_12")
        self.gridLayout_8.addWidget(self.Label_12, 1, 0, 1, 1)
        self.Label_13 = QtWidgets.QLabel(self.groupBox_4)
        self.Label_13.setObjectName("Label_13")
        self.gridLayout_8.addWidget(self.Label_13, 0, 0, 1, 1)
        self.rf_max_LE = QtWidgets.QLineEdit(self.groupBox_4)
        self.rf_max_LE.setStyleSheet("")
        self.rf_max_LE.setObjectName("rf_max_LE")
        self.gridLayout_8.addWidget(self.rf_max_LE, 1, 1, 1, 1)
        self.rft_LE = QtWidgets.QLineEdit(self.groupBox_4)
        self.rft_LE.setStyleSheet("")
        self.rft_LE.setObjectName("rft_LE")
        self.gridLayout_8.addWidget(self.rft_LE, 0, 1, 1, 1)
        self.rf_mid_LE = QtWidgets.QLineEdit(self.groupBox_4)
        self.rf_mid_LE.setStyleSheet("")
        self.rf_mid_LE.setObjectName("rf_mid_LE")
        self.gridLayout_8.addWidget(self.rf_mid_LE, 2, 1, 1, 1)
        self.rf_min_LE = QtWidgets.QLineEdit(self.groupBox_4)
        self.rf_min_LE.setStyleSheet("")
        self.rf_min_LE.setObjectName("rf_min_LE")
        self.gridLayout_8.addWidget(self.rf_min_LE, 3, 1, 1, 1)
        self.gridLayout_3.addLayout(self.gridLayout_8, 0, 0, 1, 1)
        spacerItem = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.gridLayout_3.addItem(spacerItem, 0, 1, 1, 1)
        self.gridLayout_2.addWidget(self.groupBox_4, 1, 0, 1, 1)
        self.groupBox_5 = QtWidgets.QGroupBox(self.detail_gB)
        self.groupBox_5.setObjectName("groupBox_5")
        self.gridLayout_9 = QtWidgets.QGridLayout(self.groupBox_5)
        self.gridLayout_9.setObjectName("gridLayout_9")
        self.gridLayout_10 = QtWidgets.QGridLayout()
        self.gridLayout_10.setObjectName("gridLayout_10")
        self.Label_17 = QtWidgets.QLabel(self.groupBox_5)
        self.Label_17.setObjectName("Label_17")
        self.gridLayout_10.addWidget(self.Label_17, 3, 0, 1, 1)
        self.pri_listWidget = QtWidgets.QListWidget(self.groupBox_5)
        self.pri_listWidget.setObjectName("pri_listWidget")
        self.gridLayout_10.addWidget(self.pri_listWidget, 0, 2, 4, 1)
        self.Label_18 = QtWidgets.QLabel(self.groupBox_5)
        self.Label_18.setObjectName("Label_18")
        self.gridLayout_10.addWidget(self.Label_18, 2, 0, 1, 1)
        self.Label_14 = QtWidgets.QLabel(self.groupBox_5)
        self.Label_14.setObjectName("Label_14")
        self.gridLayout_10.addWidget(self.Label_14, 1, 0, 1, 1)
        self.Label_19 = QtWidgets.QLabel(self.groupBox_5)
        self.Label_19.setObjectName("Label_19")
        self.gridLayout_10.addWidget(self.Label_19, 0, 0, 1, 1)
        self.pri_max_LE = QtWidgets.QLineEdit(self.groupBox_5)
        self.pri_max_LE.setStyleSheet("")
        self.pri_max_LE.setObjectName("pri_max_LE")
        self.gridLayout_10.addWidget(self.pri_max_LE, 1, 1, 1, 1)
        self.prit_LE = QtWidgets.QLineEdit(self.groupBox_5)
        self.prit_LE.setStyleSheet("")
        self.prit_LE.setObjectName("prit_LE")
        self.gridLayout_10.addWidget(self.prit_LE, 0, 1, 1, 1)
        self.pri_mid_LE = QtWidgets.QLineEdit(self.groupBox_5)
        self.pri_mid_LE.setStyleSheet("")
        self.pri_mid_LE.setObjectName("pri_mid_LE")
        self.gridLayout_10.addWidget(self.pri_mid_LE, 2, 1, 1, 1)
        self.pri_min_LE = QtWidgets.QLineEdit(self.groupBox_5)
        self.pri_min_LE.setStyleSheet("")
        self.pri_min_LE.setObjectName("pri_min_LE")
        self.gridLayout_10.addWidget(self.pri_min_LE, 3, 1, 1, 1)
        self.gridLayout_9.addLayout(self.gridLayout_10, 0, 0, 1, 1)
        self.gridLayout_2.addWidget(self.groupBox_5, 1, 1, 1, 1)
        self.groupBox_6 = QtWidgets.QGroupBox(self.detail_gB)
        self.groupBox_6.setObjectName("groupBox_6")
        self.gridLayout_11 = QtWidgets.QGridLayout(self.groupBox_6)
        self.gridLayout_11.setObjectName("gridLayout_11")
        self.gridLayout_12 = QtWidgets.QGridLayout()
        self.gridLayout_12.setObjectName("gridLayout_12")
        self.Label_20 = QtWidgets.QLabel(self.groupBox_6)
        self.Label_20.setObjectName("Label_20")
        self.gridLayout_12.addWidget(self.Label_20, 3, 0, 1, 1)
        self.pw_listWidget = QtWidgets.QListWidget(self.groupBox_6)
        self.pw_listWidget.setObjectName("pw_listWidget")
        self.gridLayout_12.addWidget(self.pw_listWidget, 0, 2, 4, 1)
        self.Label_21 = QtWidgets.QLabel(self.groupBox_6)
        self.Label_21.setObjectName("Label_21")
        self.gridLayout_12.addWidget(self.Label_21, 2, 0, 1, 1)
        self.Label_22 = QtWidgets.QLabel(self.groupBox_6)
        self.Label_22.setObjectName("Label_22")
        self.gridLayout_12.addWidget(self.Label_22, 1, 0, 1, 1)
        self.Label_23 = QtWidgets.QLabel(self.groupBox_6)
        self.Label_23.setObjectName("Label_23")
        self.gridLayout_12.addWidget(self.Label_23, 0, 0, 1, 1)
        self.pw_max_LE = QtWidgets.QLineEdit(self.groupBox_6)
        self.pw_max_LE.setStyleSheet("")
        self.pw_max_LE.setObjectName("pw_max_LE")
        self.gridLayout_12.addWidget(self.pw_max_LE, 1, 1, 1, 1)
        self.pwt_LE = QtWidgets.QLineEdit(self.groupBox_6)
        self.pwt_LE.setStyleSheet("")
        self.pwt_LE.setObjectName("pwt_LE")
        self.gridLayout_12.addWidget(self.pwt_LE, 0, 1, 1, 1)
        self.pw_mid_LE = QtWidgets.QLineEdit(self.groupBox_6)
        self.pw_mid_LE.setStyleSheet("")
        self.pw_mid_LE.setObjectName("pw_mid_LE")
        self.gridLayout_12.addWidget(self.pw_mid_LE, 2, 1, 1, 1)
        self.pw_min_LE = QtWidgets.QLineEdit(self.groupBox_6)
        self.pw_min_LE.setStyleSheet("")
        self.pw_min_LE.setObjectName("pw_min_LE")
        self.gridLayout_12.addWidget(self.pw_min_LE, 3, 1, 1, 1)
        self.gridLayout_11.addLayout(self.gridLayout_12, 0, 0, 1, 1)
        self.gridLayout_2.addWidget(self.groupBox_6, 1, 2, 1, 1)
        spacerItem1 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.gridLayout_2.addItem(spacerItem1, 0, 2, 1, 1)
        self.gridLayout_7 = QtWidgets.QGridLayout()
        self.gridLayout_7.setObjectName("gridLayout_7")
        self.Label_11 = QtWidgets.QLabel(self.detail_gB)
        self.Label_11.setObjectName("Label_11")
        self.gridLayout_7.addWidget(self.Label_11, 0, 0, 1, 1)
        self.label_17 = QtWidgets.QLabel(self.detail_gB)
        self.label_17.setObjectName("label_17")
        self.gridLayout_7.addWidget(self.label_17, 1, 3, 1, 1)
        self.Label_10 = QtWidgets.QLabel(self.detail_gB)
        self.Label_10.setObjectName("Label_10")
        self.gridLayout_7.addWidget(self.Label_10, 1, 0, 1, 1)
        self.pw_LE = QtWidgets.QLineEdit(self.detail_gB)
        self.pw_LE.setStyleSheet("")
        self.pw_LE.setObjectName("pw_LE")
        self.gridLayout_7.addWidget(self.pw_LE, 1, 1, 1, 1)
        self.label_12 = QtWidgets.QLabel(self.detail_gB)
        self.label_12.setObjectName("label_12")
        self.gridLayout_7.addWidget(self.label_12, 0, 5, 1, 1)
        self.label_18 = QtWidgets.QLabel(self.detail_gB)
        self.label_18.setObjectName("label_18")
        self.gridLayout_7.addWidget(self.label_18, 1, 5, 1, 1)
        self.doa_LE = QtWidgets.QLineEdit(self.detail_gB)
        self.doa_LE.setStyleSheet("")
        self.doa_LE.setObjectName("doa_LE")
        self.gridLayout_7.addWidget(self.doa_LE, 0, 4, 1, 1)
        self.pihao_LE = QtWidgets.QLineEdit(self.detail_gB)
        self.pihao_LE.setStyleSheet("")
        self.pihao_LE.setObjectName("pihao_LE")
        self.gridLayout_7.addWidget(self.pihao_LE, 0, 1, 1, 1)
        self.Label_9 = QtWidgets.QLabel(self.detail_gB)
        self.Label_9.setObjectName("Label_9")
        self.gridLayout_7.addWidget(self.Label_9, 0, 3, 1, 1)
        self.label_16 = QtWidgets.QLabel(self.detail_gB)
        self.label_16.setObjectName("label_16")
        self.gridLayout_7.addWidget(self.label_16, 1, 2, 1, 1)
        self.pa_LE = QtWidgets.QLineEdit(self.detail_gB)
        self.pa_LE.setStyleSheet("")
        self.pa_LE.setObjectName("pa_LE")
        self.gridLayout_7.addWidget(self.pa_LE, 1, 4, 1, 1)
        self.gridLayout_2.addLayout(self.gridLayout_7, 0, 0, 1, 2)
        self.gridLayout_4.addWidget(self.detail_gB, 3, 0, 1, 1)
        self.info_dock.setWidget(self.dockWidgetContents)
        Marker.addDockWidget(QtCore.Qt.DockWidgetArea(1), self.info_dock)
        self.draw_dock = QtWidgets.QDockWidget(Marker)
        self.draw_dock.setMinimumSize(QtCore.QSize(720, 180))
        self.draw_dock.setObjectName("draw_dock")
        self.dockWidgetContents_3 = QtWidgets.QWidget()
        self.dockWidgetContents_3.setBaseSize(QtCore.QSize(720, 360))
        self.dockWidgetContents_3.setObjectName("dockWidgetContents_3")
        self.verticalLayout_6 = QtWidgets.QVBoxLayout(self.dockWidgetContents_3)
        self.verticalLayout_6.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_6.setObjectName("verticalLayout_6")
        self.draw_dock.setWidget(self.dockWidgetContents_3)
        Marker.addDockWidget(QtCore.Qt.DockWidgetArea(2), self.draw_dock)
        self.actionDaka = QtWidgets.QAction(Marker)
        self.actionDaka.setObjectName("actionDaka")
        self.actionLog_dock = QtWidgets.QAction(Marker)
        self.actionLog_dock.setObjectName("actionLog_dock")
        self.actionClose_File = QtWidgets.QAction(Marker)
        self.actionClose_File.setObjectName("actionClose_File")
        self.actionDraw_dock = QtWidgets.QAction(Marker)
        self.actionDraw_dock.setObjectName("actionDraw_dock")
        self.actionFileShow = QtWidgets.QAction(Marker)
        self.actionFileShow.setObjectName("actionFileShow")
        self.actionFileHide = QtWidgets.QAction(Marker)
        self.actionFileHide.setObjectName("actionFileHide")
        self.actionDebugMode = QtWidgets.QAction(Marker)
        self.actionDebugMode.setCheckable(True)
        self.actionDebugMode.setObjectName("actionDebugMode")
        self.actionXxg = QtWidgets.QAction(Marker)
        self.actionXxg.setObjectName("actionXxg")
        self.actionStruct = QtWidgets.QAction(Marker)
        self.actionStruct.setObjectName("actionStruct")
        self.actionOrigin_dataparse = QtWidgets.QAction(Marker)
        self.actionOrigin_dataparse.setObjectName("actionOrigin_dataparse")
        self.actionTime = QtWidgets.QAction(Marker)
        self.actionTime.setObjectName("actionTime")
        self.actionPlsnum = QtWidgets.QAction(Marker)
        self.actionPlsnum.setObjectName("actionPlsnum")
        self.actionXd = QtWidgets.QAction(Marker)
        self.actionXd.setObjectName("actionXd")
        self.actionXxg_z = QtWidgets.QAction(Marker)
        self.actionXxg_z.setObjectName("actionXxg_z")
        self.tbBgColorAction = QtWidgets.QAction(Marker)
        self.tbBgColorAction.setObjectName("tbBgColorAction")
        self.drawBgColorAction = QtWidgets.QAction(Marker)
        self.drawBgColorAction.setObjectName("drawBgColorAction")
        self.action_histMark = QtWidgets.QAction(Marker)
        self.action_histMark.setObjectName("action_histMark")
        self.actionX5_DGN = QtWidgets.QAction(Marker)
        self.actionX5_DGN.setObjectName("actionX5_DGN")
        self.menuDataSource.addAction(self.actionStruct)
        self.menuDataSource.addAction(self.actionXxg)
        self.menuDataSource.addAction(self.actionXxg_z)
        self.menuDataSource.addAction(self.actionXd)
        self.menuDataSource.addAction(self.actionX5_DGN)
        self.menu_4.addAction(self.actionTime)
        self.menu_4.addAction(self.actionPlsnum)
        self.menu.addAction(self.actionDaka)
        self.menu.addAction(self.actionClose_File)
        self.menu.addAction(self.actionDebugMode)
        self.menu.addAction(self.menuDataSource.menuAction())
        self.menu.addAction(self.menu_4.menuAction())
        self.menu.addAction(self.actionOrigin_dataparse)
        self.menu_3.addAction(self.actionFileShow)
        self.menu_3.addAction(self.actionFileHide)
        self.menu_2.addAction(self.actionLog_dock)
        self.menu_2.addAction(self.actionDraw_dock)
        self.menu_2.addAction(self.menu_3.menuAction())
        self.menu_2.addAction(self.tbBgColorAction)
        self.menu_2.addAction(self.drawBgColorAction)
        self.menu_5.addAction(self.action_histMark)
        self.menubar.addAction(self.menu.menuAction())
        self.menubar.addAction(self.menu_2.menuAction())
        self.menubar.addAction(self.menu_5.menuAction())

        self.retranslateUi(Marker)
        self.res_tab.setCurrentIndex(0)
        self.actionLog_dock.triggered.connect(self.info_dock.show)
        self.actionDraw_dock.triggered.connect(self.draw_dock.show)
        self.actionFileHide.triggered.connect(self.groupBox_3.hide)
        self.actionFileShow.triggered.connect(self.groupBox_3.show)
        QtCore.QMetaObject.connectSlotsByName(Marker)

    def retranslateUi(self, Marker):
        _translate = QtCore.QCoreApplication.translate
        Marker.setWindowTitle(_translate("Marker", "全脉冲情报分析软件"))
        self.menu.setTitle(_translate("Marker", "文件"))
        self.menuDataSource.setTitle(_translate("Marker", "数据来源选择"))
        self.menu_4.setTitle(_translate("Marker", "读取模式选择"))
        self.menu_2.setTitle(_translate("Marker", "显示"))
        self.menu_3.setTitle(_translate("Marker", "文件信息"))
        self.menu_5.setTitle(_translate("Marker", "功能"))
        self.info_dock.setWindowTitle(_translate("Marker", "信息"))
        self.groupBox.setTitle(_translate("Marker", "文件设置"))
        self.label.setText(_translate("Marker", "/"))
        self.label_28.setText(_translate("Marker", "脉宽范围"))
        self.label_23.setText(_translate("Marker", "幅度范围"))
        self.label_24.setText(_translate("Marker", "°"))
        self.label_26.setText(_translate("Marker", "~"))
        self.st_pa_max_le.setText(_translate("Marker", "200"))
        self.st_pw_max_le.setText(_translate("Marker", "200"))
        self.label_29.setText(_translate("Marker", "~"))
        self.label_30.setText(_translate("Marker", "us"))
        self.st_doa_max_le.setText(_translate("Marker", "360"))
        self.label_25.setText(_translate("Marker", "dBm"))
        self.label_22.setText(_translate("Marker", "方位范围"))
        self.label_27.setText(_translate("Marker", "~"))
        self.label_20.setText(_translate("Marker", "MHz"))
        self.st_read_btn.setText(_translate("Marker", "自动处理"))
        self.once_read_btn.setText(_translate("Marker", "单步处理"))
        self.close_file_btn.setText(_translate("Marker", "结束读取"))
        self.st_rf_max_le.setText(_translate("Marker", "18000"))
        self.label_21.setText(_translate("Marker", "~"))
        self.choose_file_btn.setText(_translate("Marker", "选择文件"))
        self.label_15.setText(_translate("Marker", "文件名："))
        self.label_19.setText(_translate("Marker", "载频范围"))
        self.st_rf_min_le.setText(_translate("Marker", "8000"))
        self.st_doa_min_le.setText(_translate("Marker", "0"))
        self.st_pw_min_le.setText(_translate("Marker", "0"))
        self.st_pa_min_le.setText(_translate("Marker", "0"))
        self.st_para_set_btn.setText(_translate("Marker", "参数设置"))
        self.label_6.setText(_translate("Marker", "单步长度"))
        self.pdw_num_le.setText(_translate("Marker", "100"))
        self.unit_label.setText(_translate("Marker", "ms"))
        self.groupBox_3.setTitle(_translate("Marker", "功能按钮"))
        self.groupBox_8.setTitle(_translate("Marker", "结果显示与保存"))
        self.save_res_btn.setText(_translate("Marker", "保存辐射源结果"))
        self.draw_btn.setText(_translate("Marker", "绘制全景图"))
        self.save_all_flag_btn.setText(_translate("Marker", "保存全部标记结果"))
        self.save_flag_f_btn.setText(_translate("Marker", "保存当前标记结果"))
        self.draw_hist_btn.setText(_translate("Marker", "绘制直方图"))
        self.groupBox_9.setTitle(_translate("Marker", "分析"))
        self.trgt_choose_btn.setText(_translate("Marker", "目标筛选"))
        self.filter_wnd_btn.setText(_translate("Marker", "全脉冲人工分析"))
        self.reset_trgt_btn.setText(_translate("Marker", "清空辐射源结果"))
        self.set_prog_btn.setText(_translate("Marker", "设置进度"))
        self.groupBox_7.setTitle(_translate("Marker", "标注率"))
        self.label_3.setText(_translate("Marker", "总标注率"))
        self.label_2.setText(_translate("Marker", "当前标注率"))
        self.label_4.setText(_translate("Marker", "%"))
        self.label_5.setText(_translate("Marker", "%"))
        self.table_groupBox.setTitle(_translate("Marker", "表格显示"))
        self.res_tab.setTabText(self.res_tab.indexOf(self.tab), _translate("Marker", "辐射源"))
        self.res_tab.setTabText(self.res_tab.indexOf(self.tab_3), _translate("Marker", "结果"))
        self.res_tab.setTabText(self.res_tab.indexOf(self.tab_2), _translate("Marker", "脉冲描述字"))
        self.detail_gB.setTitle(_translate("Marker", "目标详细信息"))
        self.groupBox_4.setTitle(_translate("Marker", "载频(MHz)"))
        self.Label_15.setText(_translate("Marker", "最小值"))
        self.Label_16.setText(_translate("Marker", "中心值"))
        self.Label_12.setText(_translate("Marker", "最大值"))
        self.Label_13.setText(_translate("Marker", "类 型"))
        self.groupBox_5.setTitle(_translate("Marker", "重复周期(us)"))
        self.Label_17.setText(_translate("Marker", "最小值"))
        self.Label_18.setText(_translate("Marker", "中心值"))
        self.Label_14.setText(_translate("Marker", "最大值"))
        self.Label_19.setText(_translate("Marker", "类 型"))
        self.groupBox_6.setTitle(_translate("Marker", "脉宽(us)"))
        self.Label_20.setText(_translate("Marker", "最小值"))
        self.Label_21.setText(_translate("Marker", "中心值"))
        self.Label_22.setText(_translate("Marker", "最大值"))
        self.Label_23.setText(_translate("Marker", "类 型"))
        self.Label_11.setText(_translate("Marker", "批号"))
        self.label_17.setText(_translate("Marker", "幅度"))
        self.Label_10.setText(_translate("Marker", "脉宽"))
        self.label_12.setText(_translate("Marker", "°"))
        self.label_18.setText(_translate("Marker", "dBm"))
        self.Label_9.setText(_translate("Marker", "方位"))
        self.label_16.setText(_translate("Marker", "us"))
        self.draw_dock.setWindowTitle(_translate("Marker", "绘图"))
        self.actionDaka.setText(_translate("Marker", "Open"))
        self.actionLog_dock.setText(_translate("Marker", "info_dock"))
        self.actionClose_File.setText(_translate("Marker", "Close File"))
        self.actionDraw_dock.setText(_translate("Marker", "draw_dock"))
        self.actionFileShow.setText(_translate("Marker", "show"))
        self.actionFileHide.setText(_translate("Marker", "hide"))
        self.actionDebugMode.setText(_translate("Marker", "DebugMode"))
        self.actionXxg.setText(_translate("Marker", "xxg_c"))
        self.actionStruct.setText(_translate("Marker", "struct"))
        self.actionOrigin_dataparse.setText(_translate("Marker", "原始数据解析"))
        self.actionTime.setText(_translate("Marker", "按时间读取"))
        self.actionPlsnum.setText(_translate("Marker", "按脉冲数读取"))
        self.actionXd.setText(_translate("Marker", "xd"))
        self.actionXxg_z.setText(_translate("Marker", "xxg_z"))
        self.tbBgColorAction.setText(_translate("Marker", "设置表格背景颜色"))
        self.drawBgColorAction.setText(_translate("Marker", "设置绘图背景颜色"))
        self.action_histMark.setText(_translate("Marker", "历史信息标注"))
        self.actionX5_DGN.setText(_translate("Marker", "x5_dgn"))
