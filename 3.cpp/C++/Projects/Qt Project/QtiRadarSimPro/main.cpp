#include "mainwindow.h"
#include <QApplication>

UINT 	_RadarBuffer::GlobalToaH    = 0;
UINT 	_RadarBuffer::GlobalToaL    = 0;
UINT 	_RadarBuffer::RadarRCCLNum  = 0;
UINT	_RadarBuffer::SortTimes     = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
