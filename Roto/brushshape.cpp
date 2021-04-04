#include "brushshape.h"
#include "ui_brushShape.h"

std::vector<std::vector<unsigned char>> sPattern(((64*2)+1),std::vector<unsigned char>((64*2)+1));
brushShape::brushShape(brushHandler *bah,QWidget *parent)
    : QDialog(parent)
    ,ui(new Ui::brushShape)

{
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    ui->setupUi(this);
    setWindowTitle("Brush Shape Profiler");
    bh = bah;
    rp = new RadialProfiler(bh,this);
    qy = new QImage((64*8)+1, (64*8)+1, QImage::Format_ARGB32_Premultiplied);
    qy->fill(Qt::white);
    ui->label->setPixmap(QPixmap::fromImage(*qy));
}


void brushShape::on_spinBox_2_valueChanged(int width)
{
    w = width;
    update();
}

void brushShape::paintEvent(QPaintEvent *event){
}

void brushShape::on_buttonBox_accepted()
{
    bh->setSize(w);
    for(int i = 0; i < (w*2)+1; i++){
        for(int j = 0; j < (w*2)+1; j++){
            sPattern[i][j] = pattern2[i][j];
        }
    }
    brush->sendTo(sPattern);
    bh->setShape("Custom");
    done(1);
}

void brushShape::on_buttonBox_rejected()
{
    close();
}

int brushShape::getW(){
    return w;
}

void brushShape::mouseMoveEvent(QMouseEvent *event){
    if((lb == Qt::LeftButton)&& drawing){
        QPoint lPoint = event->pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qy,temPoint);
        int tempx, tempy;
        tempx = temPoint.x()/4;
        tempy = temPoint.y()/4;
        if( pattern2[tempx][tempy] != '1'&& (tempx>(-1) && tempy>(-1))
                && (tempx< 128 && tempy< 128)){
            pattern2[tempx][tempy] = '1';
        }
        ui->label->setPixmap(QPixmap::fromImage(*qy));
    }
    else if ((lb == Qt::RightButton)&& drawing){
        QPoint lPoint = event->pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qy,temPoint);
        int tempx, tempy;
        tempx = temPoint.x()/4;
        tempy = temPoint.y()/4;
        if( pattern2[tempx][tempy] != 0 && (tempx>(-1) && tempy>(-1))
                && (tempx< 128 && tempy< 128)){
            pattern2[tempx][tempy] = 0;

        }
        ui->label->setPixmap(QPixmap::fromImage(*qy));
    }
    update();
}

void brushShape::mousePressEvent(QMouseEvent *event){
    rp -> updateSize(4);
    if(event->button() == Qt::LeftButton){
        bh->setBrushColor(Qt::black);
        QPoint lPoint = event-> pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qy,temPoint);
        drawing = true;
        int tempx, tempy;
        tempx = temPoint.x()/4;
        tempy = temPoint.y()/4;
        if(pattern2[tempx][tempy] != '1' && (tempx>(-1) && tempy>(-1))
                && (tempx< 128 && tempy< 128)){
            pattern2[tempx][tempy] = '1';
        }
        lb = Qt::LeftButton;
        bh->setInterpolationActive(true);
        ui->label->setPixmap(QPixmap::fromImage(*qy));
        update();
    }
    else if(event ->button() == Qt::RightButton){
        bh->setBrushColor(Qt::white);
        QPoint lPoint = event-> pos();
        QPoint temPoint =  QPoint(lPoint.x()-16,lPoint.y()-16);
        bh->applyBrush(qy,temPoint);
        drawing = true;
        int tempx, tempy;
        tempx = temPoint.x()/4;
        tempy = temPoint.y()/4;
        if(pattern2[tempx][tempy] != 0 && (tempx>(-1) && tempy>(-1))
                && (tempx< 128 && tempy< 128)){
            pattern2[tempx][tempy] = 0;
        }
        lb = Qt::RightButton;
        bh->setInterpolationActive(true);
        ui->label->setPixmap(QPixmap::fromImage(*qy));
        update();
    }
}

void brushShape::mouseReleaseEvent(QMouseEvent *event){
    bh->setInterpolationActive(false);
    bh->setBrushColor(Qt::black);
}
