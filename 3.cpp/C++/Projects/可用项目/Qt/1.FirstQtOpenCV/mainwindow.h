#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QCloseEvent"
#include "QSettings"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_QinputPushButton_pressed();

    void on_QoutputPushButton_pressed();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void LoadSettings();
    void SaveSettings();

    Ui::MainWindow *ui;
    QSettings settings;
};

#endif // MAINWINDOW_H
