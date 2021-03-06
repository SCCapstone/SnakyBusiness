#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include <list>
#include <QImage>
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

#include <iostream>
using namespace std;

class Layer {

public:

    Layer();
    Layer(QSize qs);
    Layer(QImage in, int alphaValue);
    Layer(const Layer &layer);
    ~Layer();
    QImage *getCanvas();
    vector <list <Triangle> > getTriangles();
    vector <SplineVector> getVectors();
    void pasteVectors(list <SplineVector> svs);
    vector <unsigned char> getActiveVectors();
    void spinWheel(int dy);
    void release(MouseButton button);
    void moveLeft(QPoint qp);
    void moveRight(QPoint qp);
    void pressLeft(QPoint qp);
    MouseButton pressRight(QPoint qp);
    void doubleClickLeft(QPoint qp, bool ctrlFlag);
    void doubleClickRight(QPoint qp);
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
    void setVectorTaperType(int i);
    void setVectorFilter(string s);
    void setVectorMode(int m);
    void swapColors();
    void swapTapers();
    pair <char, char> getVectorTapers();
    pair <QRgb, QRgb> getVectorColors();
    void cleanUp();
    void deselect();
    void deleteSelected();

private:

    void calcLine();
    float getipol(float a, float b, float ipol);

    vector <SplineVector> vects;
    vector <list <Triangle>> tris;
    vector <unsigned char> activeVects;
    char activePt;
    QImage *qi;
    float ipolPts, limiter = ipolMin, limitCnt = 2.0;
    int alpha;
    bool shiftFlag;
    QPoint deltaMove;

};

#endif // LAYER_H
