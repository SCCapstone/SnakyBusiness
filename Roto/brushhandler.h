#ifndef brushHANDLER_H
#define brushHANDLER_H

#include <vector>
#include <list>
#include <QPoint>
#include <QImage>
#include <brush.h>
#include <graphics.h>
#include <stdfuncs.h>

using std::vector;
using std::list;
using std::min;
using std::max;
using graphics::Filter;

enum appMethod {overwrite, additive, subtractive, filter, radial, sample};

const string appMethods[] = {"Overwrite", "Additive", "Subtractive", "Filter", "Radial", "Sample"};
const int numMethods = 6;
const int maxDensity = RAND_MAX / 256;
const int minDensity = 0;
const unsigned char maxStrength = 255;
const unsigned char minStrength = 1;    // should this be set by user?

class brushHandler {

public:

    brushHandler(unsigned char str = 128, int size = 30, int density = 0, string type = appMethods[0], QColor qc = QColor(0xFF000000));
    ~brushHandler();
    void setAlpha(int val);
    void setAppMethod(string type);
    void setPoint(QPoint qp);
    void setDensity(int density);
    void setStrength(int str);
    void setShape(string shape);
    void setColor(QColor qc);
    void setFilter(string filterName);
    void setSamplePoint(QPoint sPnt);
    void setRelativePoint(QPoint rPnt);
    const unsigned char *const *const getBrushMap();
    void setPattern(int xDim, int yDim, unsigned char **pattern);
    void setPatternInUse(int used);
    void radialUpdate(int size, vector <int> pts);
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
    void densityUp();
    void densityDown();
    void strengthUp();
    void strengthDown();
    void applyBrush(QImage *qi, QPoint qp);
    void setInterpolationActive(bool flag);

private:

    void setSize(int size);
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

    bool ipolActive, patternInUse;
    int sprayDensity, checkEdgeSize, alpha;
    unsigned char strength, patternXDim, patternYDim, **patternMap, **checkMap;
    appMethod method;
    Brush brush;
    QColor color;
    Filter brushFilter;
    QPoint currPnt, lastPnt, relativityPoint, samplePoint;
    list <QPoint> toProcess;
    float **radialMap;
    vector <int> radialValues;
};

#endif // brushHANDLER_H

