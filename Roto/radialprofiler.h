#ifndef RADIALPROFILER_H
#define RADIALPROFILER_H

#include <vector>
#include <string>
#include <QMainWindow>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QTimer>
#include <QCloseEvent>
#include <brushhandler.h>

using Qt::Key_Left;
using Qt::Key_Right;
using std::vector;
using std::to_string;

const int wSize = 512;
const int numpts = 4;
const int flashMs = 1000;

QT_BEGIN_NAMESPACE
namespace Ui { class RadialProfiler; }
QT_END_NAMESPACE

class RadialProfiler : public QMainWindow
{
    Q_OBJECT

public:

    RadialProfiler(brushHandler *bHandler, QWidget *parent = nullptr);
    ~RadialProfiler();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    void showRelative();
    void updateSize(int size);

public slots:

    void flashCPts();

private:

    void calcLine();
    float getipol(float a, float b, float ipol);

    Ui::RadialProfiler *ui;
    float xZoom;
    QPoint controlPts[4];
    QImage *qi;
    int activePt;
    bool form;
    QTimer *timer;
    float ipolPts = 0.00001;
    int radius;
    vector <int> pts;
    brushHandler *bh;
};

#endif // RADIALPROFILER_H
