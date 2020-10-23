#include <brushhandler.h>


brushHandler::brushHandler(int size, string type, QColor qc) {
    setAppMethod(type);
    setColor(qc);
    brush.setRadius(size);
}

brushHandler::~brushHandler() {

}

void brushHandler::setAppMethod(string type) {
    resetPoint();
    int i = 0;
    for (i = 0; i < numMethods; ++i)
        if (type == appMethods[i])
            break;
    if (i == numMethods)
        i = 0;
    method = appMethod(i);
}

void brushHandler::resetPoint() {
    currPnt = QPoint(-1000, -1000);
}

void brushHandler::setShape(string shape) {
    brush.setShape(shape);
}

void brushHandler::setPoint(QPoint qp) {
    currPnt = qp;
}

void brushHandler::setSize(int size) {
    brush.setRadius(size);
}

void brushHandler::sizeUp() {
    setSize(brush.getRadius() + 1);
}

void brushHandler::sizeDown() {
    setSize(brush.getRadius() - 1);
}

int brushHandler::getSize() {
    return brush.getRadius();
}

int brushHandler::getFullSize() {
    return brush.getFullSize();
}

void brushHandler::setColor(QColor qc) {
    color = qc;
}

QColor brushHandler::getColor() {
    return color;
}

void brushHandler::applyBrush(QImage *qi, QPoint qp) {
    setPoint(qp);
    switch (method) {
    case appMethod::overwrite:
        overwrite(qi);
        break;
    }
}

int brushHandler::onScreen(int y, int x, int yMax, int xMax)  {
    return y >=0 && y < yMax && x >= 0 && x < xMax;
}

void brushHandler::overwrite(QImage *qi) {
    const unsigned char *const *const brushMap = brush.getBrushMap();
    int radius = brush.getRadius(), xMax = qi->width(), yMax = qi->height();
    QRgb qc = color.rgba();
    int x = 0;
    for (int i = currPnt.x() - radius; i <= currPnt.x() + radius; ++i) {
        int y = 0;
        for (int j = currPnt.y() - radius; j <= currPnt.y() + radius; ++j) {
            if (onScreen(i, j, xMax, yMax) && brushMap[x][y])
                qi->setPixel(i, j, qc);
            ++y;
        }
        ++x;
    }
}

