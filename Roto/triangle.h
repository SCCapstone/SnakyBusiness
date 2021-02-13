#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QPoint>

class Triangle
{
public:
    Triangle(QPoint A, QPoint B, QPoint C);
    QPoint A();
    QPoint B();
    QPoint C();

private:
    QPoint a,b,c;
};

#endif // TRIANGLE_H
