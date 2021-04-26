#include "patternprofiler.h"
#include "ui_patternprofiler.h"

patternProfiler::patternProfiler(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::patternProfiler)
{
    this->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    ui->setupUi(this);
    setWindowTitle("Pattern Profiler");
    qb = QImage(maxRadius, maxRadius, QImage::Format_ARGB32_Premultiplied);
    bh.setSize(0);
    qb.fill(0x00FFFFFF);
    for (int i = 0; i < 2; ++i) {
        pattern.push_back(vector <unsigned char> ());
        for (int j = 0; j < 2; ++j)
            pattern[i].push_back(i == j ? 1 : 0);
    }
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            qb.setPixel(i, j, i == j ? 0xFF000000 : 0xFFFFFFFF);
    ui->label->setPixmap(QPixmap::fromImage(qb.scaled(maxRadius * dispScale, maxRadius * dispScale)));
}

patternProfiler::~patternProfiler() {
    delete ui;
}

void patternProfiler::on_spinBox_2_valueChanged(int width) {
    if (pattern.size() > static_cast<size_t>(width)) {
        for (size_t i = width; i < pattern.size(); ++i)
            for (size_t j = 0; j < pattern[0].size(); ++j)
                qb.setPixel(i, j, 0x00FFFFFF);
        while (pattern.size() > static_cast<size_t>(width))
            pattern.pop_back();
    }
    if (pattern.size() < static_cast<size_t>(width)) {
        for (int i = pattern.size(); i < width; ++i)
            for (size_t j = 0; j < pattern[0].size(); ++j)
                qb.setPixel(i, j, 0xFFFFFFFF);
        vector <unsigned char> v;
        while (v.size() < pattern[0].size())
            v.push_back(0);
        while (pattern.size() < static_cast<size_t>(width))
            pattern.push_back(v);
    }
    ui->label->setPixmap(QPixmap::fromImage(qb.scaled(maxRadius * dispScale, maxRadius * dispScale)));
}

void patternProfiler::on_spinBox_valueChanged(int height) {
    if (pattern[0].size() > static_cast<size_t>(height)) {
        for (size_t i = 0; i < pattern.size(); ++i)
            for (size_t j = height; j < pattern[i].size(); ++j)
                qb.setPixel(i, j, 0x00FFFFFF);
        for (size_t i = 0; i < pattern.size(); ++i)
            while (pattern[i].size() > static_cast<size_t>(height))
                pattern[i].pop_back();
    }
    if (pattern[0].size() < static_cast<size_t>(height)) {
        for (size_t i = 0; i < pattern.size(); ++i)
            for (int j = pattern[0].size(); j < height; ++j)
                qb.setPixel(i, j, 0xFFFFFFFF);
        for (size_t i = 0; i < pattern.size(); ++i)
            while (pattern[i].size() < static_cast<size_t>(height))
                pattern[i].push_back(0);
    }
    ui->label->setPixmap(QPixmap::fromImage(qb.scaled(maxRadius * dispScale, maxRadius * dispScale)));
}

void patternProfiler::on_buttonBox_accepted() {
    done(1);
}

void patternProfiler::on_buttonBox_rejected() {
    done(0);
}

vector <vector <unsigned char> > patternProfiler::getPattern() {
    return pattern;
}

void patternProfiler::mouseMoveEvent(QMouseEvent *event) {
    if (drawing) {
        QPoint lPoint = event->pos();
        QPoint temPoint = QPoint(lPoint.x() / dispScale - 1, lPoint.y() / dispScale - 1);
        if (static_cast<size_t>(temPoint.x()) < pattern.size() && static_cast<size_t>(temPoint.y()) < pattern[0].size()) {
            bh.applyBrush(&qb, temPoint);
            if (lb == Qt::LeftButton)
                pattern[temPoint.x()][temPoint.y()] = 1;
            else if (lb == Qt::RightButton)
                pattern[temPoint.x()][temPoint.y()] = 0;
            ui->label->setPixmap(QPixmap::fromImage(qb.scaled(maxRadius * dispScale, maxRadius * dispScale)));
        }
    }
}

void patternProfiler::mousePressEvent(QMouseEvent *event) {
    QPoint lPoint = event->pos();
    QPoint temPoint = QPoint(lPoint.x() / dispScale - 1, lPoint.y() / dispScale - 1);
    if (static_cast<size_t>(temPoint.x()) < pattern.size() && static_cast<size_t>(temPoint.y()) < pattern[0].size()) {
        if (event->button() == Qt::LeftButton) {
            bh.setBrushColor(0xFF000000);
            lb = Qt::LeftButton;
        }
        else if (event->button() == Qt::RightButton) {
            bh.setBrushColor(0xFFFFFFFF);
            lb = Qt::RightButton;
        }
        bh.applyBrush(&qb, temPoint);
        drawing = true;
        bh.setInterpolationActive(true);
        ui->label->setPixmap(QPixmap::fromImage(qb.scaled(maxRadius * dispScale, maxRadius * dispScale)));
    }
}

void patternProfiler::mouseReleaseEvent(QMouseEvent *event){
    bh.setInterpolationActive(false);
    drawing = false;
}
