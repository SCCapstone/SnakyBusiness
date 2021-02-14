#include "layer.h"


Layer::Layer(QSize qs) {
    activePt = -1;
    alpha = 255;
    qi = new QImage(qs, QImage::Format_ARGB32_Premultiplied);
    for (int i = 0; i < qs.width(); ++i)
        for (int j = 0; j < qs.height(); ++j)
            qi->setPixelColor(i, j, 0xFFFFFFFF);
    shiftFlag = false;
    ipolPts = ipolMin * 2;
}

Layer::~Layer() {
    delete qi;
}

QImage * Layer::getCanvas() {
    return qi;
}

vector <list <Triangle> > Layer::getTriangles() {
    return tris;
}

vector <SplineVector> Layer::getVectors() {
    return vects;
}

vector <unsigned char> Layer::getActiveVectors() {
    return activeVects;
}

float Layer::getipol(float a, float b, float ipol) {
    return a + static_cast<float>(b - a) * ipol;
}

void Layer::calcLine() {
    if (activeVects.empty())
        return;
    for (unsigned char activeVect : activeVects) {
        vector <QPointF> workPts;
        SplineVector sv = vects[activeVect];
        vector <QPoint> controlPts = sv.getControls();
        char numpts = static_cast<char>(controlPts.size()) - 1;
        for (char i = 0; i < numpts + 1; ++i)
            workPts.push_back(QPointF(controlPts[i]));
        list <pair <QPoint, QPoint> > pairs;
        for (float ipol = 0.0; ipol <= 1.0; ipol += ipolPts) {

            float twidth = static_cast<float>(sv.getWidth()) * sqrt(2.0 * abs(abs(ipol - 0.5) - 0.5));
            for (int max = numpts; max > 1; --max) {    // og  > 0
                for (char i = 0; i < max; ++i) {
                    workPts[i].setX(getipol(workPts[i].x(), workPts[i + 1].x(), ipol));
                    workPts[i].setY(getipol(workPts[i].y(), workPts[i + 1].y(), ipol));
                }
                workPts[max] = controlPts[max];
            }
            float dx = workPts[1].x() - workPts[0].x(), dy = workPts[1].y() - workPts[0].y();
            workPts[0].setX(getipol(workPts[0].x(), workPts[1].x(), ipol));
            workPts[0].setY(getipol(workPts[0].y(), workPts[1].y(), ipol));
            workPts[1] = controlPts[1];
            float slope = dx / dy;
            float sqrSlope = slope * slope;
            QPointF midPt = workPts[0];
            float dist = twidth; //static_cast<float>(sv.getWidth());
            float inverter = slope < 0.0 ? 1.0 : -1.0;
            int x1 = static_cast<int>(((inverter * dist) / sqrt(1 + sqrSlope)) + midPt.x());
            int y1 = static_cast<int>((dist / sqrt(1 + (1 / sqrSlope))) + midPt.y());
            int x2 = midPt.x() + (midPt.x() - x1);
            int y2 = midPt.y() + (midPt.y() - y1);
            pairs.push_back(pair <QPoint, QPoint> (QPoint(x1, y1), QPoint(x2, y2)));
            workPts[0] = controlPts[0];
        }
        tris[activeVect].clear();
        pair <QPoint, QPoint> first = pairs.front();
        pairs.pop_front();
        int x, y;
        while (pairs.size() > 0) {
            pair <QPoint, QPoint> second = pairs.front();
            pairs.pop_front();
            x = first.first.x() - second.second.x();
            y = first.first.y() - second.second.y();
            int dist1 = x * x + y * y;
            x = first.second.x() - second.first.x();
            y = first.second.y() - second.first.y();
            int dist2 = x * x + y * y;
            if (dist1 > dist2) {
                tris[activeVect].push_back(Triangle(first.first, second.second, first.second));
                tris[activeVect].push_back(Triangle(first.first, second.second, second.first));
            }
            else {
                tris[activeVect].push_back(Triangle(first.second, second.first, first.first));
                tris[activeVect].push_back(Triangle(first.second, second.first, second.second));
            }
            first = second;
        }
        if (!shiftFlag)
            vects[activeVect].calcBounds();
    }
}

void Layer::spinWheel(int dy) {
    for (unsigned char i : activeVects)
        vects[i].setWidth(vects[i].getWidth() + dy);
    calcLine();
}

void Layer::release(MouseButton button) {
    activePt = -1;
    if (shiftFlag)
        vects[activeVects[0]].cleanup();
    if (button >= 8)
        shiftFlag = false;
}

void Layer::moveLeft(QPoint qp) {
    if (activePt == -1) {
        if (shiftFlag) {
            for (unsigned char i : activeVects)
                vects[i].scale(qp);
        }
        else {
            for (unsigned char activeVect : activeVects) {
                int dx = qp.x() - deltaMove.x(), dy = qp.y() - deltaMove.y();
                vects[activeVect].translate(dx, dy);
            }
            deltaMove = qp;
        }
    }
    else if (activeVects.size() == 1){
        vects[activeVects[0]].movePt(qp, activePt);
    }
    calcLine();
}

void Layer::moveRight(QPoint qp) {
    for (unsigned char i : activeVects)
        vects[i].rotate(qp);
    calcLine();
}

void Layer::pressLeft(QPoint qp) {
    if (activeVects.size() != 0) {
        if (!shiftFlag) {
            vector <QPoint> controlPts = vects[activeVects[0]].getControls();
            for (size_t i = 0; i < controlPts.size(); ++i)
                if (abs(qp.x() - controlPts[i].x()) + abs(qp.y() - controlPts[i].y()) < ptSize + 2) {
                    activePt = i;
                    break;
                }
            if (activePt == -1)
                deltaMove = qp;
        }
        else {
            QPoint origin = qp;
            if (activeVects.size() == 1)
                vects[activeVects[0]].prescale(origin);
            else {
                int minX = INT_MAX, maxX = 0, minY = INT_MAX, maxY = 0;
                for (unsigned char activeVect : activeVects) {
                    pair <QPoint, QPoint> bounds = vects[activeVect].prescale(origin);
                    if (bounds.first.x() < minX)
                        minX = bounds.first.x();
                    if (bounds.first.y() < minY)
                        minY = bounds.first.y();
                    if (bounds.second.x() > maxX)
                        maxX = bounds.second.x();
                    if (bounds.second.y() > maxY)
                        maxY = bounds.second.y();
                }
                for (unsigned char i : activeVects)
                    vects[i].scaleSpec(minX, minY, maxX, maxY);
            }
        }
    }
}

MouseButton Layer::pressRight(QPoint qp) {
    MouseButton response = RightButton;
    if (activeVects.size() == 0) {
        if (vects.size() < CHAR_MAX - 1) {
            vects.push_back(SplineVector(qp, QPoint(qp.x() + 1, qp.y())));
            tris.push_back(list <Triangle> ());
            activeVects.push_back(vects.size() - 1);
            activePt = 1;
            response = LeftButton;
            calcLine();
        }
    }
    else if (shiftFlag) {
        if (activeVects.size() == 1)
            vects[activeVects[0]].prerotate(qp);
        else {
            int minX = INT_MAX, maxX = 0, minY = INT_MAX, maxY = 0;
            for (unsigned char activeVect : activeVects) {
                pair <QPoint, QPoint> bounds = vects[activeVect].prerotate(qp);
                if (bounds.first.x() < minX)
                    minX = bounds.first.x();
                if (bounds.first.y() < minY)
                    minY = bounds.first.y();
                if (bounds.second.x() > maxX)
                    maxX = bounds.second.x();
                if (bounds.second.y() > maxY)
                    maxY = bounds.second.y();
            }
            for (unsigned char i : activeVects)
                vects[i].rotateSpec(minX, minY, maxX, maxY);
        }
    }
    else if (activeVects.size() == 1) {
        vector <QPoint> controlPts = vects[activeVects[0]].getControls();
        bool flag = true;
        size_t index;
        for (size_t i = 0; i < controlPts.size(); ++i)
            if (abs(qp.x() - controlPts[i].x()) + abs(qp.y() - controlPts[i].y()) < ptSize + 2) {
                index = i;
                flag = false;
                break;
            }
        if (!flag) {
            if (controlPts.size() == 2) {
                unsigned char activeVect = activeVects[0];
                vects.erase(vects.begin() + activeVect);
                tris.erase(tris.begin() + activeVect);
                activeVects.pop_back();
            }
            else
                vects[activeVects[0]].removePt(index);
        }
        else if (flag) {
            size_t minDist1 = INT_MAX, minDist2 = INT_MAX, index1 = 1, index2 = 1, dist;
            for (size_t i = 0; i < controlPts.size(); ++i) {
                dist = abs(qp.x() - controlPts[i].x()) + abs(qp.y() - controlPts[i].y());
                if (dist < minDist1) {
                    minDist2 = minDist1;
                    minDist1 = dist;
                    index2 = index1;
                    index1 = i;
                }
            }
            index = index2 > index1 ? index1 + 1 : index1;
            if (index1 == 0)
                index = 1;
            vects[activeVects[0]].addPt(qp, index);
            activePt = index;
            response = LeftButton;
        }
        if (limitCnt > 1.0)
            limitCnt -= 1.0;
        calcLine();
    }
    return response;
}

void Layer::doubleClickLeft(QPoint qp, bool ctrlFlag) {
    if (activeVects.size() != 0 && !ctrlFlag)
        activeVects.clear();
    else {
        int dist = INT_MAX;
        char index = -1, size = static_cast<char>(vects.size());
        for (char i = 0; i < size; ++i) {
            vector <QPoint> pts = vects[i].getControls();
            int tdist = abs(qp.x() - pts[0].x()) + abs(qp.y() - pts[0].y());
            if (tdist < dist) {
                dist = tdist;
                index = i;
            }
            tdist = abs(qp.x() - pts[pts.size() - 1].x()) + abs(qp.y() - pts[pts.size() - 1].y());
            if (tdist < dist) {
                dist = tdist;
                index = i;
            }
        }
        bool flag = true;
        for (char c : activeVects)
            if (c == index) {
                flag = false;
                break;
            }
        if (flag)
            activeVects.push_back(index);
    }
}

void Layer::doubleClickRight(QPoint qp) {
    if (activeVects.size() != 0) {
        int dist = INT_MAX;
        size_t index = 0;
        for (size_t i = 0; i < activeVects.size(); ++i) {
            vector <QPoint> pts = vects[activeVects[i]].getControls();
            int tdist = abs(qp.x() - pts[0].x()) + abs(qp.y() - pts[0].y());
            if (tdist < dist) {
                dist = tdist;
                index = i;
            }
            tdist = abs(qp.x() - pts[pts.size() - 1].x()) + abs(qp.y() - pts[pts.size() - 1].y());
            if (tdist < dist) {
                dist = tdist;
                index = i;
            }
        }
        activeVects.erase(activeVects.begin() + index);
    }
}

void Layer::setShiftFlag(bool b) {
    shiftFlag = b;
}

void Layer::setAlpha(int a) {
    QImage alphaLayer (qi->size(), QImage::Format_Alpha8);
    alpha = a;
    alphaLayer.fill(static_cast<unsigned int>(alpha));
    qi->setAlphaChannel(alphaLayer);
}

int Layer::getAlpha() {
    return alpha;
}

int Layer::getWidth() {
    return activeVects.size() == 1 ? vects[activeVects[0]].getWidth() : -1;
}

void Layer::setWidth(int w) {
    if (activeVects.size() == 1) {
        unsigned char i = activeVects[0];
        vects[i].setWidth(vects[i].getWidth());
    }
}

void Layer::widthUp() {
    for (unsigned char i : activeVects)
        vects[i].setWidth(vects[i].getWidth() + 1);
}

void Layer::widthDown() {
    for (unsigned char i : activeVects)
        vects[i].setWidth(vects[i].getWidth() - 1);
}

void Layer::cleanUp() {
    for (unsigned char i : activeVects)
            vects[i].cleanup();
}

void Layer::deleteSelected() {
    if (activeVects.size() == 0)
        return;
    sort(activeVects.begin(), activeVects.end());
    for (int i = activeVects.size() - 1; i >= 0; --i) {
        unsigned char activeVect = activeVects[i];
        vects.erase(vects.begin() + activeVect);
        tris.erase(tris.begin() + activeVect);
    }
    activeVects.clear();
}

void Layer::deselect() {
    activeVects.clear();
}

