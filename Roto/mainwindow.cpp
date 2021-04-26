#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(string startPath, string projectFile, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumSize(450, 450);
    hide();
    long long t = stdFuncs::getTime();
    QDir::setCurrent(startPath.c_str());
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
        logo.setWindowFlag(Qt::WindowType::WindowStaysOnTopHint, true);
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
        QMenu* rFiltering = static_cast<QMenu *>(objFetch.at("Apply Filter To Selection"));
        for (string name : filterNames) {
            QAction *sAction = sFiltering->addAction((name).c_str());
            QAction *bAction = bFiltering->addAction((name).c_str());
            QAction *rAction = rFiltering->addAction((name).c_str());
            connect(sAction, &QAction::triggered, this, [=]() { this->changeScreenFilter(sAction->text().toStdString()); });
            connect(bAction, &QAction::triggered, this, [=]() { this->changeBrushFilter(bAction->text().toStdString()); });
            connect(rAction, &QAction::triggered, this, [=]() { this->applyRasterFilter(rAction->text().toStdString()); });
            log(name, sAction);
            log(name, bAction);
            log(name, rAction);
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
    move(center - rect().center());
    if (projectFile == "") {
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
        t = stdFuncs::getTime(t);
        if (logoFound)
            std::this_thread::sleep_for(std::chrono::milliseconds(t > 2000 ? 0 : 2000 - t));
        show();
        logo.hide();
        resPrompt.exec();
        int sizeY = stoi(resPrompt.textValue().toStdString());
        ioh->setDims(QSize(static_cast<int>((16.0 / 9.0) * static_cast<float>(sizeY)), sizeY));
    }
    setMode(Brush_Mode);
    sr->updateHoverMap(bh.getSize(), bh.getBrushMap());
    sr->setHoverActive(true);
    brushProlfiler = new brushShape(this);
    pp = new patternProfiler(this);
    if (projectFile != "") {
        show();
        ioh->load(QString(projectFile.c_str()));
        saveFileName = projectFile.c_str();
        string toFind = projectFile.find_last_of("/") >= projectFile.length() ? "\\" : "/";
        projectFile = projectFile.substr(projectFile.find_last_of(toFind) + 1);
        projectFile = projectFile.substr(0, projectFile.length() - 6);
        setWindowTitle(QString("Glass Opus - ") + projectFile.c_str());
        refresh();
        t = stdFuncs::getTime(t);
        if (logoFound)
            std::this_thread::sleep_for(std::chrono::milliseconds(t > 2000 ? 0 : 2000 - t));
        logo.hide();
    }
    histograms = new QLabel();
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
        if (mode == Brush_Mode)
            takeFlag = true;
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
        if (!shiftFlag)
            takeFlag = false;
        return;
    }
    if (mode == Brush_Mode) {
        sr->setHoverActive(true);
        bh.setInterpolationActive(false);
        refresh();
    }
    else if (mode == Spline_Mode || (mode == Raster_Mode && event->button() != Qt::RightButton))
        ioh->getWorkingLayer()->release(qp, event->button());
    else if (event->button() >= 8) {
        takeFlag = false;
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
    scrollLock.lock();
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
            if (bh.getBrushShape() == custom)
                bh.setShape(brushShapes[bh.getBrushShape()], brushProlfiler->getShapeSize(bh.getSize()));
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
    scrollLock.unlock();
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
        bool maxFlag = isMaximized();
        QRect reset = geometry();
        string formats = "";
        for (string s : acceptedImportImageFormats)
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
            if (maxFlag)
                this->showMaximized();
            if (flag)
                resizeCheck->showRelative();
        }
        refresh();
        setGeometry(reset);
        if (maxFlag)
            this->showMaximized();
    }
    else if (btnPress == "Open") {
        QMessageBox::StandardButton prompt = QMessageBox::question(this, "Open Project File", "Opening a project file will replace the current work. Continue?", QMessageBox::Yes|QMessageBox::No);
        if (prompt == QMessageBox::Yes) {
            QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"), "/", tr("Glass Opus project files (*.glass)"));
            if (fileName == "")
                return;
            int ret = 0;
            ioh->load(fileName);
            if (ret == 0) {
                saveFileName = fileName;
                string s = saveFileName.toStdString();
                s = s.substr(s.find_last_of("/") + 1);
                s = s.substr(0, s.length() - 6);
                setWindowTitle(QString("Glass Opus - ") + s.c_str());
                refresh();
            }
            else if (ret == 1)
                qme->showMessage("File load aborted. File may be corrupted.");
            else if (ret == 2)
                qme->showMessage("File could not be opened.");
        }

    }
    else if (btnPress == "Help") {
        bool found = QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath() + Doc_Loc + Doc_FileName));
        if (!found)
            downloadItem(Doc_Loc, Doc_FileName, DownLoadThenOpen, "Documentation Not Found", "Documentation PDF not found locally/offline.\nFetch and download documentation online?");
    }
    else if (btnPress == "About") {
        QMessageBox qmb(QMessageBox::Question, "Glass Opus", "Glass Opus is an open source rotoscoping software for students and artists. The software will provide a variety of features to allow the users to see their work from start to finish. Drawing with a variety brushes and vectors, image manipulation, and filtering are among the many features than one can employ to create their vision.\n\nThe focus of Glass Opus, and the team behind it, is to provide a free software that students and artists can use to further their work and portfolio. This is often a difficult endeavor for artists due to the restrictive cost of major softwares. Since Glass Opus is open source, users can tweak features or add their own to suit specific needs. It will also serve as a foundation for those who seek to improve their knowledge in image processing and manipulation, as well as basic graphics programming.", QMessageBox::Yes, this);
        qmb.exec();
    }
    else if (btnPress == "Exit") {
        QMessageBox qmb(QMessageBox::Question, "Glass Opus", "Close Glass Opus?", QMessageBox::Yes, this);
        if (ioh->getNumLayers() != 0)
            qmb.addButton("Save and Close", QMessageBox::AcceptRole);
        qmb.addButton("No", QMessageBox::RejectRole);
        int choice = qmb.exec();
        if (choice == QMessageBox::AcceptRole && ioh->getNumLayers() == 0)
            choice = QMessageBox::RejectRole;
        if (choice == QMessageBox::AcceptRole) {
            if (saveFileName.isEmpty())
                saveFileName = QFileDialog::getSaveFileName(this, tr("Save Project"), "/", tr("Glass Opus project files (*.glass)"));
            ioh->save(saveFileName);
        }
        if (choice != QMessageBox::RejectRole)
            QApplication::exit();
    }
    else if (btnPress == "Insert Layer")
        ioh->addLayer();
    if (ioh->getWorkingLayer() == nullptr)
        return;
    else if (btnPress == "Export") {    //TODO
        sr->stopFlashing();
        string formats = "";
        for (string s : acceptedExportImageFormats)
            formats += " *." + s;
        formats = "Media Files (" + formats.substr(1) + ")";
        QString fileName = QFileDialog::getSaveFileName(this, tr("Export"), "/", tr(formats.c_str()));
        if (fileName == "")
            return;
        string fn = fileName.toStdString();
        int index = fn.find_last_of('.');
        string fileType = fn.substr(index + 1);
        if (std::find(acceptedExportImageFormats.begin(), acceptedExportImageFormats.end(), fileType) != acceptedExportImageFormats.end())
            ioh->exportImage(fileName);
        if (mode != Brush_Mode)
            sr->resume();
    }
    else if (btnPress == "Save") {
        if (saveFileName.isEmpty())
            saveFileName = QFileDialog::getSaveFileName(this, tr("Save Project"), "/", tr("Glass Opus project files (*.glass)"));
        ioh->save(saveFileName);

    }
    else if (btnPress == "Save As") {
        saveFileName = QFileDialog::getSaveFileName(this, tr("Save Project As"), "/", tr("Glass Opus project files (*.glass)"));
        ioh->save(saveFileName);
        string s = saveFileName.toStdString();
        s = s.substr(s.find_last_of("/") + 1);
        s = s.substr(0, s.length() - 6);
        setWindowTitle(QString("Glass Opus - ") + s.c_str());
    }
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
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a brush radius", bh.getSize(), minRadius, maxRadius, 1, &ok);
        if (ok) {
            radialProfiler->updateSize(ret);
            if (bh.getBrushShape() == custom)
                bh.setShape(brushShapes[bh.getBrushShape()], brushProlfiler->getShapeSize(bh.getSize()));
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
    else if (btnPress == "Transparency Fill")
        bh.setFillColor(QColor(255, 255, 255, 0));
    else if (btnPress == "Apply Kernal To Selection") {
        string ret;
        while (true) {
            QInputDialog kerPrompt;
            QDir qdir(QDir::currentPath() + Kernal_Loc);
            QStringList items, temp = qdir.entryList();
            temp.pop_front();
            temp.pop_front();
            for (QString qs : temp)
                if (qs != "(Custom)") {
                    string str = qs.toStdString();
                    str = str.substr(0, str.find_last_of("."));
                    items.push_back(str.c_str());
                }
            items.push_back("(Custom)");
            kerPrompt.setOptions(QInputDialog::UseListViewForComboBoxItems);
            kerPrompt.setComboBoxItems(items);
            kerPrompt.setTextValue(items.first());
            kerPrompt.setWindowTitle("Kernal Selection");
            kerPrompt.setWhatsThis("Kernals allow for a variety of effects to be applied to the image");
            kerPrompt.exec();
            ret = kerPrompt.textValue().toStdString();
            if (ret != "(Custom)")
                break;
            else {
                QString fileName = QFileDialog::getOpenFileName(this, tr("Import Kernal"), "/", tr("*.txt"));
                if (fileName != "") {
                    string file = fileName.toStdString();
                    file = file.substr(file.find_last_of("/") + 1);
                    vector <vector <float> > kernal = graphics::ImgSupport::loadKernal(fileName.toStdString()).second;
                    if (!(kernal.size() == 1 && kernal[0].size() == 1 && kernal[0][0] == 1.0))
                        QFile(fileName).copy(QDir::currentPath() + Kernal_Loc + file.c_str());
                }
            }
        }
        ioh->getWorkingLayer()->applyKernalToSelection(progress, QDir::currentPath().toStdString() + Kernal_Loc.toStdString() + ret + ".txt");
    }
    else if (btnPress == "Layer Opacity (Alpha)") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a layer alpha", ioh->getWorkingLayer()->getAlpha(), 1, graphics::maxColor, 1, &ok);
        if (ok)
            ioh->getWorkingLayer()->setAlpha(ret);
    }
    else if (btnPress == "Brush Mode") {
        ioh->getWorkingLayer()->deselect();
        setMode(Brush_Mode);
    }
    else if (btnPress == "Vector Mode") {
        ioh->getWorkingLayer()->deselect();
        setMode(Spline_Mode);
        setSamplePt(QPoint(-1000, -1000));
    }
    else if (btnPress == "Raster Mode") {
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
            ioh->getWorkingLayer();
        }
    }
    else if (btnPress == "Layer Filter Strength") {
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Glass Opus", "Set current layer's filter strength", ioh->getWorkingLayer()->getFilterStrength(), 1, graphics::maxColor, 1, &ok) - 1;
        if (ok)
            ioh->getWorkingLayer()->setFilterStrength(ret);
    }
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
    else if (btnPress == "Zoom 100%")
        sr->setZoom(1.0);
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
    else if (btnPress == "Shape Profiler") {
        brushProlfiler->exec();
        bh.setShape(brushShapes[bh.getBrushShape()], brushProlfiler->getShapeSize(bh.getSize()));
    }
    else if (btnPress == "Pattern Profiler") {
        pp->exec();
        bh.setPattern(pp->getPattern());
    }
    else if (btnPress == "Histogram Equalization") {
        QInputDialog kerPrompt;
        QStringList items;
        items.push_back("Equalize Intensity");
        items.push_back("Equalize Red Channel");
        items.push_back("Equalize Green Channel");
        items.push_back("Equalize Blue Channel");
        kerPrompt.setOptions(QInputDialog::UseListViewForComboBoxItems);
        kerPrompt.setComboBoxItems(items);
        kerPrompt.setTextValue(items.first());
        kerPrompt.setWindowTitle("Histogram Equalization");
        int execCode = kerPrompt.exec();
        if (execCode == 0)
            return;
        int index = items.indexOf(kerPrompt.textValue());
        int histo[bins];
        for (int i = 0; i < bins; ++i)
            histo[i] = 0;
        QImage image = ioh->getWorkingLayer()->getCanvas()->copy();
        // Fill the array(s) tht the histograms will be constructed from.
        int value;
        int total = 0;
        for (int x = 0; x < image.width(); ++x)
            for (int y = 0; y < image.height(); ++y) {
                QColor qc = image.pixelColor(x, y);
                if (qc.alpha() != 0) {
                    switch (index) {
                    case 0:
                        value = static_cast<int>(static_cast<float>(qc.red() + qc.green() + qc.blue()) / 3.0 + 0.5);
                        break;
                    case 1:
                        value = qc.red();
                        break;
                    case 2:
                        value = qc.green();
                        break;
                    case 3:
                        value = qc.blue();
                        break;
                    }
                    ++total;
                    ++histo[value];
                }
            }
        int i = 0;
        while (i < bins && histo[i] == 0)
            ++i;
        if (i == bins || histo[i] == total)
            return;
        float scale = static_cast<double>(bins - 1) / static_cast<double>(total - histo[i]);
        int lut[bins];
        for (int j = 0; j < bins; ++j)
            lut[j] = 0;
        int sum = 0;
        for (++i; i < bins; ++i) {
            sum += histo[i];
            lut[i] = max(0, min(255, static_cast<int>(static_cast<float>(sum) * scale)));
        }
        QImage *canvas = ioh->getWorkingLayer()->getCanvas();
        for (int x = 0; x < image.width(); ++x)
            for (int y = 0; y < image.height(); ++y) {
                QColor qc = image.pixelColor(x, y);
                if (qc.alpha() != 0) {
                    switch (index) {
                    case 0:
                        qc = QColor(lut[qc.red()], lut[qc.green()], lut[qc.blue()]);
                        break;
                    case 1:
                        qc.setRed(lut[qc.red()]);
                        break;
                    case 2:
                        qc.setGreen(lut[qc.green()]);
                        break;
                    case 3:
                        qc.setBlue(lut[qc.blue()]);
                        break;
                    }
                    canvas->setPixelColor(x, y, qc);
                }
            }
    }
    else if (btnPress == "Histograms") {
        int h = 3 * bins;
        QImage qi (QSize(bins * 2, h), QImage::Format_ARGB32_Premultiplied);
        histograms->resize(qi.width(), qi.height());
        histograms->setWindowFilePath(("Histogram of Layer " + to_string(ioh->getActiveLayer() + 1)).c_str());
        qi.fill(0xFF000000);
        int histo[4][bins];
        for (int x = 0; x < bins; ++x)
            for (int y = 0; y < 4; ++y)
                histo[y][x] = 0;
        QImage image = ioh->getWorkingLayer()->getCanvas()->copy();
        // Fill the array(s) tht the histograms will be constructed from.
        int total = 0;
        for (int x = 0; x < image.width(); ++x)
            for (int y = 0; y < image.height(); ++y) {
                QColor qc = image.pixelColor(x, y);
                if (qc.alpha() != 0) {
                    int intensity = static_cast<int>(static_cast<float>(qc.red() + qc.green() + qc.blue()) / 3.0 + 0.5);
                    ++histo[0][intensity];
                    ++histo[1][qc.red()];
                    ++histo[2][qc.green()];
                    ++histo[3][qc.blue()];
                    ++total;
                }
            }
        int maxI = 0, cutoff = (total) / 4;
        for (int j = 0; j < 4; ++j)
            for (int i = 1; i < bins - 1; ++i)
                if (histo[j][i] < cutoff)
                    maxI = max(maxI, histo[j][i]);
        // Draw histograms.
        double div = static_cast<double>(h / 2 - 1) / static_cast<double>(maxI);
        for (int x = 0; x < bins; ++x) {
            QRgb value = static_cast<QRgb>(bins + x) / 2;
            for  (int j = 0; j < 4; ++j) {
                QRgb color = 0xFF000000;
                if (j != 0)
                    color += (value << (8 * (2 - (j - 1))));
                else
                    for (unsigned char i= 0; i < 3; ++i)
                        color += value << (8 * i);
                int rowOffset = (h / 2) * (j / 2);
                int colOffset = x + bins * (j % 2);
                int stop = h / 2 + rowOffset;
                int start = stop - static_cast<int>(static_cast<double>(histo[j][x]) * div);
                start = max(start, rowOffset);
                for (int y = start; y < stop; ++y)
                    qi.setPixelColor(colOffset, y, color);
            }
        }
        histograms->setPixmap(QPixmap::fromImage(qi));
        histograms->setWindowModality(Qt::ApplicationModal);
        histograms->show();
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
    if (shape == "Custom") {
        brushProlfiler->exec();
        bh.setShape(shape, brushProlfiler->getShapeSize(bh.getSize()));
    }
    else
        bh.setShape(shape);
    sr->updateHoverMap(bh.getSize(), bh.getBrushMap());
}

void MainWindow::changeBrushMethod(string method) {
    bh.setAppMethod(method);
}

void MainWindow::applyRasterFilter(string s) {
    int defaultStr = 255;
    for (int i = 0; i < graphics::numFilters; ++i)
        if (filterNames[i] == s) {
            defaultStr = graphics::filterPresets[i];
            break;
        }
    bool ok = false;
    int ret = QInputDialog::getInt(this, "Glass Opus", "Please enter a strength", defaultStr, graphics::minColor, graphics::maxColor, 1, &ok);
    if (ok)
        ioh->getWorkingLayer()->applyFilterToRaster(Filter(defaultStr, s));
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
        if (!ctrlFlag) {
            if (mode == Brush_Mode)
                takeFlag = true;
            setShiftFlag(true);
        }
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
        if (ctrlFlag)
            ioh->getWorkingLayer()->selectAll();
        break;
    case Key_F7:
        QMessageBox::StandardButton prompt = QMessageBox::question(this, "Run Diagnostic Mode", "You are about to run Diagnostic Mode. Running tests will wipe your current project. Continue?", QMessageBox::Yes|QMessageBox::No);
        if (prompt == QMessageBox::Yes) {
            runTests();
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Key_Shift:
        if (mode == Brush_Mode)
            takeFlag = false;
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
        bool maxFlag = isMaximized();
        QRect reset = geometry();
        QList<QUrl> urlList = mimeData->urls();
        for (int i = 0; i < urlList.size(); ++i) {
            QString fileName = urlList.at(i).toLocalFile();
            string fn = fileName.toStdString();
            int index = fn.find_last_of('.');
            string fileType = fn.substr(index + 1);
            if (std::find(acceptedImportImageFormats.begin(), acceptedImportImageFormats.end(), fileType) != acceptedImportImageFormats.end()) {
                bool flag = ioh->importImage(fileName);
                if (flag) {
                    setGeometry(reset);
                    if (maxFlag)
                        this->showMaximized();
                    resizeCheck->showRelative();
                    while (resizeCheck->isVisible())
                        QCoreApplication::processEvents();
                }
            }
        }
        refresh();
        setGeometry(reset);
        if (maxFlag)
            this->showMaximized();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox qmb(QMessageBox::Question, "Glass Opus", "Close Glass Opus?", QMessageBox::Yes, this);
    if (ioh->getNumLayers() != 0)
        qmb.addButton("Save and Close", QMessageBox::AcceptRole);
    qmb.addButton("No", QMessageBox::RejectRole);
    int choice = qmb.exec();
    if (choice == QMessageBox::AcceptRole && ioh->getNumLayers() == 0)
        choice = QMessageBox::RejectRole;
    if (choice == QMessageBox::AcceptRole) {
        if (saveFileName.isEmpty())
            saveFileName = QFileDialog::getSaveFileName(this, tr("Save Project"), "/", tr("Glass Opus project files (*.glass)"));
        ioh->save(saveFileName);
    }
    if (choice == QMessageBox::RejectRole)
        event->ignore();
    else
        QApplication::exit();
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

void MainWindow::runTests() {
    QInputDialog testPrompt;
    QStringList tests;
    QMessageBox qmb;
    tests.push_back("Run All Tests");
    tests.push_back("Brush Shapes");
    tests.push_back("Brush Colors");
    tests.push_back("Spray Density");
    tests.push_back("Brush Methods");
    tests.push_back("Vector Test");
    tests.push_back("Raster Test");
    tests.push_back("Save/Load");
    testPrompt.setOptions(QInputDialog::UseListViewForComboBoxItems);
    testPrompt.setComboBoxItems(tests);
    testPrompt.setWindowTitle("Test Selection");
    testPrompt.setWhatsThis("This menu is for selection of what test to run");
    testPrompt.exec();
    if (testPrompt.textValue() == "") {
        ioh->clearFrame();
        ioh->addLayer();
        return;
    }
    if (testPrompt.textValue() == "Run All Tests") {
        qmb.setText("Running Brush Shapes Test");
        qmb.exec();
        brushShapesTest();
        qmb.setText("Running Brush Colors Test");
        qmb.exec();
        brushColorsTest();
        qmb.setText("Running Spray Density Test");
        qmb.exec();
        sprayDensityTest();
        qmb.setText("Running Brush Methods Test");
        qmb.exec();
        brushMethodsTest();
        qmb.setText("Running Vector Test");
        qmb.exec();
        vectorTest();
        qmb.setText("Running Raster Test");
        qmb.exec();
        rasterTest();
        qmb.setText("Running Save Load Test");
        qmb.exec();
        saveLoadTest();
        qmb.setText("All Tests Concluded");
        qmb.exec();
    }
    if (testPrompt.textValue() == "Brush Shapes")
        brushShapesTest();
    else if (testPrompt.textValue() == "Brush Colors")
        brushColorsTest();
    else if (testPrompt.textValue() == "Spray Density")
        sprayDensityTest();
    else if (testPrompt.textValue() == "Brush Methods")
        brushMethodsTest();
    else if (testPrompt.textValue() == "Vector Test")
        vectorTest();
    else if (testPrompt.textValue() == "Raster Test")
        rasterTest();
    else if (testPrompt.textValue() == "Save/Load")
        saveLoadTest();
    runTests();
}

void MainWindow::brushShapesTest() {
    ioh->clearFrame();
    ioh->addLayer();
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    canvas->fill(0xFFFFFFFF);
    layer->setAlpha(255);
    bh.setBrushColor(0xFF000000);

    bh.setShape("Square");
    bh.applyBrush(canvas, QPoint(w / 9, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 9, h / 4));
    bh.applyBrush(canvas, QPoint(w / 9, 3 * (h / 4)));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Circle");
    bh.applyBrush(canvas, QPoint(2 * (w / 9), h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(2 * w / 9, h / 4));
    bh.applyBrush(canvas, QPoint(2 * w / 9, 3 * (h / 4)));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Horizontal");
    bh.applyBrush(canvas, QPoint(3 * w / 9, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 9, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w/9, 3 * (h / 4)));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Vertical");
    bh.applyBrush(canvas, QPoint(4 * w / 9, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(4 * w / 9, h / 4));
    bh.applyBrush(canvas, QPoint(4 * w / 9, 3 * (h / 4)));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Left Diagonal");
    bh.applyBrush(canvas, QPoint(5 * w / 9, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(5 * w / 9, h / 4));
    bh.applyBrush(canvas, QPoint(5 * w / 9, 3 * (h / 4)));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Right Diagonal");
    bh.applyBrush(canvas, QPoint(6 * w / 9, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(6 * w / 9, h / 4));
    bh.applyBrush(canvas, QPoint(6 * w / 9, 3 * (h / 4)));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Diamond");
    bh.applyBrush(canvas, QPoint(7 * w / 9, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(7 * w / 9, h / 4));
    bh.applyBrush(canvas, QPoint(7 * w / 9, 3 * (h / 4)));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Octagon");
    bh.applyBrush(canvas, QPoint(8 * w / 9, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(8 * w / 9, h / 4));
    bh.applyBrush(canvas, QPoint(8 * w / 9, 3 * (h / 4)));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    canvas->fill(0xFFFFFFFF);
    bh.setShape("Square");
    bh.applyBrush(canvas, QPoint(w / 4, h / 9));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, h / 9));
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 9));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Circle");
    bh.applyBrush(canvas, QPoint(w / 4, 2 * h / 9));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, 2 * h / 9));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 2 * h / 9));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Horizontal");
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 9));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 9));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 3 * h / 9));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Vertical");
    bh.applyBrush(canvas, QPoint(w / 4, 4 * h / 9));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4,  4 * h / 9));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 4 * h / 9));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Left Diagonal");
    bh.applyBrush(canvas, QPoint(w / 4, 5 * h / 9));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, 5 * h / 9));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 5 * h / 9));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Right Diagonal");
    bh.applyBrush(canvas, QPoint(w / 4, 6 * h / 9));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, 6 * h / 9));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 6 * h / 9));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Diamond");
    bh.applyBrush(canvas, QPoint(w / 4, 7 * h / 9));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, 7 * h / 9));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 7 * h / 9));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setShape("Octagon");
    bh.applyBrush(canvas, QPoint(w / 4, 8 * h / 9));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, 8 * h / 9));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 8 * h / 9));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    canvas->fill(0xFFFFFFFF);
    bh.setShape("Square");
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    canvas->fill(0xFFFFFFFF);
    bh.setShape("Circle");
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    canvas->fill(0xFFFFFFFF);
    bh.setShape("Horizontal");
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    canvas->fill(0xFFFFFFFF);
    bh.setShape("Vertical");
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    canvas->fill(0xFFFFFFFF);
    bh.setShape("Left Diagonal");
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    canvas->fill(0xFFFFFFFF);
    bh.setShape("Right Diagonal");
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    canvas->fill(0xFFFFFFFF);
    bh.setShape("Diamond");
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    canvas->fill(0xFFFFFFFF);
    bh.setShape("Octagon");
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 4, h / 4));
    bh.applyBrush(canvas, QPoint(w / 4, 3 * h / 4));
    bh.setInterpolationActive(false);
}

void MainWindow::brushColorsTest() {
    ioh->clearFrame();
    ioh->addLayer();
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    canvas->fill(0xFFFFFFFF);
    layer->setAlpha(255);

    bh.setBrushColor(0xFF000000);
    bh.applyBrush(canvas, QPoint(w / 10, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 10, h / 4));
    bh.applyBrush(canvas, QPoint(w / 10, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setBrushColor(0xFFFF0000);
    bh.applyBrush(canvas, QPoint(2 * w / 10, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(2 * w / 10, h / 4));
    bh.applyBrush(canvas, QPoint(2 * w / 10, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setBrushColor(0xFFFFFF00);
    bh.applyBrush(canvas, QPoint(3 * w / 10, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 10, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 10, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setBrushColor(0xFF00FF00);
    bh.applyBrush(canvas, QPoint(4 * w / 10, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(4 * w / 10, h / 4));
    bh.applyBrush(canvas, QPoint(4 * w / 10, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setBrushColor(0xFF00FFFF);
    bh.applyBrush(canvas, QPoint(5 * w / 10, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(5 * w / 10, h / 4));
    bh.applyBrush(canvas, QPoint(5 * w / 10, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setBrushColor(0xFF0000FF);
    bh.applyBrush(canvas, QPoint(6 * w / 10, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(6 * w / 10, h / 4));
    bh.applyBrush(canvas, QPoint(6 * w / 10, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setBrushColor(0xFFA0A0A0);
    bh.applyBrush(canvas, QPoint(7 * w / 10, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(7 * w / 10, h / 4));
    bh.applyBrush(canvas, QPoint(7 * w / 10, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setBrushColor(0xFF55AA22);
    bh.applyBrush(canvas, QPoint(8 * w / 10, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(8 * w / 10, h / 4));
    bh.applyBrush(canvas, QPoint(8 * w / 10, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setBrushColor(0xFFBB0AAA);
    bh.applyBrush(canvas, QPoint(9 * w / 10, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(9 * w / 10, h / 4));
    bh.applyBrush(canvas, QPoint(9 * w / 10, 3 * h / 4));
    bh.setInterpolationActive(false);

    bh.setBrushColor(0xFF000000);
}

void MainWindow::sprayDensityTest() {
    ioh->clearFrame();
    ioh->addLayer();
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    layer->setAlpha(255);
    canvas->fill(0xFFFFFFFF);
    bh.setBrushColor(0xFF000000);
    bh.setDensity(1);
    bh.applyBrush(canvas, QPoint(w / 5, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 5, h / 4));
    bh.applyBrush(canvas, QPoint(w / 5, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setDensity(41);
    bh.applyBrush(canvas, QPoint(2 * w / 5, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(2 * w / 5, h / 4));
    bh.applyBrush(canvas, QPoint(2 * w / 5, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setDensity(81);
    bh.applyBrush(canvas, QPoint(3 * w / 5, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 5, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 5, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setDensity(121);
    bh.applyBrush(canvas, QPoint(4 * w / 5, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(4 * w / 5, h / 4));
    bh.applyBrush(canvas, QPoint(4 * w / 5, 3 * h / 4));
    bh.setInterpolationActive(false);

    bh.setDensity(0);
}

void MainWindow::rasterTest() {
    ioh->clearFrame();
    ioh->addLayer();
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    layer->setAlpha(255);
    canvas->fill(0xFFFFFFFF);

    for (int i = w / 4; i < w / 2; ++i) {
        for (int j = h / 4; j < h / 2; ++j) {
            canvas->setPixelColor(i, j, 0xFFFF0000);
        }
    }
    for (int i = w / 2; i < 3 * w / 4; ++i) {
        for (int j = h / 4; j < h / 2; ++j) {
            canvas->setPixelColor(i, j, 0xFF0000FF);
        }
    }
    for (int i = w / 4; i < w / 2; ++i) {
        for (int j = h / 2; j < 3 * h / 4; ++j) {
            canvas->setPixelColor(i, j, 0xFF00FF00);
        }
    }
    for (int i = w / 2; i < 3 * w / 4; ++i) {
        for (int j = h / 2; j < 3 * h / 4; ++j) {
            canvas->setPixelColor(i, j, 0xFFFFFF00);
        }
    }
    refresh();
    Sleep(500);

    layer->setMode(Raster_Mode);
    layer->selectAll();
    layer->flipHori();
    refresh();
    Sleep(500);
    layer->flipVert();
    refresh();
    Sleep(500);

    for (int i = 1; i < 315; ++i) {
        layer->spinWheel(30 * i);
        refresh();
        Sleep(2);
    }
    refresh();
    Sleep(500);
    layer->deselect();
    layer->setMode(Brush_Mode);
}

void MainWindow::brushMethodsTest() {
    ioh->clearFrame();
    ioh->addLayer();
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    layer->setAlpha(255);
    canvas->fill(0xFF55AA22);
    bh.setBrushColor(0xFFFF0000);
    bh.setAppMethod("Overwrite");
    bh.applyBrush(canvas, QPoint(w / 5, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(w / 5, h / 4));
    bh.applyBrush(canvas, QPoint(w / 5, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setAppMethod("Additive");
    bh.applyBrush(canvas, QPoint(2 * w / 5, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(2 * w / 5, h / 4));
    bh.applyBrush(canvas, QPoint(2 * w / 5, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setAppMethod("Subtractive");
    bh.applyBrush(canvas, QPoint(3 * w / 5, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(3 * w / 5, h / 4));
    bh.applyBrush(canvas, QPoint(3 * w / 5, 3 * h / 4));
    bh.setInterpolationActive(false);
    refresh();
    Sleep(500);

    bh.setAppMethod("Filter");
    bh.setFilter("Negative");
    bh.setFilterStrength(maxStrength);
    bh.applyBrush(canvas, QPoint(4 * w / 5, h / 4));
    bh.setInterpolationActive(true);
    bh.applyBrush(canvas, QPoint(4 * w / 5, h / 4));
    bh.applyBrush(canvas, QPoint(4 * w / 5, 3 * h / 4));
    bh.setInterpolationActive(false);

    // Reset brush type for further testing
    bh.setAppMethod("Overwrite");
}

void MainWindow::saveLoadTest() {
    ioh->clearFrame();
    QMessageBox qmb;
    qmb.setText("Populating Layers");
    qmb.exec();
    ioh->addLayer();
    QImage * canvas = ioh->getWorkingLayer()->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    ioh->getWorkingLayer()->setAlpha(255);
    for (int i = 0; i < w / 4; i++) {
        for (int j = 0; j < h; j++) {
            canvas->setPixelColor(i,j,0xFF000000);
        }
    }
    list <SplineVector> svs;
    SplineVector sv;
    sv.addPt(QPoint( w /6, h / 6), 0);
    sv.addPt(QPoint(w / 6, h / 2), 1);
    sv.addPt(QPoint(w / 5, h / 4), 2);
    sv.setColor1(0xFFFFFFFF);
    sv.setColor2(0xFFFFFFFF);
    svs.push_back(sv);
    ioh->getWorkingLayer()->pasteVectors(svs);
    refresh();

    Sleep(500);

    ioh->addLayer();
    ioh->getWorkingLayer()->setAlpha(150);
    canvas = ioh->getWorkingLayer()->getCanvas();
    for (int i = w / 4; i < w / 2; i++) {
        for (int j = 0; j < h; j++) {
            canvas->setPixelColor(i,j,0xFFFF0000);
        }
    }
    refresh();
    Sleep(500);

    ioh->addLayer();
    ioh->getWorkingLayer()->setAlpha(100);
    canvas = ioh->getWorkingLayer()->getCanvas();
    for (int i = w / 2; i < 3 * (w / 4); i++) {
        for (int j = 0; j < h; j++) {
            canvas->setPixelColor(i,j,0xFFFFFF00);
        }
    }
    refresh();
    Sleep(500);

    ioh->addLayer();
    ioh->getWorkingLayer()->setAlpha(10);
    canvas = ioh->getWorkingLayer()->getCanvas();
    for (int i = 3 * (w / 4); i < w; i++) {
        for (int j = 0; j < h; j++) {
            canvas->setPixel(i,j,0xFFFF00FF);
        }
    }
    refresh();
    Sleep(500);
    qmb.setText("Saving Project");
    qmb.exec();
    ioh->save("SaveLoadTest.glass");
    ioh->clearFrame();
    qmb.setText("Loading Project");
    qmb.exec();
    ioh->load("SaveLoadTest.glass");
    QFile file("SaveLoadTest.glass");
    file.remove();
}

void MainWindow::vectorTest() {
    ioh->clearFrame();
    ioh->addLayer();
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    canvas->fill(0xFFFFFFFF);
    SplineVector sv;
    sv.addPt(QPoint(w / 4, h / 4), 0);
    sv.addPt(QPoint(w / 2, h / 4), 1);
    sv.addPt(QPoint(w / 4, h / 2), 2);
    sv.addPt(QPoint(w / 2, h / 2), 3);
    list <SplineVector> svs;
    svs.push_back(sv);
    layer->setMode(Spline_Mode);
    layer->pasteVectors(svs);
    for (int i = 0; i < 10; ++i) {
        layer->setWidth(i);
        refresh();
        Sleep(200);
    }
    layer->setWidth(0);
    refresh();
    layer->deleteSelected();
    layer->setMode(Brush_Mode);
}

MainWindow::~MainWindow() {
    delete histograms;
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
