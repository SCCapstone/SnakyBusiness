#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "opencv_handler.h"
#include <iostream>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(0,0, len, len);
    setWindowTitle("Title");
    createMenubar("mainMenubar");
    QMenu* sFiltering = static_cast<QMenu *>(objFetch.at("Screen Filtering"));
    QMenu* bFiltering = static_cast<QMenu *>(objFetch.at("Brush Filtering"));
    for (string name : filterNames) {
        QAction *sAction = sFiltering->addAction((name).c_str());
        QAction *bAction = bFiltering->addAction((name).c_str());
        connect(sAction, &QAction::triggered, this, [=]() { this->changeScreenFilter(sAction->text().toStdString()); });
        connect(bAction, &QAction::triggered, this, [=]() { this->changeBrushFilter(bAction->text().toStdString()); });
        log(name, sAction);
        log(name, bAction);
    }
    QMenu* bShape = static_cast<QMenu *>(objFetch.at("Brush Shape"));
    for (string name : brushShapes) {
        QAction *bAction = bShape->addAction((name).c_str());
        connect(bAction, &QAction::triggered, this, [=]() { this->changeBrushShape(bAction->text().toStdString()); });
        log(name, bAction);
    }
    QMenu* bMethod = static_cast<QMenu *>(objFetch.at("Brush Method"));
    for (string name : appMethods) {
        QAction *bAction = bMethod->addAction((name).c_str());
        connect(bAction, &QAction::triggered, this, [=]() { this->changeBrushMethod(bAction->text().toStdString()); });
        log(name, bAction);
    }
    qi = new QImage(len, len, QImage::Format_ARGB32_Premultiplied);
    for (int i = 0; i < len; ++i)
        for (int j = 0; j < len; ++j)
            qi->setPixel(i, j, 0xFFFFFFFF);
    sampleFlag = 0;
    sampleFlasher = new QTimer(this);
    connect(sampleFlasher, SIGNAL(timeout()), this, SLOT(toggleSamplePnt()));
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {

    QPoint qp = imgSupport.getZoomCorrected(event->pos());
    if (lastButton == Qt::LeftButton) {
        if (trackDrawSpeed) {
            auto tim = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
            long long t1 = tim.count();
            bh.applyBrush(qi, qp);
            tim = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
            long long t2 = tim.count();
            cout << (t2 - t1) << endl;
        }
        else
            bh.applyBrush(qi, qp);
    }
    else if (lastButton == Qt::RightButton)
        bh.setSamplePoint(qp);
    repaint();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QPoint qp = imgSupport.getZoomCorrected(event->pos());
    lastButton = event->button();
    if(lastButton == Qt::RightButton) {
        bh.setSamplePoint(qp);
    }
    else if (lastButton == Qt::LeftButton) {
        bh.setRelativePoint(qp);
        bh.applyBrush(qi, qp);
    }
    repaint();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {

}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {

}

void MainWindow::createMenubar(string filename) {

    QMenuBar *menubar = new QMenuBar(this);
//    log(filename + UI_FileType, menubar);
    fstream uiFile;
    uiFile.open(filename+".txt",ios::in);
    if (uiFile.is_open()){
        string fromFile;
        while(getline(uiFile, fromFile)) {
            string item = "";
            size_t i;
            for (i = 0; fromFile[i] != ';'; ++i)
                item += fromFile[i];
            QMenu *menu = menubar->addMenu(item.c_str());
            log(item, menu);
            ++i;
            fromFile = fromFile.substr(i, fromFile.length() - i);
            addItems(menu, fromFile);
        }
        uiFile.close();
    }
}

void MainWindow::addItems(QMenu *menu, string menuItems) {

    string item = "";
    for (size_t i = 0; i < menuItems.length(); ++i) {
        if (menuItems[i] == ',') {
            addAction(menu, item);
            item = "";
        }
        else if (menuItems[i] == '>') {
            QMenu *subMenu = menu->addMenu(item.c_str());
            log(item, subMenu);
            if (menuItems[i + 1] == '(') {
                string subItems = "";
                int flag = 0;
                for (i += 2; true; ++i) {
                    subItems += menuItems[i];
                    if (menuItems[i] == '(')
                        ++flag;
                    else if (menuItems[i] == ')') {
                        if (flag == 0)
                            break;
                        --flag;
                    }
                }
                addItems(subMenu, subItems);
                ++i;
            }
            else
                ++i;
            item = "";
        }
        else
            item += menuItems[i];
    }
}

void MainWindow::addAction(QMenu *menu, string item) {

    QAction *action = menu->addAction(item.c_str());
    connect(action, &QAction::triggered, this, [=]() { this->doSomething(item); });
    log(item, action);
}

void MainWindow::doSomething(string btnPress) {

    cout << btnPress << endl;
    // process the actions here

    // first thing to do is load test images
    // https://doc.qt.io/qt-5/qfiledialog.html
    // for our custom dialogs it looks as though we must use the QDialog or QWidget classes to add components to
    if (btnPress == "Import Image") {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
        delete qi;
        QImage qiTemp(fileName);
        qi = new QImage(qiTemp.convertToFormat(QImage::Format_ARGB32_Premultiplied));
        repaint();
    }
    else if (btnPress == "Import Video") {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video"), "/", tr("Video Files (*.mkv *.mp4)"));
        string utf8_file = fileName.toUtf8().constData();
        OpenCV_Handler cv = OpenCV_Handler();
        cv.playVideo(utf8_file);
    }
    else if (btnPress == "Choose Color") {
        QColor color = QColorDialog::getColor(bh.getColor(), this);
        bh.setColor(color);
    }
    else if (btnPress == "Brush Size") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a brush size/radius", bh.getSize(), minRadius, maxRadius, 1, &ok );
        if (ok)
            bh.setSize(ret);
    }
    else if (btnPress == "Screen Strength") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a screen filter strength", screenFilter.getStrength(), minStrength, maxStrength, 1, &ok );
        if (ok)
            screenFilter.setStrength(ret);
    }
    else if (btnPress == "Brush Strength") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a brush strength", bh.getStength(), minStrength, maxStrength, 1, &ok );
        if (ok)
            bh.setStrength(ret);
    }
    else if (btnPress == "Spray Density") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a spray density", bh.getDensity(), minDensity, maxDensity, 1, &ok );
        if (ok)
            bh.setDensity(ret);
    }
}

void MainWindow::changeScreenFilter(string filterName) {
    screenFilter.setFilter(filterName);
    repaint();
}

void MainWindow::changeBrushFilter(string filterName) {
    bh.setFilter(filterName);
}

void MainWindow::changeBrushShape(string shape) {
    bh.setShape(shape);
}

void MainWindow::changeBrushMethod(string method) {
    bh.setAppMethod(method);
    if (method == "Sample") {
        sampleFlag = 1;
        sampleFlasher->start(sampleFlashTime);
    }
    else
        sampleFlasher->stop();
}

void MainWindow::log(string title, QObject *obj) {

    objFetch[title] = obj;
    toDel.push_front(obj);
}

void appTo(QImage *qi, Filter f) {
    f.applyTo(qi);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Up:
        imgSupport.zoomIn();
        break;
    case Qt::Key_Down:
        imgSupport.zoomOut();
        break;
    case Qt::Key_I:
        ImgSupport::rotate180(qi);
        break;
    case Qt::Key_V:
        ImgSupport::flipVertical(qi);
        break;
    case Qt::Key_H:
        ImgSupport::flipHorizontal(qi);
        break;
    case Qt::Key_L:
        ImgSupport::rotate90Left(qi);
        break;
    case Qt::Key_R:
        ImgSupport::rotate90Right(qi);
        break;
    }
    repaint();
}

void MainWindow::paintEvent(QPaintEvent *event) {

    QImage final = qi->copy();
    screenFilter.applyTo(&final);
    if (sampleFlag) {
        QPoint qp = bh.getSamplePoint();
        for (int i = qp.x() - ptSize; i < qp.x() + ptSize; ++i)
            for (int j = qp.y() - ptSize; j < qp.y() + ptSize; ++j) {
                int dist = abs(i - qp.x()) + abs(j - qp.y());
                if (dist >= ptSize - 1 && dist <= ptSize && i >= 0 && i < final.width() && j >= 0 && j < final.height())
                    final.setPixel(i, j, graphics::Filtering::negative(final.pixelColor(i, j), 255));
            }
    }
    final = imgSupport.zoomImg(final);
    QPainter qp(this);
    qp.drawImage(0, 0, final);
}

void MainWindow::toggleSamplePnt() {
    sampleFlag = !sampleFlag;
    repaint();
}

MainWindow::~MainWindow() {

    this->hide();
    sampleFlasher->stop();
    delete sampleFlasher;
    delete ui;
    delete qi;
    objFetch.clear();
    while (!toDel.empty()) {
        QObject *obj = toDel.front();
        delete obj;
        toDel.pop_front();
    }
}
