#ifndef PATTERNPROFILER_H
#define PATTERNPROFILER_H

#include <QDialog>
#include<QMouseEvent>
#include <QPainter>
#include <vector>

#include "brushhandler.h"
#include "brush.h"
#include "radialprofiler.h"

using namespace std;

namespace Ui {
class patternProfiler;
}

class patternProfiler : public QDialog
{
    Q_OBJECT

public:
    patternProfiler(brushHandler *bah, QWidget *parent = nullptr);
    ~patternProfiler();
    vector<vector<unsigned char>> getPattern();
    int getWidth();
    int getHight();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void on_spinBox_2_valueChanged(int width);
    void on_spinBox_valueChanged(int hight);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::patternProfiler *ui;
    brushHandler *bh;
    bool drawing;
    RadialProfiler *rd;
    Qt::MouseButton lb;
    QImage *qb;
    unsigned char pattern[maxRadius][maxRadius];

};

#endif // PATTERNPROFILER_H
