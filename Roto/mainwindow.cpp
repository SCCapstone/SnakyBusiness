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
    vs = new viewScroller(this);
    vs->setWidgetResizable(true);
    sr = new screenRender(vs);
    ioh = new DataIOHandler();
    sr->updateViews(ioh->getWorkingLayer(), ioh->getForeground(), ioh->getBackground());
    setCentralWidget(vs);
    setGeometry(0,0, defaultSize.width(), defaultSize.height());
    setWindowTitle("Glass Opus");
    createMenubar();
    QMenu* sFiltering = static_cast<QMenu *>(objFetch.at("Layer Filter"));
    QMenu* bFiltering = static_cast<QMenu *>(objFetch.at("Brush Filter"));
    for (string name : filterNames) {
        QAction *sAction = sFiltering->addAction((name).c_str());
        QAction *bAction = bFiltering->addAction((name).c_str());
        connect(sAction, &QAction::triggered, this, [=]() { this->changeScreenFilter(sAction->text().toStdString()); });
        connect(bAction, &QAction::triggered, this, [=]() { this->changeBrushFilter(bAction->text().toStdString()); });
        log(name, sAction);
        log(name, bAction);
    }
    QMenu* vFiltering = static_cast<QMenu *>(objFetch.at("Vector Filter"));
    for (string name : vectorFilters) {
        QAction *vAction = vFiltering->addAction((name).c_str());
        connect(vAction, &QAction::triggered, this, [=]() { this->changeVectorFilter(vAction->text().toStdString()); });
        log(name, vAction);
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
    radialProfiler = new RadialProfiler(&bh, this);
    QString docs = "userDocsFile.txt";
    QFile file(docs);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    qtb.setGeometry(this->geometry());
    qtb.setText(file.readAll());
    file.close();
    mode = Brush_Mode;
    onePress = false;
    vs->setWidget(sr);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QPoint qp = sr->getZoomCorrected(vs->getScrollCorrected(event->pos()));
    statusBar()->showMessage((to_string(qp.x()) + "," + to_string(qp.y())).c_str(), 1000);
    if (mode == Brush_Mode) {
        if (lastButton == LeftButton) {
            bh.applyBrush(ioh->getWorkingLayer()->getCanvas(), qp);
        }
        else if (lastButton == RightButton) {
            if (bh.getMethodIndex() == appMethod::sample)
                setSamplePt(qp);
            else
                bh.applyBrush(ioh->getWorkingLayer()->getCanvas(), qp);
        }
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
    if (ctrlFlag || onePress)
        return;
    onePress = true;
    if (event->button() >= 8) {
        setShiftFlag(true);
        return;
    }
    lastButton = event->button();
    QPoint qp = sr->getZoomCorrected(vs->getScrollCorrected(event->pos()));
    if (mode == Brush_Mode) {
        if (lastButton == RightButton) {
            if (bh.getMethodIndex() == appMethod::sample)
                setSamplePt(qp);
            else {
                bh.setAlpha(0);
                bh.applyBrush(ioh->getWorkingLayer()->getCanvas(), qp);
                bh.setInterpolationActive(true);
            }
        }
        else if (lastButton == LeftButton) {
            bh.setRelativePoint(qp);
            bh.applyBrush(ioh->getWorkingLayer()->getCanvas(), qp);
            bh.setInterpolationActive(true);
        }
    }
    else if (mode == Spline_Mode) {
        Layer *layer = ioh->getWorkingLayer();
        if(lastButton == RightButton)
           lastButton = layer->pressRight(qp);
        else if (lastButton == LeftButton)
            layer->pressLeft(qp);
    }
    refresh();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (mode == Brush_Mode) {
        bh.setInterpolationActive(false);
        bh.setAlpha(ioh->getWorkingLayer()->getAlpha());
    }
    else if (mode == Spline_Mode)
        ioh->getWorkingLayer()->release(event->button());
    else if (event->button() >= 8) {
        setShiftFlag(false);
        if (onePress)
            ioh->getWorkingLayer()->cleanUp();
        return;
    }
    onePress = false;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    QPoint qp = sr->getZoomCorrected(vs->getScrollCorrected(event->pos()));
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
            bh.setStrength(bh.getStength() + dy);
            statusBar()->showMessage(("Brush Strength: " + to_string(bh.getStength())).c_str(), 1000);
        }
        else if (ctrlFlag) {
            bh.setDensity(bh.getDensity() + dy);
            statusBar()->showMessage(("Brush Density: " + to_string(bh.getDensity())).c_str(), 1000);
        }
        else {
            radialProfiler->updateSize(bh.getSize() + dy);
            statusBar()->showMessage(("Brush Radius: " + to_string(bh.getSize())).c_str(), 1000);
        }
    }
    else if (mode == Spline_Mode) {
        if (shiftFlag) {
            vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
            if (activeVects.size() != 1)
                return;
            ioh->getWorkingLayer()->setVectorTaper1(ioh->getWorkingLayer()->getVectorTapers().first + dy);
            statusBar()->showMessage(("Vector Taper 1: " + to_string(ioh->getWorkingLayer()->getVectorTapers().first)).c_str(), 1000);
        }
        else if (ctrlFlag) {
            vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
            if (activeVects.size() != 1)
                return;
            ioh->getWorkingLayer()->setVectorTaper2(ioh->getWorkingLayer()->getVectorTapers().second + dy);
            statusBar()->showMessage(("Vector Taper 2: " + to_string(ioh->getWorkingLayer()->getVectorTapers().second)).c_str(), 1000);
        }
        else {
            ioh->getWorkingLayer()->spinWheel(dy);
            if (ioh->getWorkingLayer()->getActiveVectors().size() == 1)
                statusBar()->showMessage(("Vector Width: " + to_string(ioh->getWorkingLayer()->getWidth())).c_str(), 1000);
        }
    }
    refresh();
}

void MainWindow::setSamplePt(QPoint qp) {
    bh.setSamplePoint(qp);
    sr->setSamplePt(qp);
}

void MainWindow::setShiftFlag(bool b) {
    shiftFlag = b;
    ioh->getWorkingLayer()->setShiftFlag(b);
}

void MainWindow::createMenubar() {
    QMenuBar *menubar = new QMenuBar(this);
    menubar->setGeometry(QRect(0, 0, 800, 20));
    log(UI_FileName + UI_FileType, menubar);
    fstream uiFile;
    uiFile.open(UI_FileName + UI_FileType,ios::in);
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
    //cout << btnPress << endl;
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
                l = stdFuncs::getTime();
                QImage qi = QImage(frame.cols, frame.rows, QImage::Format_ARGB32_Premultiplied);
                for(int r = 0; r < frame.rows; ++r)
                    for(short c = 0; c < frame.cols; ++c) {
                        RGB& rgb = frame.ptr<RGB>(r)[c];
                        qi.setPixel(c, r, qRgb(rgb.red,rgb.green,rgb.blue));
                    }
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
    else if (btnPress == "Brush Color") {
        QColor color = QColorDialog::getColor(bh.getColor(), this);
        bh.setColor(color);
    }
    else if (btnPress == "Radial Profiler") {
        radialProfiler->showRelative();
    }
    else if (btnPress == "Brush Radius") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a brush size/radius", bh.getSize(), minRadius, maxRadius, 1, &ok );
        if (ok)
            radialProfiler->updateSize(ret);
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
    else if (btnPress == "Vector Width") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a spray density", ioh->getWorkingLayer()->getWidth(), minWidth, maxWidth, 1, &ok );
        if (ok)
            ioh->getWorkingLayer()->setWidth(ret);
        refresh();
    }
    else if (btnPress == "Vector Taper 1") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a spray density", ioh->getWorkingLayer()->getVectorTapers().first, minTaper, maxTaper, 1, &ok );
        if (ok)
            ioh->getWorkingLayer()->setVectorTaper1(ret);
        refresh();
    }
    else if (btnPress == "Vector Taper 2") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a spray density", ioh->getWorkingLayer()->getVectorTapers().second, minTaper, maxTaper, 1, &ok );
        if (ok)
            ioh->getWorkingLayer()->setVectorTaper2(ret);
        refresh();
    }
    else if (btnPress == "Vector Color 1") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        QColor color = QColorDialog::getColor(ioh->getWorkingLayer()->getVectorColors().second, this);
        ioh->getWorkingLayer()->setVectorColor2(color.rgba());
        refresh();
    }
    else if (btnPress == "Vector Color 2") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        QColor color = QColorDialog::getColor(ioh->getWorkingLayer()->getVectorColors().first, this);
        ioh->getWorkingLayer()->setVectorColor1(color.rgba());
        refresh();
    }
    else if (btnPress == "Single Taper") {
        ioh->getWorkingLayer()->setVectorTaperType(Single);
        refresh();
    }
    else if (btnPress == "Double Taper") {
        ioh->getWorkingLayer()->setVectorTaperType(Double);
        refresh();
    }
    else if (btnPress == "Swap Colors") {
        ioh->getWorkingLayer()->swapColors();
        refresh();
    }
    else if (btnPress == "Swap Tapers") {
        ioh->getWorkingLayer()->swapTapers();
        refresh();
    }
    else if (btnPress == "Color Vector") {
        ioh->getWorkingLayer()->setVectorMode(ColorFill);
        refresh();
    }
    else if (btnPress == "Filter Vector") {
        ioh->getWorkingLayer()->setVectorMode(Filtered);
        refresh();
    }
    else if (btnPress == "Layer Opacity (Alpha)") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a layer alpha", ioh->getWorkingLayer()->getAlpha(), 1, graphics::maxColor, 1, &ok );
        if (ok)
            ioh->getWorkingLayer()->setAlpha(ret);
        bh.setAlpha(ret);
    }
    else if (btnPress == "Brush Mode") {
        mode = Brush_Mode;
        ioh->getWorkingLayer()->deselect();
    }
    else if (btnPress == "Spline Mode") {
        mode = Spline_Mode;
        setSamplePt(QPoint(-1000, -1000));
    }
    else if (btnPress == "Copy") {
        if (mode == Spline_Mode)
            ioh->copyVectors();
    }
    else if (btnPress == "Cut") {
        if (mode == Spline_Mode)
            ioh->cutVectors();
        refresh();
    }
    else if (btnPress == "Delete") {
        if (mode == Spline_Mode)
            ioh->deleteVectors();
        refresh();
    }
    else if (btnPress == "Paste") {
        if (mode == Spline_Mode)
            ioh->pasteVectors();
        refresh();
    }
    else if (btnPress == "Help") {
        qtb.move(0,0);
        qtb.show();
    }
}

void MainWindow::changeVectorFilter(string s) {
    ioh->getWorkingLayer()->setVectorFilter(s);
    refresh();
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
    QImage *qi = ioh->getWorkingLayer()->getCanvas();
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
                statusBar()->showMessage(("Brush Strength: " + to_string(bh.getStength())).c_str(), 1000);
            }
            else if (ctrlFlag) {
                bh.densityDown();
                statusBar()->showMessage(("Brush Density: " + to_string(bh.getDensity())).c_str(), 1000);
            }
            else {
                radialProfiler->updateSize(bh.getSize() - 1);
                statusBar()->showMessage(("Brush Radius: " + to_string(bh.getSize())).c_str(), 1000);
            }
        }
        else if (mode == Spline_Mode)
            ioh->getWorkingLayer()->widthDown();
        break;
    case Key_Right:
        if (mode == Brush_Mode) {
            if (shiftFlag) {
                bh.strengthUp();
                statusBar()->showMessage(("Brush Strength: " + to_string(bh.getStength())).c_str(), 1000);
            }
            else if (ctrlFlag) {
                bh.densityUp();
                statusBar()->showMessage(("Brush Density: " + to_string(bh.getDensity())).c_str(), 1000);
            }
            else {
                radialProfiler->updateSize(bh.getSize() + 1);
                statusBar()->showMessage(("Brush Radius: " + to_string(bh.getSize())).c_str(), 1000);
            }
        }
        else if (mode == Spline_Mode)
            ioh->getWorkingLayer()->widthUp();
        break;
    case Key_Control:
        if (!shiftFlag)
            ctrlFlag = true;
        break;
    case Key_Shift:
        if (!ctrlFlag)
            setShiftFlag(true);
        break;
    case Key_Escape:
        ioh->getWorkingLayer()->deselect();
        break;
    case Key_Backspace:
    case Key_Delete:
        ioh->getWorkingLayer()->deleteSelected();
        break;
    case Key_X:
        if (ctrlFlag && mode == Spline_Mode)
            ioh->cutVectors();
        break;
    case Key_C:
        if (ctrlFlag && mode == Spline_Mode)
            ioh->copyVectors();
        break;
    case Key_V:
        if (ctrlFlag && mode == Spline_Mode)
            ioh->pasteVectors();
        break;
        /*
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
        */
    }
    refresh();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Key_Shift:
        setShiftFlag(false);
        if (onePress)
            ioh->getWorkingLayer()->cleanUp();
        break;
    case Key_Control:
        ctrlFlag = false;
        break;
    }
}
void MainWindow::resizeEvent(QResizeEvent *event) {
    QMenuBar *menu = dynamic_cast<QMenuBar *>(objFetch.at(UI_FileName + UI_FileType));
    menu->resize(width(), menu->height());
}

void MainWindow::refresh() {
    sr->repaint();
}

void MainWindow::paintEvent(QPaintEvent *event) {

}

MainWindow::~MainWindow() {
    resizeCheck->doClose();
    delete resizeCheck;
    hide();
    delete ioh;
    delete sr;
    delete vs;
    delete ui;
    objFetch.clear();
    while (!toDel.empty()) {
        QObject *obj = toDel.front();
        delete obj;
        toDel.pop_front();
    }
}

