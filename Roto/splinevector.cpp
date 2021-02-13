#include <splinevector.h>


SplineVector::SplineVector(QPoint a, QPoint b, int Width) {
    controlPts.push_back(a);
    controlPts.push_back(b);
    width = Width;
    setColors(0xFF00DD00, 0xFFAA00AA);
}

SplineVector SplineVector::operator = (const SplineVector &sv) {
    vector <QPoint> cpts = sv.controlPts;
    SplineVector ret(cpts.front(), cpts.back(), sv.width);
    for (size_t i = 1; i < cpts.size() - 1; ++i)
        ret.addPt(cpts[i], i);
    return ret;
}

vector <QPoint> SplineVector::getControls() {
    return controlPts;
}

void SplineVector::addPt(QPoint qp, size_t index) {
    if (controlPts.size() < UCHAR_MAX) {
        controlPts.push_back(QPoint(0,0));
        for (size_t i = controlPts.size() - 1; i > index; --i)
            controlPts[i] = controlPts[i - 1];
        controlPts[index] = qp;
    }
}

void SplineVector::removePt(size_t index) {
    controlPts.erase((controlPts.begin() + index));
}

void SplineVector::movePt(QPoint loc, size_t index) {
    if (controlPts.size() == 2) {
        QPoint qp = controlPts[index];
        controlPts[index] = loc;
        if (controlPts[0].x() == controlPts[1].x() && controlPts[0].y() == controlPts[1].y())
            controlPts[index] = qp;
    }
    else
        controlPts[index] = loc;
}

void SplineVector::translate(int dx, int dy) {
    for (size_t i = 0; i < controlPts.size(); ++i) {
        controlPts[i].setX(controlPts[i].x() + dx);
        controlPts[i].setY(controlPts[i].y() + dy);
    }
}

pair <QPoint, QPoint> SplineVector::getBounds() {
    return pair <QPoint, QPoint> (QPoint(minX, minY), QPoint(maxX, maxY));
}

void SplineVector::calcBounds() {
    maxY = maxX = 0;
    minY = minX = INT_MAX;
    for (QPoint qp : controlPts) {
        if (qp.x() < minX)
            minX = qp.x();
        if (qp.x() > maxX)
            maxX = qp.x();
        if (qp.y() < minY)
            minY = qp.y();
        if (qp.y() > maxY)
            maxY = qp.y();
    }
}

void SplineVector::setBounds(int mix, int miy, int max, int may) {
    minX = mix;
    minY = miy;
    maxX = max;
    maxY = may;
}

void SplineVector::scaleSpec(int mix, int miy, int max, int may) {
    setBounds(mix, miy, max, may);
}

pair <QPoint, QPoint> SplineVector::prescale(QPoint origin) {
    backup = controlPts;
    orig = origin;
    calcBounds();
    return getBounds();
}

void SplineVector::scale(QPoint qp) {
    int xOffset = qp.x() - orig.x(), yOffset = qp.y() - orig.y();
    float dx = static_cast<float>(xOffset), dy = static_cast<float>(yOffset);
    xOffset /= 2;
    yOffset /= 2;
    float diffX = static_cast<float>(maxX - minX), diffY = static_cast<float>(maxY - minY);
    for (size_t i = 0; i < controlPts.size(); ++i) {
        int cx = static_cast<int>((static_cast<float>(backup[i].x() - minX) * dx) / diffX);
        int cy = static_cast<int>((static_cast<float>(backup[i].y() - minY) * dy) / diffY);
        controlPts[i].setX(backup[i].x() + cx - xOffset);
        controlPts[i].setY(backup[i].y() + cy - yOffset);
    }
}

void SplineVector::cleanup() {
    backup.clear();
}

void SplineVector::rotateSpec(int mix, int miy, int max, int may) {
    setBounds(mix, miy, max, may);
    orig = QPoint(minX + (maxX - minX) / 2, minY + (maxY - minY) / 2);
}

pair <QPoint, QPoint> SplineVector::prerotate(QPoint offset) {
    backup = controlPts;
    offs = offset;
    calcBounds();
    orig = QPoint(minX + (maxX - minX) / 2, minY + (maxY - minY) / 2);
    return getBounds();
}

void SplineVector::rotate(QPoint qp) {
    int ox = orig.x(), oy = orig.y();
    float angle = atan2(qp.y() - oy, qp.x() - ox) - atan2(offs.y() - oy, offs.x() - ox);
    float s = sin(angle);
    float c = cos(angle);
    for (size_t i = 0; i < controlPts.size(); ++i) {
        float x = static_cast<float>(backup[i].x() - ox), y = static_cast<float>(backup[i].y() - oy);
        float xnew = x * c - y * s, ynew = x * s + y * c;
        controlPts[i].setX(static_cast<int>(xnew) + ox);
        controlPts[i].setY(static_cast<int>(ynew) + oy);
    }
}

void SplineVector::setWidth(int val) {
    if (val >= 0 && val <= UCHAR_MAX)
        width = static_cast<unsigned char>(val);
}

int SplineVector::getWidth() {
    return static_cast<int>(width);
}

int SplineVector::getNumPts() {
    return static_cast<int>(controlPts.size());
}

void SplineVector::setColors(QRgb a, QRgb b) {
    colors[0] = a;
    colors[1] = b;
}

pair <QRgb, QRgb> SplineVector::getColors() {
    return pair <QRgb, QRgb> (colors[0], colors[1]);
}
