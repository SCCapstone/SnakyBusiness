#ifndef VECTOR_H
#define VECTOR_H

#include <vector>
#include <QPoint>
#include <QColor>
#include <stdfuncs.h>

using std::vector;
using std::pair;

const int minTaper = 0;
const int maxTaper = 10;
const int minWidth = 0;
const int maxWidth = UCHAR_MAX;

class SplineVector {

public:

    SplineVector(QPoint a, QPoint b, int Width = 1);
    SplineVector operator = (const SplineVector &sv);
    vector <QPoint> getControls();
    void addPt(QPoint qp, size_t index);
    void removePt(size_t index);
    void movePt(QPoint loc, size_t index);
    void translate(int dx, int dy);
    void scaleSpec(int mix, int miy, int max, int may);
    void rotateSpec(int mix, int miy, int max, int may);
    pair <QPoint, QPoint> prescale(QPoint origin);
    void scale(QPoint qp);
    pair <QPoint, QPoint> prerotate(QPoint offset);
    void rotate(QPoint qp);
    void cleanup();
    void setWidth(int val);
    int getWidth();
    int getNumPts();
    void setColor1(QRgb a);
    void setColor2(QRgb b);
    void setTaper1(int a);
    void setTaper2(int b);
    char getTaperType();
    pair <char, char> getTaper();
    pair <QRgb, QRgb> getColors();
    pair <QPoint, QPoint> getBounds();
    void calcBounds();

private:

    void setBounds(int mix, int miy, int max, int may);

    vector <QPoint> controlPts, backup;
    QPoint orig, offs;
    int minX, maxX, minY, maxY;
    unsigned char width;
    char taper1, taper2, taperType;
    QRgb color1, color2;
};

#endif // VECTOR_H

