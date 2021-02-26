#ifndef SCREENRENDER_H
#define SCREENRENDER_H

#include <vector>
#include <string>
#include <QWidget>
#include <QScrollArea>
#include <QPainter>
#include <QTimer>
#include <stdfuncs.h>
#include <graphics.h>
#include <layer.h>

using std::vector;
using std::to_string;
using graphics::Filtering;
using graphics::ImgSupport;

const int flashSpeed = 1000;

class screenRender : public QWidget {

    Q_OBJECT

public:

    explicit screenRender(QWidget *parent = nullptr);
    ~screenRender();
    void updateViews(Layer *working, QImage fg, QImage bg);
    double getZoom();
    void setZoom(double Zoom);
    void zoomIn();
    void zoomOut();
    void showPts();
    QPoint getZoomCorrected(QPoint qp);
    void paintEvent(QPaintEvent *event);
    void setSamplePt(QPoint qp);
    void stopFlashing();
    void showFg(bool shown);
    void setSizeDisplay(int i);

private:

    void calcTri(Triangle t);
    void fillBTri(QPoint a, QPoint b, QPoint c);
    void fillTTri(QPoint a, QPoint b, QPoint c);
    void calcTriSafe(Triangle t);
    void fillBTriSafe(QPoint a, QPoint b, QPoint c);
    void fillTTriSafe(QPoint a, QPoint b, QPoint c);
    void doZoom();

    QPoint brushLoc;
    Layer *workLayer;
    QPixmap bgLayers, fgLayers;
    QImage qi, bgPrescaled, fgPrescaled;
    QTimer *flasher;
    bool flashFlag, fgVisible;
    QColor flashColor;
    float adder;
    QColor color;
    QPoint samplePoint;
    ImgSupport screenZoom;
    int sizeDisplay;
    graphics::Filter filter;

public slots:

    void toggleFlasher();
};

#endif // SCREENRENDER_H

