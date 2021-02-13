#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    shiftFlag = false;
    ctrlFlag = false;
    lastButton = NoButton;
    sr = new screenRender();
    sr->setSamplePt(bh.getSamplePoint());
    ioh = new DataIOHandler();
    sr->updateViews(ioh->getWorkingLayer(), ioh->getForeground(), ioh->getBackground());
    this->setCentralWidget(sr);
    setGeometry(0,0, defaultSize.width(), defaultSize.height());
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
    resizeCheck = new resizeWindow(this, ioh);
    QString docs = "userDocsFile.txt";
    QFile file(docs);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    qtb.setGeometry(this->geometry());
    qtb.setText(file.readAll());
    file.close();
    mode = Brush_Mode;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QPoint qp = sr->getZoomCorrected(event->pos());
    if (mode == Brush_Mode) {
        if (lastButton == LeftButton) {
            QImage *qi = ioh->getWorkingLayer()->getCanvas();
            bh.applyBrush(qi, qp);
        }
        else if (lastButton == RightButton)
            bh.setSamplePoint(qp);
    }
    else if (mode == Spline_Mode) {
        if (ctrlFlag)
            return;
        Layer *layer = ioh->getWorkingLayer();
        if (lastButton == Qt::LeftButton)
            layer->moveLeft(qp);
        else if (lastButton == RightButton && shiftFlag)
            layer->moveRight(qp);
    }
    refresh();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (ctrlFlag)
        return;
    if (event->button() >= 8) {
        setShiftFlag(true);
        return;
    }
    setLastButton(event->button());
    QPoint qp = sr->getZoomCorrected(event->pos());
    if (mode == Brush_Mode) {
        if(lastButton == RightButton)
            bh.setSamplePoint(qp);
        else if (lastButton == LeftButton) {
            bh.setRelativePoint(qp);
            QImage *qi = ioh->getWorkingLayer()->getCanvas();
            bh.applyBrush(qi, qp);
            bh.setInterpolationActive(true);
        }
    }
    else if (mode == Spline_Mode) {
        Layer *layer = ioh->getWorkingLayer();
        if(lastButton == RightButton) {
           MouseButton button = layer->pressRight(qp);
           setLastButton(button);
        }
        else if (lastButton == LeftButton)
            layer->pressLeft(qp);
    }
    refresh();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (mode == Brush_Mode)
        bh.setInterpolationActive(false);
    else if (mode == Spline_Mode)
        ioh->getWorkingLayer()->release(event->button());
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    QPoint qp = sr->getZoomCorrected(event->pos());
    MouseButton button = event->button();
    if (mode == Spline_Mode) {
        if (shiftFlag)
            return;
        Layer *layer = ioh->getWorkingLayer();
        if (button == LeftButton)
            layer->doubleClickLeft(qp, ctrlFlag);
        else if (button == RightButton && ctrlFlag)
            layer->doubleClickRight(qp);
        sr->showPts();
        refresh();
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    int dy = event->angleDelta().y();
    dy = abs(dy)/ dy;
    if (mode == Brush_Mode) {
        if (shiftFlag) {
            if (dy > 0)
                bh.strengthUp();
            else
                bh.strengthDown();
            sr->setSizeDisplay(bh.getStength());
        }
        else if (ctrlFlag) {
            if (dy > 0)
                bh.densityUp();
            else
                bh.densityDown();
            sr->setSizeDisplay(bh.getDensity());
        }
        else {
            if (dy > 0)
                bh.sizeUp();
            else
                bh.sizeDown();
            sr->setSizeDisplay(bh.getSize());
        }
    }
    else if (mode == Spline_Mode && !shiftFlag) {
        ioh->getWorkingLayer()->spinWheel(dy);
        sr->setSizeDisplay(ioh->getWorkingLayer()->getWidth());
    }
    refresh();
}

void MainWindow::setLastButton(MouseButton button) {
    lastButton = button;
    sr->setLastButton(button);
}

void MainWindow::setShiftFlag(bool b) {
    shiftFlag = b;
    ioh->getWorkingLayer()->setShiftFlag(b);
    sr->setShiftFlag(b);
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
                //ioh->setMediaLayer(qi);
                refresh();
            }
            cap.release();
        }
        refresh();
    }
    else if (btnPress == "Export") {    //TODO
        QString fileName = QFileDialog::getSaveFileName(this, tr("Export"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
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
    }
    else if (btnPress == "Spray Density") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a spray density", bh.getDensity(), minDensity, maxDensity, 1, &ok );
        if (ok)
            bh.setDensity(ret);
    }
    else if (btnPress == "Brush Mode") {
        mode = Brush_Mode;
    }
    else if (btnPress == "Spline Mode") {
        mode = Spline_Mode;
    }
    else if (btnPress == "Help") {
        qtb.move(0,0);
        qtb.show();
    }
}

void MainWindow::changeScreenFilter(string filterName) {
    ioh->setScreenFilter(filterName);
    refresh();
}

void MainWindow::changeBrushFilter(string filterName) {
    bh.setFilter(filterName);
}

void MainWindow::changeBrushShape(string shape) {
    bh.setShape(shape);
}

void MainWindow::changeBrushMethod(string method) {
    bh.setAppMethod(method);
}

void MainWindow::log(string title, QObject *obj) {
    objFetch[title] = obj;
    toDel.push_front(obj);
}

void appTo(QImage *qi, Filter f) {
    f.applyTo(qi);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
//    QImage *qi = ioh->getWorkingLayer()->getCanvas();
    switch (event->key()) {
    case Key_Up:
        sr->zoomIn();
        break;
    case Key_Down:
        sr->zoomOut();
        break;
    case Key_Left:
        if (mode == Brush_Mode) {
            if (shiftFlag) {
                bh.strengthDown();
                sr->setSizeDisplay(bh.getStength());
            }
            else if (ctrlFlag) {
                bh.densityDown();
                sr->setSizeDisplay(bh.getDensity());
            }
            else {
                bh.sizeDown();
                sr->setSizeDisplay(bh.getSize());
            }
        }
        else if (mode == Spline_Mode)
            ioh->getWorkingLayer()->widthDown();
        break;
    case Key_Right:
        if (mode == Brush_Mode) {
            if (shiftFlag) {
                bh.strengthUp();
                sr->setSizeDisplay(bh.getStength());
            }
            else if (ctrlFlag) {
                bh.densityUp();
                sr->setSizeDisplay(bh.getDensity());
            }
            else {
                bh.sizeUp();
                sr->setSizeDisplay(bh.getSize());
            }
        }
        else if (mode == Spline_Mode)
            ioh->getWorkingLayer()->widthUp();
        break;
    case Key_Control:
        ctrlFlag = true;
        break;
    case Key_Shift:
        setShiftFlag(true);
        break;
    case Key_Escape:
        ioh->getWorkingLayer()->deselect();
        break;
    case Key_Backspace:
    case Key_Delete:
        ioh->getWorkingLayer()->deleteSelected();
        break;
//    case Qt::Key_I:
//        ImgSupport::rotate180(qi);
//        break;
//    case Qt::Key_V:
//        ImgSupport::flipVertical(qi);
//        break;
//    case Qt::Key_H:
//        ImgSupport::flipHorizontal(qi);
//        break;
//    case Qt::Key_L:
//        ImgSupport::rotate90Left(qi);
//        break;
//    case Qt::Key_R:
//        ImgSupport::rotate90Right(qi);
//        break;
    }
//    if (limitCnt > 1.0)
//        limitCnt -= 1.0;
    refresh();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Key_Shift:
        setShiftFlag(false);
        ioh->getWorkingLayer()->cleanUp();
        break;
    case Key_Control:
        ctrlFlag = false;
        break;
    }
}

void MainWindow::refresh() {
    sr->repaint();
}

void MainWindow::paintEvent(QPaintEvent *event) {

}

MainWindow::~MainWindow() {
    resizeCheck->doClose();
    delete resizeCheck;
    this->hide();
    delete ui;
    objFetch.clear();
    while (!toDel.empty()) {
        QObject *obj = toDel.front();
        delete obj;
        toDel.pop_front();
    }
}
