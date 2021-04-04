#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <QPoint>

class Triangle {

public:

    Triangle(QPoint A, QPoint B, QPoint C);
    Triangle(const Triangle &t);
    Triangle& operator = (const Triangle &t);
    QPoint A(), B(), C();

private:

    QPoint a,b,c;
};

#endif // TRIANGLE_H
