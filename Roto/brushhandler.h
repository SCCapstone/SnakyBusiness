#ifndef brushHANDLER_H
#define brushHANDLER_H

#include <brush.h>
#include <graphics.h>
#include <QPoint>
#include <QImage>
#include <iostream>

using graphics::Filter;
using std::list;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::cout;
using std::endl;

enum appMethod {overwrite};
const string appMethods[] = {"Overwrite"};
const int numMethods = 1;

const unsigned char maxStrength = 255;
const unsigned char minStrength = 1;


class brushHandler
{
public:

    brushHandler(int size = 30, string type = appMethods[0], QColor qc = QColor(0xFF000000));
    ~brushHandler();
    void setAppMethod(string type);
    void setPoint(QPoint qp);
    void setSize(int size);
    void setShape(string shape);
    void setColor(QColor qc);
    QColor getColor();
    int getSize();
    int getFullSize();
    void sizeUp();
    void sizeDown();
    void applyBrush(QImage *qi, QPoint qp);

private:

    void resetPoint();
    int onScreen(int x, int y, int xMax, int yMax);
    void overwrite(QImage *qi);

    appMethod method;
    Brush brush;
    QColor color;
    QPoint currPnt;

};

#endif // brushHANDLER_H
