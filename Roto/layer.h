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

class Layer {

public:

    Layer(QSize qs);
    ~Layer();
    QImage *getCanvas();
    vector <list <Triangle> > getTriangles();
    vector <SplineVector> getVectors();
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

