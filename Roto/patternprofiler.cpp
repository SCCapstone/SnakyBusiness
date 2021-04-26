#include "patternprofiler.h"
#include "ui_patternprofiler.h"

//chrashes/doesn't function if put in .h
vector<vector<unsigned char>>Spattern (64,vector<unsigned char>(64));
int wi,hi;

patternProfiler::patternProfiler(brushHandler *bah, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::patternProfiler)

{
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    ui->setupUi(this);
    setWindowTitle("Pattern Profiler");
    wi = 2;
    hi = 2;
    bh = bah;
    bh ->setShape("Square");
    bh -> setPatternInUse(0);
    rd = new RadialProfiler(bh, this);
    qb = new QImage((64*8)+1, (64*8)+1, QImage::Format_ARGB32_Premultiplied);
    qb->fill(Qt::white);
    ui->label->setPixmap(QPixmap::fromImage(*qb));
    for(int i = 0; i < maxRadius; i++){
        for(int j = 0; j < maxRadius; j++){
            pattern[i][j] = 0;
        }
    }


}

patternProfiler::~patternProfiler()
{
    delete qb;
    delete ui;

}

void patternProfiler::on_spinBox_2_valueChanged(int width)
{
    wi = width;
    repaint();
}

void patternProfiler::on_spinBox_valueChanged(int hight)
{
    hi = hight;
    repaint();
}

void patternProfiler::on_buttonBox_accepted()
{
    for(int i = 0; i < (64*8)+1; i++){
        for(int j = 0; j < (64*8)+1; j++){
            if(qb->pixelColor(i,j) == Qt::black){
                int ti = i/8;
                int tj = j/8;
                pattern[ti][tj] = 1;
            }
        }
    }
    for(int i = 0; i < 64; i++){
        for(int j  = 0; j < 64; j++){
            Spattern[i][j] = pattern[i][j];
        }
    }

    bh->setPattern(wi,hi,Spattern);
    bh->setPatternInUse(1);
    rd -> updateSize(30);
    done(1);
}

void patternProfiler::on_buttonBox_rejected()
{
    rd -> updateSize(30);
    done(0);
}

int getWidth(){
    int w = wi;
    return w;
}

int getHighth(){
    int h = hi;
    return h;
}

vector<vector<unsigned char>> getPattern(){
    return Spattern;
}

void patternProfiler::mouseMoveEvent(QMouseEvent *event){
    if((lb == Qt::LeftButton)&& drawing){
        QPoint lPoint = event->pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qb,temPoint);
        ui->label->setPixmap(QPixmap::fromImage(*qb));
    }
    else if ((lb == Qt::RightButton)&& drawing){
        QPoint lPoint = event->pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qb,temPoint);
        ui->label->setPixmap(QPixmap::fromImage(*qb));
    }
    update();
}

void patternProfiler::mousePressEvent(QMouseEvent *event){
    rd -> updateSize(8);
    if(event->button() == Qt::LeftButton){
        bh->setBrushColor(Qt::black);
        QPoint lPoint = event-> pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qb,temPoint);;
        drawing = true;
        lb = Qt::LeftButton;
        bh->setInterpolationActive(true);
        ui->label->setPixmap(QPixmap::fromImage(*qb));
        update();
    }
    else if(event ->button() == Qt::RightButton){
        bh->setBrushColor(Qt::white);
        QPoint lPoint = event-> pos();
        QPoint temPoint =  QPoint(lPoint.x()-16,lPoint.y()-16);
        bh->applyBrush(qb,temPoint);
        drawing = true;
        lb = Qt::RightButton;
        bh->setInterpolationActive(true);
        ui->label->setPixmap(QPixmap::fromImage(*qb));
        update();
    }
}

void patternProfiler::mouseReleaseEvent(QMouseEvent *event){
    bh->setInterpolationActive(false);
    bh->setBrushColor(Qt::black);
}
