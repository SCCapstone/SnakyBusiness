#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ioh = new DataIOHandler();
    QSize size = ioh->getBounds();
    imgSupport.setLayerSize(size);
    setGeometry(0,0, size.width(), size.width());
    setWindowTitle("Glass Opus");
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
    sampleFlag = 0;
    sampleFlasher = new QTimer(this);
    connect(sampleFlasher, SIGNAL(timeout()), this, SLOT(toggleSamplePnt()));
    resizeCheck = new resizeWindow(this, ioh);
    QString docs = "userDocsFile.txt";
    QFile file(docs);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    qtb.setGeometry(this->geometry());
    qtb.setText(file.readAll());
    file.close();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    // AVERAGE CASE ON MOST EXPENSIVE SETTINGS (SEE REPAINT) AVERAGE 2ms, MAX WAS 51ms HOWEVER (BEFORE UPDATE).
    QPoint qp = imgSupport.getZoomCorrected(event->pos());
    if (lastButton == Qt::LeftButton) {
        QImage *qi = ioh->getCanvasLayer();
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
        QImage *qi = ioh->getCanvasLayer();
        bh.applyBrush(qi, qp);
        bh.setInterpolationActive(true);
    }
    repaint();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    bh.setInterpolationActive(false);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {

}

void MainWindow::createMenubar(string filename) {
    QMenuBar *menubar = new QMenuBar(this);
    menubar->setGeometry(QRect(0, 0, 800, 20));
    log(filename + UI_FileType, menubar);
    fstream uiFile;
    uiFile.open(filename + UI_FileType,ios::in);
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
    if (btnPress == "Import") {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Import"), "/", tr("Media Files (*.png *.jpg *.bmp *.mp4 *.avi *.mkv)"));
        if (fileName == "")
            return;
        string fn = fileName.toStdString();
        int index = fn.find_last_of('.');
        string fileType = fn.substr(index + 1);
        if (fileType == "png" || fileType == "jpg" || fileType == "bmp") {
            bool flag = ioh->importImage(fileName);
            if (flag)
                resizeCheck->showRelative();
        }
        else {
            VideoCapture cap = VideoCapture(fileName.toStdString());
            if(!cap.isOpened()) {
                return;
            }
            list <Mat> list;
            long long l;
            while(true) {
                l = stdFuncs::getTime();
                Mat frame;
                cap >> frame;
                if (frame.empty())
                    break;
                list.push_back(frame);
                cout << stdFuncs::getChange(l);
                l = stdFuncs::getTime();
                QImage qi = QImage(frame.cols, frame.rows, QImage::Format_ARGB32_Premultiplied);
                for(int r = 0; r < frame.rows; ++r)
                    for(short c = 0; c < frame.cols; ++c) {
                        RGB& rgb = frame.ptr<RGB>(r)[c];
                        qi.setPixel(c, r, qRgb(rgb.red,rgb.green,rgb.blue));
                    }
                cout << "\t" << stdFuncs::getChange(l) << "\t" << list.size() << endl;
                ioh->setMediaLayer(qi);
                repaint();
            }
            cap.release();
            //destroyAllWindows();
        }
        repaint();
    }
    else if (btnPress == "Export") {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Import"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
        if (fileName == "")
            return;
        string fn = fileName.toStdString();
        int index = fn.find_last_of('.');
        string fileType = fn.substr(index + 1);
        if (fileType == "png" || fileType == "jpg" || fileType == "bmp") {
            ioh->exportImage(fileName);
        }
        else {

        }
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
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a screen filter strength", ioh->getFilterStrength(), minStrength, maxStrength, 1, &ok );
        if (ok)
            ioh->setFilterStrength(ret);
    }
    else if (btnPress == "Brush Strength") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a brush strength", bh.getStength(), minStrength, maxStrength, 1, &ok );
        if (ok)
            bh.setStrength(ret);
        cout << ok << endl;
    }
    else if (btnPress == "Spray Density") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a spray density", bh.getDensity(), minDensity, maxDensity, 1, &ok );
        if (ok)
            bh.setDensity(ret);
    }
    else if (btnPress == "Help") {
        qtb.move(0,0);
        qtb.show();
    }
}

void MainWindow::changeScreenFilter(string filterName) {
    ioh->setScreenFilter(filterName);
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
    QImage *qi = ioh->getCanvasLayer();
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
    // TAKES ABOUT 8ms MAX ON MOST EXPENSIVE SETTINGS - FILTER BRUSH (GBR), 64 SIZE, 2 SPRAY DENSITY.
    QSize size = ioh->getBounds();
    imgSupport.setLayerSize(size);
    QImage canvas = ioh->getCanvasLayer()->copy();
    QImage media = ioh->getFilteredMLayer();
    if (sampleFlag) {
        QPoint qp = bh.getSamplePoint();
        for (int i = qp.x() - ptSize; i < qp.x() + ptSize; ++i)
            for (int j = qp.y() - ptSize; j < qp.y() + ptSize; ++j) {
                int dist = abs(i - qp.x()) + abs(j - qp.y());
                if (dist >= ptSize - 1 && dist <= ptSize && i >= 0 && i < canvas.width() && j >= 0 && j < canvas.height())
                    canvas.setPixel(i, j, graphics::Filtering::negative(canvas.pixelColor(i, j), 255));
            }
    }
    canvas.setAlphaChannel(imgSupport.getAlphaLayer());
    canvas = imgSupport.zoomImg(canvas);
    media = imgSupport.zoomImg(media);
    QPainter qp(this);
    qp.drawImage(0, 20, media);
    qp.drawImage(0, 20, canvas);
}

void MainWindow::toggleSamplePnt() {
    sampleFlag = !sampleFlag;
    repaint();
}

MainWindow::~MainWindow() {
    resizeCheck->doClose();
    delete resizeCheck;
    this->hide();
    sampleFlasher->stop();
    delete sampleFlasher;
    delete ui;
    objFetch.clear();
    while (!toDel.empty()) {
        QObject *obj = toDel.front();
        delete obj;
        toDel.pop_front();
    }
}
