#ifndef QT_THREADELECTVISION_H
#define QT_THREADELECTVISION_H

#include <QTimer>
#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QReadWriteLock>
#include "PdwStruct/ElectromagneticVision.hpp"

class Qt_ThreadElectVision : public QThread
{
    Q_OBJECT
private:
    bool                stopThreadFlag = false;
    bool                ProcessRunFlag = false;
    cv::String          pdwfilePath;                // PDW文件路径
    QImage              progressImage;              // 处理图像QImage
    int                 ScheduleVal;                // 处理图像进度
    QReadWriteLock      RwLock;
    _ElectromagneticVision<_xyg_z, PdwType, cv::Vec4b>  ElectromagneticVision;
protected:
    void    run()   Q_DECL_OVERRIDE         // 定时器周期处理事件
    {
        bool NewImageFlag = false;
        while(stopThreadFlag != true)
        {
            while(ProcessRunFlag == true)
            {
                RwLock.lockForWrite();
                _rtnQtFrameDesc& rtnQtFrameDesc = ElectromagneticVision.ProcessFrame();
                NewImageFlag = rtnQtFrameDesc.GetEnable();
                if(NewImageFlag)
                {
                    progressImage   = QImage(rtnQtFrameDesc.VideoFrameMat.data, rtnQtFrameDesc.VideoFrameMat.cols,
                                             rtnQtFrameDesc.VideoFrameMat.rows, rtnQtFrameDesc.VideoFrameMat.step, QImage::Format_RGB888);
                    ScheduleVal     = rtnQtFrameDesc.Progress;
                }
                else
                {
                    ElectromagneticVision.CloseFile();
                    ProcessStop();
                }
                RwLock.unlock();
                emit newImage(NewImageFlag);
                msleep(100);
            }
            msleep(500);
        }
    }
public:
    Qt_ThreadElectVision() : ElectromagneticVision(1024, 1024)
    {
        ;
    }
    void    GetFrameImage(QImage *ReadToImg, int *ReadSchedule)
    {
        RwLock.lockForRead();
        *ReadToImg      = progressImage;
        *ReadSchedule   = ScheduleVal;
        RwLock.unlock();
    }
public slots:
    void    ProcessRun(QString& fileName)
    {
        RwLock.lockForWrite();
        pdwfilePath = fileName.toStdString();
        ElectromagneticVision.OpenFile(pdwfilePath);
        ProcessRunFlag = true;
        RwLock.unlock();
    }
    void    ProcessStop()
    {
        RwLock.lockForWrite();
        ElectromagneticVision.CloseFile();
        ProcessRunFlag = false;
        RwLock.unlock();
    }
    void    ChangeAddTime(int value)
    {
        RwLock.lockForWrite();
        value = (value < 1) ? 1 : ((value > 1000) ? 1000 : value);
        ElectromagneticVision.SetAddTime(float(value)/float(10));       // 0.1 - 100(ms)
        RwLock.unlock();
    }
    void    ThreadStop()
    {
        stopThreadFlag = true;
    }
signals:
    void    newImage(bool Valid);           // 生成新的一帧
};

#endif // QT_THREADELECTVISION_H
