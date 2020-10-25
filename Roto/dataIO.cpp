#include "dataIO.h"

QImage * DataIOHandler::loadImage() {
    QStringList fileName = QFileDialog::getOpenFileNames(nullptr, "Open File", "/home", "Images (*.png *.xpm *.jpg");
    QImage *Image = new QImage();
    QString path = fileName.join("");
    Image->QImage::load(path);

    return Image;

}

DataIOHandler::DataIOHandler() {

    loadImage();

}

void DataIOHandler::saveImage(QImage * file) {
    QString saveFileName = QFileDialog::getSaveFileName(nullptr, "Save Image", QString(), "Images (*.png");

    file->save(saveFileName);
}
