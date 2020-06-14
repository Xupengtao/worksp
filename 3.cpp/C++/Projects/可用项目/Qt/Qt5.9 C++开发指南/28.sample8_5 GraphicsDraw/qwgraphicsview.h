#ifndef QWGRAPHICSVIEW_H
#define QWGRAPHICSVIEW_H

#include <QPoint>
#include <QObject>
#include <QMouseEvent>
#include <QGraphicsView>

class QWGraphicsView : public QGraphicsView
{
    Q_OBJECT
protected:
    void mouseMoveEvent(QMouseEvent *event)                 //鼠标移动事件
    {
        QPoint point=event->pos();                      //QGraphicsView的坐标
        emit mouseMovePoint(point);                     //释放信号
        QGraphicsView::mouseMoveEvent(event);
    }
    void mousePressEvent(QMouseEvent *event)                //鼠标左键按下事件
    {
        if (event->button()==Qt::LeftButton)
        {
            QPoint point=event->pos();                  //QGraphicsView的坐标
            emit mouseClicked(point);                   //释放信号
        }
        QGraphicsView::mousePressEvent(event);
    }
    void mouseDoubleClickEvent(QMouseEvent *event)          //鼠标双击事件
    { 
        if (event->button()==Qt::LeftButton)
        {
            QPoint point=event->pos();                  //QGraphicsView的坐标
            emit mouseDoubleClick(point);               //释放信号
        }
        QGraphicsView::mouseDoubleClickEvent(event);
    }
    void keyPressEvent(QKeyEvent *event)                    //按键事件
    { 
        emit keyPress(event);
        QGraphicsView::keyPressEvent(event);
    }
public:
    QWGraphicsView(QWidget *parent = nullptr) : QGraphicsView(parent)
    {
    
    }
signals:
    void mouseMovePoint(QPoint point);                      //鼠标移动
    void mouseClicked(QPoint point);                        //鼠标单击
    void mouseDoubleClick(QPoint point);                    //双击事件
    void keyPress(QKeyEvent *event);                        //按键事件
};
#endif // QWGRAPHICSVIEW_H
