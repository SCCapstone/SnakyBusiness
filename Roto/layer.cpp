#include "layer.h"


Layer::Layer() {
    activePt = -1;
    alpha = 0;
    qi = new QImage();
    shiftFlag = false;
    ipolPts = ipolMin;
    mode = Brush_Mode;
    selection = NoSelect;
    postAngle = 0.0;
    selectOgActive = false;
    deltaMove =  boundPt1 = boundPt2 = rotateAnchor = QPoint(-1000, -1000);
}

Layer::Layer(QSize qs) {
    activePt = -1;
    alpha = 255;
    qi = new QImage(qs, QImage::Format_ARGB32);
    qi->fill(0x00000000);
    shiftFlag = false;
    ipolPts = ipolMin;
    mode = Brush_Mode;
    selection = NoSelect;
    postAngle = 0.0;
    selectOgActive = false;
    deltaMove =  boundPt1 = boundPt2 = rotateAnchor = QPoint(-1000, -1000);
}

Layer::Layer(QImage in, int alphaValue) {
    activePt = -1;
    alpha = alphaValue;
    // need to fill alpha?
    qi = new QImage(in);
    shiftFlag = false;
    ipolPts = ipolMin;
    mode = Brush_Mode;
    selection = NoSelect;
    postAngle = 0.0;
    selectOgActive = false;
    deltaMove =  boundPt1 = boundPt2 = rotateAnchor = QPoint(-1000, -1000);
}

Layer::Layer(const Layer &layer) {
    *this = layer;
}

Layer& Layer::operator=(const Layer &layer) {
    mode = Brush_Mode;
    selection = NoSelect;
    vects = layer.vects;
    tris = layer.tris;
    //active vects don't get copied.
    activePt = -1;
    qi = new QImage(layer.qi->copy());
    // raster select og doesn't get copied
    ipolPts = layer.ipolPts;
    limiter = layer.limiter;
    limitCnt = layer.limitCnt;
    postAngle = 0.0;
    alpha = layer.alpha;
    shiftFlag = false;
    selectOgActive = false;
    selecting = false;
    deltaMove =  boundPt1 = boundPt2 = rotateAnchor = QPoint(-1000, -1000);
    filter = layer.filter;
    return *this;
}

void Layer::pasteVectors(list<SplineVector> svs) {
    deselect();
    unsigned char i = vects.size();
    vects.insert(vects.end(), svs.begin(), svs.end());
    while (tris.size() < vects.size())
        tris.push_back(list <Triangle> ());
    for (size_t j = i; j < vects.size(); ++j)
        vects[j].cleanup();
    while (i < vects.size())
        activeVects.push_back(i++);
    calcLine();
}

void Layer::pasteRaster(QImage rasterIn, double angleIn, pair<QPoint, QPoint> bounds) {
    boundPt1 = bounds.first;
    boundPt2 = bounds.second;
    rasterselectOg = rasterIn;
    postAngle = angleIn;
    selectOgActive = true;
    deltaMove = rotateAnchor = boundPt1;
}

QImage Layer::getRaster() {
    return rasterselectOg;
}

double Layer::getAngle() {
    return postAngle;
}

pair <QPoint, QPoint> Layer::getBounds() {
    return pair <QPoint, QPoint> (boundPt1, boundPt2);
}

Layer::~Layer() {
    delete qi;
}

QImage * Layer::getCanvas() {
    return qi;
}

QImage Layer::getRenderCanvas() {
    QImage img = qi->copy();
    if (selectOgActive)
        drawRasterSelection(&img);
    return img.convertToFormat(QImage::Format_ARGB32);
}

vector <QPoint> Layer::getRasterEdges() {
    vector <QPoint> pts;
    if (mode == Raster_Mode && (selectOgActive || selecting)) {
        pts.push_back(boundPt1);
        pts.push_back(selecting ? deltaMove : boundPt2);
        pts.push_back(QPoint(boundPt1.x(), pts[1].y()));
        pts.push_back(QPoint(pts[1].x(), boundPt1.y()));
        pts.push_back(QPoint(boundPt1.x(), (boundPt1.y() + pts[1].y()) / 2));
        pts.push_back(QPoint(pts[1].x(), (boundPt1.y() + pts[1].y()) / 2));
        pts.push_back(QPoint((boundPt1.x() + pts[1].x()) / 2, boundPt1.y()));
        pts.push_back(QPoint((boundPt1.x() + pts[1].x()) / 2, pts[1].y()));
    }
    return pts;
}

void Layer::flipVert() {
    graphics::ImgSupport::flipVertical(&rasterselectOg);
}

void Layer::flipHori() {
    graphics::ImgSupport::flipHorizontal(&rasterselectOg);
}

void Layer::fillColor(QPoint qp, QColor qc) {
    int w = qi->width(), h = qi->height();
    list <QPoint> toProcess;
    toProcess.push_back(qp);
    QRgb og = qi->pixel(qp.x(), qp.y());
    QRgb nu = qc.rgba();
    if (og == nu)
        return;
    while (!toProcess.empty()) {
        QPoint pt = toProcess.front();
        toProcess.pop_front();
        if (pt.x() >= 0 && pt.x() < w && pt.y() >= 0 && pt.y() < h && qi->pixel(pt.x(), pt.y()) == og) {
            qi->setPixel(pt.x(), pt.y(), nu);
            toProcess.push_back(QPoint(pt.x() - 1, pt.y()));
            toProcess.push_back(QPoint(pt.x() + 1, pt.y()));
            toProcess.push_back(QPoint(pt.x(), pt.y() - 1));
            toProcess.push_back(QPoint(pt.x(), pt.y() + 1));
        }
    }
}

void Layer::patternFill(QPoint qp, QColor qc) {
    int w = qi->width(), h = qi->height();
    list <QPoint> toProcess;
    toProcess.push_back(qp);
    QRgb og = qi->pixel(qp.x(), qp.y());
    QRgb nu = qc.rgba();
    int xpsize = 20, ypsize = 20;
    unsigned char **arr = new unsigned char *[xpsize];
    for (int i = 0; i < xpsize; ++i) {
        arr[i] = new unsigned char[ypsize];
        for (int j = 0; j < ypsize; ++j)
            arr[i][j] = (j > i) ? 1 : 0;
    }
    unsigned char **check = new unsigned char *[qi->width()];
    for (int i = 0; i < qi->width(); ++i) {
        check[i] = new unsigned char[qi->height()];
        for (int j = 0; j < qi->height(); ++j)
            check[i][j] = 1;
    }
    if (og == nu)
        return;
    while (!toProcess.empty()) {
        QPoint pt = toProcess.front();
        toProcess.pop_front();
        if (pt.x() >= 0 && pt.x() < w && pt.y() >= 0 && pt.y() < h && qi->pixel(pt.x(), pt.y()) == og && check[pt.x()][pt.y()]) {
            check[pt.x()][pt.y()] = 0;
            if (arr[pt.x() % xpsize][pt.y() % ypsize])
                qi->setPixel(pt.x(), pt.y(), nu);
            toProcess.push_front(QPoint(pt.x() - 1, pt.y()));
            toProcess.push_front(QPoint(pt.x() + 1, pt.y()));
            toProcess.push_front(QPoint(pt.x(), pt.y() - 1));
            toProcess.push_front(QPoint(pt.x(), pt.y() + 1));
        }
    }
    for (int i = 0; i < xpsize; ++i)
        delete [] arr[i];
    delete [] arr;
    for (int i = 0; i < qi->width(); ++i)
        delete [] check[i];
    delete [] check;
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

void Layer::setVectorTaperType(Taper t) {
    if (activeVects.size() != 1)
        return;
    vects[activeVects[0]].setTaperType(t);
    calcLine();
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
        pair <unsigned char, unsigned char> taper = sv.getTaper();
        float taper1 = taper.first == 0 ? 0.0 : 1.0 / static_cast<float>((9 - (taper.first - 1)) + 1), taper2 = taper.second == 0 ? 0.0 : 1.0 / static_cast<float>((9 - (taper.second - 1)) + 1);
        for (float ipol = 0.0; ipol <= 1.0; ipol += ipolPts) {
            float twidth = static_cast<float>(sv.getWidth());
            if (taper.first != 0 || taper.second != 0) {
                if (sv.getTaperType() == Single)
                    twidth *= pow(ipol, taper1);
                else {
                    float f = 2.0 * abs(abs(ipol - 0.5) - 0.5);
                    twidth *= pow(f, ipol <= 0.5 ? taper1 : taper2);
                }
            }
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
    }
}

void Layer::spinWheel(int dy) {
    if (mode == Spline_Mode) {
        for (unsigned char i : activeVects)
            vects[i].setWidth(vects[i].getWidth() + dy);
        calcLine();
    }
    else if (mode == Raster_Mode && selectOgActive)
        postAngle += dy < 0 ? -0.01 : 0.01;
}

void Layer::release(QPoint qp, MouseButton button) {
    if (mode == Spline_Mode) {
        activePt = -1;
        if (shiftFlag)
            for (unsigned char i : activeVects)
                vects[i].cleanup();
        if (button >= 8)
            shiftFlag = false;
    }
    else if(mode == Raster_Mode) {
        if (!selectOgActive) {
            if (rasterselectOg.isNull() && boundPt1 != qp) {
                selecting = false;
                boundPt2 = qp;
                boundPt2.setX(min(boundPt2.x(), qi->width() - 1));
                boundPt2.setY(min(boundPt2.y(), qi->height() - 1));
                boundPt2.setX(max(boundPt2.x(), 0));
                boundPt2.setY(max(boundPt2.y(), 0));
                QPoint minPt(min(boundPt1.x(), boundPt2.x()), min(boundPt1.y(), boundPt2.y()));
                QPoint maxPt(max(boundPt1.x(), boundPt2.x()), max(boundPt1.y(), boundPt2.y()));
                boundPt1 = minPt;
                boundPt2 = maxPt;
                rasterselectOg = QImage((1 + boundPt2.x()) - boundPt1.x(), (1 + boundPt2.y()) - boundPt1.y(), QImage::Format_ARGB32);
                if (boundPt1.x() < 0 || boundPt1.y() < 0)
                    return;
                for (int i = boundPt1.x(); i <= boundPt2.x(); ++i)
                    for (int j = boundPt1.y(); j <= boundPt2.y(); ++j) {
                        rasterselectOg.setPixel(i - boundPt1.x(), j - boundPt1.y(), qi->pixel(i, j));
                        qi->setPixel(i, j, 0x00000000);
                    }
                selectOgActive = true;
            }
            else
                deselect();
        }
        else {
            if (shiftFlag) {
                int ox = (boundPt1.x() + boundPt2.x()) / 2, oy = (boundPt1.y() + boundPt2.y()) / 2;
                postAngle += atan2(rotateAnchor.y() - oy, rotateAnchor.x() - ox) - atan2(deltaMove.y() - oy, deltaMove.x() - ox);
                deltaMove = rotateAnchor = qp;
            }
            else {
                selection = NoSelect;
            }
       }
   }
}

void Layer::moveLeft(QPoint qp) {
    if (mode == Spline_Mode) {
        if (activePt == -1) {
            if (shiftFlag)
                for (unsigned char i : activeVects)
                    vects[i].scale(qp);
            else {
                for (unsigned char activeVect : activeVects) {
                    int dx = qp.x() - deltaMove.x(), dy = qp.y() - deltaMove.y();
                    vects[activeVect].translate(dx, dy);
                }
                deltaMove = qp;
            }
        }
        else if (activeVects.size() == 1)
            vects[activeVects[0]].movePt(qp, activePt);
        calcLine();
    }
    else if (mode == Raster_Mode) {
        if (shiftFlag) {


            // SKEW   =======================================================================================================================================


        }
        else {
            if (selection == BodySelect) {
                int dx = qp.x() - deltaMove.x();
                int dy = qp.y() - deltaMove.y();
                boundPt1.setX(boundPt1.x() + dx);
                boundPt1.setY(boundPt1.y() + dy);
                boundPt2.setX(boundPt2.x() + dx);
                boundPt2.setY(boundPt2.y() + dy);
            }
            else if (selection == Bottom)
                boundPt2.setY(qp.y() < boundPt1.y() ? boundPt1.y() + 1 : qp.y());
            else if (selection == Top)
                boundPt1.setY(qp.y() > boundPt2.y() ? boundPt2.y() - 1 : qp.y());
            else if (selection == Left)
                boundPt1.setX(qp.x() > boundPt2.x() ? boundPt2.x() - 1 : qp.x());
            else if (selection == Right)
                boundPt2.setX(qp.x() < boundPt1.x() ? boundPt1.x() + 1 : qp.x());
            else if (selection == BottomLeft) {
                boundPt2.setY(qp.y() < boundPt1.y() ? boundPt1.y() + 1 : qp.y());
                boundPt1.setX(qp.x() > boundPt2.x() ? boundPt2.x() - 1 : qp.x());
            }
            else if (selection == BottomRight) {
                boundPt2.setY(qp.y() < boundPt1.y() ? boundPt1.y() + 1 : qp.y());
                boundPt2.setX(qp.x() < boundPt1.x() ? boundPt1.x() + 1 : qp.x());
            }
            else if (selection == TopLeft) {
                boundPt1.setY(qp.y() > boundPt2.y() ? boundPt2.y() - 1 : qp.y());
                boundPt1.setX(qp.x() > boundPt2.x() ? boundPt2.x() - 1 : qp.x());
            }
            else if (selection == TopRight) {
                boundPt1.setY(qp.y() > boundPt2.y() ? boundPt2.y() - 1 : qp.y());
                boundPt2.setX(qp.x() < boundPt1.x() ? boundPt1.x() + 1 : qp.x());
            }
            deltaMove = rotateAnchor = qp;
        }
    }
}

void Layer::moveRight(QPoint qp) {
    if (mode == Spline_Mode) {
        for (unsigned char i : activeVects)
            vects[i].rotate(qp);
        calcLine();
    }
    else if (mode == Raster_Mode)
        deltaMove = qp;
}

void Layer::pressLeft(QPoint qp) {
    if (mode == Spline_Mode) {
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
    else if (mode == Raster_Mode) {
        if (!selectOgActive) {
            deselect();
            boundPt1 = qp;
            postAngle = 0.0;
            deltaMove = qp;
            selecting = true;
        }
        else {
            bool isTopElseBottom = abs(qp.y() - boundPt1.y()) < abs(qp.y() - boundPt2.y()) ? true : false;
            bool isLeftElseRight = abs(qp.x() - boundPt1.x()) < abs(qp.x() - boundPt2.x()) ? true : false;
            bool validVert = isTopElseBottom ? abs(qp.y() - boundPt1.y()) <= 5 : abs(qp.y() - boundPt2.y()) <= 5;
            bool validHori = isLeftElseRight ? abs(qp.x() - boundPt1.x()) <= 5 : abs(qp.x() - boundPt2.x()) <= 5;
            int ox = (boundPt1.x() + boundPt2.x()) / 2, oy = (boundPt1.y() + boundPt2.y()) / 2;
            postAngle += atan2(rotateAnchor.y() - oy, rotateAnchor.x() - ox) - atan2(deltaMove.y() - oy, deltaMove.x() - ox);
            deltaMove = qp;
            rotateAnchor = qp;
            if (validHori || validVert) {
                if (isTopElseBottom) {
                    if (isLeftElseRight) {
                        if (validHori && validVert && !shiftFlag)
                            selection = TopLeft;
                        else if (validHori)
                            selection = Left;
                        else if (validVert)
                            selection = Top;
                    }
                    else {
                        if (validHori && validVert && !shiftFlag)
                            selection = TopRight;
                        else if (validHori)
                            selection = Right;
                        else if (validVert)
                            selection = Top;
                    }
                }
                else {
                    if (isLeftElseRight) {
                        if (validHori && validVert && !shiftFlag)
                            selection = BottomLeft;
                        else if (validHori)
                            selection = Left;
                        else if (validVert)
                            selection = Bottom;
                    }
                    else {
                        if (validHori && validVert && !shiftFlag)
                            selection = BottomRight;
                        else if (validHori)
                            selection = Right;
                        else if (validVert)
                            selection = Bottom;
                    }
                }
            }
            else if (qp.x() > boundPt1.x() && qp.x() < boundPt2.x() && qp.y() > boundPt1.y() && qp.y() < boundPt2.y())
                selection = BodySelect;
            else {
                deselect();
                boundPt1 = qp;
                postAngle = 0.0;
                deltaMove = qp;
                selecting = true;
            }
        }
    }
}

MouseButton Layer::pressRight(QPoint qp) {
    MouseButton response = RightButton;
    if (mode == Spline_Mode) {
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
                size_t minDist1 = INT_MAX, dist;
                index = 1;
                for (size_t i = 0; i < controlPts.size(); ++i) {
                    dist = stdFuncs::sqrDist(qp, controlPts[i]);
                    if (dist < minDist1) {
                        minDist1 = dist;
                        index = i;
                    }
                }
                if (index == 0)
                    index = 1;
                if (index != 0 && index != controlPts.size() - 1 && stdFuncs::sqrDist(qp, controlPts[index - 1]) > stdFuncs::sqrDist(qp, controlPts[index + 1]) && stdFuncs::sqrDist(qp, controlPts[0]) > stdFuncs::sqrDist(qp, controlPts[controlPts.size() - 1]))
                        index = index + 1;
                vects[activeVects[0]].addPt(qp, index);
                activePt = index;
                response = LeftButton;
            }
            calcLine();
        }
    }
    else if (mode == Raster_Mode && shiftFlag) {
        deltaMove = qp;
        rotateAnchor = qp;
    }
    return response;
}

void Layer::doubleClickLeft(QPoint qp, bool ctrlFlag) {
    if (mode == Spline_Mode) {
        if (activeVects.size() != 0) {
            if (!ctrlFlag)
                deselect();
            else {
                int dist = INT_MAX;
                char index = -1, size = static_cast<char>(vects.size());
                for (char i = 0; i < size; ++i) {
                    vector <QPoint> pts = vects[i].getControls();
                    int tdist = stdFuncs::sqrDist(qp, pts[0]);
                    if (tdist < dist) {
                        dist = tdist;
                        index = i;
                    }
                    tdist = stdFuncs::sqrDist(qp, pts[pts.size() - 1]);
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
        else if (vects.size() != 0) {
            int dist = INT_MAX;
            char index = -1, size = static_cast<char>(vects.size());
            for (char i = 0; i < size; ++i) {
                vector <QPoint> pts = vects[i].getControls();
                int tdist = stdFuncs::sqrDist(qp, pts[0]);
                if (tdist < dist) {
                    dist = tdist;
                    index = i;
                }
                tdist = stdFuncs::sqrDist(qp, pts[pts.size() - 1]);
                if (tdist < dist) {
                    dist = tdist;
                    index = i;
                }
            }
            activeVects.push_back(index);
        }
    }
}

void Layer::doubleClickRight(QPoint qp) {
    if (mode == Spline_Mode && activeVects.size() != 0) {
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

void Layer::setMode(EditMode m) {
    mode = m;
}

bool Layer::isShiftActive() {
    return shiftFlag;
}

void Layer::setShiftFlag(bool b) {
    shiftFlag = b;
}

void Layer::setAlpha(int a) {
    alpha = a;
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
        vects[i].setWidth(w);
    }
    calcLine();
}

void Layer::widthUp() {
    for (unsigned char i : activeVects)
        vects[i].setWidth(vects[i].getWidth() + 1);
}

void Layer::widthDown() {
    for (unsigned char i : activeVects)
        vects[i].setWidth(vects[i].getWidth() - 1);
}

void Layer::setVectorColor1(QRgb a) {
    if (activeVects.size() != 1)
        return;
    vects[activeVects[0]].setColor1(a);
    calcLine();
}

void Layer::setVectorColor2(QRgb b) {
    if (activeVects.size() != 1)
        return;
    vects[activeVects[0]].setColor2(b);
    calcLine();
}

pair <QRgb, QRgb> Layer::getVectorColors() {
    if (activeVects.size() == 1)
        return vects[activeVects[0]].getColors();
    return pair <QRgb, QRgb> (0x00000000, 0x00000000);
}

void Layer::setVectorTaper1(int a) {
    if (activeVects.size() != 1)
        return;
    vects[activeVects[0]].setTaper1(a);
    calcLine();
}

void Layer::setVectorTaper2(int b) {
    if (activeVects.size() != 1)
        return;
    vects[activeVects[0]].setTaper2(b);
    calcLine();
}

void Layer::setVectorFilter(string s) {
    vects[activeVects[0]].setFilter(s);
}

void Layer::setVectorMode(VectorMode vm) {
    if (activeVects.size() == 1)
        vects[activeVects[0]].setMode(vm);
}

void Layer::swapColors() {
    if (activeVects.size() == 1)
        vects[activeVects[0]].swapColors();
}

void Layer::swapTapers() {
    if (activeVects.size() == 1)
        vects[activeVects[0]].swapTapers();
    calcLine();
}

pair <char, char> Layer::getVectorTapers() {
    if (activeVects.size() != 1)
        return pair <char, char> (11, 11);
    return vects[activeVects[0]].getTaper();
}

unsigned char Layer::getVectorTaperType() {
    if (activeVects.size() != 1)
        return 0;
    return vects[activeVects[0]].getTaperType();
}

int Layer::getVectorFilterStrength() {
    if (activeVects.size() != 1)
        return -1;
    return vects[activeVects[0]].getFilter().getStrength();
}

void Layer::setVectorFilterStrength(int str) {
    if (activeVects.size() != 1)
        return;
    return vects[activeVects[0]].setFilterStrength(str);
}

void Layer::cleanUp() {
    for (unsigned char i : activeVects)
            vects[i].cleanup();
}

void Layer::deleteSelected() {
    if (mode == Spline_Mode && activeVects.size() != 0) {
        sort(activeVects.begin(), activeVects.end());
        for (int i = activeVects.size() - 1; i >= 0; --i) {
            unsigned char activeVect = activeVects[i];
            vects.erase(vects.begin() + activeVect);
            tris.erase(tris.begin() + activeVect);
        }
    }
    else {

        rasterselectOg = QImage();
    }
    deselect();
}

void Layer::drawRasterSelection(QImage *img) {
    if (rasterselectOg.isNull())
        return;
    QImage rasterEdit = rasterselectOg.scaled(1 + boundPt2.x() - boundPt1.x(), 1 + boundPt2.y() - boundPt1.y());
    int ox = (boundPt1.x() + boundPt2.x()) / 2, oy = (boundPt1.y() + boundPt2.y()) / 2;
    float angle = atan2(rotateAnchor.y() - oy, rotateAnchor.x() - ox) - atan2(deltaMove.y() - oy, deltaMove.x() - ox);
    angle += postAngle;
    int midX1 = rasterEdit.width() / 2, midY1 = rasterEdit.height() / 2;
    QImage rasterOverlay(static_cast<int>(1.0 + abs(static_cast<float>(rasterEdit.width()) * cos(angle)) + abs(static_cast<float>(rasterEdit.height()) * sin(angle))), static_cast<int>(1.0 + abs(static_cast<float>(rasterEdit.width()) * sin(angle)) + abs(static_cast<float>(rasterEdit.height()) * cos(angle))), QImage::Format_ARGB32);
    rasterOverlay.fill(0x00000000);
    int midX2 = (rasterOverlay.width() - 1) / 2, midY2 = (rasterOverlay.height() - 1) / 2;
    for (int i = -midX2; i <= midX2; ++i)
        for (int j = -midY2; j <= midY2; ++j) {
            QPoint pt;
            pt.setX(cos(-angle) * i + sin(-angle) * j);
            pt.setY(-sin(-angle) * i + cos(-angle) * j);
            if (i + midX2 >= 0 && i + midX2 < rasterOverlay.width() && j + midY2 >= 0 && j + midY2 < rasterOverlay.height() && pt.x() + midX1 >= 0 && pt.x() + midX1 < rasterEdit.width() && pt.y() + midY1 >= 0 && pt.y() + midY1 < rasterEdit.height())
                rasterOverlay.setPixel(i + midX2, j + midY2, rasterEdit.pixel(pt.x() + midX1, pt.y() + midY1));
        }
    QPainter qp;
    qp.begin(img);
    qp.drawImage(boundPt1.x() + (rasterEdit.width() - rasterOverlay.width()) / 2, boundPt1.y() + (rasterEdit.height() - rasterOverlay.height()) / 2, rasterOverlay);
    qp.end();
}

void Layer::selectAll() {
    if (mode == Spline_Mode) {
        for (size_t i = 0; i < vects.size(); ++i)
            activeVects.push_back(static_cast<unsigned char>(i));
    }
    else if (mode == Raster_Mode) {
        deselect();
        boundPt1 = QPoint(0, 0);
        boundPt2 = QPoint(qi->width() - 1, qi->height() - 1);
        rasterselectOg = qi->copy();
        qi->fill(0x00000000);
        deltaMove = rotateAnchor = boundPt1;
        selectOgActive = true;
    }
}

void Layer::deselect() {
    if (mode == Spline_Mode) {
        for (unsigned char i : activeVects)
            vects[i].cleanup();
        activeVects.clear();
        activePt = -1;
    }
    else if (mode == Raster_Mode) {
        drawRasterSelection(qi);
        rasterselectOg = QImage();
        selection = NoSelect;
        selectOgActive = false;
        postAngle = 0.0;
        selecting = selectOgActive = false;
    }
    deltaMove =  boundPt1 = boundPt2 = rotateAnchor = QPoint(-1000, -1000);
}

void Layer::clearVectors() {
    vects.clear();
    tris.clear();
    activeVects.clear();
}

Filter Layer::getFilter() {
    return filter;
}

int Layer::getFilterStrength() {
    return filter.getStrength();
}

void Layer::setFilterStrength(int str) {
    filter.setStrength(str);
}

void Layer::setFilter(string filterName) {
    filter.setFilter(filterName);
}

bool Layer::isRotating() {
    return selectOgActive;
}

void Layer::applyFilterToRaster(Filter f) {
    if (!rasterselectOg.isNull())
        f.applyTo(&rasterselectOg);
}

void Layer::applyKernalToSelection(QProgressDialog *qpd, string fileName) {
    if (!rasterselectOg.isNull()) {
        KernalData kernalInfo = graphics::ImgSupport::loadKernal(fileName);
        graphics::Filtering::applyKernal(qpd, &rasterselectOg, kernalInfo);
    }
}
