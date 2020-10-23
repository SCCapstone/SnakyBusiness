#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QImage>
#include <string>
#include <functional>
#include <stdfuncs.h>

using std::string;
using std::function;


namespace graphics {

const string filterNames[] = {"Normal (rgb)", "Greyscale", "Polarize", "Negative"};
const int filterPresets[] =  {0             , 255        , 128         , 0       };
const int numFilters = 4;

const double minZoom = 0.01;
const double maxZoom = 8.0;
const int minColor = 0;
const int maxColor = 255;

class Filter {
public:

    Filter(int strength = filterPresets[0], string filterName = filterNames[0]);
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
    static QRgb toRGB (int r, int g, int b);
    static QRgb greyscale (QColor qc, int strength);
    static QRgb polarize (QColor qc, int strength);
    static QRgb negative (QColor qc, int strength);
    static QRgb rgb (QColor qc, int strength);

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

private:

    static int getSize(double dim, double zoom);

    double zoom;
};

// TODO update names

const function <QRgb (QColor, int)> filters[] = {Filtering::rgb, Filtering::greyscale, Filtering::polarize, Filtering::negative};

}
#endif // GRAPHICS_H
