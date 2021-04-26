#include "patternprofiler.h"
#include "ui_patternprofiler.h"

//chrashes/doesn't function if put in .h
vector<vector<unsigned char>>Spattern (64,vector<unsigned char>(64));
int wi,hi;

/* This Profiler takes the applications
 * most basic drawing fuctions and using
 * them to allow the user to create their
 * patterns
 */

//Initalizes brush's Pattern Profiler
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
// deconstructor of pattern profiler
patternProfiler::~patternProfiler()
{
    delete qb;
    delete ui;

}

/* Width spindail change signal call
 * controls the size of the
 * width of the pattern that
 * will be made
 */
void patternProfiler::on_spinBox_2_valueChanged(int width)
{
    wi = width;
    repaint();
}

/* hight spindail change signal call
 * controls the size of the
 * hight of the pattern that
 * will be made
 */
void patternProfiler::on_spinBox_valueChanged(int hight)
{
    hi = hight;
    repaint();
}

/* Accepting button signal call
 * Changes exterenal brush to
 * selected size then stores
 * custom(drawn) shape in vector
 * to be sent off to brush handler
 */
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
/* recjeting button signal call
 * closes programe wiht out sending
 * a vector and resets size
 */
void patternProfiler::on_buttonBox_rejected()
{
    rd -> updateSize(30);
    done(0);
}
/* Fucntion that returns the width
 * in case extrenal function needs it
 */
int getWidth(){
    int w = wi;
    return w;
}
/* Fucntion that returns the hight
 * in case extrenal function needs it
 */
int getHighth(){
    int h = hi;
    return h;
}
/* Fucntion that returns the pattern
 * in case extrenal function needs it
 */
vector<vector<unsigned char>> getPattern(){
    return Spattern;
}

/* Mouse movent dection
 * gets point as mouse is moving if
 * the user is drawing and applies white or
 * black square at that point depending on
 * right or left mouse button is being
 * pressed respectively
 */
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

/* Mouse button press dection
 * gets point at mouse click and applies
 * white or black square at that point depending on
 * right or left mouse button is being pressed respectively
 * sets lets the mouse movement event know that the user is drawing
 */
void patternProfiler::mousePressEvent(QMouseEvent *event){
    rd -> updateSize(8);
    if(event->button() == Qt::LeftButton){
        bh->setBrushColor(Qt::black);
        QPoint lPoint = event-> pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qb,temPoint);
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

/* Mouse button release dection
 * resets brush color to black
 */
void patternProfiler::mouseReleaseEvent(QMouseEvent *event){
    bh->setInterpolationActive(false);
    bh->setBrushColor(Qt::black);
}
