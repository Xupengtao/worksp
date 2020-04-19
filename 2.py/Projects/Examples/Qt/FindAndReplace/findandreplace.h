#ifndef FINDANDREPLACE_H
#define FINDANDREPLACE_H

#include <QDialog>

namespace Ui {
class FindAndReplaceDlg;
}

class FindAndReplaceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindAndReplaceDlg(QWidget *parent = 0);
    ~FindAndReplaceDlg();

private:
    Ui::FindAndReplaceDlg *ui;
};

#endif // FINDANDREPLACE_H
