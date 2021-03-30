#include <screenrender.h>


screenRender::screenRender(DataIOHandler *dioh, QWidget *parent) : QWidget(parent) {
    brushLoc = QPoint(0,0);
    flashFlag = false;
    flasher = new QTimer(this);
    connect(flasher, SIGNAL(timeout()), this, SLOT(toggleFlasher()));
    fgVisible = true;
    adder = 0.1;
    samplePoint = QPoint(-1000, -1000);
    ioh = dioh;
    flasher->start(flashSpeed);
    filter.setFilter("Greyscale");
}

screenRender::~screenRender() {
    flasher->stop();
    delete flasher;
}

double screenRender::getZoom() {
    return screenZoom.getZoom();
}

void screenRender::setZoom(double Zoom){
    screenZoom.setZoom(Zoom);
    doZoom();
}

void screenRender::zoomIn() {
    screenZoom.zoomIn();
    doZoom();
}

void screenRender::zoomOut() {
    screenZoom.zoomOut();
    doZoom();
}

QPoint screenRender::getZoomCorrected(QPoint qp) {
    return screenZoom.getZoomCorrected(qp);
}

void screenRender::doZoom() {
    if (!fgPrescaled.isNull())
        fgLayers.convertFromImage(screenZoom.zoomImg(fgPrescaled));
    else
        fgLayers = QPixmap();
    if (!bgPrescaled.isNull())
        bgLayers.convertFromImage(screenZoom.zoomImg(bgPrescaled));
    else
        bgLayers = QPixmap();
    repaint();
}

void screenRender::showPts() {
    flashFlag = true;
}

void screenRender::toggleFlasher() {
    flashFlag = !flashFlag;
    repaint();
}

void screenRender::updateViews() {
    flasher->stop();
    workLayer = ioh->getWorkingLayer();
    bgPrescaled = ioh->getBackground();
    fgPrescaled = ioh->getForeground();
    doZoom();
    flasher->start(flashSpeed);
}

void screenRender::paintEvent(QPaintEvent *event) {
    if (ioh->wasUpdated())
        updateViews();
    if (workLayer == nullptr)
        return;
    QPainter qp(this);
    if (!bgLayers.isNull())
        qp.drawPixmap(0, 0, bgLayers);
    qi = workLayer->getRenderCanvas();
    setFixedSize(screenZoom.getZoomCorrected(qi.size()));
    int w = qi.width(), h = qi.height();
    vector <list <Triangle> > tris = workLayer->getTriangles();
    vector <SplineVector> vects = workLayer->getVectors();
    vector <unsigned char> activeVects = workLayer->getActiveVectors();
    int alpha = workLayer->getAlpha();
    for (size_t i = 0; i < tris.size(); ++i) {
        pair <QRgb, QRgb> colors = vects[i].getColors();
        QColor ca = QColor (colors.first), cb = QColor(colors.second);
        ca.setAlpha(alpha);
        int width = vects[i].getWidth();
        pair <QPoint, QPoint> bounds = vects[i].getBounds();
        if (bounds.first.x() > bounds.second.x()) {
            int temp = bounds.first.x();
            bounds.first.setX(bounds.second.x());
            bounds.second.setX(temp);
        }
        if (bounds.first.y() > bounds.second.y()) {
            int temp = bounds.first.y();
            bounds.first.setY(bounds.second.y());
            bounds.second.setY(temp);
        }
        bool flag = bounds.first.x() - 1 > width && bounds.first.y() - 1 > width && bounds.second.x() + 1 < w - width && bounds.second.y() + 1 < h - width;
        if (vects[i].getMode() == ColorFill) {
            if (flag) { //normal draw
                if (colors.first == colors.second) {
                    color = ca;
                    for (Triangle &t : tris[i])
                        fillTri(t);
                }
                else {
                    cb.setAlpha(alpha);
                    float ccomp = 1.0 / static_cast<float>(tris[i].size());
                    float cnt = 0.0;
                    for (Triangle &t : tris[i]) {
                        float ccc = ccomp * cnt;
                        int r = static_cast<int>((ccc * static_cast<float>(ca.red())) + ((1.0 - ccc) * static_cast<float>(cb.red())));
                        int g = static_cast<int>((ccc * static_cast<float>(ca.green())) + ((1.0 - ccc) * static_cast<float>(cb.green())));
                        int b = static_cast<int>((ccc * static_cast<float>(ca.blue())) + ((1.0 - ccc) * static_cast<float>(cb.blue())));
                        color = QColor(r, g, b, alpha);
                        fillTri(t);
                        cnt += 1.0;
                    }
                }
            }
            else {  // safe draw
                if (colors.first == colors.second) {
                    color = ca;
                    for (Triangle t : tris[i]) {
                        unsigned char flag = 0;
                        if (t.A().x() < 0 || t.A().x() >= w || t.A().y() < 0 || t.A().y() >= h)
                            ++flag;
                        if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                            ++flag;
                        if (flag > 0)
                            fillTriSafe(t);
                        if (t.C().x() < 0 || t.C().x() >= w || t.C().y() < 0 || t.C().y() >= h)
                            ++flag;
                        if (flag == 0)
                            fillTri(t);
                        else if (flag != 3)
                            fillTriSafe(t);
                    }
                }
                else {
                    cb.setAlpha(alpha);
                    float ccomp = 1.0 / static_cast<float>(tris[i].size());
                    float cnt = 0.0;
                    for (Triangle t : tris[i]) {
                        unsigned char flag = 0;
                        if (t.A().x() < 0 || t.A().x() >= w || t.A().y() < 0 || t.A().y() >= h)
                            ++flag;
                        if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                            ++flag;
                        if (flag > 0)
                            fillTriSafe(t);
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
                        color = QColor(r, g, b, alpha);
                        if (flag == 0)
                            fillTri(t);
                        else
                            fillTriSafe(t);
                        cnt += 1.0;
                    }
                }
            }
        }
        else {
            filter = vects[i].getFilter();
            if (flag) //normal draw
                for (Triangle t : tris[i])
                    filterTri(t);
            else  // safe draw
                for (Triangle t : tris[i]) {
                    unsigned char flag = 0;
                    if (t.A().x() < 0 || t.A().x() >= w || t.A().y() < 0 || t.A().y() >= h)
                        ++flag;
                    if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                        ++flag;
                    if (flag > 0)
                        filterTriSafe(t);
                    if (t.C().x() < 0 || t.C().x() >= w || t.C().y() < 0 || t.C().y() >= h)
                        ++flag;
                    if (flag == 3)
                        continue;
                    if (flag == 0)
                        filterTri(t);
                    else
                        filterTriSafe(t);
                }
        }
    }
    for (unsigned char activeVect : activeVects) {
        vector <QPoint> controlPts = vects[activeVect].getControls();
        for (QPoint qp : controlPts) {
            for (int i = stdFuncs::clamp(qp.x() - ptSize, 0, w); i < stdFuncs::clamp(qp.x() + ptSize + 1, 0, w); ++i)
                for (int j = stdFuncs::clamp(qp.y() - ptSize, 0, h); j < stdFuncs::clamp(qp.y() + ptSize + 1, 0, h); ++j) {
                    int dist = abs(i - qp.x()) + abs(j - qp.y());
                    if ((flashFlag || dist >= ptSize - 1) && dist <= ptSize)
                        qi.setPixel(i, j, Filtering::negative(qi.pixelColor(i, j), 255));
                }
        }
    }
    for (int i = stdFuncs::clamp(samplePoint.x() - ptSize, 0, w); i < stdFuncs::clamp(samplePoint.x() + ptSize + 1, 0, w); ++i)
        for (int j = stdFuncs::clamp(samplePoint.y() - ptSize, 0, h); j < stdFuncs::clamp(samplePoint.y() + ptSize + 1, 0, h); ++j) {
            int dist = abs(i - samplePoint.x()) + abs(j - samplePoint.y());
            if ((flashFlag || dist >= ptSize - 1) && dist <= ptSize)
                qi.setPixel(i, j, Filtering::negative(qi.pixelColor(i, j), 255));
        }
    for (QPoint qp : workLayer->getRasterEdges()) {
        for (int i = stdFuncs::clamp(qp.x() - ptSize, 0, w); i < stdFuncs::clamp(qp.x() + ptSize + 1, 0, w); ++i)
            for (int j = stdFuncs::clamp(qp.y() - ptSize, 0, h); j < stdFuncs::clamp(qp.y() + ptSize + 1, 0, h); ++j) {
                int dist = abs(i - qp.x()) + abs(j - qp.y());
                if ((flashFlag || dist >= ptSize - 1) && dist <= ptSize)
                    qi.setPixel(i, j, Filtering::negative(qi.pixelColor(i, j), 255));
            }
    }
    qp.drawImage(0, 0, screenZoom.zoomImg(qi));
    if (fgVisible && !fgLayers.isNull())
        qp.drawPixmap(0, 0, fgLayers);
}

void screenRender::setSamplePt(QPoint qp) {
    samplePoint = qp;
}

void screenRender::stopFlashing() {
    flasher->stop();
    flashFlag = false;
    repaint();
}

void screenRender::showFg(bool shown) {
    fgVisible = shown;
}

void screenRender::fillTri(Triangle t) {
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
        fillBTri(c, b, a);
    else if (b.y() == c.y())
        fillTTri(a, b, c);
    else {
        QPoint d (a.x() + static_cast<float>(c.x() - a.x()) * (static_cast<float>(b.y() - a.y()) / static_cast<float>(c.y() - a.y())) , b.y());
        fillBTri(c, b, d);
        fillTTri(a, b, d);
    }
}

void screenRender::fillBTri(QPoint a, QPoint b, QPoint c) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = adder * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = adder * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    for (float y = a.y(); y >= b.y(); y -= adder)
    {
        for (int x = static_cast<int>(curx1); x <= static_cast<int>(curx2); ++x)
            qi.setPixel(x, static_cast<int>(y), color.rgba());//Filtering::greyscale((qi.pixel(x, static_cast<int>(y))), 255));
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void screenRender::fillTTri(QPoint a, QPoint b, QPoint c) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = adder * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = adder * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    for (float y = a.y(); y <= b.y(); y += adder)
    {
        for (int x = static_cast<int>(curx1); x <= static_cast<int>(curx2); ++x)
            qi.setPixel(x, static_cast<int>(y), color.rgba());//Filtering::greyscale((qi.pixel(x, static_cast<int>(y))), 255));
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void screenRender::fillTriSafe(Triangle t) {
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
        fillBTriSafe(c, b, a);
    else if (b.y() == c.y())
        fillTTriSafe(a, b, c);
    else {
        QPoint d (a.x() + static_cast<float>(c.x() - a.x()) * (static_cast<float>(b.y() - a.y()) / static_cast<float>(c.y() - a.y())) , b.y());
        fillBTriSafe(c, b, d);
        fillTTriSafe(a, b, d);
    }
}

void screenRender::fillBTriSafe(QPoint a, QPoint b, QPoint c) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = adder * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = adder * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    float endY = b.y() < 0 ? 0 : b.y();
    for (float y = a.y() >= qi.height() ? qi.height() - 1 : a.y(); y >= endY; y -= adder)  {
        int startX = curx1 < 0.0 ? 0 : static_cast<int>(curx1), endX = static_cast<int>(curx2) >= qi.width() ? qi.width() - 1 : static_cast<int>(curx2);
        for (int x = startX; x <= endX; ++x)
            qi.setPixel(x, static_cast<int>(y), color.rgba());
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void screenRender::fillTTriSafe(QPoint a, QPoint b, QPoint c) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = adder * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = adder * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    float endY = b.y() >= qi.height() ? qi.height() - 1 : b.y();
    for (float y = a.y() < 0 ? 0 : a.y(); y <= endY; y += adder) {
        int startX = curx1 < 0.0 ? 0 : static_cast<int>(curx1), endX = static_cast<int>(curx2) >= qi.width() ? qi.width() - 1 : static_cast<int>(curx2);
        for (int x = startX; x <= endX; ++x)
            qi.setPixel(x, static_cast<int>(y), color.rgba()); //qi->setPixel(x, y, 0xFF00FF00);
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void screenRender::filterTri(Triangle t) {
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
        filterBTri(c, b, a);
    else if (b.y() == c.y())
        filterTTri(a, b, c);
    else {
        QPoint d (a.x() + static_cast<float>(c.x() - a.x()) * (static_cast<float>(b.y() - a.y()) / static_cast<float>(c.y() - a.y())) , b.y());
        filterBTri(c, b, d);
        filterTTri(a, b, d);
    }
}

void screenRender::filterBTri(QPoint a, QPoint b, QPoint c) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = adder * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = adder * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    for (float y = a.y(); y >= b.y(); y -= adder)
    {
        for (int x = static_cast<int>(curx1); x <= static_cast<int>(curx2); ++x) {
            int Y = static_cast<int>(y);
            qi.setPixel(x, Y, filter.applyTo(qi.pixelColor(x, Y)));
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void screenRender::filterTTri(QPoint a, QPoint b, QPoint c) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = adder * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = adder * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    for (float y = a.y(); y <= b.y(); y += adder)
    {
        for (int x = static_cast<int>(curx1); x <= static_cast<int>(curx2); ++x) {
            int Y = static_cast<int>(y);
            qi.setPixel(x, Y, filter.applyTo(qi.pixelColor(x, Y)));
        }
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void screenRender::filterTriSafe(Triangle t) {
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
        filterBTriSafe(c, b, a);
    else if (b.y() == c.y())
        filterTTriSafe(a, b, c);
    else {
        QPoint d (a.x() + static_cast<float>(c.x() - a.x()) * (static_cast<float>(b.y() - a.y()) / static_cast<float>(c.y() - a.y())) , b.y());
        filterBTriSafe(c, b, d);
        filterTTriSafe(a, b, d);
    }
}

void screenRender::filterBTriSafe(QPoint a, QPoint b, QPoint c) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = adder * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = adder * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    float endY = b.y() < 0 ? 0 : b.y();
    for (float y = a.y() >= qi.height() ? qi.height() - 1 : a.y(); y >= endY; y -= adder)  {
        int startX = curx1 < 0.0 ? 0 : static_cast<int>(curx1), endX = static_cast<int>(curx2) >= qi.width() ? qi.width() - 1 : static_cast<int>(curx2);
        for (int x = startX; x <= endX; ++x) {
            int Y = static_cast<int>(y);
            qi.setPixel(x, Y, filter.applyTo(qi.pixelColor(x, Y)));
        }
        curx1 -= invslope1;
        curx2 -= invslope2;
    }
}

void screenRender::filterTTriSafe(QPoint a, QPoint b, QPoint c) {
    if (b.x() > c.x()) {
        QPoint tmp = b;
        b = c;
        c = tmp;
    }
    float invslope1 = adder * static_cast<float>(b.x() - a.x()) / static_cast<float>(b.y() - a.y());
    float invslope2 = adder * static_cast<float>(c.x() - a.x()) / static_cast<float>(c.y() - a.y());
    float curx1 = static_cast<float>(a.x());
    float curx2 = static_cast<float>(a.x());
    float endY = b.y() >= qi.height() ? qi.height() - 1 : b.y();
    for (float y = a.y() < 0 ? 0 : a.y(); y <= endY; y += adder) {
        int startX = curx1 < 0.0 ? 0 : static_cast<int>(curx1), endX = static_cast<int>(curx2) >= qi.width() ? qi.width() - 1 : static_cast<int>(curx2);
        for (int x = startX; x <= endX; ++x) {
            int Y = static_cast<int>(y);
            qi.setPixel(x, Y, filter.applyTo(qi.pixelColor(x, Y)));
        }
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

