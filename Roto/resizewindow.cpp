#include "resizewindow.h"
#include "ui_resizewindow.h"


resizeWindow::resizeWindow(QWidget *parent, DataIOHandler *ioh)
    : QMainWindow(parent)
    , ui(new Ui::resizeWindow)
{
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(finish()));
    setFixedSize(size());
    setWindowTitle("Resize Layers");
    ignoreClose = true;
    parentIOH = ioh;
}

resizeWindow::~resizeWindow()
{
    delete ui;
}

void resizeWindow::showRelative() {
    QMainWindow *qmw = dynamic_cast<QMainWindow*>(this->parent());
    qmw->setEnabled(false);
    this->setEnabled(true);
    int x = qmw->x() + qmw->width() / 2 - this->width() / 2;
    int y = qmw->y() + qmw->height() / 2 - this->height() / 2;
    move(x, y);
    show();
}

void resizeWindow::doClose() {
    ignoreClose = false;
    close();
}

void resizeWindow::closeEvent(QCloseEvent *event) {
    if (ignoreClose)
        finish();
    else
        event->ignore();
}

void resizeWindow::finish() {
    hide();
    parentIOH->scale(scaleType(ui->comboBox_2->currentIndex()));
    QMainWindow *qmw = dynamic_cast<QMainWindow *>(this->parent());
    QSize qs = parentIOH->getWorkingLayer()->getCanvas()->size();
    int X = qmw->x(), Y = qmw->y();
    qmw->setGeometry(X, Y, qs.width(), qs.height());
    qmw->move(X, Y);
    qmw->setEnabled(true);
}
