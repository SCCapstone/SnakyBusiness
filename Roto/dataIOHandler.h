#ifndef DATAIO_H
#define DATAIO_H

#include <QImageReader>
#include <QFileDialog>      // import should be within MainWindow.h for now.
#include <QImage>

#include <graphics.h>

#include <list>

using std::list;
using std::pair;

using graphics::Filter;

const int defaultSize = 700;

struct RGB {
    uchar blue;
    uchar green;
    uchar red;
};

class DataIOHandler {
public:
    DataIOHandler();
    //DataIOHandler(QImage * qi);
    ~DataIOHandler();
    bool importImage(QString fileName);
    void exportImage(QString fileName);
    bool importVideo(QString fileName);
    void exportVideo(QString fileName);
    void save(QString projectName);
    void load(QString projectName);
    void setScreenFilter(string filterName);
    void setFilterStrength(int strength);
    int getFilterIndex();
    int getFilterStrength();
    QImage *getCanvasLayer();
    QImage *getMediaLayer();
    QImage getFilteredMLayer();
    QSize getBounds();
    QSize fullBounds();
    void scale(int option1, int option2);

    void setMediaLayer (QImage qi);

private:
    void scaleLayers(int option1, int option2);
    void scaleLists(int layer, int scaleType);
    void applyFilter();

    QImage *canvasLayer, *mediaLayer, filteredMLayer;
    list <QImage *> canvasLeft, canvasRight, mediaLeft, mediaRight;
    QString file;
    Filter screenFilter;    
};

#endif // DATAIO_H
