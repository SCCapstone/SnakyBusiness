#ifndef DATAIO_H
#define DATAIO_H

#include <thread>
#include <vector>
#include <list>
#include <atomic>
#include <mutex>
#include <fstream>
#include <QImageReader>
#include <QFileDialog>
#include <QProgressDialog>
#include <QImage>
#include <QPainter>
#include <layer.h>
#include <graphics.h>
#include <base85.h>
#include <string>

using std::thread;
using std::vector;
using std::list;
using std::pair;
using std::iter_swap;
using std::swap;
using std::atomic_int;
using std::mutex;
using std::ofstream;
using std::ifstream;
using std::getline;
using graphics::Filter;

enum scaleType {dontScale, bestFit, aspectRatio, toWidth, toHeight};
enum importType {image, video};

struct RGB {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
};

static mutex locker;
static atomic_int progressMarker;
const unsigned char maxLayer = 64;

class DataIOHandler {

public:

    DataIOHandler(QProgressDialog *progress);
    ~DataIOHandler();
    static void renderFrame(QProgressDialog *fqpd, QImage *ret, vector <Layer *> layers);
    static void renderLayer(QProgressDialog *fqpd, QProgressDialog *qpd, QImage *toProcess, int alpha, Filter filter, vector <SplineVector> vects, vector<list <Triangle> > tris = vector <list <Triangle> > ());
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
    Layer *getWorkingLayer();
    QImage getBackground();
    QImage getForeground();
    void save(QString projectName);
    int load(QString projectName);
    void clearFrame();
    vector <Layer *> backup();

    vector<int> findPoints(QImage *qi);
    void vectorCheck(SplineVector sv);

private:

    QSize dims;
    vector <Layer *> frame;
    list <SplineVector> vectorCopySlot;
    Layer layerCopySlot;
    QString file;
    unsigned char activeLayer;
    QImage importImg, rasterCopySlot;
    importType importType;
    bool updated;
    double angleCopySlot;
    pair <QPoint, QPoint> boundCopySlot;
    QProgressDialog *progress;
};

#endif // DATAIO_H
