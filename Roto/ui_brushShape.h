/********************************************************************************
** Form generated from reading UI file 'brushShape.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BRUSHSHAPE_H
#define UI_BRUSHSHAPE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_brushShape
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_3;
    QVBoxLayout *verticalLayout_4;
    QSpinBox *spinBox_2;
    QVBoxLayout *verticalLayout;
    QDialogButtonBox *buttonBox;

    void setupUi(QWidget *brushShape)
    {
        if (brushShape->objectName().isEmpty())
            brushShape->setObjectName(QString::fromUtf8("brushShape"));
        brushShape->resize(282, 244);
        verticalLayout_3 = new QVBoxLayout(brushShape);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(brushShape);
        label->setObjectName(QString::fromUtf8("label"));
        label->setScaledContents(true);

        horizontalLayout_2->addWidget(label);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        label_3 = new QLabel(brushShape);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(label_3);


        horizontalLayout_3->addLayout(verticalLayout_5);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        spinBox_2 = new QSpinBox(brushShape);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));
        spinBox_2->setMaximum(64);

        verticalLayout_4->addWidget(spinBox_2);


        horizontalLayout_3->addLayout(verticalLayout_4);


        verticalLayout_2->addLayout(horizontalLayout_3);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        buttonBox = new QDialogButtonBox(brushShape);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        horizontalLayout->addLayout(verticalLayout);


        verticalLayout_3->addLayout(horizontalLayout);


        retranslateUi(brushShape);

        QMetaObject::connectSlotsByName(brushShape);
    } // setupUi

    void retranslateUi(QWidget *brushShape)
    {
        brushShape->setWindowTitle(QCoreApplication::translate("brushShape", "Form", nullptr));
        label->setText(QString());
        label_3->setText(QCoreApplication::translate("brushShape", "Size", nullptr));
    } // retranslateUi

};

namespace Ui {
    class brushShape: public Ui_brushShape {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BRUSHSHAPE_H
