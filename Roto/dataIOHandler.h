#ifndef DATAIO_H
#define DATAIO_H

#include <QImageReader>
#include <QFileDialog>      // import should be within MainWindow.h for now.
#include <QImage>

class DataIOHandler {
public:

    // CHECK CPP FILE
    
    DataIOHandler();
    DataIOHandler(QImage * file);   // not needed
    ~DataIOHandler();

    static QImage * loadImage();    //  non static, QString parameter needed.
    QImage * loadImage(QImage * file);      //not needed.
    static void saveImage(QString saveFileName, QImage * file);     // remove QString parameter.
    void saveImageAs(QImage * file);        // Add QString parameter.
    
private:
    QString saveFileName;
};

#endif // DATAIO_H
