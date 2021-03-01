#ifndef VIEWSCROLLER_H
#define VIEWSCROLLER_H

#include <QScrollArea>
#include <QWheelEvent>
#include <QScrollBar>

class viewScroller : public QScrollArea
{
public:
    viewScroller(QWidget *parent);
    void wheelEvent(QWheelEvent *event);
    QPoint getScrollCorrected(QPoint qp);
};

#endif // VIEWSCROLLER_H
