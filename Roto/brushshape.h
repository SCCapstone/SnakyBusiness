#ifndef BRUSHSHAPE_H
#define BRUSHSHAPE_H

#include <QMainWindow>
#include <QWidget>
#include <QDialog>
#include<QMouseEvent>
#include<vector>

#include <brush.h>
#include <brushhandler.h>
#include <radialprofiler.h>
#include <dataIOHandler.h>


QT_BEGIN_NAMESPACE
namespace Ui { class brushShape; }
QT_END_NAMESPACE

class brushShape : public QDialog
{
    Q_OBJECT
public:
     brushShape(brushHandler *bah,QWidget *parent = nullptr);
     void paintEvent(QPaintEvent *event);
     int getW();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:

     void on_spinBox_2_valueChanged(int arg1);

     void on_buttonBox_rejected();

     void on_buttonBox_accepted();


private:
     Ui::brushShape *ui;
     QImage *qi;
     int radius;
     brushHandler *bh;
     Brush *brush;
     RadialProfiler *rp,*rg;
     Qt::MouseButton lb;
     bool drawing;
     unsigned char pattern2[(maxRadius*2)+1][(maxRadius*2)+1];
     QImage *qy;
     int w = 2, h = 2;
};

#endif // BRUSHSHAPE_H
