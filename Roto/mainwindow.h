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
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QErrorMessage>
#include <QFileSystemWatcher>
#include <QDir>
#include <QScrollBar>
#include <QProcess>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QScreen>
#include <QLabel>
#include <QDropEvent>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QProgressDialog>

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
#include <undoRedo.h>
#include <viewscroller.h>
#include <algorithm>

using std::string;
using std::to_string;
using std::list;
using std::to_string;
using std::function;
using std::unordered_map;
using std::map;
using std::fstream;
using std::ios;
using std::find;
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
using Qt::Key_A;
using Qt::Key_Z;
using Qt::Key_Y;

using cv::VideoCapture;
using cv::Mat;
using cv::destroyAllWindows;

using graphics::vectorFilters;
using graphics::filterNames;
using graphics::Filter;

const QString UI_FileName = "mainMenubar.txt";
const QString Doc_FileName = "Glass_Opus_Documentation.pdf";
const QString WinIco_FileName = "execIco.png";
const QString Logo_FileName = "Logo.png";
const QString UI_Loc = "/Menus/";
const QString Icon_Loc = UI_Loc + "Icons/";
const QString Doc_Loc = "/Documentation/";
const QString FetchLink = "https://github.com/SCCapstone/SnakyBusiness/raw/master";
const vector <string> acceptedImportImageFormats = {"bmp", "jpg", "jpeg", "png", "ppm", "xbm", "xpm", "gif", "pbm", "pgm"};
const vector <string> acceptedExportImageFormats = {"bmp", "jpg", "jpeg", "png", "ppm", "xbm", "xpm"};
const vector <string> acceptedImportVideoFormats = {"mp4", "avi", "mkv"};
const vector <string> acceptedExportVideoFormats = acceptedImportVideoFormats;

enum downloadAction {DownloadThenRestart, DownLoadThenOpen};

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
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void resizeEvent(QResizeEvent *event);
    void closeEvent(QCloseEvent *event);
    void hoverEvent(QHoverEvent *event);

public slots:
    void changeVectorFilter(string s);
    void changeScreenFilter(string s);
    void changeBrushFilter(string s);
    void changeBrushMethod(string s);
    void changeBrushShape(string s);
    void doSomething(string btnPress);
    void downloadFinished();
    void downloadTimeout();

private:
    void log(string title, QObject *obj);
    bool createMenubar();
    void addItems(QMenu *menu, string items);
    void addAction(QMenu *menu, string s);
    void refresh();
    void setShiftFlag(bool b);
    void setSamplePt(QPoint qp);
    void downloadItem(QString subfolder, QString fileName, downloadAction action, QString promptTitle, QString promptText);
    void createDocImgs();
    void setMode(EditMode emode);

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
    QFileSystemWatcher downloadWatcher;
    QTimer downloadTimer;
    QErrorMessage *qme;
    unordered_map <string, QObject *> objFetch;
    list <QObject *> toDel;
    QString dSubfolder, dFileName;
    downloadAction dAction;
    bool takeFlag;
    appMethod tempMethod = overwrite;
    QProgressDialog *progress;
    QUndoStack *undoStack;
};

void appTo(QImage *qi, Filter f);

#endif // MAINWINDOW_H
