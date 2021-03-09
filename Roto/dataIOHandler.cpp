#include "dataIOHandler.h"


DataIOHandler::DataIOHandler() {
    activeFrame = 0;
    activeLayer = 0;
    Layer *l = new Layer(defaultSize);
    vector <Layer *> frame;
    frame.push_back(l);
    frames.push_back(frame);
    updated = true;
}

DataIOHandler::~DataIOHandler() {
    for (size_t i = 0; i < frames.size(); ++i) {
        for (size_t j = 0; j < frames[i].size(); ++j)
            delete frames[i][j];
        frames[i].clear();
    }
}

void DataIOHandler::compileFrame() {
    QImage *qi = new QImage(frames[activeFrame][0]->getCanvas()->size(), QImage::Format_ARGB32_Premultiplied);
    unsigned char temp = activeLayer;
    activeLayer = 0;
    renderFrame(qi, frames[temp]);
    frames[temp].insert(frames[temp].begin(), new Layer(qi->copy(), 255));
    for (size_t i = frames[temp].size(); i > 1; --i) {
        delete frames[temp][i];
        frames[temp].pop_back();
    }
    updated = true;
}

void DataIOHandler::compileLayer() {
    Layer *layer = getWorkingLayer();
    layer->deselect();
    renderLayer(layer->getCanvas(), layer->getAlpha(), layer->getFilter(), layer->getVectors(), layer->getTriangles());
    layer->clearVectors();
}

void DataIOHandler::renderFrame(QImage *ret, vector <Layer *> layers) {
    ret->fill(0xFFFFFFFF);
    vector <QImage *> imgs;
    for (size_t i = 0; i < layers.size(); ++i)
        imgs.push_back(new QImage(layers[i]->getCanvas()->copy()));
    vector <thread> imgThreads;
    for(size_t i = 0; i < layers.size(); ++i) {
        thread th = thread(renderLayer, imgs[i], layers[i]->getAlpha(), layers[i]->getFilter(), layers[i]->getVectors(), layers[i]->getTriangles());
        imgThreads.push_back(move(th));
    }
    for(auto &th : imgThreads)
        th.join();
    QPainter qp;
    qp.begin(ret);
    for (size_t i = 0; i < imgs.size(); ++i) {
        qp.drawImage(0, 0, *imgs[i]);
        delete imgs[i];
    }
    qp.end();
    // this assume everything will complete before leaving scope. probably don't need global var or mutex;
}

void DataIOHandler::renderLayer(QImage *toProcess, int alpha, Filter filter, vector<SplineVector> vects, vector<list<Triangle>> tris) {
    int w = toProcess->width(), h = toProcess->height();
    vector <list <Triangle> *> dTris;
    if (!tris.empty()) {
        for (size_t i = 0; i < vects.size(); ++i)
            dTris.push_back(new list <Triangle> (tris[i]));
    }
    else if (vects.size() != 0) {
        for (size_t i = 0; i < vects.size(); ++i)
            dTris.push_back( new list <Triangle> ());
        vector <thread> splineThreads;
        for(size_t i = 0; i < dTris.size(); ++i) {
            thread th = thread(calcLine, vects[i], dTris[i]);
            splineThreads.push_back(move(th));
        }
        for(auto &th : splineThreads)
            th.join();
    }
    QRgb color;
    for (size_t i = 0; i < dTris.size(); ++i) {
        Filter vf = vects[i].getFilter();
        pair <QRgb, QRgb> colors = vects[i].getColors();
        QColor ca = QColor (colors.first), cb = QColor(colors.second);
        ca.setAlpha(alpha);
        int width = vects[i].getWidth();
        pair <QPoint, QPoint> bounds = vects[i].getBounds();
        bool flag = bounds.first.x() > width && bounds.first.y() > width && bounds.second.x() < w - width && bounds.second.y() < h - width;
        if (vects[i].getMode() == ColorFill) {
            if (flag) { //normal draw
                if (colors.first == colors.second) {
                    color = ca.rgba();
                    for (Triangle &t : *dTris[i])
                        fillTri(toProcess, t, color);
                }
                else {
                    cb.setAlpha(alpha);
                    float ccomp = 1.0 / static_cast<float>((*dTris[i]).size());
                    float cnt = 0.0;
                    for (Triangle &t : *dTris[i]) {
                        float ccc = ccomp * cnt;
                        int r = static_cast<int>((ccc * static_cast<float>(ca.red())) + ((1.0 - ccc) * static_cast<float>(cb.red())));
                        int g = static_cast<int>((ccc * static_cast<float>(ca.green())) + ((1.0 - ccc) * static_cast<float>(cb.green())));
                        int b = static_cast<int>((ccc * static_cast<float>(ca.blue())) + ((1.0 - ccc) * static_cast<float>(cb.blue())));
                        color = QColor(r, g, b, alpha).rgba();
                        fillTri(toProcess, t, color);
                        cnt += 1.0;
                    }
                }
            }
            else {  // safe draw
                if (colors.first == colors.second) {
                    color = ca.rgba();
                    for (Triangle &t : *dTris[i]) {
                        unsigned char flag = 0;
                        if (t.A().x() < 0 || t.A().x() >= w || t.A().y() < 0 || t.A().y() >= h)
                            ++flag;
                        if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                            ++flag;
                        if (flag > 0)
                            fillTriSafe(toProcess, t, color);
                        if (t.C().x() < 0 || t.C().x() >= w || t.C().y() < 0 || t.C().y() >= h)
                            ++flag;
                        if (flag == 0)
                            fillTri(toProcess, t, color);
                        else if (flag != 3)
                            fillTriSafe(toProcess, t, color);
                    }
                }
                else {
                    cb.setAlpha(alpha);
                    float ccomp = 1.0 / static_cast<float>((*dTris[i]).size());
                    float cnt = 0.0;
                    for (Triangle &t : *dTris[i]) {
                        unsigned char flag = 0;
                        if (t.A().x() < 0 || t.A().x() >= w || t.A().y() < 0 || t.A().y() >= h)
                            ++flag;
                        if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                            ++flag;
                        if (flag > 0)
                            fillTriSafe(toProcess, t, color);
                        if (t.C().x() < 0 || t.C().x() >= w || t.C().y() < 0 || t.C().y() >= h)
                            ++flag;
                        if (flag == 3) {
                            cnt += 1.0;
                            continue;
                        }
                        float ccc = ccomp * cnt;
                        int r = static_cast<int>((ccc * static_cast<float>(ca.red())) + ((1.0 - ccc) * static_cast<float>(cb.red())));
                        int g = static_cast<int>((ccc * static_cast<float>(ca.green())) + ((1.0 - ccc) * static_cast<float>(cb.green())));
                        int b = static_cast<int>((ccc * static_cast<float>(ca.blue())) + ((1.0 - ccc) * static_cast<float>(cb.blue())));
                        color = QColor(r, g, b, alpha).rgba();
                        if (flag == 0)
                            fillTri(toProcess, t, color);
                        else
                            fillTriSafe(toProcess, t, color);
                        cnt += 1.0;
                    }
                }
            }
        }
        else {
            if (flag) //normal draw
                for (Triangle &t : *dTris[i])
                    filterTri(toProcess, t, vf);
            else  // safe draw
                for (Triangle &t : *dTris[i]) {
                    unsigned char flag = 0;
                    if (t.A().x() < 0 || t.A().x() >= w || t.A().y() < 0 || t.A().y() >= h)
                        ++flag;
                    if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                        ++flag;
                    if (flag > 0)
                        filterTriSafe(toProcess, t, vf);
                    if (t.C().x() < 0 || t.C().x() >= w || t.C().y() < 0 || t.C().y() >= h)
                        ++flag;
                    if (flag == 3)
                        continue;
                    if (flag == 0)
                        filterTri(toProcess, t, vf);
                    else
                        filterTriSafe(toProcess, t, vf);
                }
        }
    }
    for (size_t i = 0; i < vects.size(); ++i)
        delete dTris[i];
    filter.applyTo(toProcess);
}

void DataIOHandler::calcLine(SplineVector sv, list<Triangle> *tris) {
    vector <QPointF> workPts;
    vector <QPoint> controlPts = sv.getControls();
    char numpts = static_cast<char>(controlPts.size()) - 1;
    for (char i = 0; i < numpts + 1; ++i)
        workPts.push_back(QPointF(controlPts[i]));
    list <pair <QPoint, QPoint> > pairs;
    pair <unsigned char, unsigned char> taper = sv.getTaper();
    float taper1 = taper.first == 0 ? 0.0 : 1.0 / static_cast<float>((9 - (taper.first - 1)) + 1), taper2 = taper.second == 0 ? 0.0 : 1.0 / static_cast<float>((9 - (taper.second - 1)) + 1);
    for (float ipol = 0.0; ipol <= 1.0; ipol += ipolMin) {
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
                workPts[i].setX(Layer::getipol(workPts[i].x(), workPts[i + 1].x(), ipol));
                workPts[i].setY(Layer::getipol(workPts[i].y(), workPts[i + 1].y(), ipol));
            }
            workPts[max] = controlPts[max];
        }
        float dx = workPts[1].x() - workPts[0].x(), dy = workPts[1].y() - workPts[0].y();
        workPts[0].setX(Layer::getipol(workPts[0].x(), workPts[1].x(), ipol));
        workPts[0].setY(Layer::getipol(workPts[0].y(), workPts[1].y(), ipol));
        workPts[1] = controlPts[1];
        float slope = dx / dy;
        float sqrSlope = slope * slope;
        QPointF midPt = workPts[0];
        float dist = twidth;
        float inverter = slope < 0.0 ? 1.0 : -1.0;
        int x1 = static_cast<int>(((inverter * dist) / sqrt(1 + sqrSlope)) + midPt.x());
        int y1 = static_cast<int>((dist / sqrt(1 + (1 / sqrSlope))) + midPt.y());
        int x2 = midPt.x() + (midPt.x() - x1);
        int y2 = midPt.y() + (midPt.y() - y1);
        pairs.push_back(pair <QPoint, QPoint> (QPoint(x1, y1), QPoint(x2, y2)));
        workPts[0] = controlPts[0];
    }
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
            tris->push_back(Triangle(first.first, second.second, first.second));
            tris->push_back(Triangle(first.first, second.second, second.first));
        }
        else {
            tris->push_back(Triangle(first.second, second.first, first.first));
            tris->push_back(Triangle(first.second, second.first, second.second));
        }
        first = second;
    }
}

void DataIOHandler::fillTri(QImage *toProcess, Triangle t, QRgb color) {
    QPoint a = t.A(), b = t.B(), c = t.C();
    if (a.y() > b.y()) {
        QPoint tmp = a;
        a = b;
        b = tmp;
    }
    if (b.y() > c.y()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
        if (a.y() > b.y()) {
            tmp = a;
            a = b;
            b = tmp;
        }
    }
    if (b.y() == a.y())
        fillBTri(toProcess, c, b, a, color);
    else if (b.y() == c.y())
        fillTTri(toProcess, a, b, c, color);
    else {
        QPoint d (a.x() + static_cast<float>(c.x() - a.x()) * (static_cast<float>(b.y() - a.y()) / static_cast<float>(c.y() - a.y())) , b.y());
        fillBTri(toProcess, c, b, d, color);
        fillTTri(toProcess, a, b, d, color);
    }
}

void DataIOHandler::fillTriSafe(QImage *toProcess, Triangle t, QRgb color) {
    QPoint a = t.A(), b = t.B(), c = t.C();
    if (a.y() > b.y()) {
        QPoint tmp = a;
        a = b;
        b = tmp;
    }
    if (b.y() > c.y()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
        if (a.y() > b.y()) {
            tmp = a;
            a = b;
            b = tmp;
        }
    }
    if (b.y() == a.y())
        fillBTriSafe(toProcess, c, b, a, color);
    else if (b.y() == c.y())
        fillTTriSafe(toProcess, a, b, c, color);
    else {
        QPoint d (a.x() + static_cast<float>(c.x() - a.x()) * (static_cast<float>(b.y() - a.y()) / static_cast<float>(c.y() - a.y())) , b.y());
        fillBTriSafe(toProcess, c, b, d, color);
        fillTTriSafe(toProcess, a, b, d, color);
    }
}

void DataIOHandler::fillBTri(QImage *toProcess, QPoint a, QPoint b, QPoint c, QRgb color) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = ipolMin * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = ipolMin * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    for (float y = a.y(); y >= b.y(); y -= ipolMin)
    {
        for (int x = static_cast<int>(curx1); x <= static_cast<int>(curx2); ++x)
            toProcess->setPixel(x, static_cast<int>(y), color);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void DataIOHandler::fillBTriSafe(QImage *toProcess, QPoint a, QPoint b, QPoint c, QRgb color) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = ipolMin * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = ipolMin * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    float endY = b.y() < 0 ? 0 : b.y();
    for (float y = a.y() >= toProcess->height() ? toProcess->height() - 1 : a.y(); y >= endY; y -= ipolMin)  {
        int startX = curx1 < 0.0 ? 0 : static_cast<int>(curx1), endX = static_cast<int>(curx2) >= toProcess->width() ? toProcess->width() - 1 : static_cast<int>(curx2);
        for (int x = startX; x <= endX; ++x)
            toProcess->setPixel(x, static_cast<int>(y), color);
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void DataIOHandler::fillTTri(QImage *toProcess, QPoint a, QPoint b, QPoint c, QRgb color) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = ipolMin * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = ipolMin * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    for (float y = a.y(); y <= b.y(); y += ipolMin)
    {
        for (int x = static_cast<int>(curx1); x <= static_cast<int>(curx2); ++x)
            toProcess->setPixel(x, static_cast<int>(y), color);//Filtering::greyscale((qi.pixel(x, static_cast<int>(y))), 255));
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void DataIOHandler::fillTTriSafe(QImage *toProcess, QPoint a, QPoint b, QPoint c, QRgb color) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = ipolMin * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = ipolMin * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    float endY = b.y() >= toProcess->height() ? toProcess->height() - 1 : b.y();
    for (float y = a.y() < 0 ? 0 : a.y(); y <= endY; y += ipolMin) {
        int startX = curx1 < 0.0 ? 0 : static_cast<int>(curx1), endX = static_cast<int>(curx2) >= toProcess->width() ? toProcess->width() - 1 : static_cast<int>(curx2);
        for (int x = startX; x <= endX; ++x)
            toProcess->setPixel(x, static_cast<int>(y), color); //qi->setPixel(x, y, 0xFF00FF00);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void DataIOHandler::filterTri(QImage *toProcess, Triangle t, Filter f) {
    QPoint a = t.A(), b = t.B(), c = t.C();
    if (a.y() > b.y()) {
        QPoint tmp = a;
        a = b;
        b = tmp;
    }
    if (b.y() > c.y()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
        if (a.y() > b.y()) {
            tmp = a;
            a = b;
            b = tmp;
        }
    }
    if (b.y() == a.y())
        filterBTri(toProcess, c, b, a, f);
    else if (b.y() == c.y())
        filterTTri(toProcess, a, b, c, f);
    else {
        QPoint d (a.x() + static_cast<float>(c.x() - a.x()) * (static_cast<float>(b.y() - a.y()) / static_cast<float>(c.y() - a.y())) , b.y());
        filterBTri(toProcess, c, b, d, f);
        filterTTri(toProcess, a, b, d, f);
    }
}

void DataIOHandler::filterTriSafe(QImage *toProcess, Triangle t, Filter f) {
    QPoint a = t.A(), b = t.B(), c = t.C();
    if (a.y() > b.y()) {
        QPoint tmp = a;
        a = b;
        b = tmp;
    }
    if (b.y() > c.y()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
        if (a.y() > b.y()) {
            tmp = a;
            a = b;
            b = tmp;
        }
    }
    if (b.y() == a.y())
        filterBTriSafe(toProcess, c, b, a, f);
    else if (b.y() == c.y())
        filterTTriSafe(toProcess, a, b, c, f);
    else {
        QPoint d (a.x() + static_cast<float>(c.x() - a.x()) * (static_cast<float>(b.y() - a.y()) / static_cast<float>(c.y() - a.y())) , b.y());
        filterBTriSafe(toProcess, c, b, d, f);
        filterTTriSafe(toProcess, a, b, d, f);
    }
}

void DataIOHandler::filterBTri(QImage *toProcess, QPoint a, QPoint b, QPoint c, Filter f) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = ipolMin * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = ipolMin * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    for (float y = a.y(); y >= b.y(); y -= ipolMin)
    {
        for (int x = static_cast<int>(curx1); x <= static_cast<int>(curx2); ++x) {
            int Y = static_cast<int>(y);
            toProcess->setPixel(x, Y, f.applyTo(toProcess->pixelColor(x, Y)));
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void DataIOHandler::filterBTriSafe(QImage *toProcess, QPoint a, QPoint b, QPoint c, Filter f) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = ipolMin * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = ipolMin * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    float endY = b.y() < 0 ? 0 : b.y();
    for (float y = a.y() >= toProcess->height() ? toProcess->height() - 1 : a.y(); y >= endY; y -= ipolMin)  {
        int startX = curx1 < 0.0 ? 0 : static_cast<int>(curx1), endX = static_cast<int>(curx2) >= toProcess->width() ? toProcess->width() - 1 : static_cast<int>(curx2);
        for (int x = startX; x <= endX; ++x) {
            int Y = static_cast<int>(y);
            toProcess->setPixel(x, Y, f.applyTo(toProcess->pixelColor(x, Y)));
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void DataIOHandler::filterTTri(QImage *toProcess, QPoint a, QPoint b, QPoint c, Filter f) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = ipolMin * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = ipolMin * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    for (float y = a.y(); y <= b.y(); y += ipolMin)
    {
        for (int x = static_cast<int>(curx1); x <= static_cast<int>(curx2); ++x) {
            int Y = static_cast<int>(y);
            toProcess->setPixel(x, Y, f.applyTo(toProcess->pixelColor(x, Y)));
        }
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void DataIOHandler::filterTTriSafe(QImage *toProcess, QPoint a, QPoint b, QPoint c, Filter f) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = ipolMin * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = ipolMin * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    float endY = b.y() >= toProcess->height() ? toProcess->height() - 1 : b.y();
    for (float y = a.y() < 0 ? 0 : a.y(); y <= endY; y += ipolMin) {
        int startX = curx1 < 0.0 ? 0 : static_cast<int>(curx1), endX = static_cast<int>(curx2) >= toProcess->width() ? toProcess->width() - 1 : static_cast<int>(curx2);
        for (int x = startX; x <= endX; ++x) {
            int Y = static_cast<int>(y);
            toProcess->setPixel(x, Y, f.applyTo(toProcess->pixelColor(x, Y)));
        }
        curx1 += invslope1;
        curx2 += invslope2;
    }
}


void DataIOHandler::setActiveLayer(int i, EditMode mode) {
    activeLayer = static_cast<unsigned char>(i);
    getWorkingLayer()->setMode(mode);
    updated = true;
}

int DataIOHandler::getNumLayers() {
    return frames[activeFrame].size();
}

int DataIOHandler::getActiveLayer() {
    return activeLayer;
}

bool DataIOHandler::wasUpdated() {
    bool b = updated;
    updated = false;
    return b;
}

Layer * DataIOHandler::getWorkingLayer() {
    if (frames.empty())
        return nullptr;
    return frames[activeFrame][activeLayer];
}

void DataIOHandler::addLayer() {
    frames[activeFrame].push_back(new Layer(dims));
    activeLayer = frames[activeFrame].size() - 1;
    updated = true;
}

void DataIOHandler::copyLayer() {
    layerCopySlot = Layer(*frames[activeFrame][activeLayer]);
    updated = true;
}

void DataIOHandler::pasteLayer() {
    if (layerCopySlot.getCanvas()->isNull())
        return;
    activeLayer = frames[activeFrame].size();
    frames[activeFrame].push_back(new Layer(layerCopySlot));
    updated = true;
}

void DataIOHandler::deleteLayer() {
    delete frames[activeFrame][activeLayer];
    frames[activeFrame].erase((frames[activeFrame].begin() + activeFrame));
    if (activeLayer == frames[activeFrame].size()) {
        if (frames[activeFrame].size() == 0)
            deleteFrame();
        else {
            --activeLayer;
            updated = true;
        }
    }
}

void DataIOHandler::moveBackward() {
    if (activeLayer != 0) {
        swap(frames[activeFrame][activeLayer - 1], frames[activeFrame][activeLayer]);
        --activeLayer;
    }
    updated = true;
}

void DataIOHandler::moveForward() {
    if (activeLayer != frames[activeFrame].size() - 1) {
        swap(frames[activeFrame][activeLayer], frames[activeFrame][activeLayer + 1]);
        ++activeLayer;
    }
    updated = true;
}

void DataIOHandler::moveToFront() {
    while (activeLayer != frames[activeFrame].size() - 1) {
        swap(frames[activeFrame][activeLayer], frames[activeFrame][activeLayer + 1]);
        ++activeLayer;
    }
    updated = true;
}

void DataIOHandler::moveToBack() {
    while (activeLayer != 0) {
        swap(frames[activeFrame][activeLayer - 1], frames[activeFrame][activeLayer]);
        --activeLayer;
    }
    updated = true;
}

void DataIOHandler::deleteFrame() {
    // set new active layer to the new frame's number of layers - 1
    //check if last / only frame
    updated = true;
}

QImage DataIOHandler::getBackground() {
    if (activeLayer == 0)
        return QImage();
    vector <Layer *> layers = frames[activeFrame];
    QImage qi = *layers[0]->getCanvas();
    QPainter p;
    p.begin(&qi);
    for (size_t i = 0; i < activeLayer; ++i)
        p.drawImage(0, 0, *layers[i]->getCanvas());
    p.end();
    return qi;
}

QImage DataIOHandler::getForeground() {
    vector <Layer *> layers = frames[activeFrame];
    if (activeLayer == layers.size() - 1)
        return QImage();
    QImage qi = *layers[activeLayer + 1]->getCanvas();
    QPixmap qpix;
    QPainter p;
    p.begin(&qi);
    for (size_t i = activeLayer + 2; i < layers.size(); ++i)
        p.drawImage(0, 0, *layers[i]->getCanvas());
    p.end();
    return qi;
}

void DataIOHandler::copyVectors() {
    vectorCopySlot.clear();
    Layer *layer = getWorkingLayer();
    vector <unsigned char> activeVectors = layer->getActiveVectors();
    vector <SplineVector> vects = layer->getVectors();
    for (unsigned char i : activeVectors)
        vectorCopySlot.push_back(vects[i]);
}

void DataIOHandler::cutVectors() {
    copyVectors();
    deleteVectors();
}

void DataIOHandler::deleteVectors() {
    getWorkingLayer()->deleteSelected();
}

void DataIOHandler::pasteVectors() {
    getWorkingLayer()->pasteVectors(vectorCopySlot);
}

void DataIOHandler::copyRaster() {
    cout << "here" << endl;
    Layer *layer = getWorkingLayer();
    rasterCopySlot = layer->getRaster();
    angleCopySlot = layer->getAngle();
    boundCopySlot = layer->getBounds();
}

void DataIOHandler::cutRaster() {
    copyRaster();
    deleteRaster();
}

void DataIOHandler::pasteRaster() {
    Layer *layer = getWorkingLayer();
    layer->deselect();
    layer->pasteRaster(rasterCopySlot, angleCopySlot, boundCopySlot);
}

void DataIOHandler::deleteRaster() {
    getWorkingLayer()->deleteSelected();
}

void DataIOHandler::setScreenFilter(string filterName) {
    screenFilter.setFilter(filterName);
    applyFilter();
}

void DataIOHandler::setFilterStrength(int strength) {
    screenFilter.setStrength(strength);
}

int DataIOHandler::getFilterIndex() {
    return screenFilter.getFilterIndex();
}

int DataIOHandler::getFilterStrength() {
    return screenFilter.getStrength();
}

void DataIOHandler::applyFilter() {

}

bool DataIOHandler::importImage(QString fileName) {
    importImg = QImage(fileName).convertToFormat(QImage::Format_ARGB32_Premultiplied);
    importType = image;
    bool match = importImg.width() == defaultSize.width() && importImg.height() == defaultSize.height();
    if (match)
        scale(dontScale);
    return !match;
}


void DataIOHandler::exportImage(QString fileName) {
    QImage *out = new QImage(defaultSize, QImage::Format_ARGB32_Premultiplied);
    renderFrame(out, frames[activeFrame]);
    out->save(fileName);
}

void DataIOHandler::scale(scaleType type) {
    QImage toLayer(defaultSize, QImage::Format_ARGB32_Premultiplied);
    toLayer.fill(0x00000000);
    QPainter qp;
    qp.begin(&toLayer);
    switch (type) {
    case dontScale:
        qp.drawImage(0, 0, importImg);
        break;
    case bestFit:
        if (toLayer.width() > toLayer.height())
            qp.drawImage(0, 0, importImg.scaledToHeight(toLayer.height()));
        else
            qp.drawImage(0, 0, importImg.scaledToWidth(toLayer.width()));
        break;
    case aspectRatio:
        qp.drawImage(0, 0, importImg.scaled(toLayer.width(), toLayer.height()));
        break;
    case toWidth:
        qp.drawImage(0,0, importImg.scaledToWidth(toLayer.width()));
        break;
    case toHeight:
        qp.drawImage(0, 0, importImg.scaledToHeight(toLayer.height()));
        break;
    }
    qp.end();
    importImg = toLayer;
    if (importType == image) {
        frames[activeFrame].push_back(new Layer(importImg, 255));
        updated = true;
        activeLayer = frames[activeFrame].size() - 1;
    }
}

bool DataIOHandler::importVideo(QString fileName) {
    file = fileName;
    return false;
}

void DataIOHandler::exportVideo(QString fileName) {

}

