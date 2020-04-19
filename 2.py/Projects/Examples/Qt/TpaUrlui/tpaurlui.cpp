#include "tpaurlui.h"
#include "ui_tpaurlui.h"

TpaUrlui::TpaUrlui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TpaUrlui)
{
    ui->setupUi(this);
}

TpaUrlui::~TpaUrlui()
{
    delete ui;
}
