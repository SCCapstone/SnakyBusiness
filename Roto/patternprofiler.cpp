#include "patternprofiler.h"
#include "ui_patternprofiler.h"

//chrashes/doesn't function if put in .h
vector<vector<unsigned char>>Spattern (64,vector<unsigned char>(64));
unsigned char pattern[maxRadius][maxRadius];
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
    bh->setPatternInUse(0);
    rd = new RadialProfiler(bh, this);
    qb = new QImage((64*8)+1, (64*8)+1, QImage::Format_ARGB32_Premultiplied);
    qb->fill(Qt::white);
    ui->label->setPixmap(QPixmap::fromImage(*qb));


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

void patternProfiler::paintEvent(QPaintEvent *event){

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
        QPoint temPoint =  QPoint(lPoint.x()-16,lPoint.y()-16);
        bh->applyBrush(qb,temPoint);
        int tempx, tempy;
        tempx = temPoint.x()/8;
        tempy = temPoint.y()/8;
        if( pattern[tempx][tempy] != '1'&& (tempx>(-1) && tempy>(-1))
                && (tempx< 64 && tempy< 64)){
            pattern[tempx][tempy] = '1';
        }
        ui->label->setPixmap(QPixmap::fromImage(*qb));
    }
    else if ((lb == Qt::RightButton)&& drawing){
        QPoint lPoint = event->pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qb,temPoint);
        int tempx, tempy;
        tempx = temPoint.x()/8;
        tempy = temPoint.y()/8;
        if( pattern[tempx][tempy] != 0 && (tempx>(-1) && tempy>(-1))
                && (tempx< 65 && tempy< 65)){
            pattern[tempx][tempy] = 0;

        }
        ui->label->setPixmap(QPixmap::fromImage(*qb));
    }
    update();
}

void patternProfiler::mousePressEvent(QMouseEvent *event){
    rd -> updateSize(8);
    if(event->button() == Qt::LeftButton){
        bh->setBrushColor(Qt::black);
        QPoint lPoint = event-> pos();
        QPoint temPoint =  QPoint(lPoint.x()-16,lPoint.y()-16);
        bh->applyBrush(qb,temPoint);
        drawing = true;
        int tempx, tempy;
        tempx = temPoint.x()/8;
        tempy = temPoint.y()/8;
        if(pattern[tempx][tempy] != '1' && (tempx>(-1) && tempy>(-1))
                && (tempx< 64 && tempy< 64)){
        pattern[tempx][tempy] = '1';}
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
    int tempx, tempy;
    tempx = temPoint.x()/8;
    tempy = temPoint.y()/8;
    if(pattern[tempx][tempy] != 0 && (tempx>(-1) && tempy>(-1))
            && (tempx< 65 && tempy< 65)){
    pattern[tempx][tempy] = 0;}
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
