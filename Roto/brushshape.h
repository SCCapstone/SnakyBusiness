#ifndef BRUSHSHAPE_H
#define BRUSHSHAPE_H

#include <QMainWindow>
#include <QWidget>
#include <QDialog>
#include <QMouseEvent>
#include <vector>

#include <brush.h>
#include <brushhandler.h>

const int dispScaler = 4;

QT_BEGIN_NAMESPACE
namespace Ui { class brushShape; }
QT_END_NAMESPACE

class brushShape : public QDialog
{
    Q_OBJECT

public:
     brushShape(QWidget *parent = nullptr);
     vector <vector <unsigned char> > getShapeSize(int size);

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
     QImage qi;
     brushHandler bh;
     Qt::MouseButton lb;
     bool drawing;
};

#endif // BRUSHSHAPE_H
