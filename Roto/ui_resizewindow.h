/********************************************************************************
** Form generated from reading UI file 'resizewindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESIZEWINDOW_H
#define UI_RESIZEWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_resizeWindow
{
public:
    QWidget *centralwidget;
    QLabel *label_3;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_4;
    QSpacerItem *horizontalSpacer_5;
    QFrame *line;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QComboBox *comboBox_2;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_3;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *resizeWindow)
    {
        if (resizeWindow->objectName().isEmpty())
            resizeWindow->setObjectName(QString::fromUtf8("resizeWindow"));
        resizeWindow->resize(322, 191);
        centralwidget = new QWidget(resizeWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(40, 20, 47, 13));
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(-1, -1, 321, 171));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        label_4 = new QLabel(verticalLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);


        verticalLayout->addLayout(horizontalLayout);

        line = new QFrame(verticalLayoutWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 2, 4, 1, 1);

        comboBox_2 = new QComboBox(verticalLayoutWidget);
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

        gridLayout->addWidget(comboBox_2, 0, 3, 1, 1);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 0, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 2, 1, 1);

        pushButton = new QPushButton(verticalLayoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 2, 3, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 0, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);

        resizeWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(resizeWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        resizeWindow->setStatusBar(statusbar);

        retranslateUi(resizeWindow);

        QMetaObject::connectSlotsByName(resizeWindow);
    } // setupUi

    void retranslateUi(QMainWindow *resizeWindow)
    {
        resizeWindow->setWindowTitle(QCoreApplication::translate("resizeWindow", "resizeWindow", nullptr));
        label_3->setText(QString());
        label_4->setText(QCoreApplication::translate("resizeWindow", "The Canvas and Import Media have Different Dimensions.", nullptr));
        comboBox_2->setItemText(0, QCoreApplication::translate("resizeWindow", "Do Not Scale", nullptr));
        comboBox_2->setItemText(1, QCoreApplication::translate("resizeWindow", "Scale to Best Fit", nullptr));
        comboBox_2->setItemText(2, QCoreApplication::translate("resizeWindow", "Scale to Aspect Ratio", nullptr));
        comboBox_2->setItemText(3, QCoreApplication::translate("resizeWindow", "Scale to Width", nullptr));
        comboBox_2->setItemText(4, QCoreApplication::translate("resizeWindow", "Scale to Height", nullptr));

        label_2->setText(QCoreApplication::translate("resizeWindow", "Method of Scaling to Layer", nullptr));
        pushButton->setText(QCoreApplication::translate("resizeWindow", "Finish", nullptr));
    } // retranslateUi

};

namespace Ui {
    class resizeWindow: public Ui_resizeWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESIZEWINDOW_H
