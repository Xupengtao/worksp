# -*- coding: utf-8 -*-
"""
Created on Tue Nov  7 17:16:36 2017

@author: xupengtao
"""

import sys
import re
from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QFileDialog
from Vhdltext_verify import *

class Vhdltext_verify_Gui(QtWidgets.QWidget):
    def __init__ (self, parent = None):
        super(Vhdltext_verify_Gui,self).__init__()
        self.quit_button = QtWidgets.QPushButton("run", self)
        self.quit_button.clicked.connect(self.verify)
    
    def verify(self):
        files, filetype = QFileDialog.getOpenFileNames(self, "选取多文件","E:/Workspace/14m148/FPGA/OED45/daban/spu_fpga","Text Files(*.vhd)")
        print(files)
        for filename in files:
            print (filename)
            vhd_t = []
            a = Vhdltext_verify()
            a.file_recomment(vhd_t,str(filename))
            for i in range(len(vhd_t)):
                vhd_ttemp = vhd_t[i]
                vhd_t[i] = a.resp_verify(vhd_t[i])
                vhd_ttemp2 = vhd_t[i]
                vhd_t[i] = a.entity_verify(vhd_t[i])
                vhd_t[i] = a.arch_verify(vhd_t[i])
                if vhd_ttemp2 == vhd_t[i]:
                    vhd_t[i] = vhd_ttemp
#                    vhd_t[i] = vhd_t[i]
            vhdfile_pattern = re.compile(r'.*/(\w+)\.vhd')
            match = vhdfile_pattern.match(filename)
            if match:
                a.file_out(vhd_t,'E:/Workspace/remove_comment/out/'+match.group(1)+'.vhd')
            
app = QtWidgets.QApplication(sys.argv)
a = Vhdltext_verify_Gui()
a.show()
sys.exit(app.exec_())