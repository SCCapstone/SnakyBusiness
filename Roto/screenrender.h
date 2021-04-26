#ifndef SCREENRENDER_H
#define SCREENRENDER_H

#include <algorithm>
#include <vector>
#include <string>
#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <QProgressDialog>
#include <QHoverEvent>
#include <QMouseEvent>
#include <stdfuncs.h>
#include <graphics.h>
#include <layer.h>
#include <dataIOHandler.h>
#include <thread>

using std::find;
using std::vector;
using std::to_string;
using std::thread;
using graphics::Filter;
using graphics::Filtering;
using graphics::ImgSupport;

const int flashSpeed = 1000;
static mutex hoverLock;

class screenRender : public QWidget {

    Q_OBJECT

public:

    explicit screenRender(DataIOHandler *dioh, QWidget *parent = nullptr);
    ~screenRender();
    void mouseMoveEvent(QMouseEvent *event);
    void leaveEvent(QMouseEvent *event);
    double getZoom();
    void setZoom(double Zoom);
    void zoomIn();
    void zoomOut();
    void showPts();
    QPoint getZoomCorrected(QPoint qp);
    void paintEvent(QPaintEvent *event);
    void setSamplePt(QPoint qp);
    void stopFlashing();
    void resume();
    void showFg(bool shown);
    void setMode(EditMode emode);
    void updateHoverMap(int r, const unsigned char const* const* arr);
    void setHoverActive(bool active);

private:

    void updateViews();
    void fillTri(Triangle t);
    void fillBTri(QPoint a, QPoint b, QPoint c);
    void fillTTri(QPoint a, QPoint b, QPoint c);
    void filterTri(Triangle t);
    void filterBTri(QPoint a, QPoint b, QPoint c);
    void filterTTri(QPoint a, QPoint b, QPoint c);
    void fillTriSafe(Triangle t);
    void fillBTriSafe(QPoint a, QPoint b, QPoint c);
    void fillTTriSafe(QPoint a, QPoint b, QPoint c);
    void filterTriSafe(Triangle t);
    void filterBTriSafe(QPoint a, QPoint b, QPoint c);
    void filterTTriSafe(QPoint a, QPoint b, QPoint c);
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
    Filter filter;
    DataIOHandler *ioh;
    EditMode mode;
    unsigned char **hoverMap;
    int radius;
    bool hoverActive;
    long long time;
    int yStart, yMid, yEnd;
    unsigned int alphaVal;

public slots:

    void toggleFlasher();
};

#endif // SCREENRENDER_H
