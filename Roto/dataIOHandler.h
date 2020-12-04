#ifndef DATAIO_H
#define DATAIO_H

#include <QImageReader>
#include <QFileDialog>      // import should be within MainWindow.h for now.
#include <QImage>

class DataIOHandler {
public:
    // CHECK CPP FILE
    
    DataIOHandler();
    //DataIOHandler(QImage * qi);
    ~DataIOHandler();

    QImage * importImage(QString file);    //  non static, QString parameter needed.
    void exportImage(QString saveFileName);     // non static, remove QImage parameter.

    QImage * getBaseLayer();
    void setBaseLayer(QImage * newImage);
    
    // FUNCTIONS FOR THE UI TO GET THE DRAW LAYER AND ACTIVE MEDIA LAYER. SEE CPP FILE FOR CLARIFICATION.
    
private:
    QImage * Image;
    QString saveFileName;
    std::list<QImage *> leftFrames;
    std::list<QImage *> rightFrames;
};

#endif // DATAIO_H
