#include "dataIOHandler.h"

/* It seems the functions were made with the idea of a save file in mind, or perhap I am incorrect. However,
 * If you also want to commits from writing the saving feature, it'd be quite easy. I've detailed it in the saveLoad.txt.
 * I'll upload it to the repo / github.
 */

QImage * DataIOHandler::importImage(QString file) {
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
    /* The call about should be QFileDialog::getOpenFileName
     * The call should be made in the MainWindow.cpp, and it's output (QString) should be fed to this method.
     * The nullptr should be 'this', so that it references the qmainwindow qwidget as a parent.
     * Eventually, this will need to be generalized such that if the file is an image file, it 
     * calls an image loader function (like this one) otherwise, it should call the video loader if it is a
     * video.
     *
     * Also, I belive the typing call with "Images (*.png ..." is ill formated, and the quotes go on the outside of the parenthetical
     */
    QImage temp(file);
    return new QImage(temp.convertToFormat(QImage::Format_ARGB32_Premultiplied));
}

DataIOHandler::DataIOHandler() {

    this->Image = nullptr;

}

QImage * DataIOHandler::getBaseLayer() {

    return this->Image;

}

void DataIOHandler::setBaseLayer(QImage *newImage) {

    this->Image = newImage;

}

DataIOHandler::~DataIOHandler() {

    saveFileName.QString::~QString();
    Image->QImage::~QImage();
    leftFrames.~list();
    rightFrames.~list();

}

//DataIOHandler::DataIOHandler(QImage * qi) {
//
//    this->Image = qi;
//
//}

// non static method
void DataIOHandler::exportImage(QString saveFileName) {        // exportImage would be a more accurate name. QImage param not needed
    
    /* similar to the load function above, the commented-out call below call too should be in MainWindow.cpp with the nullptr replaced.
     * since this exports a still image, and not a video, it should export the active draw layer mentioned in load function above in the
     * same way it already does with the provided QString.
     */
    Image->save(saveFileName);
}



// update media namestring and window title when saving a save file, but not the image.
