#ifndef SITUATIONGRAPHICVIEW_H
#define SITUATIONGRAPHICVIEW_H

#include <math.h>
#include <QDrag>
#include <QPoint>
#include <QObject>
#include <QMimeData>
#include <QMouseEvent>
#include <QGraphicsView>

class _SituationGraphicView : public QGraphicsView
{
private:
    bool    leftButtonPressSign = false;
    QPoint  dropPoint, front_point, move_point;
    QString PlatformName;
    Q_OBJECT
public:
    _SituationGraphicView(QWidget *parent = nullptr) : QGraphicsView(parent)
    {
        setAcceptDrops(true);                                   // 启动放下
    }
    void dropEvent(QDropEvent *event)
    {
        if(event->mimeData()->hasText())
        {
            PlatformName = event->mimeData()->text();
            if(PlatformName != "")
            {
                dropPoint = event->pos();
                emit SituationAddPlatformSignal(dropPoint, PlatformName);
            }
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
protected:
    template<typename T>
    inline T pow(T a)
    {
        return a*a;
    }
    void mouseMoveEvent(QMouseEvent *event)                     // 鼠标移动事件
    {
        if(leftButtonPressSign == true)
        {
            move_point = event->pos();                      // QGraphicsView的坐标
            if(sqrt(pow<double>(move_point.x() - front_point.x()) + pow<double>(move_point.y() - front_point.y())) > 10)
            {
                emit mouseMovePoint(front_point, move_point);
                front_point = move_point;
                QGraphicsView::mouseMoveEvent(event);
            }
        }
    }
    void mousePressEvent(QMouseEvent *event)                    // 鼠标左键按下事件
    {
        if (event->button() == Qt::LeftButton)
        {
            leftButtonPressSign = true;
            front_point = event->pos();                     // QGraphicsView的坐标
            emit mouseClicked(front_point);
        }
        QGraphicsView::mousePressEvent(event);
    }
    void mouseReleaseEvent(QMouseEvent *event)                  // 鼠标左键弹起事件
    {
        if (event->button() == Qt::LeftButton)
        {
            leftButtonPressSign = false;
            emit mouseReleased();
        }
    }
    void mouseDoubleClickEvent(QMouseEvent *event)              // 鼠标双击事件
    {
        if (event->button()==Qt::LeftButton)
        {
            QPoint point=event->pos();                      // QGraphicsView的坐标
            emit mouseDoubleClick(point);
        }
        QGraphicsView::mouseDoubleClickEvent(event);
    }
    void keyPressEvent(QKeyEvent *event)                        // 按键事件
    {
        emit keyPress(event);
        QGraphicsView::keyPressEvent(event);
    }
signals:
    void SituationAddPlatformSignal(QPoint point, QString PlatformName);    // 态势添加平台
    void mouseMovePoint(QPoint point1, QPoint point2);                      // 鼠标移动
    void mouseClicked(QPoint point);                                        // 鼠标单击
    void mouseReleased();                                                   // 鼠标释放事件
    void mouseDoubleClick(QPoint point);                                    // 双击事件
    void keyPress(QKeyEvent *event);                                        // 按键事件
};

#endif // SITUATIONGRAPHICVIEW_H
