#include "findandreplace.h"
#include "ui_findandreplace.h"

FindAndReplaceDlg::FindAndReplaceDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindAndReplaceDlg)
{
    ui->setupUi(this);
}

FindAndReplaceDlg::~FindAndReplaceDlg()
{
    delete ui;
}
