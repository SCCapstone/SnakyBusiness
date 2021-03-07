#ifndef DATAIO_H
#define DATAIO_H

#include <vector>
#include <list>
#include <QImageReader>
#include <QFileDialog>
#include <QImage>
#include <QPainter>
#include <layer.h>
#include <graphics.h>

using std::vector;
using std::list;
using std::pair;
using std::iter_swap;
using graphics::Filter;

enum scaleType {dontScale, bestFit, aspectRatio, toWidth, toHeight};
enum importType {image, video};

const QSize defaultSize (1200, 900);

struct RGB {
    uchar blue;
    uchar green;
    uchar red;
};

class DataIOHandler {

public:

    DataIOHandler();
    ~DataIOHandler();
    void setActiveLayer(int i);
    int getNumLayers();
    int getActiveLayer();
    bool wasUpdated();
    void copyVectors();
    void cutVectors();
    void deleteVectors();
    void pasteVectors();
    void addLayer(QImage qi = QImage());
    void copyLayer();
    void pasteLayer();
    void deleteLayer();
    void moveBackward();
    void moveForward();

    void deleteFrame();
    /*
    void addFrame(int num);
    void duplicateFrame(int count);
    void copyFrames(int first, int last = -1);
    void pasteFrames(int after);
    void removeFrames(int first, int last = -1);
    // Update the menu file to match.
    */

    void scale(scaleType option);

    // almost everything below is outdated
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


private:

    void scaleLayers(int option1, int option2);
    void scaleLists(int layer, int scaleType);
    void applyFilter();

    QSize dims;
    vector <vector <Layer *> > frames;
    list <SplineVector> vectorCopySlot;
    Layer layerCopySlot;
    QString file;
    Filter screenFilter;
    unsigned char activeLayer, activeFrame;
    QImage importImg;
    importType importType;
    bool updated;
};

#endif // DATAIO_H

