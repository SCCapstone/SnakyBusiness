#include "dataIOHandler.h"
#include "mainwindow.h"         // not needed
#include "ui_mainwindow.h"      // not needed

/* It seems the functions were made with the idea of a save file in mind, or perhap I am incorrect. However,
 * If you also want to commits from writing the saving feature, it'd be quite easy. I've detailed it in the saveLoad.txt.
 * I'll upload it to the repo / github.
 */

QImage * DataIOHandler::loadImage() {       
    /* Should be non-static, and take QString parameter filename. importImage would be a more accurate name.
     * This function should have no return type as the as the QImage pointer that is drawn to the screen
     * Should actually be a private member of the dataIO_handler class, as should the draw layer. Ideally,
     * there should be a single QImage pointer for displaying the import media, one for displaying the
     * draw layer, two std::list <QImage *> for the draw frames on either side on either side of the draw
     * layer when importing video, and similary two more lists of the same type for the import media.
     *
     * should have a bool flag that alerts the front end to whether there is already import media, if so, ask if the user
     * wants to replace the current media with what they intend to import.
     * eventually, when we have save files, we'll want to ask the user if they want to load the media upon loading the save file,
     * as they may instead be in the final stages of work, where they no longer need it - saving ram.
     */
    QStringList fileName = QFileDialog::getOpenFileNames(nullptr, "Open File", "/home", "Images (*.png *.xpm *.jpg");
    /* The call about should be QFileDialog::getOpenFileName
     * The call should be made in the MainWindow.cpp, and it's output (QString) should be fed to this method.
     * The nullptr should be 'this', so that it references the qmainwindow qwidget as a parent.
     * Eventually, this will need to be generalized such that if the file is an image file, it 
     * calls an image loader function (like this one) otherwise, it should call the video loader if it is a
     * video.
     *
     * Also, I belive the typing call with "Images (*.png ..." is ill formated, and the quotes go on the outside of the parenthetical
     */
    QImage *Image = new QImage();
    QString path = fileName.join("");
    Image->QImage::load(path);
    return Image;
    /*The four lines above can be replaced with:
     * QImage temp(filename)
     * return new QImage(temp.convertToFormat(QImage::Format_ARGB32_Premultiplied));  
     */
    // on the mainwindow.cpp end, if there is no save file, the window title should be updated. unconditionally update import media name string.
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

/* The function below is not needed as the data handler should be a container for
 * either a singular image, or video. It should also have active frames and the
 * undo and redo stack(s). It should also have the string name of the import media,
 * used in the save function below.
 */
DataIOHandler::DataIOHandler(QImage * file) {

    loadImage(file);

}

// non static method
void DataIOHandler::saveImage(QString saveFileName, QImage * file) {        // exportImage would be a more accurate name. QImage param not needed
    
    /* similar to the load function above, the commented-out call below call too should be in MainWindow.cpp with the nullptr replaced.
     * since this exports a still image, and not a video, it should export the active draw layer mentioned in load function above in the
     * same way it already does with the provided QString.
     */
    
    //if (saveFileName.isEmpty()) {
    //    saveFileName = QFileDialog::getSaveFileName(nullptr, "Save Image", QString(), "Images (*.png)");
    //}
    file->save(saveFileName);
    //file->save(saveFileName);
}



// update media namestring and window title when saving a save file, but not the image.

// This function below is not needed.
void DataIOHandler::saveImageAs(QImage * file) {
    saveFileName = QFileDialog::getSaveFileName(nullptr, "Save Image As", QString(), "Images (*.png)");

    file->save(saveFileName);
}
