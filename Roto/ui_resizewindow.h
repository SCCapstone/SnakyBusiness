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
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
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
    QLabel *label_4;
    QFrame *line;
    QFormLayout *formLayout;
    QLabel *label;
    QComboBox *comboBox;
    QLabel *label_2;
    QComboBox *comboBox_2;
    QSpacerItem *verticalSpacer;
    QPushButton *pushButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *resizeWindow)
    {
        if (resizeWindow->objectName().isEmpty())
            resizeWindow->setObjectName(QString::fromUtf8("resizeWindow"));
        resizeWindow->resize(323, 186);
        centralwidget = new QWidget(resizeWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(40, 20, 47, 13));
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 10, 301, 151));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(verticalLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout->addWidget(label_4);

        line = new QFrame(verticalLayoutWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        comboBox = new QComboBox(verticalLayoutWidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, comboBox);

        label_2 = new QLabel(verticalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        comboBox_2 = new QComboBox(verticalLayoutWidget);
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->addItem(QString());
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

        formLayout->setWidget(1, QFormLayout::FieldRole, comboBox_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        formLayout->setItem(2, QFormLayout::FieldRole, verticalSpacer);

        pushButton = new QPushButton(verticalLayoutWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        formLayout->setWidget(3, QFormLayout::FieldRole, pushButton);


        verticalLayout->addLayout(formLayout);

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
        label->setText(QCoreApplication::translate("resizeWindow", "Layer to Scale", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("resizeWindow", "Canvas Layer", nullptr));
        comboBox->setItemText(1, QCoreApplication::translate("resizeWindow", "Media Layer", nullptr));

        label_2->setText(QCoreApplication::translate("resizeWindow", "Method of Scaling to Other Layer", nullptr));
        comboBox_2->setItemText(0, QCoreApplication::translate("resizeWindow", "Do Not Scale", nullptr));
        comboBox_2->setItemText(1, QCoreApplication::translate("resizeWindow", "Scale to Width", nullptr));
        comboBox_2->setItemText(2, QCoreApplication::translate("resizeWindow", "Scale to Height", nullptr));
        comboBox_2->setItemText(3, QCoreApplication::translate("resizeWindow", "Scale to Aspect Ratio", nullptr));

        pushButton->setText(QCoreApplication::translate("resizeWindow", "Finish", nullptr));
    } // retranslateUi

};

namespace Ui {
    class resizeWindow: public Ui_resizeWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESIZEWINDOW_H
