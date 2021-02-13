#include "graphics.h"

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

QRgb graphics::Filtering::toRGB(int a, int r, int g, int b) {
    QRgb color = static_cast<QRgb>(a) << 24;
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
    return toRGB(qc.alpha(), red, green, blue);
}

QRgb graphics::Filtering::polarize(QColor qc, int strength) {
    int combined = (qc.red() + qc.blue() + qc.green()) / 3;
    return combined > strength ? toRGB(qc.alpha(), 255, 255, 255) : toRGB(qc.alpha(), 0, 0, 0);;
}

QRgb graphics::Filtering::negative(QColor qc, int strength) {
    return toRGB(qc.alpha(), 255 - qc.red(), 255 - qc.green(), 255 - qc.blue());
}

QRgb graphics::Filtering::enshadow(QColor qc, int strength) {
    int combined = (qc.red() + qc.blue() + qc.green()) / 3;
    return combined > strength ? qc.rgba() : (static_cast<QRgb>(qc.alpha()) << 24);
}

QRgb graphics::Filtering::redChannel(QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.red(), qc.red(), qc.red());
}

QRgb graphics::Filtering::greenChannel(QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.green(), qc.green(), qc.green());
}

QRgb graphics::Filtering::blueChannel(QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.blue(), qc.blue(), qc.blue());
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
    return toRGB(qc.alpha(), red, combined, combined);
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
    return toRGB(qc.alpha(), combined, green, combined);
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
    return toRGB(qc.alpha(), combined, combined, blue);
}

QRgb graphics::Filtering::redFilter(QColor qc, int strength) {
    int combined = (qc.red() + qc.green() + qc.blue()) / 3;
    return qc.red() > combined ? toRGB(qc.alpha(), combined, combined, combined) : toRGB(qc.alpha(), qc.red(), qc.green(), qc.blue());
}

QRgb graphics::Filtering::greenFilter(QColor qc, int strength) {
    int combined = (qc.red() + qc.green() + qc.blue()) / 3;
    return qc.green() > combined ? toRGB(qc.alpha(), combined, combined, combined) : toRGB(qc.alpha(), qc.red(), qc.green(), qc.blue());
}

QRgb graphics::Filtering::blueFilter(QColor qc, int strength) {
    int combined = (qc.red() + qc.green() + qc.blue()) / 3;
    return qc.blue() > combined ? toRGB(qc.alpha(), combined, combined, combined) : toRGB(qc.alpha(), qc.red(), qc.green(), qc.blue());
}

QRgb graphics::Filtering::rgb(QColor qc, int strength) {
    return qc.rgba();
}

QRgb graphics::Filtering::rbg(QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.red(), qc.blue(), qc.green());
}

QRgb graphics::Filtering::grb(QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.green(), qc.red(), qc.blue());
}

QRgb graphics::Filtering::gbr(QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.green(), qc.blue(), qc.red());
}

QRgb graphics::Filtering::brg(QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.blue(), qc.red(), qc.green());
}

QRgb graphics::Filtering::bgr(QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.blue(), qc.green(), qc.red());
}

int graphics::Filtering::Burn(int color, int strength) {
    return (color - strength) < 0 ? 0 : color - strength;
}

QRgb graphics::Filtering::burn (QColor qc, int strength) {
    return toRGB(qc.alpha(), Burn(qc.red(), strength), Burn(qc.green(), strength), Burn(qc.blue(), strength));
}

QRgb graphics::Filtering::burnRed (QColor qc, int strength) {
    return toRGB(qc.alpha(), Burn(qc.red(), strength), qc.green(), qc.blue());
}

QRgb graphics::Filtering::burnGreen (QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.red(), Burn(qc.green(), strength), qc.blue());
}

QRgb graphics::Filtering::burnBlue (QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.red(), qc.green(), Burn(qc.blue(), strength));
}

QRgb graphics::Filtering::burnYellow (QColor qc, int strength) {
    return toRGB(qc.alpha(), Burn(qc.red(), strength), Burn(qc.green(), strength), qc.blue());
}

QRgb graphics::Filtering::burnCyan (QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.red(), Burn(qc.green(), strength), Burn(qc.blue(), strength));
}

QRgb graphics::Filtering::burnMagenta (QColor qc, int strength) {
    return toRGB(qc.alpha(), Burn(qc.red(), strength), qc.green(), Burn(qc.blue(), strength));
}

int graphics::Filtering::Dodge(int color, int strength) {
    return (color + strength) > 255 ? 255 : color + strength;
}

QRgb graphics::Filtering::dodge (QColor qc, int strength) {
    return toRGB(qc.alpha(), Dodge(qc.red(), strength), Dodge(qc.green(), strength), Dodge(qc.blue(), strength));
}
QRgb graphics::Filtering::dodgeRed (QColor qc, int strength) {
    return toRGB(qc.alpha(), Dodge(qc.red(), strength), qc.green(), qc.blue());
}

QRgb graphics::Filtering::dodgeGreen (QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.red(), Dodge(qc.green(), strength), qc.blue());
}

QRgb graphics::Filtering::dodgeBlue (QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.red(), qc.green(), Dodge(qc.blue(), strength));
}

QRgb graphics::Filtering::dodgeYellow (QColor qc, int strength) {
    return toRGB(qc.alpha(), Dodge(qc.red(), strength), Dodge(qc.green(), strength), qc.blue());
}

QRgb graphics::Filtering::dodgeCyan (QColor qc, int strength) {
    return toRGB(qc.alpha(), qc.red(), Dodge(qc.green(), strength), Dodge(qc.blue(), strength));
}

QRgb graphics::Filtering::dodgeMagenta (QColor qc, int strength) {
    return toRGB(qc.alpha(), Dodge(qc.red(), strength), qc.green(), Dodge(qc.blue(), strength));
}

QRgb graphics::Filtering::greyFilmGrain (QColor qc, int strength) {
    int grey = rand() % (strength + 1);
    if (rand() % 2)
        grey = -grey;
    return toRGB(qc.alpha(), stdFuncs::clamp(qc.red() + grey, minColor, maxColor), stdFuncs::clamp(qc.green() + grey, minColor, maxColor), stdFuncs::clamp(qc.blue() + grey, minColor, maxColor));
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
    return toRGB(qc.alpha(), red, green, blue);
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

void graphics::ImgSupport::rotate90Right(QImage *&qi) {
    QImage copy = qi->copy();
    int w = copy.width(), h = copy.height() - 1;
    delete qi;
    qi = new QImage(copy.height(), copy.width(), QImage::Format_ARGB32_Premultiplied);
    for (int x = 0; x < w; ++x)
        for (int y = 0; y <= h; ++y)
            qi->setPixel(y, x, copy.pixel(x, h - y));
}

void graphics::ImgSupport::rotate90Left(QImage *&qi) {
    QImage copy = qi->copy();
    int w = copy.width() - 1, h = copy.height();
    delete qi;
    qi = new QImage(copy.height(), copy.width(), QImage::Format_ARGB32_Premultiplied);
    for (int x = 0; x <= w; ++x)
        for (int y = 0; y < h; ++y)
            qi->setPixel(y, x, copy.pixel(w - x, y));
}

void graphics::ImgSupport::rotate180(QImage *qi) {
    int w = qi->width(), h = qi->height() - 1;
    int halfW = (w + 1) / 2;
    --w;
    for (int i = 0; i < halfW; ++i)
        for (int j = 0; j <= h; ++j) {
            QRgb qc = qi->pixel(i, j);
            qi->setPixel(i, j, qi->pixel(w - i, h - j));
            qi->setPixel(w - i, h - j, qc);
        }
}

void graphics::ImgSupport::flipVertical(QImage *qi) {
    int w = qi->width(), h = qi->height();
    int halfW = w / 2;
    --w;
    for (int i = 0; i < halfW; ++i)
        for (int j = 0; j < h; ++j) {
            QRgb qc = qi->pixel(i, j);
            qi->setPixel(i, j, qi->pixel(w - i, j));
            qi->setPixel(w - i, j, qc);
        }
}

void graphics::ImgSupport::flipHorizontal(QImage *qi) {
    int w = qi->width(), h = qi->height();
    int halfH = h / 2;
    --h;
    for (int i = 0; i < w; ++i)
        for (int j = 0; j < halfH; ++j) {
            QRgb qc = qi->pixel(i, j);
            qi->setPixel(i, j, qi->pixel(i, h - j));
            qi->setPixel(i, h - j, qc);
        }
}
