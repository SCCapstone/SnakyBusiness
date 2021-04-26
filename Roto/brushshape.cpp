#include "brushshape.h"
#include "ui_brushShape.h"

std::vector<std::vector<unsigned char>> sPattern(((64*2)+1),std::vector<unsigned char>((64*2)+1));

/* This Profiler takes the applications
 * most basic drawing fuctions and using
 * them to allow the user to create their
 * own brush
 */

//Initalizes brush's Shape Profiler
brushShape::brushShape(brushHandler *bah, QWidget *parent)
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
    for (int i = 0; i < ((64*2)+1);i++)
    {
        for (int j = 0; j < ((64*2)+1);j++)
        {
            pattern2[i][j] = 0;

        }
    }
}

/* Spindail change signal call
 * Controls the size of the
 * brush the custom shape
 * will be sent to
 */
void brushShape::on_spinBox_2_valueChanged(int width)
{
    w = width;
    update();
}

void brushShape::paintEvent(QPaintEvent *event){
}

/* Accepting button signal call
 * Changes exterenal brush to
 * selected size then stores
 * custom(drawn) shape in vector
 * to be sent off to brush handler
 */
void brushShape::on_buttonBox_accepted()
{
    bh->setSize(w);
    for(int i = 0; i < (64*8)+1; i++){
        for(int j = 0; j < (64*8)+1; j++){
            if(qy->pixelColor(i,j) == Qt::black){
                int ti = i/4;
                int tj = j/4;
                pattern2[ti][tj] = 1;
            }
        }
    }
    for(int i = 0; i < (w*2)+1; i++){
        for(int j = 0; j < (w*2)+1; j++){
            sPattern[i][j] = pattern2[i][j];
        }
    }
    bh->setShape("Custom",sPattern);
    qy->fill(Qt::white);
    done(1);
}

/* recjeting button signal call
 * closes programe wiht out sending
 * a vector
 */
void brushShape::on_buttonBox_rejected()
{

    qy->fill(Qt::white);
    close();
}

/* Fucntion that returns the size
 * in case extrenal function needs it
 */
int brushShape::getW(){
    return w;
}

/* Mouse movent dection
 * gets point as mouse is moving if
 * the user is drawing and applies white or
 * black square at that point depending on
 * right or left mouse button is being
 * pressed respectively
 */
void brushShape::mouseMoveEvent(QMouseEvent *event){
    if((lb == Qt::LeftButton)&& drawing){
        QPoint lPoint = event->pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qy,temPoint);
        ui->label->setPixmap(QPixmap::fromImage(*qy));
    }
    else if ((lb == Qt::RightButton)&& drawing){
        QPoint lPoint = event->pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qy,temPoint);    
        ui->label->setPixmap(QPixmap::fromImage(*qy));
    }
    update();
}

/* Mouse button press dection
 * gets point at mouse click and applies
 * white or black square at that point depending on
 * right or left mouse button is being pressed respectively
 * sets lets the mouse movement event know that the user is drawing
 */
void brushShape::mousePressEvent(QMouseEvent *event){
    rp -> updateSize(8);
    if(event->button() == Qt::LeftButton){
        bh->setBrushColor(Qt::black);
        QPoint lPoint = event-> pos();
        QPoint temPoint =  QPoint(lPoint.x()-15,lPoint.y()-15);
        bh->applyBrush(qy,temPoint);;
        drawing = true;
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
        lb = Qt::RightButton;
        bh->setInterpolationActive(true);
        ui->label->setPixmap(QPixmap::fromImage(*qy));
        update();
    }
}

/* Mouse button release dection
 * resets brush color to black
 */
void brushShape::mouseReleaseEvent(QMouseEvent *event){
    bh->setInterpolationActive(false);
    bh->setBrushColor(Qt::black);
}
