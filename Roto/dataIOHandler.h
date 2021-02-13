#ifndef DATAIO_H
#define DATAIO_H

#include <QImageReader>
#include <QFileDialog>
#include <QImage>
#include <QPainter>

#include <graphics.h>

#include <list>
#include <vector>
#include <layer.h>

using std::list;
using std::pair;

using graphics::Filter;

const QSize defaultSize (700, 700);

struct RGB {
    uchar blue;
    uchar green;
    uchar red;
};

class DataIOHandler {
public:
    DataIOHandler();
    ~DataIOHandler();
    /*
    void addLayer();
    void copyLayers(int first, int last = -1);
    void pastLayers(int after);
    void removeLayers(int first, int last = -1);
    void addFrame(int num);
    void duplicateFrame(int count);
    void copyFrames(int first, int last = -1);
    void pasteFrames(int after);
    void removeFrames(int first, int last = -1);
    // Update the menu file to match.
    */
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
    Layer *getWorkingLayer();
    QImage getBackground();
    QImage getForeground();
    void scale(int option1, int option2);

private:
    void scaleLayers(int option1, int option2);
    void scaleLists(int layer, int scaleType);
    void applyFilter();

    vector <vector <Layer *> > frames;
    QString file;
    Filter screenFilter;
    unsigned char activeLayer, activeFrame;
};

#endif // DATAIO_H
