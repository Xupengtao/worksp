#include "mainwindow.h"
#include <QApplication>

UINT 	_RadarBuffer::GlobalToaH    = 0;
UINT 	_RadarBuffer::GlobalToaL    = 0;
UINT 	_RadarBuffer::RadarRCCLNum  = 0;
UINT    _RadarBuffer::SortTimes     = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
