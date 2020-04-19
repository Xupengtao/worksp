/********************************************************************************
** Form generated from reading UI file 'tpaurlui.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TPAURLUI_H
#define UI_TPAURLUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TpaUrlui
{
public:
    QWidget *centralWidget;
    QWidget *layoutWidget;
    QGridLayout *gridLayout_2;
    QLabel *label_filetype;
    QComboBox *comboBox_filetype;
    QLabel *label_filename;
    QComboBox *comboBox_filename;
    QComboBox *comboBox_srchtmclass;
    QLabel *label_srchtmclass;
    QWidget *layoutWidget1;
    QGridLayout *gridLayout_3;
    QLabel *label_filesize;
    QLineEdit *lineEdit_filesize;
    QLabel *label_oldstr;
    QLineEdit *lineEdit_oldstr;
    QLabel *label_newstr;
    QLineEdit *lineEdit_newstr;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout;
    QLabel *label_url;
    QLineEdit *lineEdit_url;
    QLabel *label_type;
    QLineEdit *lineEdit_type;
    QLabel *label_htmlpath;
    QLineEdit *lineEdit_htmlpath;
    QLabel *label_baseurl;
    QLineEdit *lineEdit_baseurl;
    QLabel *label_filepath;
    QLineEdit *lineEdit_filepath;
    QLabel *label_times;
    QLineEdit *lineEdit_times;
    QLabel *label_nextstr;
    QLineEdit *lineEdit_nextstr;
    QCheckBox *checkBox_nexttype;
    QComboBox *comboBox_urltype;
    QCheckBox *checkBox_open;
    QCheckBox *checkBox_next;
    QCheckBox *checkBox_genLink;
    QLabel *label_genLink;
    QLabel *label_genLink_2;
    QLabel *label_genLink_3;
    QLabel *label_genLink_4;
    QPushButton *pushButton_find;
    QCheckBox *checkBox_folderorfile;
    QPushButton *pushButton_rmvoidfolder;
    QPushButton *pushButton_renamefolder;
    QPushButton *pushButton_stop;
    QPushButton *pushButton_run;
    QLineEdit *lineEdit_bspath;
    QLabel *label_genLink_6;
    QLineEdit *lineEdit_htmlstr;
    QLabel *label_genLink_5;
    QLabel *label_genLink_7;
    QLineEdit *lineEdit_classstr;
    QLabel *label_genLink_8;
    QLineEdit *lineEdit_dbname;
    QLabel *label_genLink_9;
    QLineEdit *lineEdit_collection;
    QLineEdit *lineEdit_modbkey;
    QLabel *label_genLink_10;
    QPushButton *pushButton_writedb;
    QComboBox *comboBox_inserttype;
    QLabel *label_genLink_11;
    QLabel *label_genLink_12;
    QCheckBox *checkBox_writelog;
    QPushButton *pushButton_save;
    QPushButton *pushButton_load;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *TpaUrlui)
    {
        if (TpaUrlui->objectName().isEmpty())
            TpaUrlui->setObjectName(QStringLiteral("TpaUrlui"));
        TpaUrlui->resize(1091, 553);
        centralWidget = new QWidget(TpaUrlui);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(820, 270, 142, 92));
        gridLayout_2 = new QGridLayout(layoutWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_filetype = new QLabel(layoutWidget);
        label_filetype->setObjectName(QStringLiteral("label_filetype"));

        gridLayout_2->addWidget(label_filetype, 0, 0, 1, 1);

        comboBox_filetype = new QComboBox(layoutWidget);
        comboBox_filetype->setObjectName(QStringLiteral("comboBox_filetype"));

        gridLayout_2->addWidget(comboBox_filetype, 0, 1, 1, 1);

        label_filename = new QLabel(layoutWidget);
        label_filename->setObjectName(QStringLiteral("label_filename"));

        gridLayout_2->addWidget(label_filename, 1, 0, 1, 1);

        comboBox_filename = new QComboBox(layoutWidget);
        comboBox_filename->setObjectName(QStringLiteral("comboBox_filename"));

        gridLayout_2->addWidget(comboBox_filename, 1, 1, 1, 1);

        comboBox_srchtmclass = new QComboBox(layoutWidget);
        comboBox_srchtmclass->setObjectName(QStringLiteral("comboBox_srchtmclass"));

        gridLayout_2->addWidget(comboBox_srchtmclass, 2, 1, 1, 1);

        label_srchtmclass = new QLabel(layoutWidget);
        label_srchtmclass->setObjectName(QStringLiteral("label_srchtmclass"));

        gridLayout_2->addWidget(label_srchtmclass, 2, 0, 1, 1);

        layoutWidget1 = new QWidget(centralWidget);
        layoutWidget1->setObjectName(QStringLiteral("layoutWidget1"));
        layoutWidget1->setGeometry(QRect(620, 380, 341, 121));
        gridLayout_3 = new QGridLayout(layoutWidget1);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(0, 0, 0, 0);
        label_filesize = new QLabel(layoutWidget1);
        label_filesize->setObjectName(QStringLiteral("label_filesize"));

        gridLayout_3->addWidget(label_filesize, 0, 0, 1, 1);

        lineEdit_filesize = new QLineEdit(layoutWidget1);
        lineEdit_filesize->setObjectName(QStringLiteral("lineEdit_filesize"));
        lineEdit_filesize->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(lineEdit_filesize, 0, 1, 1, 1);

        label_oldstr = new QLabel(layoutWidget1);
        label_oldstr->setObjectName(QStringLiteral("label_oldstr"));

        gridLayout_3->addWidget(label_oldstr, 1, 0, 1, 1);

        lineEdit_oldstr = new QLineEdit(layoutWidget1);
        lineEdit_oldstr->setObjectName(QStringLiteral("lineEdit_oldstr"));

        gridLayout_3->addWidget(lineEdit_oldstr, 1, 1, 1, 1);

        label_newstr = new QLabel(layoutWidget1);
        label_newstr->setObjectName(QStringLiteral("label_newstr"));

        gridLayout_3->addWidget(label_newstr, 2, 0, 1, 1);

        lineEdit_newstr = new QLineEdit(layoutWidget1);
        lineEdit_newstr->setObjectName(QStringLiteral("lineEdit_newstr"));

        gridLayout_3->addWidget(lineEdit_newstr, 2, 1, 1, 1);

        layoutWidget2 = new QWidget(centralWidget);
        layoutWidget2->setObjectName(QStringLiteral("layoutWidget2"));
        layoutWidget2->setGeometry(QRect(10, 10, 951, 241));
        gridLayout = new QGridLayout(layoutWidget2);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_url = new QLabel(layoutWidget2);
        label_url->setObjectName(QStringLiteral("label_url"));

        gridLayout->addWidget(label_url, 0, 0, 1, 1);

        lineEdit_url = new QLineEdit(layoutWidget2);
        lineEdit_url->setObjectName(QStringLiteral("lineEdit_url"));
        lineEdit_url->setMinimumSize(QSize(0, 20));
        lineEdit_url->setMaximumSize(QSize(16777215, 20));
        lineEdit_url->setClearButtonEnabled(false);

        gridLayout->addWidget(lineEdit_url, 0, 1, 1, 1);

        label_type = new QLabel(layoutWidget2);
        label_type->setObjectName(QStringLiteral("label_type"));

        gridLayout->addWidget(label_type, 1, 0, 1, 1);

        lineEdit_type = new QLineEdit(layoutWidget2);
        lineEdit_type->setObjectName(QStringLiteral("lineEdit_type"));
        lineEdit_type->setMinimumSize(QSize(0, 20));
        lineEdit_type->setMaximumSize(QSize(16777215, 20));
        lineEdit_type->setClearButtonEnabled(false);

        gridLayout->addWidget(lineEdit_type, 1, 1, 1, 1);

        label_htmlpath = new QLabel(layoutWidget2);
        label_htmlpath->setObjectName(QStringLiteral("label_htmlpath"));

        gridLayout->addWidget(label_htmlpath, 2, 0, 1, 1);

        lineEdit_htmlpath = new QLineEdit(layoutWidget2);
        lineEdit_htmlpath->setObjectName(QStringLiteral("lineEdit_htmlpath"));
        lineEdit_htmlpath->setMinimumSize(QSize(0, 20));
        lineEdit_htmlpath->setMaximumSize(QSize(16777215, 20));

        gridLayout->addWidget(lineEdit_htmlpath, 2, 1, 1, 1);

        label_baseurl = new QLabel(layoutWidget2);
        label_baseurl->setObjectName(QStringLiteral("label_baseurl"));

        gridLayout->addWidget(label_baseurl, 3, 0, 1, 1);

        lineEdit_baseurl = new QLineEdit(layoutWidget2);
        lineEdit_baseurl->setObjectName(QStringLiteral("lineEdit_baseurl"));
        lineEdit_baseurl->setMinimumSize(QSize(0, 20));
        lineEdit_baseurl->setMaximumSize(QSize(16777215, 20));

        gridLayout->addWidget(lineEdit_baseurl, 3, 1, 1, 1);

        label_filepath = new QLabel(layoutWidget2);
        label_filepath->setObjectName(QStringLiteral("label_filepath"));

        gridLayout->addWidget(label_filepath, 4, 0, 1, 1);

        lineEdit_filepath = new QLineEdit(layoutWidget2);
        lineEdit_filepath->setObjectName(QStringLiteral("lineEdit_filepath"));
        lineEdit_filepath->setMinimumSize(QSize(0, 20));
        lineEdit_filepath->setMaximumSize(QSize(16777215, 20));

        gridLayout->addWidget(lineEdit_filepath, 4, 1, 1, 1);

        label_times = new QLabel(layoutWidget2);
        label_times->setObjectName(QStringLiteral("label_times"));

        gridLayout->addWidget(label_times, 5, 0, 1, 1);

        lineEdit_times = new QLineEdit(layoutWidget2);
        lineEdit_times->setObjectName(QStringLiteral("lineEdit_times"));
        lineEdit_times->setMinimumSize(QSize(0, 20));
        lineEdit_times->setMaximumSize(QSize(16777215, 20));

        gridLayout->addWidget(lineEdit_times, 5, 1, 1, 1);

        label_nextstr = new QLabel(layoutWidget2);
        label_nextstr->setObjectName(QStringLiteral("label_nextstr"));

        gridLayout->addWidget(label_nextstr, 6, 0, 1, 1);

        lineEdit_nextstr = new QLineEdit(layoutWidget2);
        lineEdit_nextstr->setObjectName(QStringLiteral("lineEdit_nextstr"));
        lineEdit_nextstr->setMinimumSize(QSize(0, 20));
        lineEdit_nextstr->setMaximumSize(QSize(16777215, 20));

        gridLayout->addWidget(lineEdit_nextstr, 6, 1, 1, 1);

        checkBox_nexttype = new QCheckBox(centralWidget);
        checkBox_nexttype->setObjectName(QStringLiteral("checkBox_nexttype"));
        checkBox_nexttype->setGeometry(QRect(981, 219, 16, 16));
        comboBox_urltype = new QComboBox(centralWidget);
        comboBox_urltype->setObjectName(QStringLiteral("comboBox_urltype"));
        comboBox_urltype->setGeometry(QRect(980, 10, 50, 20));
        checkBox_open = new QCheckBox(centralWidget);
        checkBox_open->setObjectName(QStringLiteral("checkBox_open"));
        checkBox_open->setGeometry(QRect(982, 121, 16, 16));
        checkBox_next = new QCheckBox(centralWidget);
        checkBox_next->setObjectName(QStringLiteral("checkBox_next"));
        checkBox_next->setGeometry(QRect(1001, 121, 16, 16));
        checkBox_genLink = new QCheckBox(centralWidget);
        checkBox_genLink->setObjectName(QStringLiteral("checkBox_genLink"));
        checkBox_genLink->setGeometry(QRect(980, 170, 16, 16));
        label_genLink = new QLabel(centralWidget);
        label_genLink->setObjectName(QStringLiteral("label_genLink"));
        label_genLink->setGeometry(QRect(980, 150, 81, 16));
        label_genLink_2 = new QLabel(centralWidget);
        label_genLink_2->setObjectName(QStringLiteral("label_genLink_2"));
        label_genLink_2->setGeometry(QRect(970, 390, 51, 16));
        label_genLink_3 = new QLabel(centralWidget);
        label_genLink_3->setObjectName(QStringLiteral("label_genLink_3"));
        label_genLink_3->setGeometry(QRect(980, 200, 91, 16));
        label_genLink_4 = new QLabel(centralWidget);
        label_genLink_4->setObjectName(QStringLiteral("label_genLink_4"));
        label_genLink_4->setGeometry(QRect(980, 100, 91, 16));
        pushButton_find = new QPushButton(centralWidget);
        pushButton_find->setObjectName(QStringLiteral("pushButton_find"));
        pushButton_find->setGeometry(QRect(519, 260, 75, 23));
        checkBox_folderorfile = new QCheckBox(centralWidget);
        checkBox_folderorfile->setObjectName(QStringLiteral("checkBox_folderorfile"));
        checkBox_folderorfile->setGeometry(QRect(1030, 390, 16, 16));
        pushButton_rmvoidfolder = new QPushButton(centralWidget);
        pushButton_rmvoidfolder->setObjectName(QStringLiteral("pushButton_rmvoidfolder"));
        pushButton_rmvoidfolder->setGeometry(QRect(970, 426, 91, 23));
        pushButton_renamefolder = new QPushButton(centralWidget);
        pushButton_renamefolder->setObjectName(QStringLiteral("pushButton_renamefolder"));
        pushButton_renamefolder->setGeometry(QRect(970, 470, 91, 23));
        pushButton_stop = new QPushButton(centralWidget);
        pushButton_stop->setObjectName(QStringLiteral("pushButton_stop"));
        pushButton_stop->setGeometry(QRect(981, 330, 75, 23));
        pushButton_run = new QPushButton(centralWidget);
        pushButton_run->setObjectName(QStringLiteral("pushButton_run"));
        pushButton_run->setGeometry(QRect(981, 286, 75, 23));
        lineEdit_bspath = new QLineEdit(centralWidget);
        lineEdit_bspath->setObjectName(QStringLiteral("lineEdit_bspath"));
        lineEdit_bspath->setGeometry(QRect(100, 320, 401, 20));
        lineEdit_bspath->setMinimumSize(QSize(0, 20));
        label_genLink_6 = new QLabel(centralWidget);
        label_genLink_6->setObjectName(QStringLiteral("label_genLink_6"));
        label_genLink_6->setGeometry(QRect(11, 320, 60, 16));
        lineEdit_htmlstr = new QLineEdit(centralWidget);
        lineEdit_htmlstr->setObjectName(QStringLiteral("lineEdit_htmlstr"));
        lineEdit_htmlstr->setGeometry(QRect(100, 260, 401, 20));
        lineEdit_htmlstr->setMinimumSize(QSize(0, 20));
        label_genLink_5 = new QLabel(centralWidget);
        label_genLink_5->setObjectName(QStringLiteral("label_genLink_5"));
        label_genLink_5->setGeometry(QRect(11, 260, 72, 16));
        label_genLink_7 = new QLabel(centralWidget);
        label_genLink_7->setObjectName(QStringLiteral("label_genLink_7"));
        label_genLink_7->setGeometry(QRect(10, 290, 81, 20));
        lineEdit_classstr = new QLineEdit(centralWidget);
        lineEdit_classstr->setObjectName(QStringLiteral("lineEdit_classstr"));
        lineEdit_classstr->setGeometry(QRect(100, 290, 401, 20));
        lineEdit_classstr->setMinimumSize(QSize(0, 20));
        label_genLink_8 = new QLabel(centralWidget);
        label_genLink_8->setObjectName(QStringLiteral("label_genLink_8"));
        label_genLink_8->setGeometry(QRect(10, 390, 71, 16));
        lineEdit_dbname = new QLineEdit(centralWidget);
        lineEdit_dbname->setObjectName(QStringLiteral("lineEdit_dbname"));
        lineEdit_dbname->setGeometry(QRect(99, 390, 401, 20));
        lineEdit_dbname->setMinimumSize(QSize(0, 20));
        label_genLink_9 = new QLabel(centralWidget);
        label_genLink_9->setObjectName(QStringLiteral("label_genLink_9"));
        label_genLink_9->setGeometry(QRect(11, 420, 91, 16));
        lineEdit_collection = new QLineEdit(centralWidget);
        lineEdit_collection->setObjectName(QStringLiteral("lineEdit_collection"));
        lineEdit_collection->setGeometry(QRect(100, 420, 401, 20));
        lineEdit_collection->setMinimumSize(QSize(0, 20));
        lineEdit_modbkey = new QLineEdit(centralWidget);
        lineEdit_modbkey->setObjectName(QStringLiteral("lineEdit_modbkey"));
        lineEdit_modbkey->setGeometry(QRect(99, 450, 401, 20));
        lineEdit_modbkey->setMinimumSize(QSize(0, 20));
        label_genLink_10 = new QLabel(centralWidget);
        label_genLink_10->setObjectName(QStringLiteral("label_genLink_10"));
        label_genLink_10->setGeometry(QRect(10, 450, 91, 16));
        pushButton_writedb = new QPushButton(centralWidget);
        pushButton_writedb->setObjectName(QStringLiteral("pushButton_writedb"));
        pushButton_writedb->setGeometry(QRect(520, 390, 75, 23));
        comboBox_inserttype = new QComboBox(centralWidget);
        comboBox_inserttype->setObjectName(QStringLiteral("comboBox_inserttype"));
        comboBox_inserttype->setGeometry(QRect(100, 480, 91, 20));
        label_genLink_11 = new QLabel(centralWidget);
        label_genLink_11->setObjectName(QStringLiteral("label_genLink_11"));
        label_genLink_11->setGeometry(QRect(10, 480, 91, 16));
        label_genLink_12 = new QLabel(centralWidget);
        label_genLink_12->setObjectName(QStringLiteral("label_genLink_12"));
        label_genLink_12->setGeometry(QRect(980, 240, 91, 16));
        checkBox_writelog = new QCheckBox(centralWidget);
        checkBox_writelog->setObjectName(QStringLiteral("checkBox_writelog"));
        checkBox_writelog->setEnabled(true);
        checkBox_writelog->setGeometry(QRect(981, 259, 16, 16));
        checkBox_writelog->setMouseTracking(true);
        checkBox_writelog->setTristate(false);
        pushButton_save = new QPushButton(centralWidget);
        pushButton_save->setObjectName(QStringLiteral("pushButton_save"));
        pushButton_save->setGeometry(QRect(520, 420, 75, 23));
        pushButton_load = new QPushButton(centralWidget);
        pushButton_load->setObjectName(QStringLiteral("pushButton_load"));
        pushButton_load->setGeometry(QRect(520, 450, 75, 23));
        TpaUrlui->setCentralWidget(centralWidget);
        lineEdit_classstr->raise();
        layoutWidget->raise();
        layoutWidget->raise();
        layoutWidget->raise();
        checkBox_nexttype->raise();
        comboBox_urltype->raise();
        checkBox_open->raise();
        checkBox_next->raise();
        checkBox_genLink->raise();
        label_genLink->raise();
        label_genLink_2->raise();
        label_genLink_3->raise();
        label_genLink_4->raise();
        pushButton_find->raise();
        checkBox_folderorfile->raise();
        pushButton_rmvoidfolder->raise();
        pushButton_renamefolder->raise();
        pushButton_stop->raise();
        pushButton_run->raise();
        lineEdit_bspath->raise();
        label_genLink_6->raise();
        lineEdit_htmlstr->raise();
        label_genLink_5->raise();
        label_genLink_7->raise();
        label_genLink_8->raise();
        lineEdit_dbname->raise();
        label_genLink_9->raise();
        lineEdit_collection->raise();
        lineEdit_modbkey->raise();
        label_genLink_10->raise();
        pushButton_writedb->raise();
        comboBox_inserttype->raise();
        label_genLink_11->raise();
        label_genLink_12->raise();
        checkBox_writelog->raise();
        pushButton_save->raise();
        pushButton_load->raise();
        mainToolBar = new QToolBar(TpaUrlui);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        TpaUrlui->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(TpaUrlui);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        TpaUrlui->setStatusBar(statusBar);

        retranslateUi(TpaUrlui);

        QMetaObject::connectSlotsByName(TpaUrlui);
    } // setupUi

    void retranslateUi(QMainWindow *TpaUrlui)
    {
        TpaUrlui->setWindowTitle(QApplication::translate("TpaUrlui", "TpaUrlui", Q_NULLPTR));
        label_filetype->setText(QApplication::translate("TpaUrlui", "\346\226\207\344\273\266\347\261\273\345\236\213\357\274\232  ", Q_NULLPTR));
        comboBox_filetype->clear();
        comboBox_filetype->insertItems(0, QStringList()
         << QApplication::translate("TpaUrlui", "jpg", Q_NULLPTR)
         << QApplication::translate("TpaUrlui", "mp4", Q_NULLPTR)
        );
        label_filename->setText(QApplication::translate("TpaUrlui", "\345\221\275\345\220\215\346\226\271\345\274\217\357\274\232", Q_NULLPTR));
        comboBox_filename->clear();
        comboBox_filename->insertItems(0, QStringList()
         << QApplication::translate("TpaUrlui", "auto", Q_NULLPTR)
        );
        comboBox_srchtmclass->clear();
        comboBox_srchtmclass->insertItems(0, QStringList()
         << QApplication::translate("TpaUrlui", "file", Q_NULLPTR)
         << QApplication::translate("TpaUrlui", "a", Q_NULLPTR)
         << QApplication::translate("TpaUrlui", "src", Q_NULLPTR)
        );
        label_srchtmclass->setText(QApplication::translate("TpaUrlui", "\350\265\204\346\272\220\347\261\273\345\236\213\357\274\232    ", Q_NULLPTR));
        label_filesize->setText(QApplication::translate("TpaUrlui", "\346\226\207\344\273\266\345\244\247\345\260\217\350\256\276\345\256\232\357\274\232", Q_NULLPTR));
        lineEdit_filesize->setText(QApplication::translate("TpaUrlui", "5000", Q_NULLPTR));
        label_oldstr->setText(QApplication::translate("TpaUrlui", "\345\216\237\345\247\213\345\255\227\347\254\246\344\270\262\357\274\232  ", Q_NULLPTR));
        label_newstr->setText(QApplication::translate("TpaUrlui", "\346\233\277\346\215\242\345\255\227\347\254\246\344\270\262\357\274\232", Q_NULLPTR));
        label_url->setText(QApplication::translate("TpaUrlui", "\347\275\221\345\235\200\357\274\232        ", Q_NULLPTR));
        lineEdit_url->setText(QString());
        label_type->setText(QApplication::translate("TpaUrlui", "\347\261\273\345\236\213\357\274\232", Q_NULLPTR));
        lineEdit_type->setText(QApplication::translate("TpaUrlui", "open,down,next", Q_NULLPTR));
        label_htmlpath->setText(QApplication::translate("TpaUrlui", "htmlpath\357\274\232", Q_NULLPTR));
        label_baseurl->setText(QApplication::translate("TpaUrlui", "\347\275\221\345\235\200\345\237\272\345\234\260\345\235\200\357\274\232  ", Q_NULLPTR));
        label_filepath->setText(QApplication::translate("TpaUrlui", "\345\255\230\345\202\250\346\226\207\344\273\266\345\244\271\357\274\232", Q_NULLPTR));
        lineEdit_filepath->setText(QApplication::translate("TpaUrlui", "E:\\", Q_NULLPTR));
        label_times->setText(QApplication::translate("TpaUrlui", "\351\241\265\346\225\260\357\274\232        ", Q_NULLPTR));
        lineEdit_times->setText(QApplication::translate("TpaUrlui", "1", Q_NULLPTR));
        label_nextstr->setText(QApplication::translate("TpaUrlui", "\344\270\213\344\270\200\351\241\265\345\255\227\347\254\246\344\270\262\357\274\232", Q_NULLPTR));
        lineEdit_nextstr->setText(QString());
        checkBox_nexttype->setText(QString());
        comboBox_urltype->clear();
        comboBox_urltype->insertItems(0, QStringList()
         << QApplication::translate("TpaUrlui", "get", Q_NULLPTR)
         << QApplication::translate("TpaUrlui", "post", Q_NULLPTR)
        );
        checkBox_open->setText(QString());
        checkBox_next->setText(QString());
        checkBox_genLink->setText(QString());
        label_genLink->setText(QApplication::translate("TpaUrlui", "\347\224\237\346\210\220\351\223\276\346\216\245\346\226\207\344\273\266\357\274\232", Q_NULLPTR));
        label_genLink_2->setText(QApplication::translate("TpaUrlui", "\346\226\207\344\273\266\347\261\273\345\236\213", Q_NULLPTR));
        label_genLink_3->setText(QApplication::translate("TpaUrlui", "\344\275\277\347\224\250\345\255\227\347\254\246\344\270\262\346\237\245\346\211\276\357\274\232", Q_NULLPTR));
        label_genLink_4->setText(QApplication::translate("TpaUrlui", "\347\275\221\345\235\200\351\234\200\345\237\272\345\234\260\345\235\200\357\274\232", Q_NULLPTR));
        pushButton_find->setText(QApplication::translate("TpaUrlui", "\346\237\245\346\211\276", Q_NULLPTR));
        checkBox_folderorfile->setText(QString());
        pushButton_rmvoidfolder->setText(QApplication::translate("TpaUrlui", "\346\226\207\344\273\266\345\244\271\346\225\264\347\220\206", Q_NULLPTR));
        pushButton_renamefolder->setText(QApplication::translate("TpaUrlui", "\346\226\207\344\273\266\345\244\271\351\207\215\345\221\275\345\220\215", Q_NULLPTR));
        pushButton_stop->setText(QApplication::translate("TpaUrlui", "\345\201\234\346\255\242", Q_NULLPTR));
        pushButton_run->setText(QApplication::translate("TpaUrlui", "\350\277\220\350\241\214", Q_NULLPTR));
        label_genLink_6->setText(QApplication::translate("TpaUrlui", "HTML\350\267\257\345\276\204\357\274\232", Q_NULLPTR));
        label_genLink_5->setText(QApplication::translate("TpaUrlui", "\350\267\257\345\276\204\345\255\227\347\254\246\344\270\262\357\274\232", Q_NULLPTR));
        label_genLink_7->setText(QApplication::translate("TpaUrlui", "class\345\220\215(\345\217\257\351\200\211)\357\274\232", Q_NULLPTR));
        label_genLink_8->setText(QApplication::translate("TpaUrlui", "\346\225\260\346\215\256\345\272\223\345\220\215\347\247\260\357\274\232", Q_NULLPTR));
        label_genLink_9->setText(QApplication::translate("TpaUrlui", "collection\345\220\215\347\247\260\357\274\232", Q_NULLPTR));
        label_genLink_10->setText(QApplication::translate("TpaUrlui", "Record\351\224\256\345\200\274\357\274\232", Q_NULLPTR));
        pushButton_writedb->setText(QApplication::translate("TpaUrlui", "\345\206\231\345\205\245", Q_NULLPTR));
        comboBox_inserttype->clear();
        comboBox_inserttype->insertItems(0, QStringList()
         << QApplication::translate("TpaUrlui", "page", Q_NULLPTR)
         << QApplication::translate("TpaUrlui", "key-value", Q_NULLPTR)
        );
        label_genLink_11->setText(QApplication::translate("TpaUrlui", "\345\206\231\345\205\245\346\226\271\345\274\217\357\274\232", Q_NULLPTR));
        label_genLink_12->setText(QApplication::translate("TpaUrlui", "\345\206\231\345\205\245\346\227\245\345\277\227\357\274\232", Q_NULLPTR));
        checkBox_writelog->setText(QString());
        pushButton_save->setText(QApplication::translate("TpaUrlui", "\344\277\235\345\255\230", Q_NULLPTR));
        pushButton_load->setText(QApplication::translate("TpaUrlui", "\350\275\275\345\205\245", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TpaUrlui: public Ui_TpaUrlui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TPAURLUI_H
