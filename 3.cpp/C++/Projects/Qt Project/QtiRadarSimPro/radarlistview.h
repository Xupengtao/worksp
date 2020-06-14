#ifndef DNDLISTVIEW_H
#define DNDLISTVIEW_H

#include <QMap>
#include <QDrag>
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QMimeData>
#include <QListView>
#include <QMouseEvent>

class _RadarListView : public QListView
{
    Q_OBJECT
private:
    QPoint      startPoint;
    QString     RadarName;
public:
    _RadarListView(QWidget *parent) : QListView(parent)
    {
        setSelectionMode(QAbstractItemView::SingleSelection);
        setDragEnabled(true);                                   // 启动拖动
        setAcceptDrops(false);                                  // 未启动放下
//        setDropIndicatorShown(true);                            // 显示将要被放置的位置
//        setDragDropMode(QAbstractItemView::InternalMove);       // 设置拖放模式为移动项目，否则为复制项目
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
    virtual ~_RadarListView()
    {
        ;
    }
    void setModel(QAbstractItemModel *model)
    {
        QListView::setModel(model);
    }
    void mousePressEvent(QMouseEvent *event)
    {
        if(event->button() == Qt::LeftButton)
        {
            startPoint = event->pos();
            QModelIndex hHitTest = indexAt(startPoint);
            RadarName = model()->data(hHitTest, Qt::DisplayRole).toString();
            if(RadarName != "")
            {
                startDrag(Qt::MoveAction);
            }
        }
    }
    void startDrag(Qt::DropActions supportedActions)
    {
        Q_UNUSED(supportedActions);
        QMimeData *mimeData = new QMimeData;
        mimeData->setText(RadarName);
        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        if(drag->exec(Qt::MoveAction) == Qt::MoveAction)
        {
            ;
        }
    }
};

#endif // DNDLISTVIEW_H