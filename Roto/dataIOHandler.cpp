#include "dataIOHandler.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

QImage * DataIOHandler::loadImage() {
    QStringList fileName = QFileDialog::getOpenFileNames(nullptr, "Open File", "/home", "Images (*.png *.xpm *.jpg");
    QImage *Image = new QImage();
    QString path = fileName.join("");
    Image->QImage::load(path);

    return Image;

}

QImage * DataIOHandler::loadImage(QImage * file) {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Image", "/home", "Image Files (*.png *.jpg *.bmp)");
    delete file;
    QImage fileTemp(fileName);
    file = new QImage(fileTemp.convertToFormat(QImage::Format_ARGB32_Premultiplied));

    return file;
}

DataIOHandler::DataIOHandler() {

    //loadImage();

}

DataIOHandler::DataIOHandler(QImage * file) {

    loadImage(file);

}

void DataIOHandler::saveImage(QString saveFileName, QImage * file) {
    //if (saveFileName.isEmpty()) {
    //    saveFileName = QFileDialog::getSaveFileName(nullptr, "Save Image", QString(), "Images (*.png)");
    //}
    file->save(saveFileName);
    //file->save(saveFileName);
}

void DataIOHandler::saveImageAs(QImage * file) {
    saveFileName = QFileDialog::getSaveFileName(nullptr, "Save Image As", QString(), "Images (*.png)");

    file->save(saveFileName);
}
