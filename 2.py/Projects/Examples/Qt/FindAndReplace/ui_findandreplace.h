/********************************************************************************
** Form generated from reading UI file 'findandreplace.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDANDREPLACE_H
#define UI_FINDANDREPLACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FindAndReplaceDlg
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *findLineEdit;
    QLabel *label_2;
    QLineEdit *replaceLineEdit;
    QHBoxLayout *horizontalLayout;
    QCheckBox *caseCheckBox;
    QCheckBox *wholeCheckBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QComboBox *syntaxComboBox;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QVBoxLayout *verticalLayout_3;
    QPushButton *findButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QSpacerItem *verticalSpacer_2;
    QPushButton *closeButton;

    void setupUi(QDialog *FindAndReplaceDlg)
    {
        if (FindAndReplaceDlg->objectName().isEmpty())
            FindAndReplaceDlg->setObjectName(QStringLiteral("FindAndReplaceDlg"));
        FindAndReplaceDlg->resize(338, 165);
        layoutWidget = new QWidget(FindAndReplaceDlg);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 315, 140));
        horizontalLayout_3 = new QHBoxLayout(layoutWidget);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        findLineEdit = new QLineEdit(layoutWidget);
        findLineEdit->setObjectName(QStringLiteral("findLineEdit"));

        gridLayout->addWidget(findLineEdit, 0, 1, 1, 1);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        replaceLineEdit = new QLineEdit(layoutWidget);
        replaceLineEdit->setObjectName(QStringLiteral("replaceLineEdit"));

        gridLayout->addWidget(replaceLineEdit, 1, 1, 1, 1);


        verticalLayout_2->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        caseCheckBox = new QCheckBox(layoutWidget);
        caseCheckBox->setObjectName(QStringLiteral("caseCheckBox"));

        horizontalLayout->addWidget(caseCheckBox);

        wholeCheckBox = new QCheckBox(layoutWidget);
        wholeCheckBox->setObjectName(QStringLiteral("wholeCheckBox"));

        horizontalLayout->addWidget(wholeCheckBox);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_2->addWidget(label_3);

        syntaxComboBox = new QComboBox(layoutWidget);
        syntaxComboBox->setObjectName(QStringLiteral("syntaxComboBox"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(syntaxComboBox->sizePolicy().hasHeightForWidth());
        syntaxComboBox->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(syntaxComboBox);


        verticalLayout_2->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 30, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout_3->addLayout(verticalLayout_2);

        line = new QFrame(layoutWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout_3->addWidget(line);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        findButton = new QPushButton(layoutWidget);
        findButton->setObjectName(QStringLiteral("findButton"));

        verticalLayout_3->addWidget(findButton);

        replaceButton = new QPushButton(layoutWidget);
        replaceButton->setObjectName(QStringLiteral("replaceButton"));

        verticalLayout_3->addWidget(replaceButton);

        replaceAllButton = new QPushButton(layoutWidget);
        replaceAllButton->setObjectName(QStringLiteral("replaceAllButton"));

        verticalLayout_3->addWidget(replaceAllButton);

        verticalSpacer_2 = new QSpacerItem(20, 13, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer_2);

        closeButton = new QPushButton(layoutWidget);
        closeButton->setObjectName(QStringLiteral("closeButton"));

        verticalLayout_3->addWidget(closeButton);


        horizontalLayout_3->addLayout(verticalLayout_3);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(findLineEdit);
        label_2->setBuddy(replaceLineEdit);
        label_3->setBuddy(syntaxComboBox);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(replaceAllButton, replaceLineEdit);
        QWidget::setTabOrder(replaceLineEdit, wholeCheckBox);
        QWidget::setTabOrder(wholeCheckBox, syntaxComboBox);
        QWidget::setTabOrder(syntaxComboBox, replaceButton);
        QWidget::setTabOrder(replaceButton, findLineEdit);
        QWidget::setTabOrder(findLineEdit, closeButton);
        QWidget::setTabOrder(closeButton, findButton);
        QWidget::setTabOrder(findButton, caseCheckBox);

        retranslateUi(FindAndReplaceDlg);
        QObject::connect(closeButton, SIGNAL(clicked()), FindAndReplaceDlg, SLOT(reject()));

        syntaxComboBox->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(FindAndReplaceDlg);
    } // setupUi

    void retranslateUi(QDialog *FindAndReplaceDlg)
    {
        FindAndReplaceDlg->setWindowTitle(QApplication::translate("FindAndReplaceDlg", "Find And Replace", Q_NULLPTR));
        label->setText(QApplication::translate("FindAndReplaceDlg", "Find &what", Q_NULLPTR));
        label_2->setText(QApplication::translate("FindAndReplaceDlg", "Replace &with", Q_NULLPTR));
        caseCheckBox->setText(QApplication::translate("FindAndReplaceDlg", "&Case sensitive", Q_NULLPTR));
        wholeCheckBox->setText(QApplication::translate("FindAndReplaceDlg", "Wh&ole words", Q_NULLPTR));
        label_3->setText(QApplication::translate("FindAndReplaceDlg", "&Syntax:", Q_NULLPTR));
        syntaxComboBox->clear();
        syntaxComboBox->insertItems(0, QStringList()
         << QApplication::translate("FindAndReplaceDlg", "Literal text", Q_NULLPTR)
         << QApplication::translate("FindAndReplaceDlg", "Regular expression", Q_NULLPTR)
        );
        findButton->setText(QApplication::translate("FindAndReplaceDlg", "&find", Q_NULLPTR));
        replaceButton->setText(QApplication::translate("FindAndReplaceDlg", "&Replace", Q_NULLPTR));
        replaceAllButton->setText(QApplication::translate("FindAndReplaceDlg", "Replace &All", Q_NULLPTR));
        closeButton->setText(QApplication::translate("FindAndReplaceDlg", "Close", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class FindAndReplaceDlg: public Ui_FindAndReplaceDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDANDREPLACE_H
