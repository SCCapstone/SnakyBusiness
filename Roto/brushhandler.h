#ifndef brushHANDLER_H
#define brushHANDLER_H

#include <brush.h>
#include <graphics.h>
#include <QPoint>
#include <QImage>
#include <list>
#include <time.h>
#include <chrono>
#include <iostream>

using graphics::Filter;
using std::list;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::cout;
using std::endl;
using std::min;
using std::max;

enum appMethod {overwrite, additive, subtractive, filter, radial, sample};
const string appMethods[] = {"Overwrite", "Additive", "Subtractive", "Filter", "Radial", "Sample"};
const int numMethods = 6;

const int maxDensity = RAND_MAX / 256;
const int minDensity = 0;
const unsigned char maxStrength = 255;
const unsigned char minStrength = 1;    // should this be set by user?

class brushHandler
{
public:

    brushHandler(unsigned char str = 128, int size = 30, int density = 0, string type = appMethods[0], QColor qc = QColor(0xFF000000));
    ~brushHandler();
    void setAppMethod(string type);
    void setPoint(QPoint qp);
    void setDensity(int density);
    void setStrength(int str);
    void setSize(int size);
    void setShape(string shape);
    void setColor(QColor qc);
    void setFilter(string filterName);
    void setSamplePoint(QPoint sPnt);
    QPoint getSamplePoint();
    void setRelativePoint(QPoint rPnt);
    const unsigned char *const *const getBrushMap();
    void setPattern(int xDim, int yDim, unsigned char **pattern);
    void setPatternInUse(int used);
    const unsigned char *const *const getPatternMap();
    int getPatternXDim();
    int getPatternYDim();
    bool getPatternInUse();
    int getMethodIndex();
    int getFilterIndex();
    QColor getColor();
    int getDensity();
    int getStength();
    int getSize();
    int getFullSize();
    void sizeUp();
    void sizeDown();
    void densityUp();
    void densityDown();
    void strengthUp();
    void strengthDown();
    void applyBrush(QImage *qi, QPoint qp);
    void setInterpolationActive(bool flag);

private:

    void resetPoint();
    int onScreen(int x, int y, int xMax, int yMax);
    void overwrite(QImage *qi);
    void additive(QImage *qi);
    void subractive(QImage *qi);
    void filter(QImage *qi);
    void pattern(QImage *qi);
    void radial(QImage *qi);
    void sample(QImage *qi);
    QColor getAffected();
    void shift();
    void shiftLeft();
    void shiftRight();
    void shiftUp();
    void shiftDown();

    bool active, patternInUse;
    int sprayDensity, checkEdgeSize;
    unsigned char strength, patternXDim, patternYDim;
    appMethod method;
    Brush brush;
    QColor color;
    unsigned char **patternMap;
    char **checkMap;
    Filter brushFilter;
    QPoint currPnt, lastPnt, samplePnt, relativityPoint;

    list <QPoint> toProcess;

};

#endif // brushHANDLER_H
