#ifndef BRUSH_H
#define BRUSH_H


#include <string>
#include <stdfuncs.h>
using std::string;

enum Shape {square, circle, vertical, horizontal, lDiagonal, rDiagonal, diamond, Octagon};                //  TODO custom
const string brushShapes[] = {"Square", "Circle", "Vertical", "Horizontal", "Left Diagonal", "Right Diagonal", "Diamond", "Octagon"};
const int numBrushes = 8;

const unsigned char maxRadius = 64;
const unsigned char minRadius = 1;

class Brush
{
public:

    Brush(string brushName = brushShapes[0], int Radius = 30);
    ~Brush();
    void setShape(string brushName);
    void setRadius(int r);
    int getRadius();
    int getFullSize();
    const unsigned char *const *const getBrushMap();

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

    unsigned char **brushMap;
    Shape shape;
    unsigned short size;
    unsigned char radius;
};

#endif // BRUSH_H
