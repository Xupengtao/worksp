#ifndef TPAURLUI_H
#define TPAURLUI_H

#include <QMainWindow>

namespace Ui {
class TpaUrlui;
}

class TpaUrlui : public QMainWindow
{
    Q_OBJECT

public:
    explicit TpaUrlui(QWidget *parent = 0);
    ~TpaUrlui();

private:
    Ui::TpaUrlui *ui;
};

#endif // TPAURLUI_H
