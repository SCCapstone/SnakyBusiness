#include <triangle.h>


Triangle::Triangle(QPoint A, QPoint B, QPoint C) {
    a = A;
    b = B;
    c = C;
}

Triangle::Triangle(const Triangle &t) {
    *this = t;
}

Triangle& Triangle::operator=(const Triangle &t) {
    a = t.a;
    b = t.b;
    c = t.c;
    return *this;
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
