#include "viewscroller.h"

viewScroller::viewScroller(QWidget *parent) {
    setParent(parent);
}

void viewScroller::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

QPoint viewScroller::getScrollCorrected(QPoint qp) {
    qp.setX(qp.x() + horizontalScrollBar()->value());
    qp.setY(qp.y() + verticalScrollBar()->value() - 20);
    return qp;
}
