#include "mainwindow.h"
#include <QApplication>
#include <Windows.h>


#pragma comment(lib, "urlmon.lib")

int main(int argc, char *argv[])
{
    /*
     * https://doc.qt.io/qt-5/qhoverevent.html
     * Use when drawing brush onto screen (cursor) before drag and click. or maybe QWidget::setMouseTracking(). The
     * color of the cursor should be a monochromed negative version of the color under the cursor, thus being always in contrast
     * with what is on screen.
     *
     */

    QApplication a(argc, argv);
    MainWindow w;

    //https://doc.qt.io/qt-5/qtwidgets-mainwindows-menus-example.html

    w.show();
    int i = a.exec();
    return i;
}

