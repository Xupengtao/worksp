#ifndef PLATFORMTREEVIEW_H
#define PLATFORMTREEVIEW_H

#include <QMap>
#include <QDrag>
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QMimeData>
#include <QTreeView>
#include <QStandardItem>
#include <QDragEnterEvent>

class _PlatformTreeView : public QTreeView
{
    Q_OBJECT
private:
    QPoint      startPoint, dropPoint;
    QString     PlatformName;
    QString     RadarName;
public:
    explicit _PlatformTreeView(QWidget *parent = nullptr) : QTreeView(parent)
    {
        setSelectionMode(QAbstractItemView::SingleSelection);
        setDragEnabled(true);                                   // 启动拖动
        setAcceptDrops(true);                                   // 启动放下
        setDropIndicatorShown(true);                            // 显示将要被放置的位置
//        setDragDropMode(QAbstractItemView::InternalMove);     // 设置拖放模式为移动项目，否则为复制项目
//        setDefaultDropAction(Qt::MoveAction);
//        setDragDropOverwriteMode(true);
//        setMovement(QListView::Snap);
//        setFlow(QListView::TopToBottom);
//        setStyleSheet("QListView:item"
//                      "{"
//                      "height: 50px;"
//                      "border: 1px solid black;"
//                      "}"
//                      );
    }
    void setModel(QAbstractItemModel *model)
    {
        QTreeView::setModel(model);
    }
    void mousePressEvent(QMouseEvent *event)
    {
        if(event->button() == Qt::RightButton)
        {
            startPoint = event->pos();
            QModelIndex hHitTest = indexAt(startPoint);
            PlatformName = model()->data(hHitTest, Qt::DisplayRole).toString();
            if(PlatformName != "")
            {
                startDrag(Qt::MoveAction);
            }
        }
    }
    void startDrag(Qt::DropActions supportedActions)
    {
        Q_UNUSED(supportedActions);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(PlatformName);
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        if(drag->exec(Qt::MoveAction) == Qt::MoveAction)
        {
            ;
        }
    }
    void dragEnterEvent(QDragEnterEvent *event)                 // 拖动进入事件，只有在接受放下事件时才会产生这一事件
    {
        if(event->mimeData()->hasText())
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
    void dragMoveEvent(QDragMoveEvent *event)                   // 拖动移动事件
    {
        if(event->mimeData()->hasText())
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
    void dragLeaveEvent(QDragLeaveEvent *event)                 // 拖动操作离开部件时发送该事件，前提是必须触发dragEnterEvent事件
    {
        QTreeView::dragLeaveEvent(event);
    }
    void dropEvent(QDropEvent *event)
    {
        if(event->mimeData()->hasText())
        {
            RadarName = event->mimeData()->text();
            if(RadarName != "")
            {
                dropPoint = event->pos();
                emit PlatformAddRadarSignal(dropPoint, RadarName);
            }
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
signals:
    void PlatformAddRadarSignal(QPoint dropPoint, QString RadarName);
};

#endif // PLATFORMTREEVIEW_H