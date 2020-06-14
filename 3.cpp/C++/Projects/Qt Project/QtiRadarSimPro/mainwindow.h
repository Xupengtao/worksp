#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <math.h>
#include <QTime>
#include <QKeyEvent>
#include <QMainWindow>
#include <QFontDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QGraphicsScene>
#include <QStringListModel>
#include <QGraphicsRectItem>
#include <QStandardItemModel>
#include <QGraphicsSceneDragDropEvent>
#include "ui_mainwindow.h"
#include "radarlistview.h"
#include "qt_threadiradarsimpro.h"

namespace Ui {
class MainWindow;
}

template<class T> void setBrushColor(T *item)
{
    QColor color=item->brush().color();
    color=QColorDialog::getColor(color,nullptr,"选择填充颜色");
    if (color.isValid())
    {
        item->setBrush(QBrush(color));
    }
}
class DragDropScene : public QGraphicsScene
{
public:
    DragDropScene(qreal x, qreal y, qreal width, qreal height, QObject *parent = Q_NULLPTR)
        : QGraphicsScene(x, y, width, height, parent)
    {
        ;
    }
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event)
    {
        event->accept();
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    static const int    ItemId  = 1;            // 绘图项自定义数据的key
    static const int    ItemDesciption = 2;     // 绘图项自定义数据的key
    int                 seqNum  = 0;
    int                 backZ   = 0;
    int                 frontZ  = 0;
    Ui::MainWindow      *ui;
    DragDropScene       *scene;
    QLabel              *scene1Label;
    QLabel              *scene2Label;
    QLabel              *labItemCord;
    QLabel              *labItemInfo;
    bool                recordTraceSign = false;
    QStringListModel    *radarListModel;
    QStandardItemModel  *platformTreeModel;
    Qt_iRadarSimPro     iRadarSimPro;
    QString             PlatformName;
    vector<QPointF>     TracePointFVec;
public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent), ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        radarListModel      = new QStringListModel(this);
        ui->radarListView->setModel(radarListModel);                                    // Model/View
        ui->radarListView->setEditTriggers(QAbstractItemView::NoEditTriggers);          // 设置radarListView不可编辑
        platformTreeModel   = new QStandardItemModel(this);
        ui->platformTreeView->setModel(platformTreeModel);
        platformTreeModel->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("Name")<<QStringLiteral("Boot Time"));
        ui->platformTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);       // 设置platformTreeView不可编辑
        scene1Label = new QLabel("Scene1 坐标：");                                       // 创建状态栏标签
        scene1Label->setMinimumWidth(150);
        ui->statusBar->addWidget(scene1Label);
        scene2Label = new QLabel("Scene2 坐标：");
        scene2Label->setMinimumWidth(150);
        ui->statusBar->addWidget(scene2Label);
        labItemCord = new QLabel("Item 坐标：");
        labItemCord->setMinimumWidth(150);
        ui->statusBar->addWidget(labItemCord);
        labItemInfo = new QLabel("ItemInfo: ");
        labItemInfo->setMinimumWidth(200);
        ui->statusBar->addWidget(labItemInfo);
        scene = new DragDropScene(0, 0, 750, 750);                                      // 创建QGraphicsScene
        ui->situationGraphicsView->setScene(scene);                                     // 与view关联
        ui->situationGraphicsView->setCursor(Qt::CrossCursor);                          // 设置鼠标
        ui->situationGraphicsView->setMouseTracking(true);
//        ui->situationGraphicsView->setDragMode(QGraphicsView::RubberBandDrag);        // 可视选定区域
        QObject::connect(ui->situationGraphicsView, SIGNAL(SituationAddPlatformSignal(QPoint, QString)),
                         this, SLOT(SituationAddPlatform(QPoint, QString)));
        QObject::connect(ui->situationGraphicsView, SIGNAL(mouseMovePoint(QPoint, QPoint)),
                         this, SLOT(mouseMovePoint(QPoint, QPoint)));
        QObject::connect(ui->situationGraphicsView, SIGNAL(mouseClicked(QPoint)),
                         this, SLOT(mouseClicked(QPoint)));
        QObject::connect(ui->situationGraphicsView, SIGNAL(mouseReleased()),
                         this, SLOT(mouseReleased()));
        QObject::connect(ui->situationGraphicsView, SIGNAL(mouseDoubleClick(QPoint)),
                         this, SLOT(mouseDoubleClick(QPoint)));
        QObject::connect(ui->situationGraphicsView, SIGNAL(keyPress(QKeyEvent*)),
                         this, SLOT(keyPress(QKeyEvent*)));
        QObject::connect(this, SIGNAL(saveFilePathSignal(QString)),
                         &iRadarSimPro, SLOT(setSaveFilePath(QString)));
        QObject::connect(ui->platformTreeView, SIGNAL(PlatformAddRadarSignal(QPoint, QString)),
                         this, SLOT(PlatformAddRadar(QPoint, QString)));
        iRadarSimPro.start();
        QGraphicsRectItem   *item=new QGraphicsRectItem(-10, -10, 10, 10);  // x, y 为左上角的图元局部坐标, 图元中心点为0,0
        item->setFlags(QGraphicsItem::ItemIsMovable
                       | QGraphicsItem::ItemIsSelectable
                       | QGraphicsItem::ItemIsFocusable);
        item->setBrush(QBrush(Qt::yellow));
        item->setZValue(++frontZ);
        item->setPos(375, 375);
        item->setData(ItemId, ++seqNum);
        item->setData(ItemDesciption, "矩形");
        scene->addItem(item);
        scene->clearSelection();
        item->setSelected(false);
    }
    ~MainWindow()
    {
        delete ui;
    }
    double CalcAz(QPointF TracePoint)
    {
        double x = TracePoint.x() - 375;
        double y = TracePoint.y() - 375;
        double angle;
        if(x > 0)
        {
            if(y > 0)
            {
                angle = 2*M_PI - atan(y/x);
            }
            else
            {
                angle = atan(-y/x);
            }
        }
        else
        {
            if(y > 0)
            {
                angle = M_PI + atan(-y/x);
            }
            else
            {
                angle = M_PI - atan(y/x);
            }
        }
        return angle;
    }
    void QPointFToLocVec(vector<QPointF> &TracePointFVec, _LocVec &LocVec)
    {
        bool firstPointSign = true;
        QPointF frontTracePointF;
        _LocCoordinate  LocCoordTmp;
        LocVec.clear();
        UINT FrameNo = 0;
        foreach(QPointF TracePointF, TracePointFVec)
        {
            if(firstPointSign == true)
            {
                frontTracePointF = TracePointF;
                firstPointSign   = false;
                continue;
            }
            LocCoordTmp.Az          = UINT(1024 * CalcAz(TracePointF) / (2*M_PI));
            LocCoordTmp.El          = 0;
            LocCoordTmp.Distance    = 10;
            LocCoordTmp.JitterRange = 5;
            LocCoordTmp.SortTimes   = 6*FrameNo;
            LocVec.push_back(LocCoordTmp);
            FrameNo++;
        }
    }
private slots:
    void on_addRadarAction_triggered()
    {
        bool    AddSign;
        QString RadarName;
        iRadarSimPro.AddRadar(size_t(radarListModel->rowCount()), AddSign, RadarName);
        if(AddSign == true)
        {
            radarListModel->insertRow(radarListModel->rowCount());
            QModelIndex index = radarListModel->index(radarListModel->rowCount() - 1, 0);
            radarListModel->setData(index, RadarName, Qt::DisplayRole);
//            ui->radarListView->setCurrentIndex(index);                    // 设置当前Index item
        }
    }
    void on_addTraceAction_triggered()
    {
        ;
    }
    void on_addPlatformAction_triggered()
    {
        bool AddSign;
        QString PlatformName;
        iRadarSimPro.AddPlatform(AddSign, PlatformName);
        if(AddSign == true)
        {
            QStandardItem* PlatformItem = new QStandardItem(PlatformName);
            platformTreeModel->appendRow(PlatformItem);
        }
    }
    void on_clearAction_triggered()
    {
        iRadarSimPro.Clear();
    }
    void on_runAction_triggered()
    {
        QString curPath   = QDir::currentPath();                                            // 获取系统当前目录
    //  QString  curPath  = QCoreApplication::applicationDirPath();                         // 获取应用程序的路径
        QString dlgTitle  = tr("保存文件");                                                  // 对话框标题
        QString filter    = "";                                                             // 文件过滤器
        QString aFileName = QFileDialog::getSaveFileName(this, dlgTitle, curPath, filter);
        emit saveFilePathSignal(aFileName);
        if(iRadarSimPro.isRunning())
        {
            iRadarSimPro.ProcessStart();
        }
        else
        {
            iRadarSimPro.ThreadStart();
            iRadarSimPro.ProcessStart();
            iRadarSimPro.start();
        }
    }
    void on_stopAction_triggered()
    {
        if(iRadarSimPro.isRunning())
        {
            iRadarSimPro.ProcessStop();
            iRadarSimPro.ThreadStop();
            iRadarSimPro.wait();
        }
    }
    void PlatformAddRadar(QPoint dropPoint, QString RadarName)
    {
        bool AddSign;
        QModelIndex PlatformIdx     = ui->platformTreeView->indexAt(dropPoint);
        QString PlatformName        = platformTreeModel->data(PlatformIdx, Qt::DisplayRole).toString();
        QStandardItem *PlatformItem = platformTreeModel->itemFromIndex(PlatformIdx);
        iRadarSimPro.PlatformAddRadar(PlatformName, RadarName, AddSign);
        if(AddSign == true)
        {
            QStandardItem *RadarItem = new QStandardItem(RadarName);
            PlatformItem->appendRow(RadarItem);
        }
    }
    void SituationAddPlatform(QPoint dropPoint, QString PlatformName)
    {
        bool AddSign;
        if(iRadarSimPro.hasPlatform(PlatformName))
        {
            QGraphicsEllipseItem   *item=new QGraphicsEllipseItem(-20, -20, 20, 20);
            QPointF pointScene = ui->situationGraphicsView->mapToScene(dropPoint);  // 转换到Scene坐标
            item->setFlags(QGraphicsItem::ItemIsMovable
                           | QGraphicsItem::ItemIsSelectable
                           | QGraphicsItem::ItemIsFocusable);
            item->setBrush(QBrush(Qt::blue));                               // 填充颜色
            item->setZValue(++frontZ);                                      // 用于叠放顺序
            item->setPos(pointScene.x(),pointScene.y());                    // 初始位置
            item->setData(0, PlatformName);                                 // 平台名称Data
            item->setData(ItemId, ++seqNum);                                // 自定义数据, ItemId键
            item->setData(ItemDesciption, "椭圆");                          // 自定义数据, ItemDesciption键
            scene->addItem(item);
            scene->clearSelection();
            item->setSelected(false);
            AddSign = true;
        }
        else
        {
            AddSign = false;
        }
    }
    void mouseMovePoint(QPoint point1, QPoint point2)               // 鼠标移动事件，point是 GraphicsView的坐标,物理坐标
    {
        if(recordTraceSign == true)
        {
            QPointF pointScene1 = ui->situationGraphicsView->mapToScene(point1);                                        // 转换到Scene坐标
            QPointF pointScene2 = ui->situationGraphicsView->mapToScene(point2);                                        // 转换到Scene坐标
            TracePointFVec.push_back(pointScene2);
            scene1Label->setText(QString::asprintf("Scene1 坐标：%.0f,%.0f", pointScene1.x(), pointScene1.y()));
            scene2Label->setText(QString::asprintf("Scene2 坐标：%.0f,%.0f", pointScene2.x(), pointScene2.y()));
            QGraphicsLineItem *item = new QGraphicsLineItem(0, 0, pointScene2.x() - pointScene1.x(),                    // x,y 为左上角的图元局部坐标，图元中心点为0,0
                                                                  pointScene2.y() - pointScene1.y());
            item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
            QPen pen(Qt::red);
            pen.setWidth(3);
            item->setPen(pen);
            item->setZValue(++frontZ);
            item->setPos(pointScene1.x(), pointScene1.y());
            item->setData(ItemId, ++seqNum);
            item->setData(ItemDesciption, "直线");
            scene->addItem(item);
            scene->clearSelection();
            item->setSelected(false);
        }
    }

    void mouseClicked(QPoint point)                                 // 鼠标单击事件
    {
        QPointF pointScene = ui->situationGraphicsView->mapToScene(point);          // 转换到Scene坐标
        QGraphicsItem *item = nullptr;
        recordTraceSign = false;
        item = scene->itemAt(pointScene, ui->situationGraphicsView->transform());   // 获取光标下的绘图项
        if (item != nullptr)                                                        // 有绘图项
        {
            PlatformName = item->data(0).toString();
            if(iRadarSimPro.hasPlatform(PlatformName))
            {
                recordTraceSign = true;
                TracePointFVec.clear();
                TracePointFVec.push_back(pointScene);
            }
        }
    }
    void mouseReleased()
    {
        if(recordTraceSign == true)
        {
            _LocVec LocVecTmp;
            QPointFToLocVec(TracePointFVec, LocVecTmp);
            iRadarSimPro.PlatformAddTrace(LocVecTmp);
        }
        scene->clearSelection();
        recordTraceSign = false;
    }
//    void mouseDoubleClick(QPoint point)                             // 鼠标双击事件, 调用相应的对话框, 设置填充颜色、线条颜色或字体
//    {
//        QPointF pointScene = ui->situationGraphicsView->mapToScene(point);      // 转换到Scene坐标
//        QGraphicsItem *item=nullptr;
//        item=scene->itemAt(pointScene,ui->situationGraphicsView->transform());  // 获取光标下的绘图项
//        if (item == nullptr)                                // 没有绘图项
//        {
//            return;
//        }
//        switch (item->type())                               // 绘图项的类型
//        {
//          case    QGraphicsRectItem::Type:                  // 矩形框
//          {
//            QGraphicsRectItem *theItem = qgraphicsitem_cast<QGraphicsRectItem*>(item);
//            setBrushColor(theItem);
//            break;
//          }
//          case    QGraphicsEllipseItem::Type:               // 椭圆和圆都是 QGraphicsEllipseItem
//          {
//            QGraphicsEllipseItem *theItem;
//            theItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
//            setBrushColor(theItem);
//            break;
//          }
//          case    QGraphicsPolygonItem::Type:               // 梯形和三角形
//          {
//            QGraphicsPolygonItem *theItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
//            setBrushColor(theItem);
//            break;
//          }
//          case    QGraphicsLineItem::Type:                  // 直线, 设置线条颜色
//          {
//            QGraphicsLineItem *theItem = qgraphicsitem_cast<QGraphicsLineItem*>(item);
//            QPen    pen = theItem->pen();
//            QColor  color = theItem->pen().color();
//            color=QColorDialog::getColor(color,this,"选择线条颜色");
//            if (color.isValid())
//            {
//                pen.setColor(color);
//                theItem->setPen(pen);
//            }
//            break;
//          }
//          case    QGraphicsTextItem::Type:                  // 文字, 设置字体
//          {
//            QGraphicsTextItem *theItem=qgraphicsitem_cast<QGraphicsTextItem*>(item);
//            QFont font=theItem->font();
//            bool ok=false;
//            font=QFontDialog::getFont(&ok,font,this,"设置字体");
//            if (ok)
//            {
//                theItem->setFont(font);
//            }
//            break;
//          }
//        }
//    }

//    void keyPress(QKeyEvent *event)                                 // 按键事件
//    {
//        if (scene->selectedItems().count() != 1)
//        {
//            return;                                         // 没有选中的绘图项, 或选中的多于1个
//        }
//        QGraphicsItem *item=scene->selectedItems().at(0);
//        if (event->key()==Qt::Key_Delete)                   // 删除
//        {
//            scene->removeItem(item);
//        }
//        else if (event->key()==Qt::Key_Space)               // 顺时针旋转90度
//        {
//            item->setRotation(90+item->rotation());
//        }
//        else if (event->key()==Qt::Key_PageUp)              // 放大
//        {
//            item->setScale(0.1+item->scale());
//        }
//        else if (event->key()==Qt::Key_PageDown)            // 缩小
//        {
//            item->setScale(-0.1+item->scale());
//        }
//        else if (event->key()==Qt::Key_Left)                // 左移
//        {
//            item->setX(-1+item->x());
//        }
//        else if (event->key()==Qt::Key_Right)               // 右移
//        {
//            item->setX(1+item->x());
//        }
//        else if (event->key()==Qt::Key_Up)                  // 上移
//        {
//            item->setY(-1+item->y());
//        }
//        else if (event->key()==Qt::Key_Down)                // 下移
//        {
//            item->setY(1+item->y());
//        }
//    }
//    void MainWindow::on_actItem_Rect_triggered()                  // 添加一个矩形
//    {
//        QGraphicsRectItem   *item=new QGraphicsRectItem(-50,-25,100,50);// x, y 为左上角的图元局部坐标, 图元中心点为0,0
//        item->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable
//                       | QGraphicsItem::ItemIsFocusable);
//        item->setBrush(QBrush(Qt::yellow));
//        item->setZValue(++frontZ);
//        item->setPos(-50+(qrand() % 100),-50+(qrand() % 100));
//        item->setData(ItemId,++seqNum);
//        item->setData(ItemDesciption,"矩形");
//        scene->addItem(item);
//        scene->clearSelection();
//        item->setSelected(true);
//    }
//    void MainWindow::on_actItem_Ellipse_triggered()               // 添加一个椭圆
//    {
//        QGraphicsEllipseItem   *item=new QGraphicsEllipseItem(-50,-30,100,60);
//        item->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable
//                       | QGraphicsItem::ItemIsFocusable);
//        item->setBrush(QBrush(Qt::blue));                               // 填充颜色
//        item->setZValue(++frontZ);                                      // 用于叠放顺序
//        item->setPos(-50+(qrand() % 100),-50+(qrand() % 100));          // 初始位置
//        item->setData(ItemId,++seqNum);                                 // 自定义数据, ItemId键
//        item->setData(ItemDesciption, "椭圆");                          // 自定义数据, ItemDesciption键
//        scene->addItem(item);
//        scene->clearSelection();
//        item->setSelected(true);
//    }
//    void MainWindow::on_actItem_Polygon_triggered()               // 添加一个梯形
//    {
//        QGraphicsPolygonItem   *item=new QGraphicsPolygonItem;
//        QPolygonF   points;
//        points.append(QPointF(-40,-40));
//        points.append(QPointF(40,-40));
//        points.append(QPointF(100,40));
//        points.append(QPointF(-100,40));
//        item->setPolygon(points);
//        item->setPos(-50+(qrand() % 100),-50+(qrand() % 100));
//        item->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable
//                       | QGraphicsItem::ItemIsFocusable);
//        item->setBrush(QBrush(Qt::green));
//        item->setZValue(++frontZ);
//        item->setData(ItemId,++seqNum);
//        item->setData(ItemDesciption,"梯形");
//        scene->addItem(item);
//        scene->clearSelection();
//        item->setSelected(true);
//    }
//    void MainWindow::on_actEdit_Delete_triggered()                // 删除所有选中的绘图项
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt>0)
//        for (int i=0;i<cnt;i++)
//        {
//            QGraphicsItem*  item=scene->selectedItems().at(0);
//            scene->removeItem(item);                                      // 删除绘图项
//        }
//    }
//    void MainWindow::on_actZoomIn_triggered()                     // 放大
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt==1)
//        {
//            QGraphicsItem   *item;
//            item=scene->selectedItems().at(0);
//            item->setScale(0.1+item->scale());
//        }
//        else
//            ui->View->scale(1.1,1.1);
//    }
//    void MainWindow::on_actZoomOut_triggered()                    // 缩小
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt==1)
//        {
//            QGraphicsItem   *item;
//            item=scene->selectedItems().at(0);
//            item->setScale(item->scale()-0.1);
//        }
//        else
//            ui->View->scale(0.9,0.9);
//    }
//    void MainWindow::on_actRestore_triggered()                    // 取消所有变换
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt==1)
//        {
//            QGraphicsItem* item=scene->selectedItems().at(0);
//    //        item->resetTransform();   // 不起作用
//            item->setRotation(0);
//            item->setScale(1.0);
//        }
//        else
//            ui->View->resetTransform();
//    }
//    void MainWindow::on_actRotateLeft_triggered()                 // 逆时针旋转
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt==1)
//        {
//            QGraphicsItem* item=scene->selectedItems().at(0);
//            item->setRotation(-30+item->rotation());
//        }
//        else
//            ui->View->rotate(-30);
//    }
//    void MainWindow::on_actRotateRight_triggered()                // 顺时针旋转
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt==1)
//        {
//            QGraphicsItem* item=scene->selectedItems().at(0);
//            item->setRotation(+30+item->rotation());
//        }
//        else
//            ui->View->rotate(+30);
//    }
//    void MainWindow::on_actEdit_Front_triggered()                 // bring to front,前置
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt>0)                                                        // 只处理选中的第1个绘图项
//        {
//            QGraphicsItem* item=scene->selectedItems().at(0);
//            item->setZValue(++frontZ);
//        }
//    }
//    void MainWindow::on_actEdit_Back_triggered()                  // bring to back，后置
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt>0)                                                        // 只处理选中的第1个绘图项
//        {
//            QGraphicsItem* item=scene->selectedItems().at(0);
//            item->setZValue(--backZ);
//        }
//    }
//    void MainWindow::on_actItem_Line_triggered()                  // 添加直线
//    {
//        QGraphicsLineItem   *item=new QGraphicsLineItem(-100, 0, 100, 0); // x,y 为左上角的图元局部坐标，图元中心点为0,0
//        item->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable
//                       | QGraphicsItem::ItemIsFocusable);
//        QPen    pen(Qt::red);
//        pen.setWidth(3);
//        item->setPen(pen);
//        item->setZValue(++frontZ);
//        item->setPos(-50+(qrand() % 100),-50+(qrand() % 100));
//        item->setData(ItemId,++seqNum);
//        item->setData(ItemDesciption,"直线");
//        scene->addItem(item);
//        scene->clearSelection();
//        item->setSelected(true);
//    }
//    void MainWindow::on_actItem_Text_triggered()                  // 添加文字
//    {
//        QString str=QInputDialog::getText(this,"输入文字","请输入文字");
//        if (str.isEmpty())
//            return;
//        QGraphicsTextItem   *item=new QGraphicsTextItem(str);
//        QFont   font=this->font();
//        font.setPointSize(20);
//        font.setBold(true);
//        item->setFont(font);
//        item->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable
//                       | QGraphicsItem::ItemIsFocusable);
//        item->setPos(-50+(qrand() % 100),-50+(qrand() % 100));
//        item->setZValue(++frontZ);
//        item->setData(ItemId,++seqNum);
//        item->setData(ItemDesciption,"文字");
//        scene->addItem(item);
//        scene->clearSelection();
//        item->setSelected(true);
//    }
//    void MainWindow::on_actGroup_triggered()                      // 组合
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt>1)
//        {
//           QGraphicsItemGroup* group =new QGraphicsItemGroup;     // 创建组合
//           scene->addItem(group);                                 // 组合添加到场景中
//            for (int i=0;i<cnt;i++)
//            {
//                QGraphicsItem* item=scene->selectedItems().at(0);
//                item->setSelected(false);                         // 清除选择虚线框
//                item->clearFocus();
//                group->addToGroup(item);                          // 添加到组合
//            }
//            group->setFlags(QGraphicsItem::ItemIsMovable
//                           | QGraphicsItem::ItemIsSelectable
//                           | QGraphicsItem::ItemIsFocusable);
//            group->setZValue(++frontZ);
//    //        group->clearFocus();
//            scene->clearSelection();
//            group->setSelected(true);
//         }
//    }
//    void MainWindow::on_actGroupBreak_triggered()                 // break group,打散组合
//    {
//        int cnt=scene->selectedItems().count();
//        if (cnt==1)
//        {
//            QGraphicsItemGroup  *group;
//            group=(QGraphicsItemGroup*)scene->selectedItems().at(0);
//            scene->destroyItemGroup(group);
//         }
//    }
//    void MainWindow::on_actItem_Circle_triggered()                // 添加圆形
//    {
//        QGraphicsEllipseItem   *item=new QGraphicsEllipseItem(-50,-50,100,100);
//        item->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable
//                       | QGraphicsItem::ItemIsFocusable);
//        item->setBrush(QBrush(Qt::cyan));
//        item->setZValue(++frontZ);
//        item->setPos(-50+(qrand() % 100),-50+(qrand() % 100));
//        item->setData(ItemId,++seqNum);
//        item->setData(ItemDesciption,"圆形");
//        scene->addItem(item);
//        scene->clearSelection();
//        item->setSelected(true);
//    }
//    void MainWindow::on_actItem_Triangle_triggered()              // 添加三角形
//    {
//        QGraphicsPolygonItem   *item=new QGraphicsPolygonItem;
//        QPolygonF   points;
//        points.append(QPointF(0,-40));
//        points.append(QPointF(60,40));
//        points.append(QPointF(-60,40));
//        item->setPolygon(points);
//        item->setPos(-50+(qrand() % 100),-50+(qrand() % 100));
//        item->setFlags(QGraphicsItem::ItemIsMovable
//                       | QGraphicsItem::ItemIsSelectable
//                       | QGraphicsItem::ItemIsFocusable);
//        item->setBrush(QBrush(Qt::magenta));
//        item->setZValue(++frontZ);
//        item->setData(ItemId,++seqNum);
//        item->setData(ItemDesciption,"三角形");
//        scene->addItem(item);
//        scene->clearSelection();
//        item->setSelected(true);
//    }
signals:
    void saveFilePathSignal(QString aFileName);
};

#endif // MAINWINDOW_H