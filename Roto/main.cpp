#include "mainwindow.h"
#include <QApplication>
#include <Windows.h>

#pragma comment(lib, "urlmon.lib")

int main(int argc, char *argv[])
{
    int temp = argc;
    argc = 1;
    QApplication a(argc, argv);
    string startPath = argv[0];
    MainWindow w(startPath.substr(0, startPath.find_last_of("\\")), temp == 1 ? "" : argv[1]);
    w.show();
    return a.exec();
}
