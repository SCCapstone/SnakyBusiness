#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string>
#include <list>
#include <functional>
#include <vector>
#include <fstream>
#include <QImage>
#include <QProgressDialog>
#include <QCoreApplication>
#include <stdfuncs.h>

using std::max;
using std::min;
using std::string;
using std::list;
using std::function;
using std::vector;
using std::fstream;
using std::ios;
using std::pair;

typedef vector<vector<float>> Kernal;
typedef pair<bool,Kernal> KernalData;

namespace graphics {

const string filterNames[] =   {"Normal (rgb)", "Greyscale", "Polarize", "Negative", "Burn", "Dodge", "Enshadow", "Red Channel", "Green Channel", "Blue Channel", "Red Pass", "Green Pass", "Blue Pass", "Magenta Pass", "Yellow Pass", "Cyan Pass", "Red Filter", "Green Filter", "Blue Filter", "Burn Red", "Burn Green", "Burn Blue", "Burn Yellow", "Burn Cyan", "Burn Magenta", "Dodge Red", "Dodge Green", "Dodge Blue", "Dodge Yellow", "Dodge Cyan", "Dodge Magenta", "RBG", "GRB", "GBR", "BRG", "BGR", "Color Noise", "Grey Noise"};
const string vectorFilters[] = {"Greyscale", "Polarize", "Red Pass", "Green Pass", "Blue Pass", "Magenta Pass", "Yellow Pass", "Cyan Pass", "Red Filter", "Green Filter", "Blue Filter", "Color Noise", "Grey Noise"};
const int filterPresets[] =    {0             , 255        , 128        , 0         , 20    , 20     , 64,        255          , 255            , 255           , 255       , 255         , 255        , 255           , 255          , 255        , 255         , 255           , 255          , 20        , 20          , 20         , 20           , 20         , 20            , 20         , 20           , 20          , 20            , 20          , 20             , 0    , 0    , 0    , 0    , 0    , 4           , 4              };
const int numFilters = 38;
const double minZoom = 1.0 / 4.0;
const double maxZoom = 8.0;
const int minColor = 0;
const int maxColor = 255;

class Filter {

public:

    Filter(int strength = filterPresets[0], string filterName = filterNames[0]);
    Filter (const Filter &filter);
    Filter& operator = (const Filter &lhs);
    void applyTo(QImage *qi);
    QRgb applyTo(QColor qc);
    int getStrength();
    void setStrength(int value);
    void strengthUp();
    void strengthDown();
    void setFilter(string filterName);
    int getFilterIndex();

private:

    function <QRgb (QColor, int)> filterApplicator;
    int strength;
    size_t filterIndex;
};

class Filtering {

public:

    static QRgb toRGB (int a, int r, int g, int b);
    static QRgb greyscale (QColor qc, int strength);
    static QRgb polarize (QColor qc, int strength);
    static QRgb negative (QColor qc, int strength);
    static QRgb burn (QColor qc, int strength);
    static QRgb dodge (QColor qc, int strength);
    static QRgb enshadow(QColor qc, int strength);
    static QRgb redChannel (QColor qc, int strength);
    static QRgb greenChannel (QColor qc, int strength);
    static QRgb blueChannel (QColor qc, int strength);
    static QRgb redPass (QColor qc, int strength);
    static QRgb greenPass (QColor qc, int strength);
    static QRgb bluePass (QColor qc, int strength);
    static QRgb magentaPass (QColor qc, int strength);
    static QRgb yellowPass (QColor qc, int strength);
    static QRgb cyanPass (QColor qc, int strength);
    static QRgb redFilter (QColor qc, int strength);
    static QRgb greenFilter (QColor qc, int strength);
    static QRgb blueFilter (QColor qc, int strength);
    static QRgb burnRed (QColor qc, int strength);
    static QRgb burnGreen (QColor qc, int strength);
    static QRgb burnBlue (QColor qc, int strength);
    static QRgb burnYellow (QColor qc, int strength);
    static QRgb burnCyan (QColor qc, int strength);
    static QRgb burnMagenta (QColor qc, int strength);
    static QRgb dodgeRed (QColor qc, int strength);
    static QRgb dodgeGreen (QColor qc, int strength);
    static QRgb dodgeBlue (QColor qc, int strength);
    static QRgb dodgeYellow (QColor qc, int strength);
    static QRgb dodgeCyan (QColor qc, int strength);
    static QRgb dodgeMagenta (QColor qc, int strength);
    static QRgb rgb (QColor qc, int strength);
    static QRgb rbg (QColor qc, int strength);
    static QRgb grb (QColor qc, int strength);
    static QRgb gbr (QColor qc, int strength);
    static QRgb brg (QColor qc, int strength);
    static QRgb bgr (QColor qc, int strength);
    static QRgb greyFilmGrain (QColor qc, int strength);
    static QRgb colorFilmGrain (QColor qc, int strength);
    static void applyKernal(QProgressDialog *qpd, QImage *qi, KernalData kernalInfo);

private:

    static int Burn(int color, int strength);
    static int Dodge(int color, int strength);
};

class ImgSupport {

public:

    ImgSupport();
    QImage zoomImg(QImage qi);
    double getZoom();
    void setZoom(double Zoom);
    void zoomIn();
    void zoomOut();
    QPoint getZoomCorrected(QPoint qp);
    QSize getZoomCorrected(QSize qs);
    static void flipVertical(QImage *qi);
    static void flipHorizontal(QImage *qi);
    static list <QImage *> resize(QSize reqSize);
    static KernalData loadKernal(string fileName);
    static void applyAlpha(QImage *qi, int *yStart, int *yEnd, unsigned int *alpha);

private:

    static int getSize(double dim, double zoom);

    double zoom;
};

const function <QRgb (QColor, int)> filters[] = {Filtering::rgb, Filtering::greyscale, Filtering::polarize, Filtering::negative, Filtering::burn, Filtering::dodge, Filtering::enshadow, Filtering::redChannel, Filtering::greenChannel, Filtering::blueChannel, Filtering::redPass, Filtering::greenPass, Filtering::bluePass, Filtering::magentaPass, Filtering::yellowPass, Filtering::cyanPass, Filtering::redFilter, Filtering::greenFilter, Filtering::blueFilter, Filtering::burnRed, Filtering::burnGreen, Filtering::burnBlue, Filtering::burnYellow, Filtering::burnCyan, Filtering::burnMagenta, Filtering::dodgeRed, Filtering::dodgeGreen, Filtering::dodgeBlue, Filtering::dodgeYellow, Filtering::dodgeCyan, Filtering::dodgeMagenta, Filtering::rbg, Filtering::grb, Filtering::gbr, Filtering::brg, Filtering::bgr, Filtering::colorFilmGrain, Filtering::greyFilmGrain};
}

#endif // GRAPHICS_H
