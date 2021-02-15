#include "dataIOHandler.h"


DataIOHandler::DataIOHandler() {
    activeFrame = 0;
    activeLayer = 0;
    Layer *l = new Layer(defaultSize);
    vector <Layer *> frame;
    frame.push_back(l);
    frames.push_back(frame);
}

DataIOHandler::~DataIOHandler() {
    for (size_t i = 0; i < frames.size(); ++i) {
        for (size_t j = 0; j < frames[i].size(); ++j)
            delete frames[i][j];
        frames[i].clear();
    }
}

Layer * DataIOHandler::getWorkingLayer() {
    return frames[activeFrame][activeLayer];
}

QImage DataIOHandler::getBackground() {
    if (activeLayer == 0)
        return QImage();
    vector <Layer *> layers = frames[activeFrame];
    QImage qi = *layers[0]->getCanvas();
    QPixmap qpix;
    QPainter p;
    p.begin(&qi);
    for (size_t i = 0; i < activeLayer; ++i)
        p.drawImage(0, 0, *layers[i]->getCanvas());
    p.end();
    return qi;
}

QImage DataIOHandler::getForeground() {
    vector <Layer *> layers = frames[activeFrame];
    if (activeLayer == layers.size() - 1)
        return QImage();
    QImage qi = *layers[activeLayer + 1]->getCanvas();
    QPixmap qpix;
    QPainter p;
    p.begin(&qi);
    for (size_t i = activeLayer + 2; i < layers.size(); ++i)
        p.drawImage(0, 0, *layers[i]->getCanvas());
    p.end();
    return qi;
}

void DataIOHandler::setScreenFilter(string filterName) {
    screenFilter.setFilter(filterName);
    applyFilter();
}

void DataIOHandler::setFilterStrength(int strength) {
    screenFilter.setStrength(strength);
}

int DataIOHandler::getFilterIndex() {
    return screenFilter.getFilterIndex();
}

int DataIOHandler::getFilterStrength() {
    return screenFilter.getStrength();
}

void DataIOHandler::applyFilter() {

}

bool DataIOHandler::importImage(QString fileName) {
//    file = fileName;
//    QImage qiTemp(fileName);
//    bool askResize = mediaLayer->size() != qiTemp.size();
//    delete mediaLayer;
//    mediaLayer = new QImage(qiTemp.convertToFormat(QImage::Format_ARGB32_Premultiplied));
//    return askResize;
    // tf keep aspect ratio, scale import to draw, scale draw to import, do nothing.
    // scale to longest or shortest side.
    return false;
}

void DataIOHandler::scale(int layer, int scaleType) {
    scaleLayers(layer, scaleType);
    scaleLists(layer, scaleType);
    applyFilter();
}

void DataIOHandler::scaleLayers(int layer, int scaleType) {
//    if (scaleType != 0) {
//        if (layer == 0) {
//            QImage copy = canvasLayer->copy();
//            delete canvasLayer;
//            switch (scaleType) {
//            case 1:
//                canvasLayer = new QImage(copy.scaledToWidth(mediaLayer->width(), Qt::SmoothTransformation));
//            case 2:
//                canvasLayer = new QImage(copy.scaledToWidth(mediaLayer->height(), Qt::SmoothTransformation));
//            case 3:
//                canvasLayer = new QImage(copy.scaled(mediaLayer->width(), mediaLayer->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//            }
//        }
//        else if (layer == 1) {
//            QImage copy = mediaLayer->copy();
//            delete mediaLayer;
//            switch (scaleType) {
//            case 1:
//                mediaLayer = new QImage(copy.scaledToWidth(canvasLayer->width(), Qt::SmoothTransformation));
//            case 2:
//                mediaLayer = new QImage(copy.scaledToWidth(canvasLayer->height(), Qt::SmoothTransformation));
//            case 3:
//                mediaLayer = new QImage(copy.scaled(canvasLayer->width(), canvasLayer->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//            }
//        }
//    }
}

void DataIOHandler::scaleLists(int layer, int scaleType) {
    // use concurrence
}

void DataIOHandler::exportImage(QString fileName) {

}

bool DataIOHandler::importVideo(QString fileName) {
    file = fileName;
    return false;
}

void DataIOHandler::exportVideo(QString fileName) {

}

