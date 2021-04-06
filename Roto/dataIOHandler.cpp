#include "dataIOHandler.h"


DataIOHandler::DataIOHandler(QProgressDialog *qpd) {
    progress = qpd;
    activeFrame = activeLayer = 0;
    vector <Layer *> frame;
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

void DataIOHandler::setDims(QSize size) {
    dims = size;
}

QSize DataIOHandler::getdims() {
    return dims;
}

void DataIOHandler::compileFrame() {
    QImage *qi = new QImage(frames[activeFrame][0]->getCanvas()->size(), QImage::Format_ARGB32_Premultiplied);
    unsigned char temp = activeLayer;
    activeLayer = 0;
    renderFrame(progress, qi, frames[temp]);
    frames[temp].insert(frames[temp].begin(), new Layer(qi->copy(), 255));
    for (size_t i = frames[temp].size(); i > 1; --i) {
        delete frames[temp][i];
        frames[temp].pop_back();
    }
    delete qi;
    updated = true;
}

void DataIOHandler::compileLayer() {
    Layer *layer = getWorkingLayer();
    layer->deselect();
    renderLayer(nullptr, progress, layer->getCanvas(), layer->getAlpha(), layer->getFilter(), layer->getVectors(), layer->getTriangles());
    layer->clearVectors();
}

void DataIOHandler::renderFrame(QProgressDialog *fqpd, QImage *ret, vector <Layer *> layers) {
    progressMarker = 0;
    fqpd->setValue(0);
    fqpd->setMaximum(layers.size() + 2);
    fqpd->show();
    QCoreApplication::processEvents();
    ret->fill(0xFFFFFFFF);
    vector <QImage *> imgs;
    for (size_t i = 0; i < layers.size(); ++i)
        imgs.push_back(new QImage(layers[i]->getCanvas()->copy()));
    vector <thread> imgThreads;
    for(size_t i = 0; i < layers.size(); ++i) {
        thread th = thread(renderLayer, fqpd, nullptr, imgs[i], layers[i]->getAlpha(), layers[i]->getFilter(), layers[i]->getVectors(), layers[i]->getTriangles());
        imgThreads.push_back(move(th));
    }
    ++progressMarker;
    fqpd->setValue(progressMarker);
    QCoreApplication::processEvents();
    for(auto &th : imgThreads)
        th.join();
    QPainter qp;
    qp.begin(ret);
    for (size_t i = 0; i < imgs.size(); ++i) {
        qp.drawImage(0, 0, *imgs[i]);
        delete imgs[i];
    }
    qp.end();
    ++progressMarker;
    fqpd->setValue(progressMarker);
    fqpd->close();
    fqpd->hide();
}

void DataIOHandler::renderLayer(QProgressDialog *fqpd, QProgressDialog *qpd, QImage *toProcess, int alpha, Filter filter, vector<SplineVector> vects, vector<list<Triangle>> tris) {
    if (qpd != nullptr) {
        qpd->setValue(0);
        qpd->setMaximum(1 + vects.size() + (tris.size() != vects.size() ? 1 : 0));
        qpd->show();
        QCoreApplication::processEvents();
    }
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
        if (qpd != nullptr) {
            qpd->setValue(qpd->value() + 1);
            QCoreApplication::processEvents();
        }
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
                        if (t.A().x() < 0 || t.A().x() >= w || t.A().y() < 0 || t.A().y() >= h)
                            fillTriSafe(toProcess, t, color);
                        else if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                            fillTriSafe(toProcess, t, color);
                        else if (t.C().x() < 0 || t.C().x() >= w || t.C().y() < 0 || t.C().y() >= h)
                            fillTriSafe(toProcess, t, color);
                        else
                            fillTri(toProcess, t, color);
                    }
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
                        if (t.A().x() < 0 || t.A().x() >= w || t.A().y() < 0 || t.A().y() >= h)
                            fillTriSafe(toProcess, t, color);
                        else if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                            fillTriSafe(toProcess, t, color);
                        else if (t.C().x() < 0 || t.C().x() >= w || t.C().y() < 0 || t.C().y() >= h)
                            fillTriSafe(toProcess, t, color);
                        else
                            fillTri(toProcess, t, color);
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
                        filterTriSafe(toProcess, t, vf);
                    else if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                        filterTriSafe(toProcess, t, vf);
                    else if (t.C().x() < 0 || t.C().x() >= w || t.C().y() < 0 || t.C().y() >= h)
                        filterTriSafe(toProcess, t, vf);
                    else
                        filterTri(toProcess, t, vf);
                }
        }
        if (qpd != nullptr) {
            qpd->setValue(qpd->value() + 1);
            QCoreApplication::processEvents();
        }
    }
    for (size_t i = 0; i < vects.size(); ++i)
        delete dTris[i];
    if (qpd != nullptr) {
        qpd->setValue(qpd->value() + 1);
        QCoreApplication::processEvents();
    }
    filter.applyTo(toProcess);
    if (qpd != nullptr) {
        qpd->close();
        qpd->hide();
    }
    if (fqpd != nullptr) {
        ++progressMarker;
        locker.lock();
        fqpd->setValue(progressMarker);
        QCoreApplication::processEvents();
        locker.unlock();
    }
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
    if (frames.empty() || frames[activeFrame].empty())
        return nullptr;
    return frames[activeFrame][activeLayer];
}

void DataIOHandler::addLayer() {
    frames[activeFrame].push_back(new Layer(dims));
    updated = true;
    activeLayer = frames[activeFrame].size() - 1;
}

void DataIOHandler::copyLayer() {
    layerCopySlot = Layer(*frames[activeFrame][activeLayer]);
}

void DataIOHandler::pasteLayer() {
    if (layerCopySlot.getCanvas()->isNull())
        return;
    frames[activeFrame].push_back(new Layer(layerCopySlot));
    updated = true;
    activeLayer = frames[activeFrame].size() - 1;
}

void DataIOHandler::pasteLayer(quint32 alpha) {
    if (layerCopySlot.getCanvas()->isNull())
        return;
    activeLayer = frames[activeFrame].size();
    Layer *l = new Layer(layerCopySlot);
    l->setAlpha(alpha);
    frames[activeFrame].push_back(new Layer(*l));
    updated = true;
}

void DataIOHandler::deleteLayer() {
    unsigned char temp = activeLayer;
    if (activeLayer == frames[activeFrame].size() - 1)
        --activeLayer;
    delete frames[activeFrame][temp];
    frames[activeFrame].erase((frames[activeFrame].begin() + temp));
//    if (temp == 0)
//        deleteFrame();
    if (activeFrame > 0) {
        //delete frame
        --activeFrame;
    }
    updated = true;
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

QImage DataIOHandler::getBackground() {
    if (activeLayer == 0)
        return QImage();
    progress->setMaximum(frames[activeFrame].size());
    progress->show();
    QCoreApplication::processEvents();
    vector <Layer *> layers = frames[activeFrame];
    QImage qi = layers[0]->getCanvas()->copy();
    renderLayer(nullptr, nullptr, &qi, layers[0]->getAlpha(), layers[0]->getFilter(), layers[0]->getVectors(), layers[0]->getTriangles());
    progress->setValue(0);
    QCoreApplication::processEvents();
    QPainter p;
    p.begin(&qi);
    for (size_t i = 0; i < activeLayer; ++i) {
        p.drawImage(0, 0, *layers[i]->getCanvas());
        renderLayer(nullptr, nullptr, &qi, layers[i]->getAlpha(), layers[i]->getFilter(), layers[i]->getVectors(), layers[i]->getTriangles());
        progress->setValue(i + 1);
        QCoreApplication::processEvents();
    }
    p.end();
    if (progress->value() == static_cast<int>(frames[activeFrame].size() - 1)) {
        progress->close();
        progress->hide();
    }
    return qi;
}

QImage DataIOHandler::getForeground() {
    vector <Layer *> layers = frames[activeFrame];
    if (frames[activeFrame].size() == 0 || activeLayer == layers.size() - 1)
        return QImage();
    QImage qi = layers[activeLayer + 1]->getCanvas()->copy();
    renderLayer(nullptr, nullptr, &qi, layers[activeLayer + 1]->getAlpha(), layers[activeLayer + 1]->getFilter(), layers[activeLayer + 1]->getVectors(), layers[activeLayer + 1]->getTriangles());
    progress->setValue(progress->value() + 2);
    QCoreApplication::processEvents();
    QPainter p;
    p.begin(&qi);
    for (size_t i = activeLayer + 2; i < layers.size(); ++i) {
        p.drawImage(0, 0, *layers[i]->getCanvas());
        renderLayer(nullptr, nullptr, &qi, layers[i]->getAlpha(), layers[i]->getFilter(), layers[i]->getVectors(), layers[i]->getTriangles());
        progress->setValue(i + 1);
        QCoreApplication::processEvents();
    }
    p.end();
    progress->close();
    progress->hide();
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

bool DataIOHandler::importImage(QString fileName) {
    importImg = QImage(fileName).convertToFormat(QImage::Format_ARGB32_Premultiplied);
    importType = image;
    bool match = importImg.width() == dims.width() && importImg.height() == dims.height();
    if (match)
        scale(dontScale);
    return !match;
}


void DataIOHandler::exportImage(QString fileName) {
    QImage *out = new QImage(dims, QImage::Format_ARGB32_Premultiplied);
    renderFrame(progress, out, frames[activeFrame]);
    out->save(fileName);
}

void DataIOHandler::scale(scaleType type) {
    QImage toLayer(dims, QImage::Format_ARGB32_Premultiplied);
    QImage toDraw;
    toLayer.fill(0x00000000);
    switch (type) {
    case dontScale:
        toDraw = importImg;
        break;
    case bestFit:
        if (importImg.scaledToHeight(toLayer.height()).width() > toLayer.width())
            toDraw = importImg.scaledToWidth(toLayer.width());
        else
            toDraw = importImg.scaledToHeight(toLayer.height());
        break;
    case aspectRatio:
        toDraw = importImg.scaled(toLayer.width(), toLayer.height());
        break;
    case toWidth:
        toDraw = importImg.scaledToWidth(toLayer.width());
        break;
    case toHeight:
        toDraw = importImg.scaledToHeight(toLayer.height());
        break;
    }
    QPainter qp;
    qp.begin(&toLayer);
    qp.drawImage(0, 0, toDraw);
    qp.end();
    importImg = toLayer;
    if (importType == image) {
        frames[activeFrame].push_back(new Layer(importImg, 255));
        updated = true;
        activeLayer = frames[activeFrame].size() - 1;
    }
}


void DataIOHandler::save(QString projectName) {
    saveBackup(projectName);
}

void DataIOHandler::load(QString projectName) {

}

void DataIOHandler::saveBackup(QString projectName) {
    base85 encoder;
    string backupName;
    if (QFile::exists(projectName)) {
        backupName = projectName.toStdString();
        backupName = backupName.substr(0, backupName.find_last_of(".glass")) + "__backup.glass";
        QFile::rename(projectName, QString(backupName.c_str()));
    }
    ofstream out(projectName.toStdString());
    if (out.is_open()) {
        out << encoder.toBase85(dims.width(), 2) << encoder.toBase85(dims.height(), 2);
        // push the total number of frames
        //out << encoder.toBase85(1, 1);
        out << endl;
        for (unsigned int layer = 0; layer < frames[activeFrame].size(); ++layer) {
            out << "{" << encoder.toBase85(layer) << endl;
            Layer *l = frames[activeFrame][layer];
            out << encoder.toBase85(l->getAlpha(), 2) << encoder.toBase85(l->getFilter().getFilterIndex(), 1) << encoder.toBase85(l->getFilter().getStrength(), 2) << endl;
            QImage *qi = l->getCanvas();
            for (short i = 0; i < qi->height(); ++i) {
                for (short j = 0; j < qi->width(); ++j)
                    out << encoder.toBase85(qi->pixel(j, i));
                out << endl;
            }
            vector <SplineVector> svs = l->getVectors();
            for (SplineVector sv : svs) {
                out << encoder.toBase85(sv.getWidth(), 1) << encoder.toBase85(VectorMode(sv.getMode()), 1) << encoder.toBase85(sv.getTaper().first, 1) << encoder.toBase85(sv.getTaper().second, 1) << encoder.toBase85(Taper(sv.getTaperType()), 1) << encoder.toBase85(sv.getColors().first) << encoder.toBase85(sv.getColors().second) << encoder.toBase85(sv.getFilter().getFilterIndex(), 1) << encoder.toBase85(sv.getFilter().getStrength(), 2);
                for (QPoint qp : sv.getControls())
                    out << encoder.toBase85(qp.x(), 2) << encoder.toBase85(qp.y(), 2);
                out << endl;
            }
        }
        out.close();
        cout << out.is_open() << endl;
        if (QFile::exists(backupName.c_str()))
            QFile::remove(backupName.c_str());
    }
}

void DataIOHandler::loadBackup(QString projectName) {
    base85 decoder;
    string inStr;
    ifstream in(projectName.toStdString());
    if (in.is_open()) {
        if (frames[activeFrame].size() != 0) {
            activeLayer = frames[activeFrame].size() - 1;
            while (frames[activeFrame].size() > 0) {
                delete frames[activeFrame][activeLayer];
                frames[activeFrame].pop_back();
                --activeLayer;
            }
        }
        getline(in, inStr);
        dims.setWidth(decoder.fromBase85(inStr.substr(0, 2)));
        dims.setHeight(decoder.fromBase85(inStr.substr(2, 4)));
        getline(in, inStr);
        while (getline(in, inStr)) {
            Layer *layer = new Layer(dims);
            frames[activeFrame].push_back(layer);
            layer->setAlpha(decoder.fromBase85(inStr.substr(0, 2)));
            layer->setFilter(graphics::filterNames[decoder.fromBase85(inStr.substr(2, 1))]);
            layer->setFilterStrength(decoder.fromBase85(inStr.substr(3, 2)));
            QImage *qi = layer->getCanvas();
            for (int i = 0; i < qi->height(); ++i) {
                getline(in, inStr);
                for (int j = 0; j < qi->width(); ++j)
                    qi->setPixel(j, i, decoder.fromBase85(inStr.substr(j * 5, 5)));
            }
            if (!getline(in, inStr))
                 break;
            list <SplineVector> svs;
            while (inStr[0] != '{') {
                SplineVector sv(QPoint(0, 0), QPoint(1, 1), decoder.fromBase85(inStr.substr(0, 1)));
                sv.setMode(VectorMode(decoder.fromBase85(inStr.substr(1, 1))));
                sv.setTaper1(decoder.fromBase85(inStr.substr(2, 1)));
                sv.setTaper2(decoder.fromBase85(inStr.substr(3, 1)));
                sv.setTaperType(Taper(decoder.fromBase85(inStr.substr(4, 1))));
                sv.setColor1(decoder.fromBase85(inStr.substr(5, 5)));
                sv.setColor2(decoder.fromBase85(inStr.substr(10, 5)));
                sv.setFilter(graphics::filterNames[decoder.fromBase85(inStr.substr(15, 1))]);
                sv.setFilterStrength(decoder.fromBase85(inStr.substr(16, 2)));
                sv.movePt(QPoint(decoder.fromBase85(inStr.substr(18, 2)), decoder.fromBase85(inStr.substr(20, 2))), 0);
                int offset = 1;
                for (int i = 18; i + 8 < static_cast<int>(inStr.length() - 1); i += 4) {
                    sv.addPt(QPoint(decoder.fromBase85(inStr.substr(18 + 4 * offset, 2)), decoder.fromBase85(inStr.substr(20 + 4 * offset, 2))), offset);
                    ++offset;
                }
                sv.movePt(QPoint(decoder.fromBase85(inStr.substr(18 + 4 * offset, 2)), decoder.fromBase85(inStr.substr(20 + 4 * offset, 2))), offset);
                svs.push_back(sv);
                if (!getline(in, inStr))
                    break;
            }
            layer->setMode(Spline_Mode);
            layer->pasteVectors(svs);
            layer->deselect();
            layer->setMode(Brush_Mode);
        }
        in.close();
        updated = true;
    }
}

bool DataIOHandler::importVideo(QString fileName) {
    //file = fileName;
    return false;
}

void DataIOHandler::exportVideo(QString fileName) {

}

