#include "brushshape.h"
#include "ui_brushShape.h"

brushShape::brushShape(QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::brushShape)
{
    setWindowFlag(Qt::WindowStaysOnTopHint, true);
    ui->setupUi(this);
    setWindowTitle("Brush Shape Profiler");
    bh.setSize(0);
    qi = QImage(2 * maxRadius + 1, 2 * maxRadius + 1, QImage::Format_ARGB32_Premultiplied);
    qi.fill(0xFFFFFFFF);
    ui->label->setPixmap(QPixmap::fromImage(qi.scaled(qi.width() * dispScaler, qi.height() * dispScaler)));
    ui->spinBox_2->hide();
    ui->label_3->hide();
}

vector <vector <unsigned char> > brushShape::getShapeSize(int size) {
    QImage resized = qi.scaled(2 * size + 1, 2 * size + 1);
    vector <vector <unsigned char> > ret;
    vector <unsigned char> insert;
    while (insert.size() < static_cast<size_t>(resized.width()))
        insert.push_back(0);
    while (ret.size() < static_cast<size_t>(resized.width()))
        ret.push_back(insert);
    for (int i = 0; i < resized.width(); ++i)
        for (int j = 0; j < resized.height(); ++j)
            ret[i][j] = resized.pixel(i, j) == 0xFF000000 ? 1 : 0;
    return ret;
}

void brushShape::on_spinBox_2_valueChanged(int width) {

}

void brushShape::on_buttonBox_accepted() {
    done(1);
}

void brushShape::on_buttonBox_rejected() {
    close();
}

void brushShape::mouseMoveEvent(QMouseEvent *event){
    if (drawing) {
        QPoint lPoint = event->pos();
        QPoint temPoint = QPoint(lPoint.x() / dispScaler - 3, lPoint.y() / dispScaler - 3);
        bh.applyBrush(&qi, temPoint);
        ui->label->setPixmap(QPixmap::fromImage(qi.scaled(qi.width() * dispScaler, qi.height() * dispScaler)));
        ui->label->repaint();
    }
}

void brushShape::mousePressEvent(QMouseEvent *event){
    QPoint lPoint = event->pos();
    QPoint temPoint = QPoint(lPoint.x() / dispScaler - 3, lPoint.y() / dispScaler - 3);
    if (event->button() == Qt::LeftButton) {
        bh.setBrushColor(0xFF000000);
        lb = Qt::LeftButton;
    }
    else if (event->button() == Qt::RightButton) {
        bh.setBrushColor(0xFFFFFFFF);
        lb = Qt::RightButton;
    }
    bh.applyBrush(&qi, temPoint);
    drawing = true;
    bh.setInterpolationActive(true);
    ui->label->setPixmap(QPixmap::fromImage(qi.scaled(qi.width() * dispScaler, qi.height() * dispScaler)));
    ui->label->repaint();
}

void brushShape::mouseReleaseEvent(QMouseEvent *event){
    bh.setInterpolationActive(false);
}
