#ifndef QENHANCEDGRAPHICSVIEW_H
#define QENHANCEDGRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QtMath>
#include <QContextMenuEvent>
#include <QMenu>
#include <QGraphicsItem>
#include <QDebug>
#include <QGraphicsEffect>

#include "qcustomgraphicseffect.h"

class QEnhancedGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit QEnhancedGraphicsView(QWidget *parent = nullptr) : QGraphicsView(parent)
    {

    }
protected:
    void wheelEvent(QWheelEvent *event)
    {
        if (event->orientation() == Qt::Vertical)
        {
            double angleDeltaY = event->angleDelta().y();
            double zoomFactor = qPow(1.0015, angleDeltaY);
            scale(zoomFactor, zoomFactor);
            if(angleDeltaY > 0)
            {
                this->centerOn(sceneMousePos);
                sceneMousePos = this->mapToScene(event->pos());
            }
            this->viewport()->update();
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
    void mousePressEvent(QMouseEvent *event)
    {
        if(event->button() == Qt::RightButton)
        {
            QMenu menu;
            QAction *clearAllAction = menu.addAction("Clear All");
            connect(clearAllAction,
                    SIGNAL(triggered(bool)),
                    this,
                    SLOT(clearAll(bool)));

            QAction *clearSelectedAction = menu.addAction("Clear Selected");
            connect(clearSelectedAction,
                    SIGNAL(triggered(bool)),
                    this,
                    SLOT(clearSelected(bool)));

            QAction *noEffectAction = menu.addAction("No Effect");
            connect(noEffectAction,
                    SIGNAL(triggered(bool)),
                    this,
                    SLOT(noEffect(bool)));

            QAction *blurEffectAction = menu.addAction("Blur Effect");
            connect(blurEffectAction,
                    SIGNAL(triggered(bool)),
                    this,
                    SLOT(blurEffect(bool)));

            QAction *dropShadEffectAction = menu.addAction("Drop Shadow Effect");
            connect(dropShadEffectAction,
                    SIGNAL(triggered(bool)),
                    this,
                    SLOT(dropShadowEffect(bool)));

            QAction *colorizeEffectAction = menu.addAction("Colorize Effect");
            connect(colorizeEffectAction,
                    SIGNAL(triggered(bool)),
                    this,
                    SLOT(colorizeEffect(bool)));

            QAction *customEffectAction = menu.addAction("Custom Effect");
            connect(customEffectAction,
                    SIGNAL(triggered(bool)),
                    this,
                    SLOT(customEffect(bool)));

            menu.exec(event->globalPos());
            event->accept();
        }
        else
        {
            QGraphicsView::mousePressEvent(event);
        }
    }
signals:
private slots:
    void clearAll(bool)
    {
        scene()->clear();
    }
    void clearSelected(bool)
    {
        while(scene()->selectedItems().count() > 0)
        {
            delete scene()->selectedItems().at(0);
            scene()->selectedItems().removeAt(0);
        }
    }
    void noEffect(bool)
    {
        foreach(QGraphicsItem *item, scene()->selectedItems())
        {
            item->setGraphicsEffect(Q_NULLPTR);
        }
    }
    void blurEffect(bool)
    {
        foreach(QGraphicsItem *item, scene()->selectedItems())
        {
            item->setGraphicsEffect(new QGraphicsBlurEffect(this));
        }
    }
    void dropShadowEffect(bool)
    {
        foreach(QGraphicsItem *item, scene()->selectedItems())
        {
            item->setGraphicsEffect(new QGraphicsDropShadowEffect(this));
        }
    }
    void colorizeEffect(bool)
    {
        foreach(QGraphicsItem *item, scene()->selectedItems())
        {
            item->setGraphicsEffect(new QGraphicsColorizeEffect(this));
        }
    }
    void customEffect(bool)
    {
        foreach(QGraphicsItem *item, scene()->selectedItems())
        {
            item->setGraphicsEffect(new QCustomGraphicsEffect(this));
        }
    }
private:
    QPointF sceneMousePos;
};

#endif // QENHANCEDGRAPHICSVIEW_H
