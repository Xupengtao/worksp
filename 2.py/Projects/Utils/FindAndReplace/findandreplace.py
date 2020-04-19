#!/usr/bin/env python
# Copyright (c) 2007-8 Qtrac Ltd. All rights reserved.
# This program or module is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published
# by the Free Software Foundation, either version 2 of the License, or
# version 3 of the License, or (at your option) any later version. It is
# provided for educational purposes and is distributed in the hope that
# it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
# the GNU General Public License for more details.
import os
os.chdir(r'E:/WorkSpace/Python/FindAndReplace/')

import re
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from PyQt5.QtWidgets import *
import ui_findandreplace

MAC = "qt_mac_set_native_menubar" in dir()


class FindAndReplaceDlg(QDialog,
        ui_findandreplace.Ui_FindAndReplaceDlg):

    def __init__(self, text, parent=None):
        super().__init__()
        self.__text = text
        self.__index = 0
        self.setupUi(self)


    def on_findButton_clicked(self):
        if match is not None:
            self.__index = match.end()
            self.emit(SIGNAL("found"), match.start())
        else:
            self.emit(SIGNAL("notfound"))

if __name__ == "__main__":
    import sys

    def found(where):
        print("Found at %d" % where) 

    def nomore():
        print("No more found")

    app = QApplication(sys.argv)
    form = FindAndReplaceDlg('')
    form.connect(form, SIGNAL("found"), found)
    form.connect(form, SIGNAL("notfound"), nomore)
    form.show()
    app.exec_()
    print(form.text()) 

