#ifndef DATAIO_H
#define DATAIO_H

#include <QImageReader>
#include <QFileDialog>
#include <QImage>
#include <mainwindow.h>

class DataIOHandler {
public:

    DataIOHandler();
    DataIOHandler(QImage * file);
    ~DataIOHandler();

    static QImage * loadImage();
    QImage * loadImage(QImage * file);
    static void saveImage(QString saveFileName, QImage * file);
    void saveImageAs(QImage * file);
private:
    QString saveFileName;
};

#endif // DATAIO_H
