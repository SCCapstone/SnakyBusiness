#include <splinevector.h>

// This should only be called as a blank vector to populate in dataIOHandler::load()
SplineVector::SplineVector() {
    width = 0;
    color1 = color2 = 0xFF000000;
    taper1 = taper2 = 0;
    taperType = Double;
    filter.setFilter("Greyscale");
    mode = ColorFill;
}

SplineVector::SplineVector(QPoint a, QPoint b, int Width) {
    controlPts.push_back(a);
    controlPts.push_back(b);
    width = Width;
    color1 = color2 = 0xFF000000;
    taper1 = taper2 = 0;
    taperType = Double;
    filter.setFilter("Greyscale");
    mode = ColorFill;
    minX = minY = min(a.x(), a.y());
    maxX = maxY = max(a.x(), a.y());
}

SplineVector::SplineVector(const SplineVector &sv) {
    *this = sv;
}

SplineVector& SplineVector::operator = (const SplineVector &sv) {
    controlPts = sv.controlPts;
    width = sv.width;
    color1 = sv.color1;
    color2 = sv.color2;
    taper1 = sv.taper1;
    taper2 = sv.taper2;
    taperType = sv.taperType;
    filter = sv.filter;
    mode = sv.mode;
    minX = sv.minX;
    maxX = sv.maxX;
    minY = sv.minY;
    maxY = sv.maxY;
    return *this;
}

vector <QPoint> SplineVector::getControls() {
    return controlPts;
}

void SplineVector::addPt(QPoint qp, size_t index) {
    if (controlPts.size() < maxPoints) {
        controlPts.push_back(QPoint(0,0));
        for (size_t i = controlPts.size() - 1; i > index; --i)
            controlPts[i] = controlPts[i - 1];
        controlPts[index] = qp;
    }
    minX = min(minX, qp.x());
    minY = min(minY, qp.y());
    maxX = max(maxX, qp.x());
    maxY = max(maxY, qp.y());
}

void SplineVector::removePt(size_t index) {
    QPoint qp = controlPts[index];
    controlPts.erase((controlPts.begin() + index));
    if (qp.x() == minX) {
        minX = INT_MAX;
        for (QPoint pt : controlPts)
            if (pt.x() < minX)
                minX = pt.x();
    }
    else if (qp.x() == maxX) {
        maxX = 0;
        for (QPoint pt : controlPts)
            if (pt.x() > maxX)
                maxX = pt.x();
    }
    if (qp.y() == minY) {
        minY = INT_MAX;
        for (QPoint pt : controlPts)
            if (pt.y() < minY)
                minY = pt.y();
    }
    else if (qp.y() == maxY) {
        maxY = 0;
        for (QPoint pt : controlPts)
            if (pt.y() > maxY)
                maxY = pt.y();
    }
}

void SplineVector::movePt(QPoint loc, size_t index) {
    if (controlPts.size() == 2) {
        QPoint qp = controlPts[1 - index];
        if (loc.x() != qp.x() || loc.y() != qp.y()) {
            controlPts[index] = qp;
            minX = min(controlPts[0].x(), controlPts[1].x());
            minY = min(controlPts[0].y(), controlPts[1].y());
            maxX = max(controlPts[0].x(), controlPts[1].x());
            maxY = max(controlPts[0].y(), controlPts[1].y());
        }
        else
            return;
    }
    QPoint qp = controlPts[index];
    controlPts[index] = loc;
    if (qp.x() == minX) {
        if (loc.x() <= minX)
            minX = loc.x();
        else {
            minX = INT_MAX;
            for (QPoint pt : controlPts)
                if (pt.x() < minX)
                    minX = pt.x();
        }
    }
    else if (qp.x() == maxX) {
        if (loc.x() >= maxX)
            maxX = loc.x();
        else {
            maxX = 0;
            for (QPoint pt : controlPts)
                if (pt.x() > maxX)
                    maxX = pt.x();
        }
    }
    if (qp.y() == minY) {
        if (loc.y() <= minY)
            minY = loc.y();
        else {
            minY = INT_MAX;
            for (QPoint pt : controlPts)
                if (pt.y() < minY)
                    minY = pt.y();
        }
    }
    else if (qp.y() == maxY) {
        if (loc.y() >= maxY)
            maxY = loc.y();
        else {
            maxY = 0;
            for (QPoint pt : controlPts)
                if (pt.y() > maxY)
                    maxY = pt.y();
        }
    }
}

void SplineVector::translate(int dx, int dy) {
    for (size_t i = 0; i < controlPts.size(); ++i) {
        controlPts[i].setX(controlPts[i].x() + dx);
        controlPts[i].setY(controlPts[i].y() + dy);
    }
    minX += dx;
    maxX += dx;
    minY += dy;
    maxY += dy;
}

pair <QPoint, QPoint> SplineVector::getBounds() {
    return pair <QPoint, QPoint> (QPoint(minX, minY), QPoint(maxX, maxY));
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
    maxY = maxX = -INT_MAX;
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

void SplineVector::rotateSpec(int mix, int miy, int max, int may) {
    setBounds(mix, miy, max, may);
    orig = QPoint(minX + (maxX - minX) / 2, minY + (maxY - minY) / 2);
}

pair <QPoint, QPoint> SplineVector::prerotate(QPoint offset) {
    backup = controlPts;
    offs = offset;
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
    width = static_cast<unsigned char>(stdFuncs::clamp(val, minWidth, maxWidth));
}

int SplineVector::getWidth() {
    return static_cast<int>(width);
}

int SplineVector::getNumPts() {
    return static_cast<int>(controlPts.size());
}

void SplineVector::setColor1(QRgb a) {
    color1 = a;
}

void SplineVector::setColor2(QRgb b) {
    color2 = b;
}

void SplineVector::setTaper1(int a) {
    taper1 = static_cast<char>(stdFuncs::clamp(a, minTaper, maxTaper));
}

void SplineVector::setTaper2(int b) {
    taper2 = static_cast<char>(stdFuncs::clamp(b, minTaper, maxTaper));
}

void SplineVector::setTaperType(Taper t) {
    taperType = t;
}

void SplineVector::setFilterStrength(int val) {
    filter.setStrength(val);
}

void SplineVector::setFilter(string s) {
    filter.setFilter(s);
}

void SplineVector::setMode(VectorMode vm) {
    mode = vm;
}

void SplineVector::swapColors() {
    QRgb qc = color1;
    color1 = color2;
    color2 = qc;
}

void SplineVector::swapTapers() {
    reverse(controlPts.begin(), controlPts.end());
    swapColors();
}

VectorMode SplineVector::getMode() {
    return mode;
}

Filter SplineVector::getFilter() {
    return filter;
}

Taper SplineVector::getTaperType() {
    return taperType;
}

pair <char, char> SplineVector::getTaper() {
    return pair <char, char> (taper1, taper2);
}

pair <QRgb, QRgb> SplineVector::getColors() {
    return pair <QRgb, QRgb> (color1, color2);
}

QString SplineVector::taperTypeString(Taper T) {
    switch(T) {
        case Single: return QString("Single");
        case Double: return QString("Double");
        default: return QString("");
    }
}

QString SplineVector::modeString(VectorMode v) {
    switch (v) {
        case ColorFill: return QString("ColorFill");
        case Filtered: return QString("Filtered");
        default: return QString("");
    }
}

Taper SplineVector::taperFromString(QString q) {
    if (q == "Double")
        return Double;
    else
        return Single;
}

VectorMode SplineVector::modeFromString(QString q) {
    if (q == "ColorFill")
        return ColorFill;
    else
        return Filtered;
}
