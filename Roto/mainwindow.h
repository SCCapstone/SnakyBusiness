#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <string>
#include <list>
#include <functional>
#include <unordered_map>
#include <map>
#include <fstream>
#include <thread>
#include <algorithm>

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QKeyEvent>
#include <QFileDialog>
#include <QImageReader>
#include <QMenuBar>
#include <QColorDialog>
#include <QInputDialog>
#include <QTimer>
#include <QTextBrowser>
#include <QScrollBar>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <dataIOHandler.h>
#include <brushhandler.h>
#include <graphics.h>
#include <stdfuncs.h>
#include <resizewindow.h>
#include <screenrender.h>
#include <radialprofiler.h>
#include <viewscroller.h>

using cv::VideoCapture;
using cv::Mat;
using cv::destroyAllWindows;

using graphics::vectorFilters;
using graphics::filterNames;
using graphics::Filter;

using std::string;
using std::to_string;
using std::list;
using std::to_string;
using std::function;
using std::unordered_map;
using std::map;
using std::fstream;
using std::ios;
using std::pair;

using Qt::MouseButton;
using Qt::NoButton;
using Qt::LeftButton;
using Qt::RightButton;
using Qt::Key_Shift;
using Qt::Key_Control;
using Qt::Key_Up;
using Qt::Key_Down;
using Qt::Key_Left;
using Qt::Key_Right;
using Qt::Key_Escape;
using Qt::Key_Delete;
using Qt::Key_Backspace;
using Qt::Key_X;
using Qt::Key_C;
using Qt::Key_V;

const string UI_FileType = ".txt";
const string UI_FileName = "mainMenubar";
const int trackDrawSpeed = 0;

enum EditMode {Brush_Mode, Spline_Mode};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
    void log(string title, QObject *obj);
    void createMenubar();
    void addItems(QMenu *menu, string items);
    void addAction(QMenu *menu, string s);
    void refresh();

public slots:
    void changeVectorFilter(string s);
    void changeScreenFilter(string s);
    void changeBrushFilter(string s);
    void changeBrushMethod(string s);
    void changeBrushShape(string s);
    void doSomething(string btnPress);

private:
    void setShiftFlag(bool b);
    void setSamplePt(QPoint qp);

    Ui::MainWindow *ui;
    screenRender *sr;
    viewScroller *vs;
    EditMode mode;
    bool shiftFlag, ctrlFlag, onePress;
    MouseButton lastButton;
    DataIOHandler *ioh;
    brushHandler bh;
    resizeWindow *resizeCheck;
    RadialProfiler *radialProfiler;
    QColorDialog cd;
    QTextBrowser qtb;
    unordered_map <string, QObject *> objFetch;
    list <QObject *> toDel;

};

void appTo(QImage *qi, Filter f);

#endif // MAINWINDOW_H

