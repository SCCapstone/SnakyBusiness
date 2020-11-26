#include "dataIOHandler.h"
#include "mainwindow.h"         // not needed
#include "ui_mainwindow.h"      // not needed

QImage * DataIOHandler::loadImage() {       // should be static, and take QString parameter filename
    QStringList fileName = QFileDialog::getOpenFileNames(nullptr, "Open File", "/home", "Images (*.png *.xpm *.jpg");
    /* The call about should be QFileDialog::getOpenFileName
     * The call should be made in the MainWindow.cpp, and it's output (QString) should be fed to this method.
     * The nullptr should be 'this', so that it references the qmainwindow qwidget as a parent.
     * Eventually, this will need to be generalized on the other end such that if the file is an image file, it 
     * calls and image loader function (like this one) otherwise, it should call the video loader is it is a
     * video.
     */
    QImage *Image = new QImage();
    QString path = fileName.join("");
    Image->QImage::load(path);
    return Image;
    /*The four lines above can be replaced with:
     * QImage temp(filename)
     * return new QImage(temp.convertToFormat(QImage::Format_ARGB32_Premultiplied));  
     */
    // on the mainwindow.cpp end, the window title should be updated. update media name string.
}

// The function below is not needed
QImage * DataIOHandler::loadImage(QImage * file) {
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Image", "/home", "Image Files (*.png *.jpg *.bmp)");
    delete file;
    QImage fileTemp(fileName);
    file = new QImage(fileTemp.convertToFormat(QImage::Format_ARGB32_Premultiplied));

    return file;
}

DataIOHandler::DataIOHandler() {

    //loadImage();      // not needed

}

// The function below is not needed as the data handler should be a container for
// either a singular image, or video. It should also have active frames and the
// undo and redo stack(s). It should also have the string name of the import media,
// used in the save function below.
DataIOHandler::DataIOHandler(QImage * file) {

    loadImage(file);

}

// does not need the QString parameter. non static method
void DataIOHandler::saveImage(QString saveFileName, QImage * file) {
    //if (saveFileName.isEmpty()) {
    //    saveFileName = QFileDialog::getSaveFileName(nullptr, "Save Image", QString(), "Images (*.png)");
    //}
    file->save(saveFileName);
    //file->save(saveFileName);
}

// This function should be the one to take an extra parameter - a QString for the new file name. non static method
void DataIOHandler::saveImageAs(QImage * file) {
    // similar to the load function aboce, this call too should be in MainWindow.cpp with the nullptr replaced.
    saveFileName = QFileDialog::getSaveFileName(nullptr, "Save Image As", QString(), "Images (*.png)");

    file->save(saveFileName);
    // update media namestring and window title
}
