#include "tpaurlui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TpaUrlui w;
    w.show();

    return a.exec();
}
