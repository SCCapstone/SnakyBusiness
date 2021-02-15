#include "brush.h"


Brush::Brush(string brushName, int Radius)
{
    size = 0;
    radius = static_cast<unsigned char>(Radius);
    size = 2 * radius + 1;
    setShape(brushName);
}

Brush::~Brush() {
    for (int i = 0; i < size; ++i)
        delete [] brushMap[i];
    delete [] brushMap;
}

const unsigned char *const *const Brush::getBrushMap() {
    const unsigned char *const *const retMap = brushMap;
    return retMap;
}

void Brush::setShape(string brushName) {
    int i;
    for (i = 0; i < numBrushes; ++i)
        if (brushName == brushShapes[i])
            break;
    if (i == numBrushes)
        i = 0;
    shape = Shape(i);
    update();
}

void Brush::setRadius(int r) {
    for (int i = 0; i < size; ++i)
        delete [] brushMap[i];
    delete [] brushMap;
    radius = stdFuncs::clamp(static_cast<unsigned char>(r), minRadius, maxRadius);
    size = 2 * radius + 1;
    update();
}

void Brush::update() {
    brushMap = new unsigned char*[static_cast<size_t>(size)];
    for (int i = 0; i < size; ++i)
        brushMap[i] = new unsigned char[static_cast<size_t>(size)];
    switch (shape) {
    case square:
        createSquare();
        break;
    case circle:
        createCircle();
        break;
    case vertical:
        createVertical();
        break;
    case horizontal:
        createHorizontal();
        break;
    case lDiagonal:
        createLeftDiagonal();
        break;
    case rDiagonal:
        createRightDiagonal();
        break;
    case diamond:
        createDiamond();
        break;
    case octagon:
        createOctagon();
        break;
    }
}

int Brush::getRadius() {
    return static_cast<int>(radius);
}

int Brush::getFullSize() {
    return static_cast<int>(size);
}

void Brush::createSquare() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            brushMap[i][j] = 1;
}

void Brush::createCircle() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            brushMap[i][j] = sqrt(pow(i - radius, 2) + pow(j - radius, 2)) < radius;
}

void Brush::createVertical() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            brushMap[i][j] = i == radius;
}

void Brush::createHorizontal() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            brushMap[i][j] = j == radius;
}

void Brush::createLeftDiagonal() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) {
            brushMap[i][j] = i == j;
            if (brushMap[i][j] && i != 0)
                brushMap[i][j - 1] = 1;
        }
}

void Brush::createRightDiagonal() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) {
            brushMap[i][j] = i + j == size - 1;
            if (brushMap[i][j] && i != size - 1)
                brushMap[i][j - 1] = 1;
        }
}

void Brush::createDiamond() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) {
            int x = abs(i - radius), y = abs(j - radius);
            brushMap[i][j] = (x + y <= radius);
        }
}

void Brush::createOctagon() {
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j) {
            int x = abs(i - radius), y = abs(j - radius);
            brushMap[i][j] = (x + y < (3 * radius) / 2);
        }
}

