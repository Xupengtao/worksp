#ifndef _RADARMODIFYDIALOG_H
#define _RADARMODIFYDIALOG_H

#include <QMap>
#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include "qtiradarstruct.h"
#include "ui__radarmodifydialog.h"

namespace Ui {
class _RadarModifyDialog;
}

class _RadarModifyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit _RadarModifyDialog(QWidget *parent = nullptr) : QDialog(parent), ui(new Ui::_RadarModifyDialog)
    {
        ui->setupUi(this);
        QMap<QString, _PriType> PriTypes;
        PriTypes.insert("PRI_FIX", PRI_FIX);
        PriTypes.insert("PRI_DITHER", PRI_DITHER);
        PriTypes.insert("PRI_JAGGING", PRI_JAGGING);
        PriTypes.insert("PRI_SLIDER", PRI_SLIDER);
        foreach(const QString &str, PriTypes.keys())
        {
            ui->priTypeComboBox->addItem(str,PriTypes.value(str));
        }
        QMap<QString, _RfType> RfTypes;
        RfTypes.insert("RF_CONTINUOUSLY", RF_CONTINUOUSLY);
        RfTypes.insert("RF_FIX", RF_FIX);
        RfTypes.insert("RF_AGILITY", RF_AGILITY);
        foreach(const QString &str, RfTypes.keys())
        {
            ui->rfTypeComboBox->addItem(str,RfTypes.value(str));
        }
        QMap<QString, _PwType> PwTypes;
        PwTypes.insert("PW_FIX", PW_FIX);
        PwTypes.insert("PW_DEFINE", PW_DEFINE);
        foreach(const QString &str, PwTypes.keys())
        {
            ui->pwTypeComboBox->addItem(str,PwTypes.value(str));
        }
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);             // 未输入雷达名时禁用OK按键
        
//       QStringList PriTypeList, RfTypeList, PwTypeList;
//       PriTypeList << "PRI_FIX" << "PRI_DITHER" << "PRI_JAGGING" << "PRI_SLIDER";
//       RfTypeList << "RF_CONTINUOUSLY" << "RF_FIX" << "RF_AGILITY";
//       PwTypeList << "PW_FIX" << "PW_DEFINE";
//       ui->priTypeComboBox->addItems(PriTypeList);
//       ui->rfTypeComboBox->addItems(RfTypeList);
//       ui->pwTypeComboBox->addItems(PwTypeList);
    }
    ~_RadarModifyDialog()
    {
        delete ui;
    }
    void SetValue(_QtiRadarPara& QtiRadarPara)
    {
        ui->priTypeComboBox->setCurrentText("PRI_FIX");
        ui->priValSpinBox->setValue(int(QtiRadarPara.QtPriPara.dPri));
        ui->priRangeSpinBox->setValue(int(QtiRadarPara.QtPriPara.dRange));
        ui->priNumsSpinBox->setValue(int(QtiRadarPara.QtPriPara.nPriNum));
        ui->priGroupsSpinBox->setValue(int(QtiRadarPara.QtPriPara.nGroupNum));
        ui->priArraySizeSpinBox->setValue(int(QtiRadarPara.QtPriPara.PriArrLen));
        ui->priStepSpinBox->setValue(int(QtiRadarPara.QtPriPara.dPriStep));
        QString PriArrayQStr = ArrayToQString(QtiRadarPara.QtPriPara.PriArr, JAG_NUM);
        ui->priArrayLineEdit->setText(PriArrayQStr);
        ui->rfTypeComboBox->setCurrentText("RF_FIX");
        ui->rfValSpinBox->setValue(int(QtiRadarPara.QtRfPara.dFre));
        ui->rfNumsSpinBox->setValue(int(QtiRadarPara.QtRfPara.nFreNum));
        ui->rfGroupsSpinBox->setValue(int(QtiRadarPara.QtRfPara.nGroupNum));
        ui->rfArraySizeSpinBox->setValue(int(QtiRadarPara.QtRfPara.RfArrLen));
        QString RfArrayQStr = ArrayToQString(QtiRadarPara.QtRfPara.RfArr, AGI_NUM);
        ui->rfArrayLineEdit->setText(RfArrayQStr);
        ui->pwTypeComboBox->setCurrentText("PW_FIX");
        ui->pwValSpinBox->setValue(int(QtiRadarPara.QtPwPara.dPw));
        ui->pwNumsSpinBox->setValue(int(QtiRadarPara.QtPwPara.nPwNum));
        ui->pwGroupsSpinBox->setValue(int(QtiRadarPara.QtPwPara.nGroupNum));
        ui->pwArraySizeSpinBox->setValue(int(QtiRadarPara.QtPwPara.PwArrLen));
        QString PwArrayQStr = ArrayToQString(QtiRadarPara.QtPwPara.PwArr, PW_NUM);
        ui->pwArrayLineEdit->setText(PwArrayQStr);
    }
    QString GetName()
    {
        return ui->radarNameLineEdit->text();
    }
    _QtiRadarPara GetValue()
    {
        _QtiRadarPara QtiRadarPara;
        QtiRadarPara.QtPriPara.PriType      = _PriType(ui->priTypeComboBox->currentData().toUInt());
        QtiRadarPara.QtPriPara.dPri         = size_t(ui->priValSpinBox->value());
        QtiRadarPara.QtPriPara.dRange       = size_t(ui->priRangeSpinBox->value());
        QtiRadarPara.QtPriPara.nPriNum      = size_t(ui->priNumsSpinBox->value());
        QtiRadarPara.QtPriPara.nGroupNum    = size_t(ui->priGroupsSpinBox->value());
        QtiRadarPara.QtPriPara.PriArrLen    = size_t(ui->priArraySizeSpinBox->value());
        QtiRadarPara.QtPriPara.dPriStep     = size_t(ui->priStepSpinBox->value());
        QStringToArray(ui->priArrayLineEdit->text(), QtiRadarPara.QtPriPara.PriArr, QtiRadarPara.QtPriPara.PriArrLen);
        QtiRadarPara.QtRfPara.RfType        = _RfType(ui->rfTypeComboBox->currentData().toUInt());
        QtiRadarPara.QtRfPara.dFre          = size_t(ui->rfValSpinBox->value());
        QtiRadarPara.QtRfPara.nFreNum       = size_t(ui->rfNumsSpinBox->value());
        QtiRadarPara.QtRfPara.nGroupNum     = size_t(ui->rfGroupsSpinBox->value());
        QtiRadarPara.QtRfPara.RfArrLen      = size_t(ui->rfArraySizeSpinBox->value());
        QStringToArray(ui->rfArrayLineEdit->text(), QtiRadarPara.QtRfPara.RfArr, QtiRadarPara.QtRfPara.RfArrLen);
        QtiRadarPara.QtPwPara.PwType        = _PwType(ui->pwTypeComboBox->currentData().toUInt());
        QtiRadarPara.QtPwPara.dPw           = size_t(ui->pwValSpinBox->value());
        QtiRadarPara.QtPwPara.nPwNum        = size_t(ui->pwNumsSpinBox->value());
        QtiRadarPara.QtPwPara.nGroupNum     = size_t(ui->pwGroupsSpinBox->value());
        QtiRadarPara.QtPwPara.PwArrLen      = size_t(ui->pwArraySizeSpinBox->value());
        QStringToArray(ui->pwArrayLineEdit->text(), QtiRadarPara.QtPwPara.PwArr, QtiRadarPara.QtPwPara.PwArrLen);
        return QtiRadarPara;
    }
    QString ArrayToQString(size_t *Arr, size_t ArrLens)
    {
        QString QStrTmp;
        for(size_t i = 0; i < ArrLens; i++)
        {
            if(Arr[i] == 0)
            {
                break;
            }
            QStrTmp += ',' + QString::number(int(Arr[i]));
        }
        return QStrTmp;
    }
    void QStringToArray(const QString& inQStr, size_t* Arr, size_t ArrLens)
    {
        QStringList QStrLst;
        QStrLst = inQStr.split(QRegExp("[,*/^]"));
        size_t i = 0;
        foreach(const QString &str, QStrLst)
        {
            if(i < ArrLens)
            {
                Arr[i++] = size_t(str.toUInt());
            }
            else
            {
                break;
            }
        }
    }
private:
    Ui::_RadarModifyDialog *ui;  
private slots:
    void on_radarNameLineEdit_editingFinished()
    {
        if(ui->radarNameLineEdit->text() != "")
        {
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
            ui->radarNameWarningLabel->setText("");
        }
        else
        {
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            ui->radarNameWarningLabel->setText("Input Radar Name!");
        }
    }
    void on_buttonBox_accepted()
    {
        if(ui->radarNameLineEdit->text() == "")
        {
            QMessageBox::warning(this, tr("Warning"), tr("Please input  radar`s name"), QMessageBox::Ok, QMessageBox::NoButton);
        }
    }
    void on_priTypeComboBox_currentIndexChanged(const QString &arg1)
    {
        if(arg1 == "PRI_FIX")
        {
            ui->priValSpinBox->setEnabled(true);
            ui->priRangeSpinBox->setEnabled(false);
            ui->priNumsSpinBox->setEnabled(false);
            ui->priGroupsSpinBox->setEnabled(false);
            ui->priArraySizeSpinBox->setEnabled(false);
            ui->priStepSpinBox->setEnabled(false);
            ui->priArrayLineEdit->setEnabled(false);
        }
        else if(arg1 == "PRI_DITHER")
        {
            ui->priValSpinBox->setEnabled(true);
            ui->priRangeSpinBox->setEnabled(true);
            ui->priNumsSpinBox->setEnabled(false);
            ui->priGroupsSpinBox->setEnabled(false);
            ui->priArraySizeSpinBox->setEnabled(false);
            ui->priStepSpinBox->setEnabled(false);
            ui->priArrayLineEdit->setEnabled(false);
        }
        else if(arg1 == "PRI_JAGGING")
        {
            ui->priValSpinBox->setEnabled(false);
            ui->priRangeSpinBox->setEnabled(false);
            ui->priNumsSpinBox->setEnabled(true);
            ui->priGroupsSpinBox->setEnabled(true);
            ui->priArraySizeSpinBox->setEnabled(true);
            ui->priStepSpinBox->setEnabled(false);
            ui->priArrayLineEdit->setEnabled(true);
        }
        else if(arg1 == "PRI_SLIDER")
        {
            ui->priValSpinBox->setEnabled(false);
            ui->priRangeSpinBox->setEnabled(false);
            ui->priNumsSpinBox->setEnabled(true);
            ui->priGroupsSpinBox->setEnabled(true);
            ui->priArraySizeSpinBox->setEnabled(true);
            ui->priStepSpinBox->setEnabled(true);
            ui->priArrayLineEdit->setEnabled(true);
        }
    }
    void on_rfTypeComboBox_currentIndexChanged(const QString &arg1)
    {
        if(arg1 == "RF_FIX")
        {
            ui->rfValSpinBox->setEnabled(true);
            ui->rfNumsSpinBox->setEnabled(false);
            ui->rfGroupsSpinBox->setEnabled(false);
            ui->rfArraySizeSpinBox->setEnabled(false);
            ui->rfArrayLineEdit->setEnabled(false);
        }
        else if(arg1 == "RF_CONTINUOUSLY")
        {
            ui->rfValSpinBox->setEnabled(true);
            ui->rfNumsSpinBox->setEnabled(false);
            ui->rfGroupsSpinBox->setEnabled(false);
            ui->rfArraySizeSpinBox->setEnabled(false);
            ui->rfArrayLineEdit->setEnabled(false);
        }
        else if(arg1 == "RF_AGILITY")
        {
            ui->rfValSpinBox->setEnabled(false);
            ui->rfNumsSpinBox->setEnabled(true);
            ui->rfGroupsSpinBox->setEnabled(true);
            ui->rfArraySizeSpinBox->setEnabled(true);
            ui->rfArrayLineEdit->setEnabled(true);
        }
        else if(arg1 == "RF_DIVERSITY")
        {
            ui->rfValSpinBox->setEnabled(false);
            ui->rfNumsSpinBox->setEnabled(false);
            ui->rfGroupsSpinBox->setEnabled(false);
            ui->rfArraySizeSpinBox->setEnabled(false);
            ui->rfArrayLineEdit->setEnabled(false);
        }
    }
    void on_pwTypeComboBox_currentIndexChanged(const QString &arg1)
    {
        if(arg1 == "PW_FIX")
        {
            ui->pwValSpinBox->setEnabled(true);
            ui->pwNumsSpinBox->setEnabled(false);
            ui->pwGroupsSpinBox->setEnabled(false);
            ui->pwArraySizeSpinBox->setEnabled(false);
            ui->pwArrayLineEdit->setEnabled(false);
        }
        else if(arg1 == "PW_DEFINE")
        {
            ui->pwValSpinBox->setEnabled(false);
            ui->pwNumsSpinBox->setEnabled(true);
            ui->pwGroupsSpinBox->setEnabled(true);
            ui->pwArraySizeSpinBox->setEnabled(true);
            ui->pwArrayLineEdit->setEnabled(true);
        }
    }
};

#endif // _RADARMODIFYDIALOG_H
