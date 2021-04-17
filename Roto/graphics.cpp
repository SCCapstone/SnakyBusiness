#include "graphics.h"


graphics::Filter::Filter(int str, string filterName) {
    strength = str;
    setFilter(filterName);
}

graphics::Filter::Filter(const Filter &filter) {
    strength = filter.strength;
    filterApplicator = filter.filterApplicator;
    filterIndex = filter.filterIndex;
}

graphics::Filter& graphics::Filter::operator=(const Filter &rhs) {
    strength = rhs.strength;
    filterIndex = rhs.filterIndex;
    filterApplicator = rhs.filterApplicator;
    return *this;
}
void graphics::Filter::applyTo(QImage *qi) {
    int h = qi->height(), w = qi->width();
    for (int y = 0; y < h; ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(qi->scanLine(y));
        for (int x = 0; x < w; ++x)
            line[x] = filterApplicator(QColor(line[x]), strength);
    }
}

QRgb graphics::Filter::applyTo(QColor qc) {
    return filterApplicator(qc, strength);
}

int graphics::Filter::getStrength() {
    return strength;
}

void graphics::Filter::setStrength(int value) {
    strength = stdFuncs::clamp(value, minColor, maxColor);
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
    if (qc.red() > qc.blue() && qc.red() > qc.green() && qc.red() - max(qc.green(), qc.blue()) > abs(qc.green() - qc.blue()))
        return qc.rgba();
    int grey = (qc.red() + qc.green() + qc.blue()) / 3;
    return toRGB(qc.alpha(), grey, grey, grey);
}

QRgb graphics::Filtering::greenPass(QColor qc, int strength) {
    if (qc.green() > qc.blue() && qc.green() > qc.red() && qc.green() - max(qc.red(), qc.blue()) > abs(qc.red() - qc.blue()))
        return qc.rgba();
    int grey = (qc.red() + qc.green() + qc.blue()) / 3;
    return toRGB(qc.alpha(), grey, grey, grey);
}

QRgb graphics::Filtering::bluePass(QColor qc, int strength) {
    if (qc.blue() > qc.red() && qc.blue() > qc.green() && qc.blue() - max(qc.red(), qc.green()) > abs(qc.green() - qc.red()))
        return qc.rgba();
    int grey = (qc.red() + qc.green() + qc.blue()) / 3;
    return toRGB(qc.alpha(), grey, grey, grey);
}

QRgb graphics::Filtering::magentaPass(QColor qc, int strength) {
    if (max(qc.red(), qc.blue()) - min(qc.red(), qc.blue()) < min(qc.red(), qc.blue()) - qc.green())
        return qc.rgba();
    int grey = (qc.red() + qc.green() + qc.blue()) / 3;
    return toRGB(qc.alpha(), grey, grey, grey);
}

QRgb graphics::Filtering::yellowPass(QColor qc, int strength) {
    if (max(qc.red(), qc.green()) - min(qc.red(), qc.green()) < min(qc.red(), qc.green()) - qc.blue())
        return qc.rgba();
    int grey = (qc.red() + qc.green() + qc.blue()) / 3;
    return toRGB(qc.alpha(), grey, grey, grey);
}

QRgb graphics::Filtering::cyanPass(QColor qc, int strength) {
    if (max(qc.green(), qc.blue()) - min(qc.green(), qc.blue()) < min(qc.green(), qc.blue()) - qc.red())
        return qc.rgba();
    int grey = (qc.red() + qc.green() + qc.blue()) / 3;
    return toRGB(qc.alpha(), grey, grey, grey);
}

QRgb graphics::Filtering::redFilter(QColor qc, int strength) {
    int combined = (qc.red() + qc.green() + qc.blue()) / 3;
    return (qc.red() > qc.green() && qc.red() > qc.blue()) ? toRGB(qc.alpha(), combined, combined, combined) : toRGB(qc.alpha(), qc.red(), qc.green(), qc.blue());
}

QRgb graphics::Filtering::greenFilter(QColor qc, int strength) {
    int combined = (qc.red() + qc.green() + qc.blue()) / 3;
    return (qc.green() > qc.blue() && qc.green() > qc.blue()) ? toRGB(qc.alpha(), combined, combined, combined) : toRGB(qc.alpha(), qc.red(), qc.green(), qc.blue());
}

QRgb graphics::Filtering::blueFilter(QColor qc, int strength) {
    int combined = (qc.red() + qc.green() + qc.blue()) / 3;
    return (qc.blue() > qc.red() && qc.blue() > qc.green())  ? toRGB(qc.alpha(), combined, combined, combined) : toRGB(qc.alpha(), qc.red(), qc.green(), qc.blue());
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

void graphics::Filtering::applyKernal(QProgressDialog *qpd, QImage *qi, pair <bool, vector <vector <float> > > kernalInfo) {
    bool needGreyscale = kernalInfo.first;
    qpd->setValue(0);
    qpd->setLabelText("Applying Kernal");
    qpd->setMaximum(qi->width());
    if (needGreyscale) {
        qpd->setMaximum(qpd->maximum() * 2 + 1);
        Filter filter(255, "Greyscale");
        filter.applyTo(qi);
        qpd->setValue(qpd->value() + 1);
        QCoreApplication::processEvents();
    }
    int boost = 1;
    vector <vector <float> > kernal = kernalInfo.second;
    int kernalSize = kernal.size();
    QImage image = qi->copy();
    // Apply kernal to the image.
    for (int i = 0; i < qi->width(); ++i) {
        qpd->setValue(qpd->value() + 1);
        QCoreApplication::processEvents();
        for (int j = 0; j < qi->height(); ++j) {
            int offset = kernalSize / 2;
            int xstart = max(i - offset, 0), ystart = max(j - offset, 0), xend = min(i + offset, image.width() - 1), yend = min(j + offset, image.height() - 1);
            float r = 0.0, g = 0.0, b = 0.0;
            for (int x = xstart; x <= xend; ++x) {
                int dx = x - xstart;
                for (int y = ystart; y <= yend; ++y) {
                    int dy = y - ystart;
                    QColor qc = image.pixelColor(x, y);
                    r += kernal[dx][dy] * static_cast<float>(qc.red());
                    g += kernal[dx][dy] * static_cast<float>(qc.green());
                    b += kernal[dx][dy] * static_cast<float>(qc.blue());
                }
            }
            r = stdFuncs::clamp(r, 0.0, 255.0);
            g = stdFuncs::clamp(g, 0.0, 255.0);
            b = stdFuncs::clamp(b, 0.0, 255.0);
            if (needGreyscale)
                boost = max(boost, min(static_cast<int>(max(r, max(g, b))), 255));
            qi->setPixelColor(i, j, QColor(r, g, b));
        }
    }
    // Boost contrast with greyscale output so that the result is more visible / distinguishable.
    if (needGreyscale) {
        float boostF = 255.0 / static_cast<float>(boost);
        for (int i = 0; i < qi->width(); ++i) {
            qpd->setValue(qpd->value() + 1);
            QCoreApplication::processEvents();
            for (int j = 0; j < qi->height(); ++j) {
                QColor qc = qi->pixelColor(i, j);
                int color = static_cast<int>(static_cast<float>(qc.red()) * boostF);
                qi->setPixelColor(i, j, QColor(color, color, color));
            }
        }
    }
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
    setZoom(stdFuncs::clamp(3.0 * zoom / 2.0, graphics::minZoom, graphics::maxZoom));
}

void graphics::ImgSupport::zoomOut() {
    setZoom(stdFuncs::clamp(2.0 * zoom / 3.0, graphics::minZoom, graphics::maxZoom));
}

int graphics::ImgSupport::getSize(double dim, double zoom) {
    return static_cast<int>(dim / zoom + zoom);
}

QImage graphics::ImgSupport::zoomImg(QImage qi) {
    int scale = static_cast<int>(static_cast<double>(qi.height()) * zoom);
    return qi.scaledToHeight(scale);
}

QPoint graphics::ImgSupport::getZoomCorrected(QPoint qp) {
    qp.setX(static_cast<int>(0.5 + static_cast<double>(qp.x()) / zoom));
    qp.setY(static_cast<int>(0.5 + static_cast<double>(qp.y()) / zoom));
    return qp;
}

QSize graphics::ImgSupport::getZoomCorrected(QSize qs) {
    return QSize(static_cast<int>(static_cast<float>(qs.width()) * zoom), static_cast<int>(static_cast<float>(qs.height()) * zoom));
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

pair <bool, vector <vector <float> > > graphics::ImgSupport::loadKernal(string fileName) {
    vector <vector <float> > kernal, identity;
    identity.push_back(vector <float> ());
    identity[0].push_back(1.0);
    pair <bool, vector <vector <float> > > ret = pair <bool, vector <vector <float> > > (false, identity);
    if (fileName != "") {
        int tKernalSize;
        fstream file;
        bool needGreyscale = false;
        file.open(fileName, ios::in);
        if (file.is_open()) {
            string fromFile;
            if (getline(file, fromFile)) {
                try {
                    tKernalSize = stoi(fromFile);
                    tKernalSize -= 1 - (tKernalSize % 2);
                    needGreyscale = fromFile.find("G") != string::npos;
                }
                catch (...) {
                    return ret;
                }
            }
            else
                return ret;
            if (tKernalSize < 1)
                return ret;
            for (int i = 0; i < tKernalSize; ++i) {
                kernal.push_back(vector <float> ());
                for (int j = 0; j < tKernalSize; ++j)
                    kernal[i].push_back(0.0);
            }
            int lines = 0;
            while(lines < tKernalSize && getline(file, fromFile)) {
                int cnt = 0;
                while (cnt < tKernalSize && fromFile != "") {
                    size_t index = fromFile.find(" ");
                    if (index == string::npos)
                        index = fromFile.length();
                    try {
                        kernal[lines][cnt] = stof(fromFile.substr(0, index));
                    }
                    catch (...) {
                        return ret;
                    }
                    ++cnt;
                    fromFile = index + 1 >= fromFile.length() ? "" : fromFile.substr(index + 1);
                }
                if (cnt < tKernalSize)
                    return ret;
                ++lines;
            }
            file.close();
        }
        ret.first = needGreyscale;
        ret.second = kernal;
        return ret;
    }
    return ret;
}
