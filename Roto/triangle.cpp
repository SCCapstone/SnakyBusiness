#include <triangle.h>


Triangle::Triangle(QPoint A, QPoint B, QPoint C) {
    a = A;
    b = B;
    c = C;
}

QPoint Triangle::A() {
    return a;
}

QPoint Triangle::B() {
    return b;
}

QPoint Triangle::C() {
    return c;
}

