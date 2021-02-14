#include <screenrender.h>


screenRender::screenRender(QWidget *parent) : QWidget(parent) {
    sizeDisplay = -1;
    shiftFlag = false;
    lastButton = NoButton;
    flashFlag = false;
    flasher = new QTimer(this);
    connect(flasher, SIGNAL(timeout()), this, SLOT(toggleFlasher()));
    fgVisible = true;
    adder = 0.1;
    samplePoint = QPoint(-1000, -1000);
    flasher->start(flashSpeed);
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
    if (!bgPrescaled.isNull())
        bgLayers.convertFromImage(screenZoom.zoomImg(bgPrescaled));
    repaint();
}

void screenRender::showPts() {
    flashFlag = true;
}

void screenRender::toggleFlasher() {
    flashFlag = !flashFlag;
    repaint();
}

void screenRender::updateViews(Layer *working, QImage fg, QImage bg) {
    workLayer = working;
    bgPrescaled = bg;
    fgPrescaled = fg;
    doZoom();
}

void screenRender::paintEvent(QPaintEvent *event) {
    long long t1 = stdFuncs::getTime();
    QPainter qp(this);
    if (!bgLayers.isNull())
        qp.drawPixmap(0, 0, bgLayers);
    if (workLayer == nullptr)
        return;
    qi = workLayer->getCanvas()->copy();
    int w = qi.width(), h = qi.height();
    vector <list <Triangle> > tris = workLayer->getTriangles();
    vector <SplineVector> vects = workLayer->getVectors();
    vector <unsigned char> activeVects = workLayer->getActiveVectors();
    int alpha = workLayer->getAlpha();
    for (size_t i = 0; i < tris.size(); ++i) {
        pair <QRgb, QRgb> colors = vects[i].getColors();
        QColor ca = QColor (colors.first), cb = QColor(colors.second);
        ca.setAlpha(alpha);
        unsigned char flag = 1;
        int width = vects[i].getWidth();
        pair <QPoint, QPoint> bounds = vects[i].getBounds();
        if (bounds.first.x() < width || bounds.first.y() < width || bounds.second.x() >= w - width || bounds.second.y() >= h - width)
            flag = 0;
        if (flag && (!shiftFlag || lastButton == RightButton)) { //normal draw
            if (colors.first == colors.second) {
                color = ca;
                for (Triangle t : tris[i])
                    calcTri(t);
            }
            else {
                float ccomp = 1.0 / static_cast<float>(tris[i].size());
                float cnt = 0.0;
                for (Triangle t : tris[i]) {
                    float ccc = ccomp * cnt;
                    int r = static_cast<int>((ccc * static_cast<float>(ca.red())) + ((1.0 - ccc) * static_cast<float>(cb.red())));
                    int g = static_cast<int>((ccc * static_cast<float>(ca.green())) + ((1.0 - ccc) * static_cast<float>(cb.green())));
                    int b = static_cast<int>((ccc * static_cast<float>(ca.blue())) + ((1.0 - ccc) * static_cast<float>(cb.blue())));
                    color = QColor(r, g, b, alpha);
                    calcTri(t);
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
                        calcTriSafe(t);
                    if (t.C().x() < 0 || t.C().x() >= w || t.C().y() < 0 || t.C().y() >= h)
                        ++flag;
                    if (flag == 0)
                        calcTri(t);
                    else if (flag != 3)
                        calcTriSafe(t);
                }
            }
            else {
                float ccomp = 1.0 / static_cast<float>(tris[i].size());
                float cnt = 0.0;
                for (Triangle t : tris[i]) {
                    unsigned char flag = 0;
                    if (t.A().x() < 0 || t.A().x() >= w || t.A().y() < 0 || t.A().y() >= h)
                        ++flag;
                    if (t.B().x() < 0 || t.B().x() >= w || t.B().y() < 0 || t.B().y() >= h)
                        ++flag;
                    if (flag > 0)
                        calcTriSafe(t);
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
                        calcTri(t);
                    else
                        calcTriSafe(t);
                    cnt += 1.0;
                }
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
    qp.drawImage(0, 0, screenZoom.zoomImg(qi));
    if (fgVisible && !fgLayers.isNull())
        qp.drawPixmap(0, 0, fgLayers);
    if (sizeDisplay != -1) {
        qp.drawRoundRect(0, 25, 40, 20);
        qp.setPen(QPen(Qt::black));
        qp.setFont(QFont("Arial", 9, QFont::Bold));
        qp.drawText(10, 40, QString(to_string(sizeDisplay).c_str()));
        sizeDisplay = -1;
        flasher->start(flashSpeed);
    }
    cout << stdFuncs::getChange(t1) << endl;
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

void screenRender::calcTri(Triangle t) {
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
            qi.setPixel(x, static_cast<int>(y), Filtering::greyscale((qi.pixel(x, static_cast<int>(y))), 255));
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
            qi.setPixel(x, static_cast<int>(y), Filtering::greyscale((qi.pixel(x, static_cast<int>(y))), 255));
        curx1 += invslope1;
        curx2 += invslope2;
    }
}

void screenRender::calcTriSafe(Triangle t) {
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

void screenRender::setShiftFlag(bool sf) {
    shiftFlag = sf;
}

void screenRender::setLastButton(MouseButton lb) {
    lastButton = lb;
}

void screenRender::setSizeDisplay(int i) {
    sizeDisplay = i;
    flasher->stop();
}

