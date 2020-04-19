/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTableWidget *tableWidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_camerafind;
    QPushButton *pushButton_merge;
    QPushButton *pushButton_dispnoconn;
    QPushButton *pushButton_dispchange;
    QSpacerItem *verticalSpacer_2;
    QLineEdit *lineEdit_runstatus;
    QSpacerItem *verticalSpacer_3;
    QVBoxLayout *verticalLayout_4;
    QPushButton *pushButton_load;
    QPushButton *pushButton_save;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_reset;
    QPushButton *pushButton_saveasinit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(980, 652);
        QSizePolicy sizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(980, 652));
        MainWindow->setMaximumSize(QSize(980, 652));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\271\274\345\234\206"));
        font.setPointSize(10);
        MainWindow->setFont(font);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        tableWidget = new QTableWidget(centralWidget);
        if (tableWidget->columnCount() < 3)
            tableWidget->setColumnCount(3);
        if (tableWidget->rowCount() < 100)
            tableWidget->setRowCount(100);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setGeometry(QRect(10, 10, 771, 601));
        QFont font1;
        font1.setFamily(QStringLiteral("Times New Roman"));
        font1.setPointSize(10);
        tableWidget->setFont(font1);
        tableWidget->setGridStyle(Qt::SolidLine);
        tableWidget->setSortingEnabled(true);
        tableWidget->setRowCount(100);
        tableWidget->setColumnCount(3);
        tableWidget->horizontalHeader()->setDefaultSectionSize(240);
        tableWidget->horizontalHeader()->setMinimumSectionSize(240);
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(800, 10, 131, 601));
        layoutWidget->setSizeIncrement(QSize(0, 0));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\271\274\345\234\206"));
        font2.setBold(false);
        font2.setWeight(50);
        layoutWidget->setFont(font2);
        verticalLayout_3 = new QVBoxLayout(layoutWidget);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(12);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetDefaultConstraint);
        pushButton_camerafind = new QPushButton(layoutWidget);
        pushButton_camerafind->setObjectName(QStringLiteral("pushButton_camerafind"));
        pushButton_camerafind->setMinimumSize(QSize(108, 30));
        pushButton_camerafind->setSizeIncrement(QSize(0, 0));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\345\271\274\345\234\206"));
        font3.setPointSize(10);
        font3.setBold(false);
        font3.setWeight(50);
        pushButton_camerafind->setFont(font3);

        verticalLayout_2->addWidget(pushButton_camerafind);

        pushButton_merge = new QPushButton(layoutWidget);
        pushButton_merge->setObjectName(QStringLiteral("pushButton_merge"));
        pushButton_merge->setMinimumSize(QSize(108, 30));
        pushButton_merge->setSizeIncrement(QSize(0, 0));
        pushButton_merge->setFont(font3);

        verticalLayout_2->addWidget(pushButton_merge);

        pushButton_dispnoconn = new QPushButton(layoutWidget);
        pushButton_dispnoconn->setObjectName(QStringLiteral("pushButton_dispnoconn"));
        pushButton_dispnoconn->setMinimumSize(QSize(108, 30));
        pushButton_dispnoconn->setSizeIncrement(QSize(0, 0));
        pushButton_dispnoconn->setFont(font3);

        verticalLayout_2->addWidget(pushButton_dispnoconn);

        pushButton_dispchange = new QPushButton(layoutWidget);
        pushButton_dispchange->setObjectName(QStringLiteral("pushButton_dispchange"));
        pushButton_dispchange->setMinimumSize(QSize(108, 30));
        pushButton_dispchange->setSizeIncrement(QSize(0, 0));
        pushButton_dispchange->setFont(font3);

        verticalLayout_2->addWidget(pushButton_dispchange);

        verticalSpacer_2 = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        lineEdit_runstatus = new QLineEdit(layoutWidget);
        lineEdit_runstatus->setObjectName(QStringLiteral("lineEdit_runstatus"));
        lineEdit_runstatus->setMinimumSize(QSize(108, 30));
        QFont font4;
        font4.setFamily(QString::fromUtf8("\345\215\216\346\226\207\345\275\251\344\272\221"));
        font4.setBold(false);
        font4.setItalic(true);
        font4.setWeight(50);
        lineEdit_runstatus->setFont(font4);
        lineEdit_runstatus->setAutoFillBackground(false);
        lineEdit_runstatus->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(lineEdit_runstatus);

        verticalSpacer_3 = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_3);


        verticalLayout_3->addLayout(verticalLayout_2);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        pushButton_load = new QPushButton(layoutWidget);
        pushButton_load->setObjectName(QStringLiteral("pushButton_load"));
        pushButton_load->setMinimumSize(QSize(108, 30));
        pushButton_load->setSizeIncrement(QSize(0, 0));
        pushButton_load->setFont(font3);

        verticalLayout_4->addWidget(pushButton_load);

        pushButton_save = new QPushButton(layoutWidget);
        pushButton_save->setObjectName(QStringLiteral("pushButton_save"));
        pushButton_save->setMinimumSize(QSize(108, 30));
        pushButton_save->setSizeIncrement(QSize(0, 0));
        pushButton_save->setFont(font3);

        verticalLayout_4->addWidget(pushButton_save);


        verticalLayout_3->addLayout(verticalLayout_4);

        verticalSpacer = new QSpacerItem(20, 148, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        pushButton_reset = new QPushButton(layoutWidget);
        pushButton_reset->setObjectName(QStringLiteral("pushButton_reset"));
        pushButton_reset->setMinimumSize(QSize(108, 30));
        pushButton_reset->setSizeIncrement(QSize(0, 0));
        pushButton_reset->setFont(font3);

        verticalLayout->addWidget(pushButton_reset);

        pushButton_saveasinit = new QPushButton(layoutWidget);
        pushButton_saveasinit->setObjectName(QStringLiteral("pushButton_saveasinit"));
        pushButton_saveasinit->setMinimumSize(QSize(108, 30));
        pushButton_saveasinit->setSizeIncrement(QSize(0, 0));
        pushButton_saveasinit->setFont(font3);

        verticalLayout->addWidget(pushButton_saveasinit);


        verticalLayout_3->addLayout(verticalLayout);

        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "\347\233\221\346\216\247\347\263\273\347\273\237\346\211\253\346\217\217\345\267\245\345\205\267", Q_NULLPTR));
        pushButton_camerafind->setText(QApplication::translate("MainWindow", "\346\221\204\345\203\217\345\244\264\346\211\253\346\217\217", Q_NULLPTR));
        pushButton_merge->setText(QApplication::translate("MainWindow", "\345\274\202\345\270\270\347\212\266\346\200\201\346\243\200\346\265\213", Q_NULLPTR));
        pushButton_dispnoconn->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272\346\234\252\350\277\236\346\216\245\350\256\276\345\244\207", Q_NULLPTR));
        pushButton_dispchange->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272\345\267\262\344\277\256\346\224\271\350\256\276\345\244\207", Q_NULLPTR));
        lineEdit_runstatus->setText(QApplication::translate("MainWindow", "\346\254\242\350\277\216\344\275\277\347\224\250", Q_NULLPTR));
        pushButton_load->setText(QApplication::translate("MainWindow", "\350\257\273\345\217\226", Q_NULLPTR));
        pushButton_save->setText(QApplication::translate("MainWindow", "\344\277\235\345\255\230", Q_NULLPTR));
        pushButton_reset->setText(QApplication::translate("MainWindow", "\351\207\215\347\275\256\350\207\263\345\210\235\345\247\213\351\205\215\347\275\256", Q_NULLPTR));
        pushButton_saveasinit->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\344\270\272\345\210\235\345\247\213\351\205\215\347\275\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
