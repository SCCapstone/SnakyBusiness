/********************************************************************************
** Form generated from reading UI file 'patternprofiler.ui'
**
** Created by: Qt User Interface Compiler version 5.15.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PATTERNPROFILER_H
#define UI_PATTERNPROFILER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_patternProfiler
{
public:
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_6;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_3;
    QLabel *label_2;
    QVBoxLayout *verticalLayout_4;
    QSpinBox *spinBox_2;
    QSpinBox *spinBox;
    QVBoxLayout *verticalLayout;
    QDialogButtonBox *buttonBox;

    void setupUi(QWidget *patternProfiler)
    {
        if (patternProfiler->objectName().isEmpty())
            patternProfiler->setObjectName(QString::fromUtf8("patternProfiler"));
        patternProfiler->resize(282, 244);
        verticalLayout_3 = new QVBoxLayout(patternProfiler);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        label = new QLabel(patternProfiler);
        label->setObjectName(QString::fromUtf8("label"));
        label->setEnabled(true);
        label->setCursor(QCursor(Qt::CrossCursor));
        label->setMouseTracking(false);

        verticalLayout_6->addWidget(label);


        verticalLayout_3->addLayout(verticalLayout_6);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_2->addItem(horizontalSpacer);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        label_3 = new QLabel(patternProfiler);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(label_3);

        label_2 = new QLabel(patternProfiler);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setAlignment(Qt::AlignCenter);

        verticalLayout_5->addWidget(label_2);


        horizontalLayout_3->addLayout(verticalLayout_5);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        spinBox_2 = new QSpinBox(patternProfiler);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));
        spinBox_2->setMinimum(1);
        spinBox_2->setMaximum(64);
        spinBox_2->setValue(2);

        verticalLayout_4->addWidget(spinBox_2);

        spinBox = new QSpinBox(patternProfiler);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setMinimum(1);
        spinBox->setMaximum(64);
        spinBox->setValue(2);

        verticalLayout_4->addWidget(spinBox);


        horizontalLayout_3->addLayout(verticalLayout_4);


        verticalLayout_2->addLayout(horizontalLayout_3);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        buttonBox = new QDialogButtonBox(patternProfiler);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        horizontalLayout->addLayout(verticalLayout);


        verticalLayout_3->addLayout(horizontalLayout);


        retranslateUi(patternProfiler);

        QMetaObject::connectSlotsByName(patternProfiler);
    } // setupUi

    void retranslateUi(QWidget *patternProfiler)
    {
        patternProfiler->setWindowTitle(QCoreApplication::translate("patternProfiler", "Form", nullptr));
        label_3->setText(QCoreApplication::translate("patternProfiler", "Width", nullptr));
        label_2->setText(QCoreApplication::translate("patternProfiler", "Height", nullptr));
    } // retranslateUi

};

namespace Ui {
    class patternProfiler: public Ui_patternProfiler {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PATTERNPROFILER_H
