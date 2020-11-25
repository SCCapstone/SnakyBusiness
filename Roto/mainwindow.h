#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <string>
#include <list>
#include <functional>
#include <unordered_map>
#include <map>
#include <fstream>
#include <thread>

#include <time.h>
#include <chrono>
using namespace std::chrono;
using std::chrono::milliseconds;

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QKeyEvent>
#include <QFileDialog>
#include <QImageReader>
#include <QMenuBar>
#include <QColorDialog>
#include <QInputDialog>

#include <brushHandler.h>
#include <graphics.h>
#include <dataIOHandler.h>

using graphics::filterNames;
using graphics::Filter;
using graphics::ImgSupport;

using std::cout;
using std::endl;
using std::string;
using std::list;
using std::to_string;
using std::function;
using std::unordered_map;
using std::map;
using std::fstream;
using std::ios;
using std::pair;

const string UI_FileType = ".txt";
const string UI_FileName = "mainMenubar";
const int len = 500;
const int trackDrawSpeed = 0;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);
    ~MainWindow();
    void log(string title, QObject *obj);
    void createMenubar(string filename);
    void addItems(QMenu *menu, string items);
    void addAction(QMenu *menu, string s);

    QImage *qi;
    brushHandler bh;
    Filter screenFilter;
    //DataIOHandler * ImageIO;

    QColorDialog cd;

    ImgSupport imgSupport;

    unordered_map <string, QObject *> objFetch;
    list <QObject *> toDel;

public slots:
    void changeScreenFilter(string s);
    void changeBrushFilter(string s);
    void changeBrushMethod(string s);
    void changeBrushShape(string s);
    void doSomething(string btnPress);

private:
    Ui::MainWindow *ui;
};

void appTo(QImage *qi, Filter f);

#endif // MAINWINDOW_H
