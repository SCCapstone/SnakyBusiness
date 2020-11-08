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

QRgb graphics::Filtering::redChannel(QColor qc, int strength) {
    int combined = (qc.green() + qc.blue()) / 2;
    return toRGB(qc.red(), combined, combined);

}

QRgb graphics::Filtering::greenChannel(QColor qc, int strength) {
    int combined = (qc.red() + qc.blue()) / 2;
    return toRGB(combined, qc.green(), combined);
}

QRgb graphics::Filtering::blueChannel(QColor qc, int strength) {
    int combined = (qc.red() + qc.green()) / 2;
    return toRGB(combined, combined, qc.blue());
}

QRgb graphics::Filtering::redPass(QColor qc, int strength) {
    int filter = (qc.green() + qc.blue()) / 2;
    int grey = (qc.red() + qc.green() + qc.blue()) / 3;
    int combined, red;
    if (qc.red() > grey) {
        combined = filter;
        red = qc.red();
    }
    else {
        combined = grey;
        red = grey;
    }
    return toRGB(red, combined, combined);
}

QRgb graphics::Filtering::greenPass(QColor qc, int strength) {
    int filter = (qc.green() + qc.blue()) / 2;
    int grey = (qc.red() + qc.green() + qc.blue()) / 3;
    int combined, green;
    if (qc.green() > grey) {
        combined = filter;
        green = qc.green();
    }
    else {
        combined = grey;
        green = grey;
    }
    return toRGB(combined, green, combined);
}

QRgb graphics::Filtering::bluePass(QColor qc, int strength) {
    int filter = (qc.green() + qc.blue()) / 2;
    int grey = (qc.red() + qc.green() + qc.blue()) / 3;
    int combined, blue;
    if (qc.blue() > grey) {
        combined = filter;
        blue = qc.blue();
    }
    else {
        combined = grey;
        blue = grey;
    }
    return toRGB(combined, combined, blue);
}

QRgb graphics::Filtering::redFilter(QColor qc, int strength) {
    int combined = (qc.red() + qc.green() + qc.blue()) / 3;
    return qc.red() > combined ? toRGB(combined, combined, combined) : toRGB(qc.red(), qc.green(), qc.blue());
}

QRgb graphics::Filtering::greenFilter(QColor qc, int strength) {
    int combined = (qc.red() + qc.green() + qc.blue()) / 3;
    return qc.green() > combined ? toRGB(combined, combined, combined) : toRGB(qc.red(), qc.green(), qc.blue());
}

QRgb graphics::Filtering::blueFilter(QColor qc, int strength) {
    int combined = (qc.red() + qc.green() + qc.blue()) / 3;
    return qc.blue() > combined ? toRGB(combined, combined, combined) : toRGB(qc.red(), qc.green(), qc.blue());
}

QRgb graphics::Filtering::rgb(QColor qc, int strength) {
    return qc.rgba();
}

QRgb graphics::Filtering::rbg(QColor qc, int strength) {
    return toRGB(qc.red(), qc.blue(), qc.green());
}

QRgb graphics::Filtering::grb(QColor qc, int strength) {
    return toRGB(qc.green(), qc.red(), qc.blue());
}

QRgb graphics::Filtering::gbr(QColor qc, int strength) {
    return toRGB(qc.green(), qc.blue(), qc.red());
}

QRgb graphics::Filtering::brg(QColor qc, int strength) {
    return toRGB(qc.blue(), qc.red(), qc.green());
}

QRgb graphics::Filtering::bgr(QColor qc, int strength) {
    return toRGB(qc.blue(), qc.green(), qc.red());
}

int graphics::Filtering::Burn(int color, int strength) {
    return (color - strength) < 0 ? 0 : color - strength;
}

QRgb graphics::Filtering::burn (QColor qc, int strength) {
    return toRGB(Burn(qc.red(), strength), Burn(qc.green(), strength), Burn(qc.blue(), strength));
}

QRgb graphics::Filtering::burnRed (QColor qc, int strength) {
    return toRGB(Burn(qc.red(), strength), qc.green(), qc.blue());
}

QRgb graphics::Filtering::burnGreen (QColor qc, int strength) {
    return toRGB(qc.red(), Burn(qc.green(), strength), qc.blue());
}

QRgb graphics::Filtering::burnBlue (QColor qc, int strength) {
    return toRGB(qc.red(), qc.green(), Burn(qc.blue(), strength));
}

QRgb graphics::Filtering::burnYellow (QColor qc, int strength) {
    return toRGB(Burn(qc.red(), strength), Burn(qc.green(), strength), qc.blue());
}

QRgb graphics::Filtering::burnCyan (QColor qc, int strength) {
    return toRGB(qc.red(), Burn(qc.green(), strength), Burn(qc.blue(), strength));
}

QRgb graphics::Filtering::burnMagenta (QColor qc, int strength) {
    return toRGB(Burn(qc.red(), strength), qc.green(), Burn(qc.blue(), strength));
}

int graphics::Filtering::Dodge(int color, int strength) {
    return (color + strength) > 255 ? 255 : color + strength;
}

QRgb graphics::Filtering::dodge (QColor qc, int strength) {
    return toRGB(Dodge(qc.red(), strength), Dodge(qc.green(), strength), Dodge(qc.blue(), strength));
}
QRgb graphics::Filtering::dodgeRed (QColor qc, int strength) {
    return toRGB(Dodge(qc.red(), strength), qc.green(), qc.blue());
}

QRgb graphics::Filtering::dodgeGreen (QColor qc, int strength) {
    return toRGB(qc.red(), Dodge(qc.green(), strength), qc.blue());
}

QRgb graphics::Filtering::dodgeBlue (QColor qc, int strength) {
    return toRGB(qc.red(), qc.green(), Dodge(qc.blue(), strength));
}

QRgb graphics::Filtering::dodgeYellow (QColor qc, int strength) {
    return toRGB(Dodge(qc.red(), strength), Dodge(qc.green(), strength), qc.blue());
}

QRgb graphics::Filtering::dodgeCyan (QColor qc, int strength) {
    return toRGB(qc.red(), Dodge(qc.green(), strength), Dodge(qc.blue(), strength));
}

QRgb graphics::Filtering::dodgeMagenta (QColor qc, int strength) {
    return toRGB(Dodge(qc.red(), strength), qc.green(), Dodge(qc.blue(), strength));
}

QRgb graphics::Filtering::greyFilmGrain (QColor qc, int strength) {
    int grey = rand() % (strength + 1);
    if (rand() % 2)
        grey = -grey;
    return toRGB(stdFuncs::clamp(qc.red() + grey, minColor, maxColor), stdFuncs::clamp(qc.green() + grey, minColor, maxColor), stdFuncs::clamp(qc.blue() + grey, minColor, maxColor));
}

QRgb graphics::Filtering::colorFilmGrain (QColor qc, int strength) {
    ++strength;
    int grey = rand() % strength;
    if (rand() % 2)
        grey = -grey;
    int red = stdFuncs::clamp(qc.red() + grey, minColor, maxColor);
    grey = rand() % strength;
    if (rand() % 2)
        grey = -grey;
    int green = stdFuncs::clamp(qc.green() + grey, minColor, maxColor);
    grey = rand() % strength;
    if (rand() % 2)
        grey = -grey;
    int blue = stdFuncs::clamp(qc.blue() + grey, minColor, maxColor);
    return toRGB(red, green, blue);
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
