#ifndef QT_THREADIRADARSIMPRO_H
#define QT_THREADIRADARSIMPRO_H

#include <QMap>
#include <QDir>
#include <vector>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QFileDialog>
#include <QInputDialog>
#include <QReadWriteLock>
#include "qtiradarstruct.h"
#include "radarmodifydialog.h"
#include "iRadarSimPro/iRadarSimPro.h"
#include "PdwStruct/PdwStruct.hpp"

class Qt_iRadarSimPro : public QThread
{
    Q_OBJECT
private:
    QReadWriteLock                  RwLock;
    bool                            ThreadSign      = true;
    bool                            RunSign         = false;
    QString                         saveFileName;
    _iRadarSimPro<_iRadar_Xyg>      iRadarSimPro;
    QMap<QString, _RadarMode>       RadarModeMap;       // 雷达参数Map
    QMap<QString, _QtPlatFormPara>  QtPlatFormParaMap;  // 平台参数Map
    QMap<QString, _LocVec>          QtTraceParaMap;     // 轨迹参数Map
    vector<_QtPlatFormPara>         QtPlatFormParaVec;  // 平台参数Vec
protected:
    void    run()   Q_DECL_OVERRIDE
    {
        while(ThreadSign == true)
        {
            if(RunSign == true)
            {
                QtPlatFormParaVec.clear();
                foreach(const QString &str, QtPlatFormParaMap.keys())
                {
                    QtPlatFormParaVec.push_back(QtPlatFormParaMap[str]);
                }
                bool hasPlatForm = false;
                for(size_t i = 0; i < QtPlatFormParaVec.size(); i++)
                {
                    UCHAR j = 0;
                    foreach(const QString& RadarName, QtPlatFormParaVec[i].RadarNameList)
                    {
                        hasPlatForm = true;
                        iRadarSimPro.GetPlatForm(i).SetRadar(RadarModeMap[RadarName], j++);
                    }
                    if(QtPlatFormParaVec[i].TraceName != "")
                    {
                        iRadarSimPro.GetPlatForm(i).SetAzElDis(QtTraceParaMap[QtPlatFormParaVec[i].TraceName]);
                    }
                    else
                    {
                        _LocVec LocVecTmp;
                        LocVecTmp.resize(1);
                        LocVecTmp[0].Az            = UINT(QtPlatFormParaVec[i].DoaInit);
                        LocVecTmp[0].El            = 0;
                        LocVecTmp[0].Distance      = 100;
                        LocVecTmp[0].JitterRange   = 5;
                        LocVecTmp[0].SortTimes     = 0;
                        iRadarSimPro.GetPlatForm(i).SetAzElDis(LocVecTmp);
                    }
                }
                if(hasPlatForm == false)
                {
                    RunSign = false;
                    continue;
                }
//                int runtimes = 0;
//                iRadarSimPro.runThread(RunSign, runtimes);
                iRadarSimPro.WriteFile(saveFileName.toStdString(), 100);
                RunSign = false;
            }
            msleep(500);
        }
    }
public:
    Qt_iRadarSimPro()
    {
        ;
    }
public slots:
    void    setSaveFilePath(QString filepath)
    {
        saveFileName = filepath;
    }
    void    AddRadar(size_t RadarNo, bool& AddSign, QString& RadarName)
    {
        Q_UNUSED(RadarNo);
        _QtiRadarPara QtiRadarPara;
        _RadarModifyDialog* radarModifyDialog = new _RadarModifyDialog();
        Qt::WindowFlags flags = radarModifyDialog->windowFlags();                    // 获取对话框属性标签
        radarModifyDialog->setWindowFlags(flags | Qt::MSWindowsFixedSizeDialogHint); // 设置窗口大小为固定大小
//        radarModifyDialog->setAttribute(Qt::WA_DeleteOnClose);                     // 对话框关闭时自动删除，模态显示不可使用，否则点击接受继续操作对话框会使用野指针
        radarModifyDialog->SetValue(QtiRadarPara);
        int ret = radarModifyDialog->exec();
        if(ret == QDialog::Accepted)
        {
            RadarName   = radarModifyDialog->GetName();
            if(RadarName.isEmpty())
            {
                AddSign = false;
            }
            else if(RadarModeMap.contains(RadarName))
            {
                QMessageBox::warning(radarModifyDialog, "Warning", tr("There is already a radar with the same name!"));
                qDebug() << tr("There is already a radar with the same name!");
                AddSign = false;
            }
            else
            {
                QtiRadarPara = radarModifyDialog->GetValue();
                _RadarMode RadarMode;
                RadarMode.PriPara.Modify(QtiRadarPara.QtPriPara.PriType,
                                         QtiRadarPara.QtPriPara.dPri,
                                         QtiRadarPara.QtPriPara.dRange,
                                         QtiRadarPara.QtPriPara.nPriNum,
                                         QtiRadarPara.QtPriPara.nGroupNum,
                                         QtiRadarPara.QtPriPara.PriArr,
                                         QtiRadarPara.QtPriPara.PriArrLen,
                                         QtiRadarPara.QtPriPara.dPriStep
                                         );
                RadarMode.RfPara.Modify( QtiRadarPara.QtRfPara.RfType,
                                         QtiRadarPara.QtRfPara.dFre,
                                         QtiRadarPara.QtRfPara.nFreNum,
                                         QtiRadarPara.QtRfPara.nGroupNum,
                                         QtiRadarPara.QtRfPara.RfArr,
                                         QtiRadarPara.QtRfPara.RfArrLen
                                         );
                RadarMode.PwPara.Modify( QtiRadarPara.QtPwPara.PwType,
                                         QtiRadarPara.QtPwPara.dPw,
                                         QtiRadarPara.QtPwPara.nPwNum,
                                         QtiRadarPara.QtPwPara.nGroupNum,
                                         QtiRadarPara.QtPwPara.PwArr,
                                         QtiRadarPara.QtPwPara.PwArrLen
                                         );
                RadarModeMap.insert(RadarName, RadarMode);
                AddSign = true;
            }
        }
        else
        {
            AddSign = false;
        }
        delete radarModifyDialog;
    }
    void    AddTrace(size_t TraceNo, bool& AddSign, QString& TraceName)
    {
        ;
    }
    void    AddPlatform(bool& AddSign, QString& PlatFormName)
    {
        bool OkSign     = false;
        int DoaInitTmp  = 0;
        _QtPlatFormPara QtPlatFormPara;
        PlatFormName    = QInputDialog::getText(nullptr, tr("Platform Name"),
                                                tr("Please enter the platform name"), QLineEdit::Normal, tr(""), &OkSign);
        if(OkSign && !PlatFormName.isEmpty())
        {
            if(QtPlatFormParaMap.contains(PlatFormName))
            {
                QMessageBox::warning(nullptr, "Warning", tr("There is already a platform with the same name!"));
                qDebug() << tr("There is already a platform with the same name!");
                AddSign = false;
            }
            else
            {
                DoaInitTmp = QInputDialog::getInt(nullptr, tr("Doa"), tr("Please enter the platform doa val"), 0, 0, 1024, 1, &OkSign);
                if(OkSign)
                {
                    QtPlatFormPara.DoaInit = DoaInitTmp;
                    QtPlatFormParaMap.insert(PlatFormName, QtPlatFormPara);
                    AddSign = true;
                }
                else
                {
                    AddSign = false;
                }
            }
        }
        else
        {
            AddSign = false;
        }
    }
    bool    hasPlatform(QString PlatformName)
    {
        if(QtPlatFormParaMap.contains(PlatformName))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    void    PlatformAddRadar(QString PlatformName, QString RadarName, bool& AddSign)
    {
        if(QtPlatFormParaMap.contains(PlatformName))
        {
            if(QtPlatFormParaMap[PlatformName].RadarNameList.size() < RADAR_NUM)
            {
                QtPlatFormParaMap[PlatformName].RadarNameList<<RadarName;
                AddSign = true;
            }
            else
            {
                AddSign = false;
            }
        }
        else
        {
            AddSign = false;
        }
    }
    void    PlatformAddTrace(_LocVec &LocVec)
    {

    }
    void    ProcessStart()
    {
        RunSign = true;
    }
    void    ProcessStop()
    {
        RunSign = false;
    }
    void    ThreadStart()
    {
        ThreadSign = true;
    }
    void    ThreadStop()
    {
        ThreadSign = false;
    }
    void    Clear()
    {
        RadarModeMap.clear();
        QtTraceParaMap.clear();
        QtPlatFormParaMap.clear();
        QtPlatFormParaVec.clear();
    }
signals:

};

#endif // QT_THREADIRADARSIMPRO_H