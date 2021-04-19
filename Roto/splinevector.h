#ifndef VECTOR_H
#define VECTOR_H

#include <vector>
#include <QPoint>
#include <QColor>
#include <stdfuncs.h>
#include <graphics.h>

using std::vector;
using std::pair;
using graphics::Filter;
using graphics::Filtering;

enum VectorMode {ColorFill, Filtered};
enum Taper {Single, Double};

const VectorMode lastMode = Filtered;
const Taper lastTaper = Double;
const int minTaper = 0;
const int maxTaper = 10;
const int minWidth = 0;
const int maxWidth = UCHAR_MAX;
const int maxPoints = CHAR_MAX;

class SplineVector {

public:

    SplineVector();
    SplineVector(QPoint a, QPoint b, int Width = 1);
    SplineVector(const SplineVector &sv);
    SplineVector& operator = (const SplineVector &sv);
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
    void setFilterStrength(int val);
    void setFilter(string s);
    void setTaperType(Taper t);
    void setMode(VectorMode vm);
    void swapColors();
    void swapTapers();
    VectorMode getMode();
    Filter getFilter();
    Taper getTaperType();
    pair <char, char> getTaper();
    pair <QRgb, QRgb> getColors();
    pair <QPoint, QPoint> getBounds();
    QString taperTypeString(Taper T);
    QString modeString(VectorMode v);
    Taper taperFromString(QString q);
    VectorMode modeFromString(QString q);

private:

    void setBounds(int mix, int miy, int max, int may);

    vector <QPoint> controlPts, backup;
    QPoint orig, offs;
    int minX, maxX, minY, maxY;
    unsigned char width;
    VectorMode mode;
    unsigned char taper1, taper2;
    Taper taperType;
    QRgb color1, color2;
    Filter filter;
};

#endif // VECTOR_H
