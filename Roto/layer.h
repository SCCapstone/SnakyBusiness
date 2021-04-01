#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include <list>
#include <QImage>
#include <QPainter>
#include <splinevector.h>
#include <triangle.h>

using std::vector;
using std::list;
using Qt::MouseButton;
using Qt::LeftButton;
using Qt::RightButton;

const float ipolMin = 0.001;
const float ipolMax = 0.1;
const unsigned char ptSize = 5;
const float pi = 3.14159;

enum EditMode {Brush_Mode, Spline_Mode, Raster_Mode};
enum Selection {TopLeft, Top, TopRight, Right, BottomRight, Bottom, BottomLeft, Left, BodySelect, NoSelect};

class Layer {

public:

    Layer();
    Layer(QSize qs);
    Layer(QImage in, int alphaValue);
    Layer(const Layer &layer);
    Layer& operator = (const Layer &layer);
    ~Layer();
    QImage *getCanvas();
    QImage getRenderCanvas();
    vector <QPoint> getRasterEdges();
    vector <list <Triangle> > getTriangles();
    vector <SplineVector> getVectors();
    void pasteVectors(list <SplineVector> svs);
    void pasteRaster(QImage rasterIn, double angleIn, pair <QPoint, QPoint> bounds);
    QImage getRaster();
    double getAngle();
    bool isRotating();
    pair <QPoint, QPoint> getBounds();
    vector <unsigned char> getActiveVectors();
    void spinWheel(int dy);
    void release(QPoint qp, MouseButton button);
    void moveLeft(QPoint qp);
    void moveRight(QPoint qp);
    void pressLeft(QPoint qp);
    MouseButton pressRight(QPoint qp);
    void doubleClickLeft(QPoint qp, bool ctrlFlag);
    void doubleClickRight(QPoint qp);
    void setMode(EditMode m);
    void fillColor(QPoint qp, QColor qc);
    void setShiftFlag(bool b);
    void setAlpha(int a);
    int getAlpha();
    int getWidth();
    void setWidth(int w);
    void widthUp();
    void widthDown();
    void setVectorColor1(QRgb a);
    void setVectorColor2(QRgb b);
    void setVectorTaper1(int a);
    void setVectorTaper2(int b);
    unsigned char getVectorTaperType();
    void setVectorTaperType(Taper t);
    void setVectorFilter(string s);
    void setVectorMode(VectorMode vm);
    void swapColors();
    void swapTapers();
    pair <char, char> getVectorTapers();
    pair <QRgb, QRgb> getVectorColors();
    void cleanUp();
    void selectAll();
    void deselect();
    void deleteSelected();
    void clearVectors();
    Filter getFilter();
    int getFilterStrength();
    void setFilterStrength(int str);
    void setFilter(string filterName);
    static float getipol(float a, float b, float ipol);

private:

    void calcLine();
    void drawRasterSelection(QImage *img);
    void findSelection(QPoint qp);

    EditMode mode;
    Selection selection;
    vector <SplineVector> vects;
    vector <list <Triangle>> tris;
    vector <unsigned char> activeVects;
    char activePt;
    QImage *qi, rasterselectOg;
    float ipolPts, limiter = ipolMin, limitCnt = 2.0, postAngle;
    int alpha;
    bool shiftFlag, selectOgActive, selecting;
    QPoint deltaMove, boundPt1, boundPt2, rotateAnchor;
    Filter filter;

};

#endif // LAYER_H

