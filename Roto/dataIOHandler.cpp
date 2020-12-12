#include "dataIOHandler.h"
#include <iostream>

using namespace std;

DataIOHandler::DataIOHandler() {
    // isDefaulted = true;
    canvasLayer = new QImage(QSize(defaultSize, defaultSize), QImage::Format_ARGB32_Premultiplied);
    mediaLayer = new QImage(QSize(defaultSize, defaultSize), QImage::Format_ARGB32_Premultiplied);
    for (int i = 0; i < defaultSize; ++i)
        for (int j = 0; j < defaultSize; ++j) {
            canvasLayer->setPixel(i, j, 0xFFFFFFFF);
            mediaLayer->setPixel(i, j, 0xFFFFFFFF);
        }
    applyFilter();
}

DataIOHandler::~DataIOHandler() {
    delete canvasLayer;
    delete mediaLayer;
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
    filteredMLayer = mediaLayer->copy();
    screenFilter.applyTo(&filteredMLayer);
}

bool DataIOHandler::importImage(QString fileName) {
    file = fileName;
    QImage qiTemp(fileName);
    bool askResize = mediaLayer->size() != qiTemp.size();
    delete mediaLayer;
    mediaLayer = new QImage(qiTemp.convertToFormat(QImage::Format_ARGB32_Premultiplied));
    return askResize;
    // tf keep aspect ratio, scale import to draw, scale draw to import, do nothing.
    // scale to longest or shortest side.
}

void DataIOHandler::scale(int layer, int scaleType) {
    scaleLayers(layer, scaleType);
    scaleLists(layer, scaleType);
    applyFilter();
}

void DataIOHandler::scaleLayers(int layer, int scaleType) {
    if (scaleType != 0) {
        if (layer == 0) {
            QImage copy = canvasLayer->copy();
            delete canvasLayer;
            switch (scaleType) {
            case 1:
                canvasLayer = new QImage(copy.scaledToWidth(mediaLayer->width(), Qt::SmoothTransformation));
            case 2:
                canvasLayer = new QImage(copy.scaledToWidth(mediaLayer->height(), Qt::SmoothTransformation));
            case 3:
                canvasLayer = new QImage(copy.scaled(mediaLayer->width(), mediaLayer->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            }
        }
        else if (layer == 1) {
            QImage copy = mediaLayer->copy();
            delete mediaLayer;
            switch (scaleType) {
            case 1:
                mediaLayer = new QImage(copy.scaledToWidth(canvasLayer->width(), Qt::SmoothTransformation));
            case 2:
                mediaLayer = new QImage(copy.scaledToWidth(canvasLayer->height(), Qt::SmoothTransformation));
            case 3:
                mediaLayer = new QImage(copy.scaled(canvasLayer->width(), canvasLayer->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            }
        }
    }
}

void DataIOHandler::scaleLists(int layer, int scaleType) {
    // use concurrence
}

void DataIOHandler::exportImage(QString fileName) {
    canvasLayer->save(fileName);
}

bool DataIOHandler::importVideo(QString fileName) {
    file = fileName;
    return false;
}

void DataIOHandler::exportVideo(QString fileName) {
    canvasLayer->save(fileName);
}

QImage *DataIOHandler::getCanvasLayer() {
    return canvasLayer;
}

QImage *DataIOHandler::getMediaLayer() {
    return mediaLayer;
}

QImage DataIOHandler::getFilteredMLayer() {
    return filteredMLayer;
}

QSize DataIOHandler::getBounds() {
    return canvasLayer->size();
}

QSize DataIOHandler::fullBounds() {
    QSize qs;
    qs.setWidth(max(canvasLayer->width(), mediaLayer->width()));
    qs.setHeight(max(canvasLayer->height(), mediaLayer->height()));
    return qs;
}

void DataIOHandler::setMediaLayer(QImage qi) {
    delete mediaLayer;
    mediaLayer = new QImage(qi);
    filteredMLayer = qi;
}

// update media namestring and window title when saving a save file, but not the image.
