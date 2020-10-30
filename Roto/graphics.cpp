#include <graphics.h>

graphics::Filter::Filter(int str, string filterName) {
    strength = str;
    setFilter(filterName);
}

void graphics::Filter::applyTo(QImage *qi) {
    int h = qi->height(), w = qi->width();
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            QColor qc = qi->pixelColor(x, y);
            qi->setPixel(x, y, filterApplicator(qc, strength));
        }
}

QRgb graphics::Filter::applyTo(QColor qc) {
    return filterApplicator(qc, strength);
}

int graphics::Filter::getStrength() {
    return strength;
}

void graphics::Filter::setStrength(int value) {
    strength = value > 255 ? 255 : (value < 1 ? 1 : value);
}

void graphics::Filter::strengthUp() {
    setStrength(strength + 1);
}

void graphics::Filter::strengthDown() {
    setStrength(strength - 1);
}

void graphics::Filter::setFilter(string filterName) {
    size_t i;
    for (i = 0; i < numFilters; ++i)
        if (filterName == filterNames[i])
            break;
    if (i == numFilters)
        i = 0;
    filterIndex = i;
    filterApplicator = filters[i];
    strength = filterPresets[i];
}

int graphics::Filter::getFilterIndex() {
    return static_cast<int>(filterIndex);
}

QRgb graphics::Filtering::toRGB(int r, int g, int b) {
    QRgb color = 0xFF000000;
    color += static_cast<QRgb>(r << 16);
    color += static_cast<QRgb>(g << 8);
    color += static_cast<QRgb>(b);
    return color;
}

QRgb graphics::Filtering::greyscale(QColor qc, int strength) {
    int combined = (qc.red() + qc.blue() + qc.green()) / 3;
    int red = (((255 - strength) * qc.red()) + (strength * combined)) / 255;
    int green = (((255 - strength) * qc.green()) + (strength * combined)) / 255;
    int blue = (((255 - strength) * qc.blue()) + (strength * combined)) / 255;
    return toRGB(red, green, blue);
}

QRgb graphics::Filtering::polarize(QColor qc, int strength) {
    int combined = (qc.red() + qc.blue() + qc.green()) / 3;
    return combined > strength ? 0xFFFFFFFF : 0xFF000000;
}

QRgb graphics::Filtering::negative(QColor qc, int strength) {
    return toRGB(255 - qc.red(), 255 - qc.green(), 255 - qc.blue());
}

QRgb graphics::Filtering::rgb(QColor qc, int strength) {
    return qc.rgba();
}

graphics::ImgSupport::ImgSupport() {
    zoom = 1.0;
}

double graphics::ImgSupport::getZoom() {
    return zoom;
}

void graphics::ImgSupport::setZoom(double Zoom) {
    zoom = stdFuncs::clamp(Zoom, graphics::minZoom, graphics::maxZoom);
    zoom = Zoom;
}

void graphics::ImgSupport::zoomIn() {
    setZoom(zoom + 0.01);
}

void graphics::ImgSupport::zoomOut() {
    setZoom(zoom - 0.01);
}

int graphics::ImgSupport::getSize(double dim, double zoom) {
    return static_cast<int>(dim / zoom + zoom);
}

QImage graphics::ImgSupport::zoomImg(QImage qi) {
    int scale = static_cast<int>(static_cast<double>(qi.height()) * zoom);
    return qi.scaledToHeight(scale);
}

QPoint graphics::ImgSupport::getZoomCorrected(QPoint qp) {
    qp.setX(static_cast<int>(static_cast<double>(qp.x()) / zoom));
    qp.setY(static_cast<int>(static_cast<double>(qp.y()) / zoom));
    return qp;
}
