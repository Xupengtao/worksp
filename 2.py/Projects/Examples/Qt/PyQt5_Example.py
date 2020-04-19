# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

#!/usr/bin/python3
# -*- coding: utf-8 -*-
 
"""
Py40.com PyQt5 tutorial 
 
In this example, we create a simple
window in PyQt5.
 
author: Jan Bodnar
website: py40.com 
last edited: January 2015
"""
 
#import sys
# 
##这里我们提供必要的引用。基本控件位于pyqt5.qtwidgets模块中。
#from PyQt5.QtWidgets import QApplication, QWidget
# 
# 
#if __name__ == '__main__':
#    #每一pyqt5应用程序必须创建一个应用程序对象。sys.argv参数是一个列表，从命令行输入参数。
#    app = QApplication(sys.argv)
#    #QWidget部件是pyqt5所有用户界面对象的基类。他为QWidget提供默认构造函数。默认构造函数没有父类。
#    w = QWidget()
#    #resize()方法调整窗口的大小。这离是250px宽150px高
#    w.resize(250, 150)
#    #move()方法移动窗口在屏幕上的位置到x = 300，y = 300坐标。
#    w.move(300, 300)
#    #设置窗口的标题
#    w.setWindowTitle('Simple')
#    #显示在屏幕上
#    w.show()
#    
#    #系统exit()方法确保应用程序干净的退出
#    #的exec_()方法有下划线。因为执行是一个Python关键词。因此，exec_()代替
#    sys.exit(app.exec_())

"""
py40 PyQt5 tutorial 
 
This example shows an icon
in the titlebar of the window.
 
author: Jan Bodnar
website: py40.com 
last edited: January 2015
"""
 
#import sys
#from PyQt5.QtWidgets import QApplication, QWidget
#from PyQt5.QtGui import QIcon
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI() #界面绘制交给InitUi方法
#        
#        
#    def initUI(self):
#        #设置窗口的位置和大小
#        self.setGeometry(300, 300, 300, 220)  
#        #设置窗口的标题
#        self.setWindowTitle('Icon')
#        #设置窗口的图标，引用当前目录下的web.png图片
#        self.setWindowIcon(QIcon('web.png'))        
#        
#        #显示窗口
#        self.show()
#        
#        
#if __name__ == '__main__':
#    #创建应用程序和对象
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_()) 

#import sys
#from PyQt5.QtWidgets import (QWidget, QToolTip, 
#    QPushButton, QApplication)
#from PyQt5.QtGui import QFont    
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):
#        #这种静态的方法设置一个用于显示工具提示的字体。我们使用10px滑体字体。
#        QToolTip.setFont(QFont('SansSerif', 10))
#        
#        #创建一个提示，我们称之为settooltip()方法。我们可以使用丰富的文本格式
#        self.setToolTip('This is a <b>QWidget</b> widget')
#        
#        #创建一个PushButton并为他设置一个tooltip
#        btn = QPushButton('Button', self)
#        btn.setToolTip('This is a <b>QPushButton</b> widget')
#        
#        #btn.sizeHint()显示默认尺寸
#        btn.resize(btn.sizeHint())
#        
#        #移动窗口的位置
#        btn.move(50, 50)       
#        
#        self.setGeometry(300, 300, 300, 200)
#        self.setWindowTitle('Tooltips')    
#        self.show()
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

#import sys
#from PyQt5.QtWidgets import QWidget, QPushButton, QApplication
#from PyQt5.QtCore import QCoreApplication
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):               
#        
#        qbtn = QPushButton('Quit', self)
#        qbtn.clicked.connect(QCoreApplication.instance().quit)
#        qbtn.resize(qbtn.sizeHint())
#        qbtn.move(50, 50)       
#        
#        self.setGeometry(300, 300, 250, 150)
#        self.setWindowTitle('Quit button')    
#        self.show()
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
消息框
默认情况下,如果我们单击x按钮窗口就关门了。有时我们想修改这个默认的行为。例如我们在编辑器中修改了一个文件,当关闭他的时候，我们显示一个消息框确认。
"""
 
#import sys
#from PyQt5.QtWidgets import QWidget, QMessageBox, QApplication
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):               
#        
#        self.setGeometry(300, 300, 250, 150)        
#        self.setWindowTitle('Message box')    
#        self.show()
#        
#        
#    def closeEvent(self, event):
#        
#        reply = QMessageBox.question(self, 'Message',
#            "Are you sure to quit?", QMessageBox.Yes | 
#            QMessageBox.No, QMessageBox.No)
# 
#        if reply == QMessageBox.Yes:
#            event.accept()
#        else:
#            event.ignore()        
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

#!/usr/bin/python3
# -*- coding: utf-8 -*-
 
"""
窗口显示在屏幕的中间
下面的脚本显示了如何在屏幕中心显示窗口。
"""
 
#import sys
#from PyQt5.QtWidgets import QWidget, QDesktopWidget, QApplication
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):               
#        
#        self.resize(250, 150)
#        self.center()
#        
#        self.setWindowTitle('Center')    
#        self.show()
#        
#    
#    #控制窗口显示在屏幕中心的方法    
#    def center(self):
#        
#        #获得窗口
#        qr = self.frameGeometry()
#        #获得屏幕中心点
#        cp = QDesktopWidget().availableGeometry().center()
#        #显示到屏幕中心
#        qr.moveCenter(cp)
#        self.move(qr.topLeft())
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())  

#!/usr/bin/python3
# -*- coding: utf-8 -*-
 
"""
表格布局 QGridLayout
表格布局将空间划分为行和列。我们使用QGridLayout类创建一个网格布局。
"""
 
#import sys
#from PyQt5.QtWidgets import (QWidget, QGridLayout, 
#    QPushButton, QApplication)
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):
#        
#        grid = QGridLayout()
#        self.setLayout(grid)
# 
#        names = ['Cls', 'Bck', '', 'Close',
#                 '7', '8', '9', '/',
#                '4', '5', '6', '*',
#                 '1', '2', '3', '-',
#                '0', '.', '=', '+']
#        
#        positions = [(i,j) for i in range(5) for j in range(4)]
#        
#        for position, name in zip(positions, names):
#            
#            if name == '':
#                continue
#            button = QPushButton(name)
#            grid.addWidget(button, *position)
#            
#        self.move(300, 150)
#        self.setWindowTitle('Calculator')
#        self.show()
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
评论的例子
控件可以在网格中跨越多个行或列。在下一个示例中,我们说明了这一点。
"""
 
#import sys
#from PyQt5.QtWidgets import (QWidget, QLabel, QLineEdit, 
#    QTextEdit, QGridLayout, QApplication)
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):
#        
#        title = QLabel('Title')
#        author = QLabel('Author')
#        review = QLabel('Review')
# 
#        titleEdit = QLineEdit()
#        authorEdit = QLineEdit()
#        reviewEdit = QTextEdit()
# 
#        grid = QGridLayout()
#        grid.setSpacing(10)
# 
#        grid.addWidget(title, 1, 0)
#        grid.addWidget(titleEdit, 1, 1)
# 
#        grid.addWidget(author, 2, 0)
#        grid.addWidget(authorEdit, 2, 1)
# 
#        grid.addWidget(review, 3, 0)
#        grid.addWidget(reviewEdit, 3, 1, 5, 1)
#        
#        self.setLayout(grid) 
#        
#        self.setGeometry(300, 300, 350, 300)
#        self.setWindowTitle('Review')    
#        self.show()
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
状态栏
状态栏用于显示状态信息。
"""
 
#import sys
#from PyQt5.QtWidgets import QMainWindow, QApplication
# 
# 
#class Example(QMainWindow):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):               
#        
#        self.statusBar().showMessage('Ready')
#        
#        self.setGeometry(300, 300, 250, 150)
#        self.setWindowTitle('Statusbar')    
#        self.show()
# 
# 
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
菜单栏
菜单栏是常见的窗口应用程序的一部分。(Mac OS将菜单条不同。得到类似的结果,我们可以添加以下行:menubar.setNativeMenuBar(假)。)
"""
 
#import sys
#from PyQt5.QtWidgets import QMainWindow, QAction, qApp, QApplication
#from PyQt5.QtGui import QIcon
# 
# 
#class Example(QMainWindow):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):               
#        
#        exitAction = QAction(QIcon('exit.png'), '&Exit', self)        
#        exitAction.setShortcut('Ctrl+Q')
#        exitAction.setStatusTip('Exit application')
#        exitAction.triggered.connect(qApp.quit)
# 
#        self.statusBar()
# 
#        #创建一个菜单栏
#        menubar = self.menuBar()
#        #添加菜单
#        fileMenu = menubar.addMenu('&File')
#        #添加事件
#        fileMenu.addAction(exitAction)
#        
#        self.setGeometry(300, 300, 300, 200)
#        self.setWindowTitle('Menubar')    
#        self.show()
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
工具栏
工具栏提供了一个快速访问的入口。
"""
 
#import sys
#from PyQt5.QtWidgets import QMainWindow, QAction, qApp, QApplication
#from PyQt5.QtGui import QIcon
# 
# 
#class Example(QMainWindow):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):               
#        
#        exitAction = QAction(QIcon('exit24.png'), 'Exit', self)
#        exitAction.setShortcut('Ctrl+Q')
#        exitAction.triggered.connect(qApp.quit)
#        
#        self.toolbar = self.addToolBar('Exit')
#        self.toolbar.addAction(exitAction)
#        
#        self.setGeometry(300, 300, 300, 200)
#        self.setWindowTitle('Toolbar')    
#        self.show()
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
把他们放在一起
在本节的最后一个例子中,我们将创建一个菜单条,工具栏和状态栏的小窗口
"""
 
#import sys
#from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
#from PyQt5.QtGui import QIcon
# 
# 
#class Example(QMainWindow):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):               
#        
#        textEdit = QTextEdit()
#        self.setCentralWidget(textEdit)
# 
#        exitAction = QAction(QIcon('exit24.png'), 'Exit', self)
#        exitAction.setShortcut('Ctrl+Q')
#        exitAction.setStatusTip('Exit application')
#        exitAction.triggered.connect(self.close)
# 
#        self.statusBar()
# 
#        menubar = self.menuBar()
#        fileMenu = menubar.addMenu('&File')
#        fileMenu.addAction(exitAction)
# 
#        toolbar = self.addToolBar('Exit')
#        toolbar.addAction(exitAction)
#        
#        self.setGeometry(300, 300, 350, 250)
#        self.setWindowTitle('Main window')    
#        self.show()
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
信号槽 Signals & slots
这是一个使用信号槽的PyQt5例子。
"""
 
#import sys
#from PyQt5.QtCore import Qt
#from PyQt5.QtWidgets import (QWidget, QLCDNumber, QSlider, 
#    QVBoxLayout, QApplication)
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):
#        
#        lcd = QLCDNumber(self)
#        sld = QSlider(Qt.Horizontal, self)
# 
#        vbox = QVBoxLayout()
#        vbox.addWidget(lcd)
#        vbox.addWidget(sld)
# 
#        self.setLayout(vbox)
#        sld.valueChanged.connect(lcd.display)
#        
#        self.setGeometry(300, 300, 250, 150)
#        self.setWindowTitle('Signal & slot')
#        self.show()
#        
# 
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())
    
"""
重新实现事件处理器
在PyQt5中常通过重新实现事件处理器来处理事件。
"""
 
#import sys
#from PyQt5.QtCore import Qt
#from PyQt5.QtWidgets import QWidget, QApplication
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
#        
#        self.setGeometry(300, 300, 250, 150)
#        self.setWindowTitle('Event handler')
#        self.show()
#        
#        
#    def keyPressEvent(self, e):
#        
#        if e.key() == Qt.Key_Escape:
#            self.close()
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
事件发送者
有时需要知道信号是由哪个控件发出的。对此PyQt5提供了sender()方法。
"""
 
#import sys
#from PyQt5.QtWidgets import QMainWindow, QPushButton, QApplication
# 
# 
#class Example(QMainWindow):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
# 
#        btn1 = QPushButton("Button 1", self)
#        btn1.move(30, 50)
# 
#        btn2 = QPushButton("Button 2", self)
#        btn2.move(150, 50)
#      
#        btn1.clicked.connect(self.buttonClicked)            
#        btn2.clicked.connect(self.buttonClicked)
#        
#        self.statusBar()
#        
#        self.setGeometry(300, 300, 290, 150)
#        self.setWindowTitle('Event sender')
#        self.show()
#        
#        
#    def buttonClicked(self):
#      
#        sender = self.sender()
#        self.statusBar().showMessage(sender.text() + ' was pressed')
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
发出信号
通过QObject创建的对象可以发出信号。下面的示例演示了如何发出自定义信号
"""
 
#import sys
#from PyQt5.QtCore import pyqtSignal, QObject
#from PyQt5.QtWidgets import QMainWindow, QApplication
# 
# 
#class Communicate(QObject):
#    
#    closeApp = pyqtSignal() 
#    
# 
#class Example(QMainWindow):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
# 
#        self.c = Communicate()
#        self.c.closeApp.connect(self.close)       
#        
#        self.setGeometry(300, 300, 290, 150)
#        self.setWindowTitle('Emit signal')
#        self.show()
#        
#        
#    def mousePressEvent(self, event):
#        
#        self.c.closeApp.emit()
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
QInputDialog
QInputDialog提供了一种简单方便的对话框从用户得到一个值。输入值可以是字符串,一个数字,或一个项目从一个列表。
"""
 
#import sys
#from PyQt5.QtWidgets import (QWidget, QPushButton, QLineEdit, 
#    QInputDialog, QApplication)
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
# 
#        self.btn = QPushButton('Dialog', self)
#        self.btn.move(20, 20)
#        self.btn.clicked.connect(self.showDialog)
#        
#        self.le = QLineEdit(self)
#        self.le.move(130, 22)
#        
#        self.setGeometry(300, 300, 290, 150)
#        self.setWindowTitle('Input dialog')
#        self.show()
#        
#        
#    def showDialog(self):
#        
#        text, ok = QInputDialog.getText(self, 'Input Dialog', 
#            'Enter your name:')
#        
#        if ok:
#            self.le.setText(str(text))
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
QColorDialog
QColorDialog显示一个用于选择颜色值的对话框。
"""
 
#import sys
#from PyQt5.QtWidgets import (QWidget, QPushButton, QFrame, 
#    QColorDialog, QApplication)
#from PyQt5.QtGui import QColor
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
# 
#        col = QColor(0, 0, 0) 
# 
#        self.btn = QPushButton('Dialog', self)
#        self.btn.move(20, 20)
# 
#        self.btn.clicked.connect(self.showDialog)
# 
#        self.frm = QFrame(self)
#        self.frm.setStyleSheet("QWidget { background-color: %s }" 
#            % col.name())
#        self.frm.setGeometry(130, 22, 100, 100)            
#        
#        self.setGeometry(300, 300, 250, 180)
#        self.setWindowTitle('Color dialog')
#        self.show()
#        
#        
#    def showDialog(self):
#      
#        col = QColorDialog.getColor()
# 
#        if col.isValid():
#            self.frm.setStyleSheet("QWidget { background-color: %s }"
#                % col.name())
#            
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
QFontDialog
QFontDialog对话框用以选择字体
"""
#import sys
#from PyQt5.QtWidgets import (QWidget, QVBoxLayout, QPushButton, 
#    QSizePolicy, QLabel, QFontDialog, QApplication)
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
# 
#        vbox = QVBoxLayout()
# 
#        btn = QPushButton('Dialog', self)
#        btn.setSizePolicy(QSizePolicy.Fixed,
#            QSizePolicy.Fixed)
#        
#        btn.move(20, 20)
# 
#        vbox.addWidget(btn)
# 
#        btn.clicked.connect(self.showDialog)
#        
#        self.lbl = QLabel('Knowledge only matters', self)
#        self.lbl.move(130, 20)
# 
#        vbox.addWidget(self.lbl)
#        self.setLayout(vbox)          
#        
#        self.setGeometry(300, 300, 250, 180)
#        self.setWindowTitle('Font dialog')
#        self.show()
#        
#        
#    def showDialog(self):
# 
#        font, ok = QFontDialog.getFont()
#        if ok:
#            self.lbl.setFont(font)
#        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
QFileDialog
QFileDialog是一个让用户选择文件和目录的对话框，可用以选择打开或保存文件
"""

#import sys
#from PyQt5.QtWidgets import (QMainWindow, QTextEdit, 
#    QAction, QFileDialog, QApplication)
#from PyQt5.QtGui import QIcon
# 
# 
#class Example(QMainWindow):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
# 
#        self.textEdit = QTextEdit()
#        self.setCentralWidget(self.textEdit)
#        self.statusBar()
# 
#        openFile = QAction(QIcon('open.png'), 'Open', self)
#        openFile.setShortcut('Ctrl+O')
#        openFile.setStatusTip('Open new File')
#        openFile.triggered.connect(self.showDialog)
# 
#        menubar = self.menuBar()
#        fileMenu = menubar.addMenu('&File')
#        fileMenu.addAction(openFile)       
#        
#        self.setGeometry(300, 300, 350, 300)
#        self.setWindowTitle('File dialog')
#        self.show()
#        
#        
#    def showDialog(self):
# 
#        fname = QFileDialog.getOpenFileName(self, 'Open file', '/home')
# 
#        if fname[0]:
#            f = open(fname[0], 'r')
# 
#            with f:
#                data = f.read()
#                self.textEdit.setText(data)        
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
QCheckBox
QCheckBox复选框控件，它有两个状态:打开和关闭，他是一个带有文本标签（Label）的控件。复选框常用于表示程序中可以启用或禁用的功能。
"""

#import sys
#from PyQt5.QtWidgets import QWidget, QCheckBox, QApplication
#from PyQt5.QtCore import Qt
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
# 
#        self.cb = QCheckBox('Show title', self)
#        self.cb.move(20, 20)
#        self.cb.toggle()
#        self.cb.stateChanged.connect(self.changeTitle)
#        
#        self.setGeometry(300, 300, 250, 150)
#        self.setWindowTitle('QCheckBox')
#        self.show()
#        
#        
#    def changeTitle(self, state):
#      
#        print(self.cb.checkState())
#        if state == Qt.Checked:
#            self.setWindowTitle('QCheckBox')
#        else:
#            self.setWindowTitle('')
#            
#        
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
开关按钮 Toggle button
ToggleButton是QPushButton的一种特殊模式。它是一个有两种状态的按钮：按下与未按下。通过点击在这两种状态间来回切换。这种功能在某些场景会很实用。
"""

#import sys
#from PyQt5.QtWidgets import (QWidget, QPushButton, 
#    QFrame, QApplication)
#from PyQt5.QtGui import QColor
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
# 
#        self.col = QColor(0, 0, 0)       
# 
#        redb = QPushButton('Red', self)
#        redb.setCheckable(True)
#        redb.move(10, 10)
# 
#        redb.clicked[bool].connect(self.setColor)
# 
#        greenb = QPushButton('Green', self)
#        greenb.setCheckable(True)
#        greenb.move(10, 60)
# 
#        greenb.clicked[bool].connect(self.setColor)
# 
#        blueb = QPushButton('Blue', self)
#        blueb.setCheckable(True)
#        blueb.move(10, 110)
# 
#        blueb.clicked[bool].connect(self.setColor)
# 
#        self.square = QFrame(self)
#        self.square.setGeometry(150, 20, 100, 100)
#        self.square.setStyleSheet("QWidget { background-color: %s }" %  
#            self.col.name())
#        
#        self.setGeometry(300, 300, 280, 170)
#        self.setWindowTitle('Toggle button')
#        self.show()
#        
#        
#    def setColor(self, pressed):
#        
#        source = self.sender()
#        
#        if pressed:
#            val = 255
#        else: val = 0
#                        
#        if source.text() == "Red":
#            self.col.setRed(val)                
#        elif source.text() == "Green":
#            self.col.setGreen(val)             
#        else:
#            self.col.setBlue(val) 
#            
#        self.square.setStyleSheet("QFrame { background-color: %s }" %
#            self.col.name())  
#              
#if __name__ == '__main__':
#    
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
滑动条 QSlider
QSlider是一个带有简单滑块的控件。滑块可以前后拖动。我们可以通过拖动选择一个特定的值。有时使用滑动条比直接输入数字或使用旋转框更加自然。

在下面的例子中，我们会显示一个滑动条与一个标签，标签用于显示图片，并通过滑动条控件图片的显示 。

注意这里图片只能用ico格式的，png格式图片会显示不了。
"""
#import sys
#from PyQt5.QtWidgets import (QWidget, QSlider, 
#    QLabel, QApplication)
#from PyQt5.QtCore import Qt
#from PyQt5.QtGui import QPixmap
# 
# 
#class Example(QWidget):
#    
#    def __init__(self):
#        super().__init__()
#        
#        self.initUI()
#        
#        
#    def initUI(self):      
# 
#        sld = QSlider(Qt.Horizontal, self)
#        sld.setFocusPolicy(Qt.NoFocus)
#        sld.setGeometry(30, 40, 100, 30)
#        sld.valueChanged[int].connect(self.changeValue)
#        
#        self.label = QLabel(self)
#        self.label.setPixmap(QPixmap('audio.ico'))
#        self.label.setGeometry(160, 40, 80, 30)
#        
#        self.setGeometry(300, 300, 280, 170)
#        self.setWindowTitle('QSlider')
#        self.show()
#        
#        
#    def changeValue(self, value):
# 
#        if value == 0:
#            self.label.setPixmap(QPixmap('audio.ico'))
#        elif value > 0 and value <= 30:
#            self.label.setPixmap(QPixmap('min.ico'))
#        elif value > 30 and value < 80:
#            self.label.setPixmap(QPixmap('med.ico'))
#        else:
#            self.label.setPixmap(QPixmap('max.ico'))
#            
# 
#if __name__ == '__main__':
# 
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())    

"""
进度条QProgressBar
一个进度条是一个显示任务进展的控件。QProgressBar控件提供了一个水平或垂直PyQt5工具包的进度条。程序员可以设置进度条的最小和最大值。默认值是0到99。
"""

#import sys
#from PyQt5.QtWidgets import (QWidget, QProgressBar,
#                             QPushButton, QApplication)
#from PyQt5.QtCore import QBasicTimer
# 
# 
#class Example(QWidget):
#    def __init__(self):
#        super().__init__()
# 
#        self.initUI()
# 
#    def initUI(self):
# 
#        self.pbar = QProgressBar(self)
#        self.pbar.setGeometry(30, 40, 200, 25)
# 
#        self.btn = QPushButton('Start', self)
#        self.btn.move(40, 80)
#        self.btn.clicked.connect(self.doAction)
# 
#        self.timer = QBasicTimer()
#        self.step = 0
# 
#        self.setGeometry(300, 300, 280, 170)
#        self.setWindowTitle('QProgressBar')
#        self.show()
# 
#    def timerEvent(self, e):
# 
#        if self.step >= 100:
#            self.timer.stop()
#            self.btn.setText('Finished')
#            return
# 
#        self.step = self.step + 1
#        self.pbar.setValue(self.step)
# 
#    def doAction(self):
# 
#        if self.timer.isActive():
#            self.timer.stop()
#            self.btn.setText('Start')
#        else:
#            self.timer.start(100, self)
#            self.btn.setText('Stop')
# 
# 
#if __name__ == '__main__':
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
日历控件 QCalendarWidget
QCalendarWidget提供了一个基于月份的日历控件。它使用户以一种简单直观的方式来选择日期。
"""

#import sys
#from PyQt5.QtWidgets import (QWidget, QCalendarWidget,
#                             QLabel, QApplication)
#from PyQt5.QtCore import QDate
# 
# 
#class Example(QWidget):
#    def __init__(self):
#        super().__init__()
# 
#        self.initUI()
# 
#    def initUI(self):
#        cal = QCalendarWidget(self)
#        cal.setGridVisible(True)
#        cal.move(20, 20)
#        cal.clicked[QDate].connect(self.showDate)
# 
#        self.lbl = QLabel(self)
#        date = cal.selectedDate()
#        self.lbl.setText(date.toString())
#        self.lbl.move(130, 260)
# 
#        self.setGeometry(300, 300, 350, 300)
#        self.setWindowTitle('Calendar')
#        self.show()
# 
#    def showDate(self, date):
#        self.lbl.setText(date.toString())
# 
# 
#if __name__ == '__main__':
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
QPixmap
QPixmap是用于处理图像的控件。是优化的显示图像在屏幕上。在我们的代码示例中,我们将使用QPixmap窗口显示一个图像。
"""
#import sys
#from PyQt5.QtWidgets import (QWidget, QHBoxLayout,
#                             QLabel, QApplication)
#from PyQt5.QtGui import QPixmap
# 
# 
#class Example(QWidget):
#    def __init__(self):
#        super().__init__()
# 
#        self.initUI()
# 
#    def initUI(self):
#        hbox = QHBoxLayout(self)
#        pixmap = QPixmap("icon.png")
# 
#        lbl = QLabel(self)
#        lbl.setPixmap(pixmap)
# 
#        hbox.addWidget(lbl)
#        self.setLayout(hbox)
# 
#        self.move(300, 200)
#        self.setWindowTitle('Red Rock')
#        self.show()
# 
# 
#if __name__ == '__main__':
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
文本框 QLineEdit
QLineEdit是用于输入或编辑单行文本的控件。它还有撤销重做、剪切复制和拖拽功能。
"""

#import sys
#from PyQt5.QtWidgets import (QWidget, QLabel,
#                             QLineEdit, QApplication)
# 
# 
#class Example(QWidget):
#    def __init__(self):
#        super().__init__()
# 
#        self.initUI()
# 
#    def initUI(self):
#        self.lbl = QLabel(self)
#        qle = QLineEdit(self)
# 
#        qle.move(60, 100)
#        self.lbl.move(60, 40)
# 
#        qle.textChanged[str].connect(self.onChanged)
# 
#        self.setGeometry(300, 300, 280, 170)
#        self.setWindowTitle('QLineEdit')
#        self.show()
# 
#    def onChanged(self, text):
#        self.lbl.setText(text)
#        self.lbl.adjustSize()
# 
# 
#if __name__ == '__main__':
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
QSplitter
通过QSplitter，用户可以拖动子控件边界来调整子控件的尺寸。在下面的示例中，我们展示了三个由两个QSplitter组织的QFrame控件。
"""

#import sys
#from PyQt5.QtWidgets import (QWidget, QHBoxLayout, QFrame,
#                             QSplitter, QStyleFactory, QApplication)
#from PyQt5.QtCore import Qt
# 
# 
#class Example(QWidget):
#    def __init__(self):
#        super().__init__()
# 
#        self.initUI()
# 
#    def initUI(self):
#        hbox = QHBoxLayout(self)
# 
#        topleft = QFrame(self)
#        topleft.setFrameShape(QFrame.StyledPanel)
# 
#        topright = QFrame(self)
#        topright.setFrameShape(QFrame.StyledPanel)
# 
#        bottom = QFrame(self)
#        bottom.setFrameShape(QFrame.StyledPanel)
# 
#        splitter1 = QSplitter(Qt.Horizontal)
#        splitter1.addWidget(topleft)
#        splitter1.addWidget(topright)
# 
#        splitter2 = QSplitter(Qt.Vertical)
#        splitter2.addWidget(splitter1)
#        splitter2.addWidget(bottom)
# 
#        hbox.addWidget(splitter2)
#        self.setLayout(hbox)
# 
#        self.setGeometry(300, 300, 300, 200)
#        self.setWindowTitle('QSplitter')
#        self.show()
# 
#    def onChanged(self, text):
#        self.lbl.setText(text)
#        self.lbl.adjustSize()
# 
# 
#if __name__ == '__main__':
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())

"""
下拉列表 QComboBox
QComboBox是允许用户从下拉列表中进行选择的控件。
"""

#import sys
#from PyQt5.QtWidgets import (QWidget, QLabel,
#                             QComboBox, QApplication)
# 
# 
#class Example(QWidget):
#    def __init__(self):
#        super().__init__()
# 
#        self.initUI()
# 
#    def initUI(self):
#        self.lbl = QLabel("Ubuntu", self)
# 
#        combo = QComboBox(self)
#        combo.addItem("Ubuntu")
#        combo.addItem("Mandriva")
#        combo.addItem("Fedora")
#        combo.addItem("Arch")
#        combo.addItem("Gentoo")
# 
#        combo.move(50, 50)
#        self.lbl.move(50, 150)
# 
#        combo.activated[str].connect(self.onActivated)
# 
#        self.setGeometry(300, 300, 300, 200)
#        self.setWindowTitle('QComboBox')
#        self.show()
# 
#    def onActivated(self, text):
#        self.lbl.setText(text)
#        self.lbl.adjustSize()
# 
# 
#if __name__ == '__main__':
#    app = QApplication(sys.argv)
#    ex = Example()
#    sys.exit(app.exec_())