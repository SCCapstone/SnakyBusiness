#ifndef PATTERNPROFILER_H
#define PATTERNPROFILER_H

#include <QDialog>
#include <QMouseEvent>
#include <QPainter>
#include <vector>

#include <brushhandler.h>
#include <brush.h>

using std::vector;

const int dispScale = 9;

namespace Ui {
class patternProfiler;
}

class patternProfiler : public QDialog
{
    Q_OBJECT

public:
    patternProfiler(QWidget *parent = nullptr);
    ~patternProfiler();
    vector <vector <unsigned char> > getPattern();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void on_spinBox_2_valueChanged(int width);
    void on_spinBox_valueChanged(int height);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::patternProfiler *ui;
    brushHandler bh;
    bool drawing;
    Qt::MouseButton lb;
    QImage qb;
    vector <vector <unsigned char> > pattern;

};

#endif // PATTERNPROFILER_H
