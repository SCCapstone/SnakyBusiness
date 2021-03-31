#ifndef DATAIO_H
#define DATAIO_H

#include <thread>
#include <vector>
#include <list>
#include <QImageReader>
#include <QFileDialog>
#include <QImage>
#include <QPainter>
#include <layer.h>
#include <graphics.h>

using std::thread;
using std::vector;
using std::list;
using std::pair;
using std::iter_swap;
using std::swap;
using graphics::Filter;

enum scaleType {dontScale, bestFit, aspectRatio, toWidth, toHeight};
enum importType {image, video};

struct RGB {
    uchar blue;
    uchar green;
    uchar red;
};

class DataIOHandler {

public:

    DataIOHandler();
    ~DataIOHandler();

    static void renderFrame(QImage *ret, vector <Layer *> layers);
    static void renderLayer(QImage *toProcess, int alpha, Filter filter, vector <SplineVector> vects, vector<list <Triangle> > tris = vector <list <Triangle> > ());
    static void calcLine(SplineVector sv, list <Triangle> *tris);
    static void fillTri(QImage *toProcess, Triangle t, QRgb color);
    static void fillTriSafe(QImage *toProcess, Triangle t, QRgb color);
    static void filterTri(QImage *toProcess, Triangle t, Filter f);
    static void filterTriSafe(QImage *toProcess, Triangle t, Filter f);
    static void fillBTriSafe(QImage *toProcess, QPoint a, QPoint b, QPoint c, QRgb color);
    static void fillTTriSafe(QImage *toProcess, QPoint a, QPoint b, QPoint c, QRgb color);
    static void filterBTriSafe(QImage *toProcess, QPoint a, QPoint b, QPoint c, Filter f);
    static void filterTTriSafe(QImage *toProcess, QPoint a, QPoint b, QPoint c, Filter f);
    static void fillBTri(QImage *toProcess, QPoint a, QPoint b, QPoint c, QRgb color);
    static void fillTTri(QImage *toProcess, QPoint a, QPoint b, QPoint c, QRgb color);
    static void filterBTri(QImage *toProcess, QPoint a, QPoint b, QPoint c, Filter f);
    static void filterTTri(QImage *toProcess, QPoint a, QPoint b, QPoint c, Filter f);

    void setDims(QSize size);
    QSize getdims();
    void scale(scaleType option);
    bool importImage(QString fileName);
    void exportImage(QString fileName);
    void compileLayer();
    void compileFrame();
    void setActiveLayer(int i, EditMode mode);
    int getNumLayers();
    int getActiveLayer();
    bool wasUpdated();
    void copyVectors();
    void cutVectors();
    void deleteVectors();
    void pasteVectors();
    void copyRaster();
    void cutRaster();
    void deleteRaster();
    void pasteRaster();
    void addLayer();
    void copyLayer();
    void pasteLayer();
    void deleteLayer();
    void moveBackward();
    void moveForward();
    void moveToBack();
    void moveToFront();

    /*
    void addFrame(int num);
    void duplicateFrame(int count);
    void copyFrames(int first, int last = -1);
    void pasteFrames(int after);
    void removeFrames(int first, int last = -1);
    // Update the menu file to match.
    */


    // almost everything below is outdated
    bool importVideo(QString fileName);
    void exportVideo(QString fileName);
    void save(QString projectName);
    void load(QString projectName);
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
    unsigned char activeLayer, activeFrame;
    QImage importImg, rasterCopySlot;
    importType importType;
    bool updated;
    double angleCopySlot;
    pair <QPoint, QPoint> boundCopySlot;
};

#endif // DATAIO_H

