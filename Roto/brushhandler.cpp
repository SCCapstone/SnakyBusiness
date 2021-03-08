#include <brushhandler.h>


brushHandler::brushHandler(unsigned char str, int size, int density, string type, QColor qc) {
    alpha = 255;
    setStrength(str);
    setDensity(density);
    setAppMethod(type);
    setBrushColor(qc);
    setFillColor(qc);
    brush.setRadius(size);
    checkEdgeSize = size / 2;
    size_t rad = static_cast<size_t>(2 * (checkEdgeSize + size) + 1);
    checkMap = new unsigned char *[rad];
    for (size_t i = 0; i < rad; ++i)
        checkMap[i] = new unsigned char [rad];
    for (size_t i = 0; i < rad; ++i)
        for (size_t j = 0; j < rad; ++j)
            checkMap[i][j] = 0;
    int fullSize = brush.getFullSize();
    radialMap = new float * [fullSize];
    for (int i = 0; i < fullSize; ++i)
        radialMap[i] = new float [fullSize];
    for (int i = 0; i < fullSize; ++i)
        for (int j = 0; j < fullSize; ++j)
            radialMap[i][j] = 1;
    patternInUse = false;
    patternXDim = patternYDim = 1;
    patternMap = new unsigned char*[patternXDim];
    patternMap[0] = new unsigned char[patternYDim];
    patternMap[0][0] = 0;
    ipolActive = false;
    relativityPoint = QPoint(-1000,-1000);
    resetPoint();
}

brushHandler::~brushHandler() {
    int rad = 2 * (brush.getRadius() + checkEdgeSize) + 1;
    for (int i = 0; i < rad; ++i)
        delete [] checkMap[i];
    delete [] checkMap;
    int fullSize = brush.getFullSize();
    for (int i = 0; i < fullSize; ++i)
         delete [] radialMap[i];
    delete [] radialMap;
    for (int i = 0; i < patternYDim; ++i)
        delete [] patternMap[i];
    delete [] patternMap;
}

void brushHandler::setAlpha(int val) {
    alpha = static_cast<unsigned int>(val);
    brushColor.setAlpha(alpha);
    fillColor.setAlpha(alpha);
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

void brushHandler::setFilter(string filterName) {
    resetPoint();
    brushFilter.setFilter(filterName);
}

void brushHandler::setShape(string shape) {
    brush.setShape(shape);
}

int brushHandler::getMethodIndex() {
    return method;
}

void brushHandler::setPoint(QPoint qp) {
    int dx = qp.x() - currPnt.x();
    int dy = qp.y() - currPnt.y();
    if (ipolActive) {
        if (dx != 0 && dy != 0) {
            if (abs(dx) > abs(dy)) {
                double yAdder = static_cast<double>(dy) / static_cast<double>(abs(dx));
                int xAdder = dx < 0 ? -1 : 1;
                double y = static_cast<double>(currPnt.y()) + yAdder;
                for (int x = currPnt.x() + xAdder; x != qp.x(); x += xAdder) {
                    toProcess.push_back(QPoint(x, static_cast<int>(y)));
                    y += yAdder;
                }
            }
            else {
                double xAdder = static_cast<double>(dx) / static_cast<double>(abs(dy));
                int yAdder = dy < 0 ? -1 : 1;
                double x = static_cast<double>(currPnt.x()) + xAdder;
                for (int y = currPnt.y() + yAdder; y != qp.y(); y += yAdder) {
                    toProcess.push_back(QPoint(static_cast<int>(x), y));
                    x += xAdder;
                }
            }
        }
        else if (dx == 0 && dy != 0) {
            int yAdder = dy < 0 ? -1 : 1;
            for (int y = currPnt.y() + yAdder; y != qp.y(); y += yAdder)
                toProcess.push_back(QPoint(currPnt.x(), y));
        }
        else if (dx != 0 && dy == 0) {
            int xAdder = dx < 0 ? -1 : 1;
            for (int x = currPnt.x() + xAdder; x != qp.x(); x += xAdder)
                toProcess.push_back(QPoint(x, currPnt.y()));
        }
    }
    else {
        size_t rad = static_cast<size_t>(2 * (brush.getRadius() + checkEdgeSize) + 1);
        for (size_t i = 0; i < rad; ++i)
            for (size_t j = 0; j < rad; ++j)
                checkMap[i][j] = 0;
    }
    toProcess.push_back(QPoint(qp));
}

void brushHandler::setDensity(int density) {
    sprayDensity = stdFuncs::clamp(density, minDensity, maxDensity);
}

void brushHandler::densityUp() {
    setDensity(sprayDensity + 1);
}

void brushHandler::densityDown() {
    setDensity(sprayDensity - 1);
}

int brushHandler::getDensity() {
    return sprayDensity;
}

void brushHandler::setStrength(int str) {
    strength = stdFuncs::clamp(static_cast<unsigned char>(str), minStrength, maxStrength);
    brushFilter.setStrength(str);
}

void brushHandler::strengthUp() {
    setStrength(strength + 1);
}

void brushHandler::strengthDown() {
    setStrength(strength - 1);
}

int brushHandler::getStength() {
    return strength;
}

void brushHandler::setSize(int size) {
    size_t rad = static_cast<size_t>(2 * (brush.getRadius() + checkEdgeSize) + 1);
    for (size_t i = 0; i < rad; ++i)
        delete [] checkMap[i];
    delete [] checkMap;
    brush.setRadius(size);
    checkEdgeSize = size / 2;
    rad = static_cast<size_t>(2 * (brush.getRadius() + checkEdgeSize) + 1);
    checkMap = new unsigned char *[rad];
    for (size_t i = 0; i < rad; ++i)
        checkMap[i] = new unsigned char [rad];
    for (size_t i = 0; i < rad; ++i)
        for (size_t j = 0; j < rad; ++j)
            checkMap[i][j] = 0;
}

const unsigned char *const *const brushHandler::getBrushMap() {
    return brush.getBrushMap();
}

void brushHandler::setPattern(int xDim, int yDim, unsigned char **pattern) {
    for (int j = 0; j < patternYDim; ++j)
        delete [] patternMap[j];
    delete [] patternMap;
    patternMap = pattern;
    patternXDim = static_cast<unsigned char>(xDim);
    patternYDim = static_cast<unsigned char>(yDim);
}

void brushHandler::setPatternInUse(int used) {
    patternInUse = static_cast<unsigned char>(used);
}

const unsigned char *const *const brushHandler::getPatternMap() {
    const unsigned char *const *const retMap = patternMap;
    return patternMap;
}

int brushHandler::getPatternXDim() {
    return patternXDim;
}

int brushHandler::getPatternYDim() {
    return patternYDim;
}

bool brushHandler::getPatternInUse() {
    return patternInUse;
}

void brushHandler::resetPoint() {
    currPnt = QPoint(-1000, -1000);
    samplePoint = currPnt;
    lastPnt = currPnt;
}

int brushHandler::getSize() {
    return brush.getRadius();
}

int brushHandler::getFullSize() {
    return brush.getFullSize();
}

int brushHandler::getFilterIndex() {
    return brushFilter.getFilterIndex();
}

void brushHandler::setBrushColor(QColor qc) {
    brushColor = qc;
    brushColor.setAlpha(alpha);
}

void brushHandler::setFillColor(QColor qc) {
    fillColor = qc;
    fillColor.setAlpha(alpha);
}

QColor brushHandler::getBrushColor() {
    QColor toRet = brushColor;
    toRet.setAlpha(255);
    return toRet;
}

QColor brushHandler::getFillColor() {
    QColor toRet = fillColor;
    toRet.setAlpha(255);
    return toRet;
}

void brushHandler::applyBrush(QImage *qi, QPoint qp) {
    setPoint(qp);
    switch (method) {
    case appMethod::overwrite:
        overwrite(qi);
        break;
    case appMethod::additive:
        additive(qi);
        break;
    case appMethod::subtractive:
        subractive(qi);
        break;
    case appMethod::filter:
        filter(qi);
        break;
    case appMethod::radial:
        radial(qi);
        break;
    case appMethod::sample:
        sample(qi);
        break;
    }
}

void brushHandler::setInterpolationActive(bool flag) {
    ipolActive = flag;
}

void brushHandler::setSamplePoint(QPoint sPnt) {
    samplePoint = sPnt;
}

void brushHandler::setRelativePoint(QPoint rPnt) {
    relativityPoint = rPnt;
}

int brushHandler::onScreen(int y, int x, int yMax, int xMax)  {
    return y >=0 && y < yMax && x >= 0 && x < xMax;
}

void brushHandler::shift() {
    if (currPnt.y() > lastPnt.y())
        shiftUp();
    else if (currPnt.y() < lastPnt.y())
        shiftDown();
    if (currPnt.x() > lastPnt.x())
        shiftLeft();
    else if (currPnt.x() < lastPnt.x())
        shiftRight();
}

void brushHandler::shiftLeft() {
    int rad = 2 * (brush.getRadius() + checkEdgeSize) + 1;
    for (int i = 1; i < rad; ++i)
        for (int j = 0; j < rad; ++j)
            checkMap[i - 1][j] = checkMap[i][j];
    for (int i = 0; i < rad; ++i)
        checkMap[rad - 1][i] = 0;
}

void brushHandler::shiftRight() {
    int rad = 2 * (brush.getRadius() + checkEdgeSize) + 1;
    for (int i = rad - 1; i > 0; --i)
        for (int j = 0; j < rad; ++j)
            checkMap[i][j] = checkMap[i - 1][j];
    for (int i = 0; i < rad; ++i)
        checkMap[0][i] = 0;
}

void brushHandler::shiftUp() {
    int rad = 2 * (brush.getRadius() + checkEdgeSize) + 1;
    for (int i = 0; i < rad; ++i)
        for (int j = 1; j < rad; ++j)
            checkMap[i][j - 1] = checkMap[i][j];
    for (int i = 0; i < rad; ++i)
        checkMap[i][rad - 1] = 0;
}

void brushHandler::shiftDown() {
    int rad = 2 * (brush.getRadius() + checkEdgeSize) + 1;
    for (int i = 0; i < rad; ++i)
        for (int j = rad - 1; j > 0; --j)
            checkMap[i][j] = checkMap[i][j - 1];
    for (int i = 0; i < rad; ++i)
        checkMap[i][0] = 0;
}

void brushHandler::overwrite(QImage *qi) {
    const unsigned char *const *const brushMap = brush.getBrushMap();
    int radius = brush.getRadius(), xMax = qi->width(), yMax = qi->height();
    QRgb qc = alpha == 0 ? 0x00FFFFFF : brushColor.rgba();
    while (toProcess.size() > 0) {
        QPoint p = toProcess.front();
        lastPnt = currPnt;
        currPnt = p;
        int x = 0;
        for (int i = currPnt.x() - radius; i <= currPnt.x() + radius; ++i) {
            int y = 0;
            for (int j = currPnt.y() - radius; j <= currPnt.y() + radius; ++j) {
                if (onScreen(i, j, xMax, yMax) && brushMap[x][y] && (!sprayDensity || !(rand() % sprayDensity)) && (!patternInUse || patternMap[i % patternXDim][j % patternYDim]))
                    qi->setPixel(i, j, qc);
                ++y;
            }
            ++x;
        }
        toProcess.pop_front();
    }
}

QColor brushHandler::getAffected() {
    int red = brushColor.red(), green = brushColor.green(), blue = brushColor.blue();
    red *= strength;
    red /= maxStrength;
    green *= strength;
    green /= maxStrength;
    blue *= strength;
    blue /= maxStrength;
    return QColor(red, green, blue, alpha);
}

void brushHandler::additive(QImage *qi) {
    const unsigned char *const *const brushMap = brush.getBrushMap();
    int radius = brush.getRadius(), xMax = qi->width(), yMax = qi->height();
    QColor affColor = getAffected();
    QColor setColor;
    setColor.setAlpha(alpha);
    int r = affColor.red(), g = affColor.green(), b = affColor.blue();
    while (toProcess.size() > 0) {
        QPoint p = toProcess.front();
        lastPnt = currPnt;
        currPnt = p;
        shift();
        int x = 0;
        for (int i = p.x() - radius; i <= p.x() + radius; ++i) {
            int y = 0;
            for (int j = p.y() - radius; j <= p.y() + radius; ++j) {
                if (onScreen(i, j, xMax, yMax) && brushMap[x][y] && (!sprayDensity || !(rand() % sprayDensity)) && (!patternInUse || patternMap[i % patternXDim][j % patternYDim])) {
                    if (!checkMap[x + checkEdgeSize][y + checkEdgeSize]) {
                        QColor qc = qi->pixelColor(i, j);
                        setColor.setRed((qc.red() + r) & 255);
                        setColor.setGreen((qc.green() + r) & 255);
                        setColor.setBlue((qc.blue() + r) & 255);
                        qi->setPixel(i, j, setColor.rgba());
                        checkMap[x + checkEdgeSize][y + checkEdgeSize] = 255;
                    }
                }
                ++y;
            }
            ++x;
        }
        toProcess.pop_front();
    }
}

void brushHandler::subractive(QImage *qi) {
    const unsigned char *const *const brushMap = brush.getBrushMap();
    int radius = brush.getRadius(), xMax = qi->width(), yMax = qi->height();
    QColor affColor = getAffected();
    QColor setColor;
    setColor.setAlpha(alpha);
    int r = affColor.red(), g = affColor.green(), b = affColor.blue();
    while (toProcess.size() > 0) {
        QPoint p = toProcess.front();
        lastPnt = currPnt;
        currPnt = p;
        shift();
        int x = 0;
        for (int i = currPnt.x() - radius; i <= currPnt.x() + radius; ++i) {
            int y = 0;
            for (int j = currPnt.y() - radius; j <= currPnt.y() + radius; ++j) {
                if (onScreen(i, j, xMax, yMax) && brushMap[x][y] && (!sprayDensity || !(rand() % sprayDensity)) && (!patternInUse || patternMap[i % patternXDim][j % patternYDim])) {
                    if (!checkMap[x + checkEdgeSize][y + checkEdgeSize]) {
                        QColor qc = qi->pixelColor(i, j);
                        setColor.setRed(max(0, qc.red() - r));
                        setColor.setGreen(max(0, qc.green() - g));
                        setColor.setBlue(max(0, qc.blue() - b));
                        qi->setPixel(i, j, setColor.rgba());
                        checkMap[x + checkEdgeSize][y + checkEdgeSize] = 255;
                    }
                }
                ++y;
            }
            ++x;
        }
        toProcess.pop_front();
    }
}

void brushHandler::filter(QImage *qi) {
    const unsigned char *const *const brushMap = brush.getBrushMap();
    int radius = brush.getRadius(), xMax = qi->width(), yMax = qi->height();
    while (toProcess.size() > 0) {
        QPoint p = toProcess.front();
        lastPnt = currPnt;
        currPnt = p;
        shift();
        int x = 0;
        for (int i = currPnt.x() - radius; i <= currPnt.x() + radius; ++i) {
            int y = 0;
            for (int j = currPnt.y() - radius; j <= currPnt.y() + radius; ++j) {
                if (onScreen(i, j, xMax, yMax) && brushMap[x][y] && (!sprayDensity || !(rand() % sprayDensity)) && (!patternInUse || patternMap[i % patternXDim][j % patternYDim])) {
                    if (!checkMap[x + checkEdgeSize][y + checkEdgeSize]) {
                        qi->setPixel(i, j, brushFilter.applyTo(qi->pixelColor(i, j)));
                        checkMap[x + checkEdgeSize][y + checkEdgeSize] = 255;
                    }
                }
                ++y;
            }
            ++x;
        }
        toProcess.pop_front();
    }
}

void brushHandler::radialUpdate(int size, vector<int> pts) {
    if (brush.getRadius() != size) {
        int fullSize = brush.getFullSize();
        for (int i = 0; i < fullSize; ++i)
            delete [] radialMap[i];
        delete [] radialMap;
        setSize(size);
        fullSize = brush.getFullSize();
        radialMap = new float * [fullSize];
        for (int i = 0; i < fullSize; ++i)
            radialMap[i] = new float [fullSize];
    }
    radialValues = pts;
    for (size_t i = 0; i < radialValues.size(); ++i)
        radialValues[i] = 255 - radialValues[i];
    int fullSize = brush.getFullSize(), radius = brush.getRadius();
    for (int i = 0; i < fullSize; ++i)
        for (int j = 0; j < fullSize; ++j) {
            int rad = static_cast<int>(sqrt(pow(i - radius, 2) + pow(j - radius, 2)));
            radialMap[i][j] = rad >= radius ? 0.0 : static_cast<float>(radialValues[rad]) / static_cast<float>(UCHAR_MAX);
            //radialMap[i][j] *= radialMap[i][j];
        }
}

void brushHandler::radial(QImage *qi) {
    const unsigned char *const *const brushMap = brush.getBrushMap();
    int radius = brush.getRadius(), xMax = qi->width(), yMax = qi->height();
    QRgb qc = brushColor.rgba();
    while (toProcess.size() > 0) {
        QPoint p = toProcess.front();
        lastPnt = currPnt;
        currPnt = p;
        shift();
        int x = 0;
        for (int i = currPnt.x() - radius; i <= currPnt.x() + radius; ++i) {
            int y = 0;
            int trueX = x - radius;
            for (int j = currPnt.y() - radius; j <= currPnt.y() + radius; ++j) {
                if (onScreen(i, j, xMax, yMax) && brushMap[x][y] && (!sprayDensity || !(rand() % sprayDensity)) && (!patternInUse || patternMap[i % patternXDim][j % patternYDim])) {
                    int trueY = y - radius;
                    int checkStr = sqrt(trueX * trueX + trueY * trueY) + 1;
                    if (checkMap[x + checkEdgeSize][y + checkEdgeSize] > checkStr || checkMap[x + checkEdgeSize][y + checkEdgeSize] == 0) {
                        QColor qcs(qc), pcs(qi->pixelColor(i, j));
                        float g = static_cast<float>(checkStr - 1) / static_cast <float>(radius);
                        float f = radialMap[x][y] * g;
                        qcs.setRedF(stdFuncs::clamp(qcs.redF() * f + pcs.redF() * (1.0 - f), 0.0, 1.0));
                        qcs.setGreenF(stdFuncs::clamp(qcs.greenF() * f + pcs.greenF() * (1.0 - f), 0.0, 1.0));
                        qcs.setBlueF(stdFuncs::clamp(qcs.blueF() * f + pcs.blueF() * (1.0 - f), 0.0, 1.0));
                        qi->setPixel(i, j, qcs.rgba());
                        checkMap[x + checkEdgeSize][y + checkEdgeSize] = static_cast<unsigned char>(checkStr);
                    }
                }
                ++y;
            }
            ++x;
        }
        toProcess.pop_front();
    }
}

void brushHandler::sample(QImage *qi) {
    const unsigned char *const *const brushMap = brush.getBrushMap();
    while (toProcess.size() > 0) {
        QPoint p = toProcess.front();
        lastPnt = currPnt;
        currPnt = p;
        int radius = brush.getRadius(), xMax = qi->width(), yMax = qi->height();
        int rx = currPnt.x() - relativityPoint.x(), ry = currPnt.y() - relativityPoint.y();
        int x = 0;
        for (int i = -radius; i <= radius; ++i) {
            int y = 0;
            for (int j = -radius; j <= radius; ++j) {
                if (onScreen(samplePoint.x() + i + rx, samplePoint.y() + j + ry, xMax, yMax) && onScreen(currPnt.x() + i, currPnt.y() + j, xMax, yMax) && brushMap[x][y] && (!sprayDensity || !(rand() % sprayDensity)) && (!patternInUse || patternMap[i % patternXDim][j % patternYDim]))
                    qi->setPixel(currPnt.x() + i, currPnt.y() + j, qi->pixel(samplePoint.x() + i+ rx, samplePoint.y() + j + ry));
                ++y;
            }
            ++x;
        }
        toProcess.pop_front();
    }
}

