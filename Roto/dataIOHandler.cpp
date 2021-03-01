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

void DataIOHandler::addLayer(QImage qi) {
    if (frames.empty())
        frames.push_back(vector <Layer *> ());
    Layer *layer = qi.isNull() ? new Layer(dims) : new Layer(qi, 255);
    frames[activeFrame].push_back(layer);
    correctRender();
}

void DataIOHandler::copyLayer() {
    layerCopySlot = Layer(*frames[activeFrame][activeLayer]);
    correctRender();
}

void DataIOHandler::pasteLayer() {
    if (layerCopySlot.getCanvas()->isNull())
        return;
    activeLayer = frames[activeFrame].size();
    frames[activeFrame].push_back(new Layer(layerCopySlot));
    correctRender();
}

void DataIOHandler::deleteLayer() {
    delete frames[activeFrame][activeLayer];
    frames[activeFrame].erase((frames[activeFrame].begin() + activeFrame));
    if (activeLayer == frames[activeFrame].size()) {
        if (frames[activeFrame].size() == 0)
            deleteFrame();
        else {
            --activeLayer;
            correctRender();
        }
    }
}

void DataIOHandler::moveBackward() {
    if (activeLayer != 0)
        iter_swap(frames[activeFrame].begin() + (activeLayer - 1), frames[activeFrame].begin() + activeLayer);
}

void DataIOHandler::moveForward() {
    if (activeLayer != frames[activeFrame].size() - 2)
        iter_swap(frames[activeFrame].begin() + activeLayer, frames[activeFrame].begin() + (activeLayer + 1));
}

void DataIOHandler::deleteFrame() {
    // set new active layer to the new frame's number of layers - 1
    //check if last / only frame
    correctRender();
}

void DataIOHandler::correctRender() {

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

void DataIOHandler::copyVectors() {
    vectorCopySlot.clear();
    Layer *layer = getWorkingLayer();
    vector <unsigned char> activeVectors = layer->getActiveVectors();
    vector <SplineVector> vects = layer->getVectors();
    for (unsigned char i : activeVectors)
        vectorCopySlot.push_back(vects[i]);
}

void DataIOHandler::cutVectors() {
    copyVectors();
    deleteVectors();
}

void DataIOHandler::deleteVectors() {
    getWorkingLayer()->deleteSelected();
}

void DataIOHandler::pasteVectors() {
    getWorkingLayer()->pasteVectors(vectorCopySlot);
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
    //call add layer
    // ask whether to resize to project res by keeping scale or not
    // in a future version, a class needs to exist, similar to splinevector, that renders the image and treats it as an object.




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

