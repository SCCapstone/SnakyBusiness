#include <radialprofiler.h>
#include "ui_radialprofiler.h"


RadialProfiler::RadialProfiler(brushHandler *bHandler, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RadialProfiler)
{
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    ui->setupUi(this);
    ui->statusbar->hide();
    hide();
    int strength = 255;
    bh = bHandler;
    radius = bh->getSize();
    xZoom = static_cast<float>(wSize) / static_cast<float>(radius);
    setFixedSize(QSize(wSize, wSize));
    controlPts[0] = QPoint(0, 0);
    controlPts[1] = QPoint(radius / 3, strength / 3);
    controlPts[2] = QPoint((2 * radius) / 3, (2 * strength) / 3);
    controlPts[3] = QPoint(radius, strength);
    qi = new QImage(radius + 1, 256, QImage::Format_ARGB32_Premultiplied);
    activePt = -1;
    form = false;
    for (int i = 0; i < radius; ++i)
        pts.push_back(0);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(flashCPts()));
    calcLine();
    bh->radialUpdate(radius, pts);
    setWindowTitle("Radial Brush Profiler");
}

RadialProfiler::~RadialProfiler() {
    timer->stop();
    delete timer;
    delete qi;
    delete ui;
}

void RadialProfiler::showRelative() {
    form = 0;
    QMainWindow *qmw = dynamic_cast<QMainWindow*>(this->parent());
    qmw->setEnabled(false);
    this->setEnabled(true);
    int x = qmw->x() + qmw->width() / 2 - this->width() / 2;
    int y = qmw->y() + qmw->height() / 2 - this->height() / 2;
    move(x, y);
    show();
    timer->start(flashMs);
}

void RadialProfiler::closeEvent(QCloseEvent *event) {
    timer->stop();
    bh->radialUpdate(radius, pts);
    hide();
    QMainWindow *qmw = dynamic_cast<QMainWindow*>(this->parent());
    qmw->setEnabled(true);
}

void RadialProfiler::calcLine() {
    for (int i = 0; i <= radius; ++i)
        pts[i] = 255;
    vector <QPointF> workPts;
    for (int i = 0; i < numpts; ++i)
        workPts.push_back(QPointF(controlPts[i]));
    for (float ipol = 0.0; ipol <= 1.0; ipol += ipolPts) {
        for (int max = numpts - 1; max > 0; --max)
            for (char i = 0; i < max; ++i) {
                workPts[i].setX(getipol(workPts[i].x(), workPts[i + 1].x(), ipol));
                workPts[i].setY(getipol(workPts[i].y(), workPts[i + 1].y(), ipol));
            }
        if (pts[workPts[0].x()] > workPts[0].y())
            pts[workPts[0].x()] = workPts[0].y();
    }
}

int RadialProfiler::bound(int val, int max, int min) {
    return val < min ? min : (val > max ? max : val);
}

float RadialProfiler::getipol(float a, float b, float ipol) {
    return a + static_cast<float>(b - a) * ipol;
}

void RadialProfiler::mouseMoveEvent(QMouseEvent *event) {
    QPoint qp = event->pos();
    qp = QPoint(static_cast<int>(static_cast<float>(qp.x()) / xZoom), qp.y() / 2);
    if (activePt == 0) {
        qp.setX(0);
        qp.setY(bound(qp.y(), controlPts[3].y()));
        controlPts[0] = qp;
        if (controlPts[1].y() < qp.y())
            controlPts[1].setY(qp.y());
        if (controlPts[2].y() < qp.y())
            controlPts[2].setY(qp.y());
    }
    else if (activePt == 3) {
        qp.setX(radius);
        qp.setY(bound(qp.y(), 255, controlPts[0].y()));
        controlPts[3] = qp;
        if (controlPts[1].y() > qp.y())
            controlPts[1].setY(qp.y());
        if (controlPts[2].y() > qp.y())
            controlPts[2].setY(qp.y());
    }
    else if (activePt > 0) {
        qp.setX(bound(qp.x(), radius));
        qp.setY(bound(qp.y(), controlPts[3].y(), controlPts[0].y()));
        controlPts[activePt] = qp;
    }
    calcLine();
    repaint();
}

void RadialProfiler::mousePressEvent(QMouseEvent *event) {
    QPoint qp = event->pos();
    qp = QPoint(static_cast<int>(static_cast<float>(qp.x()) / xZoom), qp.y() / 2);
    for (size_t i = 0; i < numpts; ++i)
        if (abs(qp.x() - controlPts[i].x()) + abs(qp.y() - controlPts[i].y()) < 4) {
            activePt = i;
            break;
        }
}

void RadialProfiler::mouseReleaseEvent(QMouseEvent *event) {
    activePt = -1;
}

void RadialProfiler::wheelEvent(QWheelEvent *event) {
    int delta = event->angleDelta().y();
    if (delta == 0)
        return;
    int newRadius = radius + abs(delta) / delta;
    updateSize(newRadius);
    repaint();
}

void RadialProfiler::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Key_Left:
        updateSize(radius - 1);
        break;
    case Key_Right:
        updateSize(radius + 1);
        break;
    }
    repaint();
}

void RadialProfiler::flashCPts() {
    form = !form;
    repaint();
}

void RadialProfiler::paintEvent(QPaintEvent *event) {
    qi->fill(0xFFFFFFFF);
    for (int i = 0; i <= radius; ++i) {
        QRgb color = 0xFF000000;
        for (int j = 0; j < 3; ++j)
            color += ((pts[i] / 2) << (8 * j));
        for (int j = pts[i]; j <= 255; ++j)
            qi->setPixel(i, j, color);
    }
    for (QPoint qp : controlPts) {
        for (int i = bound(qp.x() - 1, radius); i <= bound(qp.x() + 1, radius); ++i)
            for (int j = bound(qp.y() - xZoom, 255); j <= bound(qp.y() + xZoom / 2, 255); ++j) {
                int dist = abs(i - qp.x()) + abs(j - qp.y());
                if ((form || dist >= 2) && dist <= 2) {
                    QColor qc = qi->pixelColor(i, j);
                    int c = pts[qp.x()] > j ? 0 : 255;
                    qc.setRed(c);
                    qc.setGreen(c);
                    qc.setBlue(255 - c / 2);
                    qi->setPixel(i, j, qc.rgba());
                }
            }
    }
    QImage final = qi->scaled(wSize, wSize); //qi->width() * xZoom, qi->height() * yZoom);
    QPainter qp(this);
    qp.drawImage(0, 0, final);
}

void RadialProfiler::updateSize(int size) {
    timer->stop();
    int newRadius = bound(size, maxRadius, minRadius);
    if (radius != newRadius) {
        float mult = static_cast<float>(newRadius) / static_cast<float>(radius);
        size_t size = static_cast<unsigned int>(radius);
        while (pts.size() < size)
            pts.push_back(0);
        while (pts.size() > size)
            pts.pop_back();
        radius = newRadius;
        controlPts[3].setX(radius);
        controlPts[1].setX(static_cast<int>(static_cast<float>(controlPts[1].x()) * mult));
        controlPts[2].setX(static_cast<int>(static_cast<float>(controlPts[2].x()) * mult));
        xZoom = static_cast<float>(wSize) / static_cast<float>(radius);
        calcLine();
        delete qi;
        qi = new QImage(radius + 1, 256, QImage::Format_ARGB32_Premultiplied);
    }
    if (!this->isVisible())
        bh->radialUpdate(newRadius, pts);
    else
        timer->start(flashMs);
}

