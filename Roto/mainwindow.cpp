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
        ioh->setDims(QSize(static_cast<int>((16.0/9.0) * static_cast<float>(sizeY)), sizeY));
    }
    setMode(Brush_Mode);
    sr->updateHoverMap(bh.getSize(), bh.getBrushMap());
    sr->setHoverActive(true);
    brushProlfiler = new brushShape(&bh,this);
    pp = new patternProfiler(&bh,this);
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
            if (maxFlag)
                this->showMaximized();
            else
                setGeometry(reset);
            if (flag)
                resizeCheck->showRelative();
        }
        refresh();
        if (maxFlag)
            this->showMaximized();
        else
            setGeometry(reset);
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
        return;
        brushProlfiler->open();
    }
    else if (btnPress == "Pattern Profiler")
        pp->open();
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
        if (ctrlFlag)
            runTests();
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
                    if (maxFlag)
                        this->showMaximized();
                    else
                        setGeometry(reset);
                    resizeCheck->showRelative();
                    while (resizeCheck->isVisible())
                        QCoreApplication::processEvents();
                }
            }
        }
        refresh();
        if (maxFlag)
            this->showMaximized();
        else
            setGeometry(reset);
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
    tests.push_back("Save/Load");
    testPrompt.setOptions(QInputDialog::UseListViewForComboBoxItems);
    testPrompt.setComboBoxItems(tests);
    testPrompt.setWindowTitle("Test Selection");
    testPrompt.setWhatsThis("This menu is for selection of what test to run");
    testPrompt.exec();
    if (testPrompt.textValue() == "")
        return;
    if (testPrompt.textValue() == "Run All Tests") {
        qmb.setText("Running Brush Shapes Test");
        qmb.exec();
        BrushShapesTest();
        qmb.setText("Running Brush Colors Test");
        qmb.exec();
        BrushColorsTest();
        qmb.setText("Running Spray Density Test");
        qmb.exec();
        sprayDensityTest();
        qmb.setText("Running Brush Methods Test");
        qmb.exec();
        BrushMethodsTest();
        qmb.setText("Running Save Load Test");
        qmb.exec();
        saveLoadTest();
        qmb.setText("All Tests Concluded");
        qmb.exec();
    }
    if (testPrompt.textValue() == "Brush Shapes")
        BrushShapesTest();
    else if (testPrompt.textValue() == "Brush Colors")
        BrushColorsTest();
    else if (testPrompt.textValue() == "Spray Density")
        sprayDensityTest();
    else if (testPrompt.textValue() == "Brush Methods")
        BrushMethodsTest();
    else if (testPrompt.textValue() == "Save/Load")
        saveLoadTest();
}

void MainWindow::BrushShapesTest() {
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    canvas->fill(0xFFFFFFFF);
    bh.setBrushColor(0xFF000000);
    bh.setShape("Square");
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(w/9, i));
    }
    bh.setShape("Circle");
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint((w/9)*2, i));
    }
    bh.setShape("Horizontal");
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint((w/9)*3, i));
    }
    bh.setShape("Vertical");
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint((w/9)*4, i));
    }
    bh.setShape("Left Diagonal");
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint((w/9)*5, i));
    }
    bh.setShape("Right Diagonal");
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint((w/9)*6, i));
    }
    bh.setShape("Diamond");
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint((w/9)*7, i));
    }
    bh.setShape("Octagon");
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint((w/9)*8, i));
    }
}

void MainWindow::BrushColorsTest() {
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    canvas->fill(0xFFFFFFFF);
    bh.setBrushColor(0xFF000000);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(w/10, i));
    }
    bh.setBrushColor(0xFFFF0000);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(2*(w/10), i));
    }
    bh.setBrushColor(0xFFFFFF00);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(3*(w/10), i));
    }
    bh.setBrushColor(0xFF00FF00);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(4*(w/10), i));
    }
    bh.setBrushColor(0xFF00FFFF);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(5*(w/10), i));
    }
    bh.setBrushColor(0xFF0000FF);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(6*(w/10), i));
    }
    bh.setBrushColor(0xFFA0A0A0);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(7*(w/10), i));
    }
    bh.setBrushColor(0xFF55AA22);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(8*(w/10), i));
    }
    bh.setBrushColor(0xFFBB0AAA);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(9*(w/10), i));
    }
}

void MainWindow::sprayDensityTest() {
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    canvas->fill(0xFFFFFFFF);
    bh.setBrushColor(0xFF000000);
    bh.setDensity(1);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(w/5, i));
    }
    bh.setDensity(41);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(2*(w/5), i));
    }
    bh.setDensity(81);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(3*(w/5), i));
    }
    bh.setDensity(121);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.applyBrush(canvas, QPoint(4*(w/5), i));
    }
    bh.setDensity(1);
}

void MainWindow::BrushMethodsTest() {
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    canvas->fill(0xFF222222);
    bh.setBrushColor(0xFFFF0000);
    for (int i = h*0.25; i < h*0.75; i++) {
        bh.setAppMethod("Overwrite");
        bh.applyBrush(canvas, QPoint(w/5, i));
        bh.setAppMethod("Additive");
        bh.applyBrush(canvas, QPoint(2*(w/5), i));
        bh.setAppMethod("Subtractive");
        bh.applyBrush(canvas, QPoint(3*(w/5), i));
    }
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
    for (int i = 0; i < w/4; i++) {
        for (int j = 0; j < h; j++) {
            canvas->setPixelColor(i,j,0xFF000000);
        }
    }
    ioh->addLayer();
    ioh->getWorkingLayer()->setAlpha(150);
    canvas = ioh->getWorkingLayer()->getCanvas();
    for (int i = w/4; i < w/2; i++) {
        for (int j = 0; j < h; j++) {
            canvas->setPixelColor(i,j,0xFFFF0000);
        }
    }
    ioh->addLayer();
    ioh->getWorkingLayer()->setAlpha(100);
    canvas = ioh->getWorkingLayer()->getCanvas();
    for (int i = w/2; i < 3*(w/4); i++) {
        for (int j = 0; j < h; j++) {
            canvas->setPixelColor(i,j,0xFFFFFF00);
        }
    }
    ioh->addLayer();
    ioh->getWorkingLayer()->setAlpha(10);
    canvas = ioh->getWorkingLayer()->getCanvas();
    for (int i = 3*(w/4); i < w; i++) {
        for (int j = 0; j < h; j++) {
            canvas->setPixel(i,j,0xFFFF00FF);
        }
    }
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

void MainWindow::VectorTest() {
    /*
    Layer *layer = ioh->getWorkingLayer();
    QImage *canvas = layer->getCanvas();
    int w = canvas->width();
    int h = canvas->height();
    canvas->fill(0xFFFFFFFF);
    layer->setFilter("NormalRGB");
    SplineVector sv;
    sv.addPt(QPoint(w/4, h/4), 1);
    sv.addPt(QPoint(w/2, h/4), 2);
    sv.addPt(QPoint(w/4, h/2), 3);
    sv.addPt(QPoint(w/2, h/2), 4);
    layer->getVectors().push_back(sv);
    for (int i = 0; i < Filtered; ++i) {
        sv.setMode(static_cast<VectorMode>(i));
        for (int j = 0; j < Double; ++j) {
            sv.setTaperType(static_cast<Taper>(j));
            for (int h = 0; h < maxWidth; ++h) {
                sv.setWidth(i);
                for (int k = 0; k < maxTaper; ++k) {
                    sv.setTaper1(k);
                    for (int l = 0; l < maxTaper; ++l) {
                        sv.setTaper2(l);
                        for (int theta = 0; theta < 3600; ++theta) {
                            //sv.rotate(QPoint(0,0));
                        }
                    }
                }
            }
        }
    }*/
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
