#ifndef DATAIO_H
#define DATAIO_H

#include <QImageReader>
#include <QFileDialog>      // import should be within MainWindow.h for now.
#include <QImage>

class DataIOHandler {
public:
    QImage * Image;
    // CHECK CPP FILE
    
    DataIOHandler();
    DataIOHandler(QImage * qi);
    ~DataIOHandler();

    QImage * loadImage(QString file);    //  non static, QString parameter needed.
    void saveImage(QString saveFileName);     // non static, remove QImage parameter.
    
    // FUNCTIONS FOR THE UI TO GET THE DRAW LAYER AND ACTIVE MEDIA LAYER. SEE CPP FILE FOR CLARIFICATION.
    
private:
    QString saveFileName;
    //QImage * Image;
};

#endif // DATAIO_H
