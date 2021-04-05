#ifndef BRUSH_H
#define BRUSH_H

#include <string>
#include <vector>
#include <stdfuncs.h>

using std::string;
using std::vector;

enum Shape {square, circle, vertical, horizontal, lDiagonal, rDiagonal, diamond, octagon, custom};

const string brushShapes[] = {"Square", "Circle", "Vertical", "Horizontal", "Left Diagonal", "Right Diagonal", "Diamond", "Octagon", "Custom"};

const int numBrushes = 9;
const unsigned char maxRadius = 64;
const unsigned char minRadius = 0;

class Brush {

public:

    Brush(string brushName = brushShapes[0], int Radius = 10);
    ~Brush();
    void setShape(string brushName);
    void setRadius(int r);
    int getRadius();
    int getFullSize();
    const unsigned char *const *const getBrushMap();
    void sendTo(vector <vector < unsigned char > > pattern);
    vector <vector <unsigned char> > tempMap;


private:

    void update();
    void createSquare();
    void createCircle();
    void createVertical();
    void createHorizontal();
    void createLeftDiagonal();
    void createRightDiagonal();
    void createDiamond();
    void createOctagon();
    void createCustom();

    unsigned char radius, **brushMap;
    Shape shape;
    unsigned short size;
};

#endif // BRUSH_H
