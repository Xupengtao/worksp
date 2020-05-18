#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QDebug>
#include <QLabel>
#include <QMutex>
#include <QTimer>
#include <QThread>
#include <QPixmap>
#include <QLibrary>
#include <QRunnable>
#include <QFileInfo>
#include <QMimeData>
#include <QSettings>
#include <QDropEvent>
#include <QTranslator>
#include <QThreadPool>
#include <QMainWindow>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QMessageBox>
#include <QFileInfoList>
#include <QPluginLoader>
#include <QGraphicsItem>
#include <QWaitCondition>
#include <QGraphicsScene>
#include <QDragEnterEvent>
#include <QGraphicsProxyWidget>

#include "ui_mainwindow.h"
#include "cvplugininterface.h"
#include "PdwStruct/ElectromagneticVision.hpp"

#define PLUGINS_SUBFOLDER                   "/cvplugins/"
#define LANGUAGES_SUBFOLDER                 "/languages/"
#define THEMES_SUBFOLDER                    "/themes/"
#define FILE_ON_DISK_DYNAMIC_PROPERTY       "absolute_file_path"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow*         ui;
    QString                 currentThemeFile;
    QString                 currentLanguageFile;
    QString                 currentPluginFile;
    QPointer<QPluginLoader> currentPlugin;
    QPointer<QWidget>       currentPluginGui;
    QGraphicsScene          scene;
    QTranslator             translator;
    QTimer                  frameTimer;
    cv::String              pdwfilePath;
    QGraphicsPixmapItem     originalPixmap, processedPixmap;
    cv::Mat                 originalMat,    processedMat;
    QImage                  originalImage,  processedImage;
    _ElectromagneticVision<_xyg_z, PdwType, cv::Vec4b>  ElectromagneticVision;
public:
    explicit MainWindow(QWidget *parent = 0) : QMainWindow(parent), ui(new Ui::MainWindow), frameTimer(this), ElectromagneticVision(1024, 1024)
    {
        ui->setupUi(this);
        this->setAcceptDrops(true);
        loadSettings();                                                             // 设置加载
        populatePluginsMenu();                                                      // 插件菜单加载
        populateLanguagesMenu();                                                    // 语言菜单加载
        populateThemesMenu();                                                       // 主题菜单加载
        ui->graphicsView->setAcceptDrops(false);
        ui->graphicsView->setScene(&scene);                                         // graphicsView场景设置
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        ui->graphicsView->setRubberBandSelectionMode(Qt::ContainsItemShape);
        scene.addItem(&originalPixmap);                                             // graphicsView场景添加对象元素
        scene.addItem(&processedPixmap);                                            // graphicsView场景添加对象元素
        connect(&frameTimer, SIGNAL(timeout()), this, SLOT(OnTimerOut()));
        ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    }
    ~MainWindow()
    {
        delete currentPlugin;
        delete ui;
    }
private:
    void    loadSettings()                                      // 加载设置
    {
        QSettings settings("Packt", "Electromagnetic_Vision", this);
        currentThemeFile    = settings.value("currentThemeFile", "").toString();
        currentLanguageFile = settings.value("currentLanguageFile", "").toString();
        currentPluginFile   = settings.value("currentPluginFile", "").toString();
    }
    void    saveSettings()                                      // 保存设置
    {
        QSettings settings("Packt", "Electromagnetic_Vision", this);
        settings.setValue("currentThemeFile", currentThemeFile);
        settings.setValue("currentLanguageFile", currentLanguageFile);
        settings.setValue("currentPluginFile", currentPluginFile);
    }
    void    populatePluginsMenu()
    {
        QDir pluginsDir(qApp->applicationDirPath() + PLUGINS_SUBFOLDER);
        QFileInfoList pluginFiles = pluginsDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
        foreach(QFileInfo pluginFile, pluginFiles)
        {
            if(QLibrary::isLibrary(pluginFile.absoluteFilePath()))
            {
                QPluginLoader pluginLoader(pluginFile.absoluteFilePath(), this);
                if(CvPluginInterface *plugin = dynamic_cast<CvPluginInterface*>(pluginLoader.instance()))
                {
                    QAction *pluginAction = ui->menuPlugins->addAction(plugin->title());
                    pluginAction->setProperty(FILE_ON_DISK_DYNAMIC_PROPERTY, pluginFile.absoluteFilePath());
                    connect(pluginAction, SIGNAL(triggered(bool)), this, SLOT(onPluginActionTriggered(bool)));
                    if(currentPluginFile == pluginFile.absoluteFilePath())
                    {
                        pluginAction->trigger();
                    }
                }
                else
                {
                    QMessageBox::warning(this, tr("Warning"), QString(tr("Make sure %1 is a correct plugin for this application<br>"
                                                    "and it's not in use by some other application!")).arg(pluginFile.fileName()));
                }
            }
            else
            {
                QMessageBox::warning(this, tr("Warning"), QString(tr("Make sure only plugins exist in %1 folder.<br>"
                                                "%2 is not a plugin.")).arg(PLUGINS_SUBFOLDER).arg(pluginFile.fileName()));
            }
        }

        if(ui->menuPlugins->actions().count() <= 0)
        {
            QMessageBox::critical(this, tr("No Plugins"), QString(tr("This application cannot work without plugins!"
                                                                    "<br>Make sure that %1 folder exists "
                                                                    "in the same folder as the application<br>and that "
                                                                    "there are some filter plugins inside it")).arg(PLUGINS_SUBFOLDER));
            this->setEnabled(false);
        }
    }
    void    populateLanguagesMenu()
    {
        QMenu *languagesMenu = new QMenu(this);
        QAction *defaultLanguageAction = languagesMenu->addAction("English - US");      // 加载默认语言(English)
        defaultLanguageAction->setProperty(FILE_ON_DISK_DYNAMIC_PROPERTY, "");
        connect(defaultLanguageAction, SIGNAL(triggered(bool)), this, SLOT(onLanguageActionTriggered(bool)));
        QDir languagesDir(qApp->applicationDirPath() + LANGUAGES_SUBFOLDER);            // 加载所有可用语言并定制化菜单
        QFileInfoList languageFiles = languagesDir.entryInfoList(QStringList() << "*.qm", QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
        foreach(QFileInfo languageFile, languageFiles)
        {
            QAction *languageAction = languagesMenu->addAction(languageFile.baseName());
            languageAction->setProperty(FILE_ON_DISK_DYNAMIC_PROPERTY, languageFile.absoluteFilePath());
            connect(languageAction, SIGNAL(triggered(bool)), this, SLOT(onLanguageActionTriggered(bool)));

            if(currentLanguageFile == languageFile.absoluteFilePath())
            {
                languageAction->trigger();
            }
        }
        ui->actionLanguage->setMenu(languagesMenu);
    }
    void    populateThemesMenu()
    {
        QMenu *themesMenu = new QMenu(this);
        QAction *defaultThemeAction = themesMenu->addAction("Default");                 // 加载默认主题
        defaultThemeAction->setProperty(FILE_ON_DISK_DYNAMIC_PROPERTY, "");
        connect(defaultThemeAction, SIGNAL(triggered(bool)), this, SLOT(onThemeActionTriggered(bool)));
        QDir themesDir(qApp->applicationDirPath() + THEMES_SUBFOLDER);                  // 加载所有主题并定制化菜单
        QFileInfoList themeFiles = themesDir.entryInfoList(QStringList() << "*.thm", QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
        foreach(QFileInfo themeFile, themeFiles)
        {
            QAction *themeAction = themesMenu->addAction(themeFile.baseName());
            themeAction->setProperty(FILE_ON_DISK_DYNAMIC_PROPERTY, themeFile.absoluteFilePath());
            connect(themeAction, SIGNAL(triggered(bool)), this, SLOT(onThemeActionTriggered(bool)));
            if(currentThemeFile == themeFile.absoluteFilePath())
            {
                themeAction->trigger();
            }
        }
        ui->actionTheme->setMenu(themesMenu);
    }
protected:
    void    closeEvent(QCloseEvent *event)                      // 关闭触发事件
    {
        int result = QMessageBox::warning(this, tr("Exit"), tr("Are you sure you want to exit?"), QMessageBox::Yes, QMessageBox::No);
        if(result == QMessageBox::Yes)
        {
            saveSettings();
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
    void    changeEvent(QEvent *event)                          // 改变触发事件
    {
        if(event->type() == QEvent::LanguageChange)
        {
            ui->retranslateUi(this);
        }
        else
        {
            QMainWindow::changeEvent(event);
        }
    }
    void    dragEnterEvent(QDragEnterEvent *event)
    {
        QStringList acceptedFileTypes;
        acceptedFileTypes.append("jpg");
        acceptedFileTypes.append("png");
        acceptedFileTypes.append("bmp");
        if (event->mimeData()->hasUrls() && event->mimeData()->urls().count() == 1)
        {
            QFileInfo file(event->mimeData()->urls().at(0).toLocalFile());
            if(acceptedFileTypes.contains(file.suffix().toLower()))
            {
                event->acceptProposedAction();
            }
        }
    }
    void    dropEvent(QDropEvent *event)
    {
        ui->graphicsView->resetTransform();
        QPoint viewPos = ui->graphicsView->mapFromParent(event->pos());
        QPointF sceneDropPos = ui->graphicsView->mapToScene(viewPos);
        QFileInfo file(event->mimeData()->urls().at(0).toLocalFile());
        QPixmap pixmap;
        if(pixmap.load(file.absoluteFilePath()))
        {
            QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
            item->setPos(sceneDropPos);
            item->setFlag(QGraphicsItem::ItemIsSelectable);
            item->setAcceptedMouseButtons(Qt::LeftButton);
            scene.addItem(item);
            qDebug() << scene.items().count();
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("The image file cannot be read!"));
        }
    }
private slots:
    void    onPluginActionTriggered(bool)
    {
        if(!currentPlugin.isNull())
        {
            delete currentPlugin;
            delete currentPluginGui;
        }

        currentPluginFile = QObject::sender()->property(FILE_ON_DISK_DYNAMIC_PROPERTY).toString();
        currentPlugin = new QPluginLoader(currentPluginFile, this);
        currentPluginGui = new QWidget(this);
        ui->pluginLayout->addWidget(currentPluginGui);
        CvPluginInterface *currentPluginInstance = dynamic_cast<CvPluginInterface*>(currentPlugin->instance());
        if(currentPluginInstance)
        {
            currentPluginInstance->setupUi(currentPluginGui);
            connect(currentPlugin->instance(), SIGNAL(updateNeeded()), this, SLOT(onCurrentPluginUpdateNeeded()));
            connect(currentPlugin->instance(), SIGNAL(infoMessage(QString)), this, SLOT(onCurrentPluginInfoMessage(QString)));
            connect(currentPlugin->instance(), SIGNAL(errorMessage(QString)), this, SLOT(onCurrentPluginErrorMessage(QString)));

        }
    }
    void    onLanguageActionTriggered(bool)
    {
        currentLanguageFile = QObject::sender()->property(FILE_ON_DISK_DYNAMIC_PROPERTY).toString();
        qApp->removeTranslator(&translator);
        if(!currentLanguageFile.isEmpty())
        {
            translator.load(currentLanguageFile);
            qApp->installTranslator(&translator);
            ui->retranslateUi(this);
        }
    }
    void    onThemeActionTriggered(bool)
    {
        currentThemeFile = QObject::sender()->property(FILE_ON_DISK_DYNAMIC_PROPERTY).toString();
        QFile themeFile(currentThemeFile);
        if(currentThemeFile.isEmpty())
        {
            qApp->setStyleSheet("");
        }
        else
        {
            themeFile.open(QFile::ReadOnly | QFile::Text);
            QString styleSheet = themeFile.readAll();
            qApp->setStyleSheet(styleSheet);
            themeFile.close();
        }
    }
    void    onCurrentPluginUpdateNeeded()
    {
        if(!originalMat.empty())
        {
            if(!currentPlugin.isNull())
            {
                CvPluginInterface *currentPluginInstance = dynamic_cast<CvPluginInterface*>(currentPlugin->instance());
                if(currentPluginInstance)
                {
                    cv::TickMeter meter;
                    meter.start();
                    currentPluginInstance->processImage(originalMat, processedMat);
                    meter.stop();
                    qDebug() << "The process took " << meter.getTimeMilli() << "milliseconds";
                }
            }
            else
            {
                processedMat = originalMat.clone();
            }
            originalImage = QImage(originalMat.data, originalMat.cols, originalMat.rows, originalMat.step, QImage::Format_RGB888);
            originalPixmap.setPixmap(QPixmap::fromImage(originalImage.rgbSwapped()));
            processedImage = QImage(processedMat.data, processedMat.cols, processedMat.rows, processedMat.step, QImage::Format_RGB888);
            processedPixmap.setPixmap(QPixmap::fromImage(processedImage.rgbSwapped()));
        }
    }
    void    onCurrentPluginErrorMessage(QString msg)
    {
        qDebug() << "Plugin Error Message : " << msg;
    }
    void    onCurrentPluginInfoMessage(QString msg)
    {
        qDebug() << "Plugin Info Message : " << msg;
    }
    void    on_actionAboutQt_triggered()
    {
        qApp->aboutQt();
    }
    void    on_actionExit_triggered()
    {
        close();
    }
    void    on_actionOpenImage_triggered()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Input Image"), QDir::currentPath(), tr("Images") + " (*.jpg *.png *.bmp)");
        using namespace cv;
        originalMat = imread(fileName.toStdString());
        if(!originalMat.empty())
        {
            onCurrentPluginUpdateNeeded();
        }
        else if(!fileName.trimmed().isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Make sure the image file exists and it is accessible!"));
        }
    }
    void    on_viewOriginalCheck_toggled(bool checked)
    {
        originalPixmap.setVisible(checked);
        processedPixmap.setVisible(!checked);
    }
    void    on_actionSaveImage_triggered()
    {
        if(!ui->viewOriginalCheck->isChecked() && !processedMat.empty())
        {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath(), "*.jpg;;*.png;;*.bmp");
            if(!fileName.isEmpty())
            {
                cv::imwrite(fileName.toStdString(), processedMat);
            }
        }
        else if(ui->viewOriginalCheck->isChecked() && !originalMat.empty())
        {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath(), "*.jpg;;*.png;;*.bmp");
            if(!fileName.isEmpty())
            {
                cv::imwrite(fileName.toStdString(), originalMat);
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Warning"), tr("There is nothing to be saved!"));
        }
    }
    void    on_action_Camera_triggered()
    {

    }
    void    on_openPushButton_clicked()
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Input File"), QDir::currentPath(), tr("File") + " (*)");
        pdwfilePath = fileName.toStdString();
        ui->openLineEdit->setText(fileName);
    }
    void    OnTimerOut()
    {
        cv::Mat procMat = ElectromagneticVision.ProcessFrame();
        if(procMat.rows == 0)
        {
            ElectromagneticVision.CloseFile();
            frameTimer.stop();
        }
        else
        {
            originalImage = QImage(procMat.data, procMat.cols, procMat.rows, procMat.step, QImage::Format_RGB888);
            originalPixmap.setPixmap(QPixmap::fromImage(originalImage.rgbSwapped()));
        }
    }
    void    on_runPushButton_clicked()
    {
        ElectromagneticVision.OpenFile(pdwfilePath, 100);
        frameTimer.start(100);
    }
};

#endif // MAINWINDOW_H
