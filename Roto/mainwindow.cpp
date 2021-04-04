#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hide();
    setAcceptDrops(true);
    QLabel logo;
    QFile file(QDir::currentPath() + UI_Loc + Logo_FileName);
    bool logoFound = file.exists();
    QPoint center;
    QRect screenRect = QGuiApplication::screens().front()->availableGeometry();
    if (logoFound) {
        QImage qi(file.fileName());
        logo.setPixmap(QPixmap::fromImage(qi));
        center = screenRect.center();
        logo.setFixedSize(qi.size());
        logo.setWindowFlag(Qt::WindowType::FramelessWindowHint, true);
        logo.move(center - logo.rect().center());
        logo.show();
        QCoreApplication::processEvents();
    }
    qme = new QErrorMessage(this);
    shiftFlag = false;
    ctrlFlag = false;
    lastButton = NoButton;
    vs = new viewScroller(this);
    vs->setWidgetResizable(true);
    progress = new QProgressDialog("Updating Views", "", 0, 0, this, Qt::WindowType::FramelessWindowHint);
    progress->setCancelButton(nullptr);
    progress->close();
    ioh = new DataIOHandler(progress);
    sr = new screenRender(ioh, vs);
    setCentralWidget(vs);
    setGeometry(screenRect.width() / 4, screenRect.height() / 4, screenRect.width() / 2, screenRect.height() / 2);
    setWindowTitle("Glass Opus");
    bool exists = createMenubar();
    if (exists) {
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
            QFile qf(QDir::currentPath() + Icon_Loc + QString(name.c_str()) + ".png");
            if (qf.exists())
                bAction->setIcon(QIcon(qf.fileName()));
            connect(bAction, &QAction::triggered, this, [=]() { this->changeBrushShape(bAction->text().toStdString()); });
            log(name, bAction);
        }
        QMenu* bMethod = static_cast<QMenu *>(objFetch.at("Brush Method"));
        for (string name : appMethods) {
            QAction *bAction = bMethod->addAction((name).c_str());
            QFile qf(QDir::currentPath() + Icon_Loc + QString(name.c_str()) + ".png");
            if (qf.exists())
                bAction->setIcon(QIcon(qf.fileName()));
            connect(bAction, &QAction::triggered, this, [=]() { this->changeBrushMethod(bAction->text().toStdString()); });
            log(name, bAction);
        }
    }
    else
        downloadItem(UI_Loc, UI_FileName, DownloadThenRestart, "Menu Configuration Not Found", "Menu configuration not found locally/offline.\nFetch and download menu configuration online?");
    ui->menubar->setCornerWidget(dynamic_cast<QMenuBar *>(objFetch.at(UI_FileName.toStdString())), Qt::TopLeftCorner);
    resizeCheck = new resizeWindow(this, ioh);
    radialProfiler = new RadialProfiler(&bh, this);
    onePress = false;
    vs->setWidget(sr);
    file.setFileName(QDir::currentPath() + UI_Loc + Icon_Loc + WinIco_FileName);
    if (file.exists())
        setWindowIcon(QIcon(file.fileName()));
    takeFlag = false;
    if (logoFound)
        std::this_thread::sleep_for (std::chrono::seconds(2));
    move(center - rect().center());
    QInputDialog resPrompt;
    QStringList items;
    items.push_back("360p");
    items.push_back("480p");
    items.push_back("720p");
    items.push_back("900p");
    items.push_back("1080p");
    items.push_back("1444p");
    items.push_back("2160p");
    resPrompt.setOptions(QInputDialog::UseListViewForComboBoxItems);
    resPrompt.setComboBoxItems(items);
    resPrompt.setTextValue(items.first());
    resPrompt.setWindowTitle("New Project Resolution");
    resPrompt.setWhatsThis("This will set the resolution of the layers and resulting export. Importing a saved project file after this dialog will update the resolution");    
    show();
    logo.hide();
    resPrompt.exec();
    int sizeY = stoi(resPrompt.textValue().toStdString());
    ioh->setDims(QSize(static_cast<int>((16.0/9.0) * static_cast<float>(sizeY)), sizeY));
    setMode(Brush_Mode);
    sr->updateHoverMap(bh.getSize(), bh.getBrushMap());
    sr->setHoverActive(true);
    brushProlfiler = new brushShape(&bh,this);
    pp = new patternProfiler(&bh,this);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (ioh->getWorkingLayer() == nullptr)
        return;
    if (takeFlag)
        return;
    QPoint qp = sr->getZoomCorrected(vs->getScrollCorrected(event->pos()));
    statusBar()->showMessage((to_string(qp.x()) + "," + to_string(qp.y())).c_str(), 1000);
    if (mode == Brush_Mode) {
        if (lastButton == LeftButton)
            bh.applyBrush(ioh->getWorkingLayer()->getCanvas(), qp);
        else if (lastButton == RightButton) {
            if (bh.getMethodIndex() == appMethod::sample)
                setSamplePt(qp);
            else
                bh.erase(ioh->getWorkingLayer()->getCanvas(), qp);
        }
    }
    else if (mode == Spline_Mode || mode == Raster_Mode) {
        if (ctrlFlag)
            return;
        Layer *layer = ioh->getWorkingLayer();
        if (lastButton == Qt::LeftButton)
            layer->moveLeft(qp);
        else if (lastButton == RightButton && shiftFlag)
            layer->moveRight(qp);
        refresh();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (ioh->getWorkingLayer() == nullptr)
        return;
    if (ctrlFlag || onePress || takeFlag)
        return;
    onePress = true;
    if (event->button() >= 8) {
        setShiftFlag(true);
        return;
    }
    lastButton = event->button();
    QPoint qp = sr->getZoomCorrected(vs->getScrollCorrected(event->pos()));
    if (mode == Raster_Mode && event->button() == RightButton && !shiftFlag && !ioh->getWorkingLayer()->isRotating())
        ioh->getWorkingLayer()->fillColor(qp, bh.getFillColor());
    else if (mode == Brush_Mode) {
        sr->setHoverActive(false);
        if (lastButton == RightButton) {
            if (bh.getMethodIndex() == appMethod::sample)
                setSamplePt(qp);
            else {
                bh.erase(ioh->getWorkingLayer()->getCanvas(), qp);
                bh.setInterpolationActive(true);
            }
        }
        else if (lastButton == LeftButton) {
            bh.setRelativePoint(qp);
            bh.applyBrush(ioh->getWorkingLayer()->getCanvas(), qp);
            bh.setInterpolationActive(true);
        }
    }
    else if (mode == Spline_Mode || mode == Raster_Mode) {
        Layer *layer = ioh->getWorkingLayer();
        if (lastButton == RightButton)
            lastButton = layer->pressRight(qp);
        else if (lastButton == LeftButton)
            layer->pressLeft(qp);
    }
    refresh();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (ioh->getWorkingLayer() == nullptr)
        return;
    QPoint qp = sr->getZoomCorrected(vs->getScrollCorrected(event->pos()));
    if (takeFlag) {
        if (mode == Brush_Mode)
            bh.setBrushColor(ioh->getWorkingLayer()->getCanvas()->pixelColor(qp.x(), qp.y()));
        else if (mode == Raster_Mode)
            bh.setFillColor(ioh->getWorkingLayer()->getCanvas()->pixelColor(qp.x(), qp.y()));
        takeFlag = false;
        return;
    }
    if (mode == Brush_Mode) {
        sr->setHoverActive(true);
        bh.setInterpolationActive(false);
        bh.setAlpha(ioh->getWorkingLayer()->getAlpha());
        refresh();
    }
    else if (mode == Spline_Mode || (mode == Raster_Mode && event->button() != Qt::RightButton))
        ioh->getWorkingLayer()->release(qp, event->button());
    else if (event->button() >= 8) {
        setShiftFlag(false);
        if (onePress)
            ioh->getWorkingLayer()->cleanUp();
        return;
    }
    onePress = false;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    if (ioh->getWorkingLayer() == nullptr)
        return;
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
    if (ioh->getWorkingLayer() == nullptr)
        return;
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
            sr->updateHoverMap(bh.getSize(), bh.getBrushMap());
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
    else if (mode == Raster_Mode)
        ioh->getWorkingLayer()->spinWheel(dy);
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

bool MainWindow::createMenubar() {
    QFile qf(QDir::currentPath() + UI_Loc + UI_FileName);
    bool exists = qf.exists();
    if (exists) {
        QMenuBar *menubar = new QMenuBar(this);
        menubar->setGeometry(QRect(0, 0, 800, 20));
        log(UI_FileName.toStdString(), menubar);
        fstream uiFile;
        uiFile.open((QDir::currentPath() + UI_Loc + UI_FileName).toStdString(), ios::in);
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
    return exists;
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
            QFile qf(QDir::currentPath() + Icon_Loc + QString(item.c_str()) + ".png");
            if (qf.exists())
                subMenu->setIcon(QIcon(qf.fileName()));
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
    QFile qf(QDir::currentPath() + Icon_Loc + QString(item.c_str()) + ".png");
    if (qf.exists())
        action->setIcon(QIcon(qf.fileName()));
    connect(action, &QAction::triggered, this, [=]() { this->doSomething(item); });
    log(item, action);
}

void MainWindow::doSomething(string btnPress) {
    //cout << btnPress << endl;
    if (btnPress == "Import") {
        QRect reset = geometry();
        string formats = "";
        for (string s : acceptedImportImageFormats)
            formats += " *." + s;
        for (string s : acceptedImportVideoFormats)
            formats += " *." + s;
        formats = "Media Files (" + formats.substr(1) + ")";
        QString fileName = QFileDialog::getOpenFileName(this, tr("Import"), "/", tr(formats.c_str()));
        if (fileName == "")
            return;
        string fn = fileName.toStdString();
        int index = fn.find_last_of('.');
        string fileType = fn.substr(index + 1);
        if (std::find(acceptedImportImageFormats.begin(), acceptedImportImageFormats.end(), fileType) != acceptedImportImageFormats.end()) {
            bool flag = ioh->importImage(fileName);
            setGeometry(reset);
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
                refresh();
            }
            cap.release();
        }
        refresh();
    }
    else if (btnPress == "Export") {    //TODO
        sr->stopFlashing();
        string formats = "";
        for (string s : acceptedExportImageFormats)
            formats += " *." + s;
        for (string s : acceptedExportVideoFormats)
            formats += " *." + s;
        formats = "Media Files (" + formats.substr(1) + ")";
        QString fileName = QFileDialog::getSaveFileName(this, tr("Export"), "/", tr(formats.c_str()));
        if (fileName == "")
            return;
        string fn = fileName.toStdString();
        int index = fn.find_last_of('.');
        string fileType = fn.substr(index + 1);
        if (std::find(acceptedExportImageFormats.begin(), acceptedExportImageFormats.end(), fileType) != acceptedExportImageFormats.end()) {
            ioh->exportImage(fileName);
        }
        else {

        }
        sr->resume();
    }
    else if (btnPress == "Help") {
        bool found = QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath() + Doc_Loc + Doc_FileName));
        if (!found)
            downloadItem(Doc_Loc, Doc_FileName, DownLoadThenOpen, "Documentation Not Found", "Documentation PDF not found locally/offline.\nFetch and download documentation online?");
    }
    else if (btnPress == "Exit") {
        QMessageBox qmb(QMessageBox::Question, "Glass Opus", "Do you wish to Exit?", QMessageBox::Yes, this);
        qmb.addButton("No", QMessageBox::NoRole);
        int choice = qmb.exec();
        if (choice == QMessageBox::Yes)
            QApplication::exit();
    }
    else if (btnPress == "Insert Layer")
        ioh->addLayer();
    if (ioh->getWorkingLayer() == nullptr)
        return;
    else if (btnPress == "On")
        sr->showFg(true);
    else if (btnPress == "Off")
        sr->showFg(false);
    else if (btnPress == "Brush Color") {
        QColor color = QColorDialog::getColor(bh.getBrushColor(), this);
        bh.setBrushColor(color);
    }
    else if (btnPress == "Radial Profiler") {
        radialProfiler->showRelative();
    }
    else if (btnPress == "Brush Radius") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a brush radius", bh.getSize(), minRadius, maxRadius, 1, &ok );
        if (ok) {
            radialProfiler->updateSize(ret);
            sr->updateHoverMap(bh.getSize(), bh.getBrushMap());
        }
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
    else if (btnPress == "Flip Selection Vertical")
        ioh->getWorkingLayer()->flipVert();
    else if (btnPress == "Flip Selection Horizontal")
        ioh->getWorkingLayer()->flipHori();
    else if (btnPress == "Pattern On")
        bh.setPatternInUse(true);
    else if (btnPress == "Pattern Off")
        bh.setPatternInUse(false);
    else if (btnPress == "Brush Filter Strength") {
        int val = bh.getFilterStrength();
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a brush filter strength", val, graphics::minColor, graphics::maxColor, 1, &ok );
        if (ok)
            bh.setFilterStrength(ret);
    }
    else if (btnPress == "Vector Width") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a vector width", ioh->getWorkingLayer()->getWidth(), minWidth, maxWidth, 1, &ok );
        if (ok)
            ioh->getWorkingLayer()->setWidth(ret);
    }
    else if (btnPress == "Vector Filter Strength") {
        int val = ioh->getWorkingLayer()->getVectorFilterStrength();
        if (val == -1)
            return;
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a vector filter strength", val, graphics::minColor, graphics::maxColor, 1, &ok );
        if (ok)
            ioh->getWorkingLayer()->setVectorFilterStrength(ret);
    }
    else if (btnPress == "Vector Taper 1") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a first taper degree", ioh->getWorkingLayer()->getVectorTapers().first, minTaper, maxTaper, 1, &ok);
        if (ok)
            ioh->getWorkingLayer()->setVectorTaper1(ret);
    }
    else if (btnPress == "Vector Taper 2") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a second taper degree", ioh->getWorkingLayer()->getVectorTapers().second, minTaper, maxTaper, 1, &ok);
        if (ok)
            ioh->getWorkingLayer()->setVectorTaper2(ret);
    }
    else if (btnPress == "Vector Color 1") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        QColor color = QColorDialog::getColor(ioh->getWorkingLayer()->getVectorColors().second, this);
        ioh->getWorkingLayer()->setVectorColor2(color.rgba());
    }
    else if (btnPress == "Vector Color 2") {
        vector <unsigned char> activeVects = ioh->getWorkingLayer()->getActiveVectors();
        if (activeVects.size() != 1)
            return;
        QColor color = QColorDialog::getColor(ioh->getWorkingLayer()->getVectorColors().first, this);
        ioh->getWorkingLayer()->setVectorColor1(color.rgba());
    }
    else if (btnPress == "Single Taper") {
        ioh->getWorkingLayer()->setVectorTaperType(Single);
    }
    else if (btnPress == "Double Taper") {
        ioh->getWorkingLayer()->setVectorTaperType(Double);
    }
    else if (btnPress == "Swap Colors") {
        ioh->getWorkingLayer()->swapColors();
    }
    else if (btnPress == "Swap Tapers") {
        ioh->getWorkingLayer()->swapTapers();
    }
    else if (btnPress == "Color Vector") {
        ioh->getWorkingLayer()->setVectorMode(ColorFill);
    }
    else if (btnPress == "Filter Vector") {
        ioh->getWorkingLayer()->setVectorMode(Filtered);
    }
    else if (btnPress == "Fill Color") {
        QColor color = QColorDialog::getColor(bh.getFillColor(), this);
        bh.setFillColor(color);
    }
    else if (btnPress == "Take Color" && (mode == Brush_Mode || mode == Raster_Mode))
        takeFlag = true;
    else if (btnPress == "Layer Opacity (Alpha)") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a layer alpha", ioh->getWorkingLayer()->getAlpha(), 1, graphics::maxColor, 1, &ok);
        if (ok) {
            ioh->getWorkingLayer()->setAlpha(ret);
            bh.setAlpha(ret);
        }
    }
    else if (btnPress == "Brush Mode") {
        sr->stopFlashing();
        ioh->getWorkingLayer()->deselect();
        setMode(Brush_Mode);
    }
    else if (btnPress == "Vector Mode") {
        sr->resume();
        ioh->getWorkingLayer()->deselect();
        setMode(Spline_Mode);
        setSamplePt(QPoint(-1000, -1000));
        ioh->getWorkingLayer()->deselect();
    }
    else if (btnPress == "Raster Mode") {
        sr->resume();
        ioh->getWorkingLayer()->deselect();
        setMode(Raster_Mode);
        setSamplePt(QPoint(-1000, -1000));
        ioh->getWorkingLayer()->deselect();
    }
    else if (btnPress == "Copy") {
        if (mode == Spline_Mode)
            ioh->copyVectors();
        else if (mode == Raster_Mode)
            ioh->copyRaster();
    }
    else if (btnPress == "Cut") {
        if (mode == Spline_Mode)
            ioh->cutVectors();
        else if (mode == Raster_Mode)
            ioh->cutRaster();
    }
    else if (btnPress == "Delete") {
        if (mode == Spline_Mode)
            ioh->deleteVectors();
        else if (mode == Raster_Mode)
            ioh->deleteRaster();
    }
    else if (btnPress == "Paste") {
        if (mode == Spline_Mode)
            ioh->pasteVectors();
        else if (mode == Raster_Mode)
            ioh->pasteRaster();
    }
    else if (btnPress == "Select All")
        ioh->getWorkingLayer()->selectAll();
    else if (btnPress == "Set Active Layer") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Select a layer to edit", ioh->getActiveLayer() + 1, 1, ioh->getNumLayers(), 1, &ok) - 1;
        if (ok && ret != ioh->getActiveLayer()) {
            ioh->getWorkingLayer()->deselect();
            ioh->setActiveLayer(ret, mode);
        }
    }
    else if (btnPress == "Layer Filter Strength") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Set current layer's filter strength", ioh->getWorkingLayer()->getFilterStrength(), 1, graphics::maxColor, 1, &ok) - 1;
        if (ok)
            ioh->getWorkingLayer()->setFilterStrength(ret);
    }
    else if (btnPress == "Move To Front")
        ioh->moveToFront();
    else if (btnPress == "Move Backward")
        ioh->moveBackward();
    else if (btnPress == "Move Forward")
        ioh->moveForward();
    else if (btnPress == "Move To Back")
        ioh->moveToBack();
    else if (btnPress == "Move To Front")
        ioh->moveToFront();
    else if (btnPress == "Copy Layer")
        ioh->copyLayer();
    else if (btnPress == "Cut Layer") {
        ioh->copyLayer();
        ioh->deleteLayer();
    }
    else if (btnPress == "Paste Layer")
        ioh->pasteLayer();
    else if (btnPress == "Delete Layer")
        ioh->deleteLayer();
    else if (btnPress == "Compile Layer")
        ioh->compileLayer();
    else if (btnPress == "Compile Frame")
        ioh->compileFrame();
    else if (btnPress == "Zoom 100%") {
        sr->setZoom(1.0);
    }
    else if (btnPress == "Set Zoom") {
        bool ok = false;
        double ret = QInputDialog::getDouble(this, "Glass Opus", "Select a layer to edit", sr->getZoom(), graphics::minZoom, graphics::maxZoom, 2, &ok);
        if (ok)
            sr->setZoom(ret);
    }
    else if (btnPress == "Zoom In")
        sr->zoomIn();
    else if (btnPress == "Zoom Out")
        sr->zoomOut();
    else if (btnPress == "Shape Profiler"){
        brushProlfiler->open();
    }
    else if (btnPress == "Pattern Profiler"){
        pp-> open();
    }
    refresh();
}

void MainWindow::downloadItem(QString subfolder, QString fileName, downloadAction action, QString promptTitle, QString promptText) {
    dSubfolder = subfolder;
    dFileName = fileName;
    dAction = action;
    QDir qd(QDir::currentPath() + dSubfolder);
    if (!qd.exists())
        qd.mkdir(QDir::currentPath() + dSubfolder);
    QMessageBox qmb(QMessageBox::Question, promptTitle, promptText, QMessageBox::Yes, this);
    qmb.addButton("No", QMessageBox::NoRole);
    int choice = qmb.exec();
    if (choice == QMessageBox::Yes) {
        downloadWatcher.addPath(QDir::homePath() + "/Downloads/");
        downloadWatcher.connect(&downloadWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(downloadFinished()));
        downloadTimer.connect(&downloadTimer, SIGNAL(timeout()), this, SLOT(downloadTimeout()));
        downloadTimer.start(5000);
        QDesktopServices::openUrl(QUrl::fromUserInput(FetchLink + dSubfolder + dFileName));
        setEnabled(false);
    }
    else if (dAction == DownloadThenRestart)
        QApplication::exit();
}

void MainWindow::downloadFinished() {
    QFile qf(QDir::homePath() + "/Downloads/" + dFileName);
    if (qf.exists()) {
        downloadTimer.stop();
        bool copied = false;
        while (!copied)
            copied = qf.copy(QDir::currentPath() + dSubfolder + dFileName);
        QDir qd;
        qd.remove(qf.fileName());
        if (dAction == DownLoadThenOpen)
            QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath() + dSubfolder + dFileName));
        else if (dAction == DownloadThenRestart) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath() + "/Roto.exe"));
            QApplication::exit();
        }
        downloadWatcher.removePath(QDir::homePath() + "/Downloads/");
        setEnabled(true);
    }
}

void MainWindow::downloadTimeout() {
    downloadTimer.stop();
    downloadWatcher.removePath(QDir::homePath() + "/Downloads/");
    if (dAction == DownloadThenRestart) {
        qme->showMessage("Unable to download or connect to internet.\nPlease fix Glass Opus installation.");
        QTimer::singleShot(5000, this, SLOT(close()));
    }
    else {
        setEnabled(true);
        qme->showMessage("Unable to download or connect to internet.");
    }
}

void MainWindow::changeVectorFilter(string s) {
    if (ioh->getWorkingLayer() == nullptr || ioh->getWorkingLayer()->getActiveVectors().size() == 0)
        return;
    ioh->getWorkingLayer()->setVectorFilter(s);
    refresh();
}

void MainWindow::changeScreenFilter(string filterName) {
    if (ioh->getWorkingLayer() == nullptr)
        return;
    ioh->getWorkingLayer()->setFilter(filterName);
    refresh();
}

void MainWindow::changeBrushFilter(string filterName) {
    bh.setFilter(filterName);
}

void MainWindow::changeBrushShape(string shape) {
    bh.setShape(shape);
    sr->updateHoverMap(bh.getSize(), bh.getBrushMap());
}

void MainWindow::changeBrushMethod(string method) {
    bh.setAppMethod(method);
}

void MainWindow::log(string title, QObject *obj) {
    objFetch[title] = obj;
    toDel.push_front(obj);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (ioh->getWorkingLayer() == nullptr)
        return;
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
        if (ctrlFlag) {
            if (mode == Spline_Mode)
                ioh->cutVectors();
            else if (mode == Raster_Mode)
                ioh->cutRaster();
        }
        break;
    case Key_C:
        if (ctrlFlag) {
            if (mode == Spline_Mode)
                ioh->copyVectors();
            else if (mode == Raster_Mode)
                ioh->copyRaster();
        }
        break;
    case Key_V:
        if (ctrlFlag) {
            if (mode == Spline_Mode)
                ioh->pasteVectors();
            else if (mode == Raster_Mode)
                ioh->pasteRaster();
        }
        break;
    case Key_A:
        if (ctrlFlag) {
            ioh->getWorkingLayer()->selectAll();
        }
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
    if (objFetch.find(UI_FileName.toStdString()) != objFetch.end()) {
        QMenuBar *menu = dynamic_cast<QMenuBar *>(objFetch.at(UI_FileName.toStdString()));
        menu->resize(width(), menu->height());
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        for (int i = 0; i < urlList.size(); ++i) {
            QString fileName = urlList.at(i).toLocalFile();
            string fn = fileName.toStdString();
            int index = fn.find_last_of('.');
            string fileType = fn.substr(index + 1);
            if (std::find(acceptedImportImageFormats.begin(), acceptedImportImageFormats.end(), fileType) != acceptedImportImageFormats.end()) {
                bool flag = ioh->importImage(fileName);
                if (flag) {
                    resizeCheck->showRelative();
                    while (resizeCheck->isVisible())
                        QCoreApplication::processEvents();
                }
            }
        }
        refresh();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox qmb(QMessageBox::Question, "Glass Opus", "Do you wish to Exit?", QMessageBox::Yes, this);
    qmb.addButton("No", QMessageBox::NoRole);
    int choice = qmb.exec();
    if (choice != QMessageBox::Yes)
        event->ignore();
}

void MainWindow::setMode(EditMode emode) {
    mode = emode;
    if (ioh->getWorkingLayer() != nullptr)
    ioh->getWorkingLayer()->setMode(emode);
    sr->setMode(emode);
}

void MainWindow::refresh() {
    sr->repaint();
}

MainWindow::~MainWindow() {
    delete qme;
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

