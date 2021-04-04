#ifndef VIEWSCROLLER_H
#define VIEWSCROLLER_H

#include <QScrollArea>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDropEvent>
#include <QDragEnterEvent>

class viewScroller : public QScrollArea {

public:

    viewScroller(QWidget *parent);
    void wheelEvent(QWheelEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    QPoint getScrollCorrected(QPoint qp);
};

#endif // VIEWSCROLLER_H
