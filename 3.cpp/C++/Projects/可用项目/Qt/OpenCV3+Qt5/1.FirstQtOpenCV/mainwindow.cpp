#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDir"
#include "QFile"
#include "QMessageBox"
#include "opencv2/opencv.hpp"

MainWindow::MainWindow(QWidget *parent):QMainWindow(parent), ui(new Ui::MainWindow), settings("Packt", "QtCreater", this)
{
    ui->setupUi(this);
    LoadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_QinputPushButton_pressed()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Input Image", QDir::currentPath(), "Images(*.jpg *.png *.bmp)");
    if(QFile::exists(fileName))
    {
        ui->QinputLineEdit->setText(fileName);
    }
}

void MainWindow::on_QoutputPushButton_pressed()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Select Output Image", QDir::currentPath(), "*.jpg;;*.png;;*.bmp");
    if(!fileName.isEmpty())
    {
        ui->QoutputLineEdit->setText(fileName);
        cv::Mat inpImg, outImg;
        inpImg = cv::imread(ui->QinputLineEdit->text().toStdString());
        if(ui->QmedianBlurRadioButton->isChecked())
        {
            cv::medianBlur(inpImg, outImg, 5);
        }
        else if(ui->QgaussianBlurRadioButton->isChecked())
        {
            cv::GaussianBlur(inpImg, outImg, cv::Size(5, 5), 1.25);
        }
        cv::imwrite(fileName.toStdString(), outImg);
        if(ui->QdisplayImageCheckBox->isChecked())
        {
            cv::imshow("Output Image", outImg);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    int result = QMessageBox::warning(this,
                                      "Exit",
                                      "Are you sure you want to close this program?",
                                      QMessageBox::Yes,
                                      QMessageBox::No);
    if(result == QMessageBox::Yes)
    {
        SaveSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::LoadSettings()
{
    ui->QinputLineEdit->setText(settings.value("inputLineEdit", "").toString());
    ui->QoutputLineEdit->setText(settings.value("outputLineEdit", "").toString());
    ui->QmedianBlurRadioButton->setChecked(settings.value("medianBlurRadioButton", true).toBool());
    ui->QgaussianBlurRadioButton->setChecked(settings.value("gaussianBlurRadioButton", false).toBool());
    ui->QdisplayImageCheckBox->setChecked(settings.value("displayImageCheckBox", false).toBool());
}

void MainWindow::SaveSettings()
{
    settings.setValue("inputLineEdit", ui->QinputLineEdit->text());
    settings.setValue("outputLineEdit", ui->QoutputLineEdit->text());
    settings.setValue("medianBlurRadioButton", ui->QmedianBlurRadioButton->isChecked());
    settings.setValue("gaussianBlurRadioButton", ui->QgaussianBlurRadioButton->isChecked());
    settings.setValue("displayImageCheckBox", ui->QdisplayImageCheckBox->isChecked());
}
