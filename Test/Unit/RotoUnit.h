#ifndef ROTOUNIT_H
#define ROTOUNIT_H

#include "../../Roto/mainwindow.h"
#include "../../Roto/mainwindow.cpp"
#include <QtTest/QtTest>

class RotoUnit : public QObject {

    Q_OBJECT
private slots:
    void test();

};

#endif // ROTOUNIT_H
