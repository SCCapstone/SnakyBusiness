#ifndef DATAIO_H
#define DATAIO_H

#include <QImageReader>
#include <QFileDialog>
#include <QImage>

class DataIOHandler {
public:

    DataIOHandler();
    ~DataIOHandler();

    QImage * loadFile();
    void saveFile(QImage * file);
};

#endif // DATAIO_H
