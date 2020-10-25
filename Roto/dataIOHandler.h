#ifndef DATAIO_H
#define DATAIO_H

#include <QImageReader>
#include <QFileDialog>
#include <QImage>

class DataIOHandler {
public:

    DataIOHandler();
    ~DataIOHandler();

    QImage * loadImage();
    void saveImage(QImage * file);
};

#endif // DATAIO_H
