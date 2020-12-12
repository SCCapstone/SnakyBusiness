#include "resizewindow.h"
#include "ui_resizewindow.h"
#include <QStyleFactory>

resizeWindow::resizeWindow(QWidget *parent, DataIOHandler *ioh)
    : QMainWindow(parent)
    , ui(new Ui::resizeWindow)
{
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::WindowStaysOnTopHint);
    ui->setupUi(this);

    ///* dark mode

    this->setStyle(QStyleFactory::create("Fusion"));

    QPalette newPalette;
    newPalette.setColor(QPalette::Window,          QColor( 37,  37,  37));
    newPalette.setColor(QPalette::WindowText,      QColor(196, 196, 196));
    newPalette.setColor(QPalette::Base,            QColor( 60,  60,  60));
    newPalette.setColor(QPalette::AlternateBase,   QColor( 45,  45,  45));
    newPalette.setColor(QPalette::PlaceholderText, QColor(127, 127, 127));
    newPalette.setColor(QPalette::Text,            QColor(127, 127, 127));
    newPalette.setColor(QPalette::Button,          QColor( 32,  32,  32));
    newPalette.setColor(QPalette::ButtonText,      QColor(212, 212, 212));
    newPalette.setColor(QPalette::BrightText,      QColor(240, 240, 240));
    newPalette.setColor(QPalette::Highlight,       QColor( 38,  79, 120));
    newPalette.setColor(QPalette::HighlightedText, QColor(240, 240, 240));

    newPalette.setColor(QPalette::Light,           QColor( 60,  60,  60));
    newPalette.setColor(QPalette::Midlight,        QColor( 52,  52,  52));
    newPalette.setColor(QPalette::Dark,            QColor( 30,  30,  30) );
    newPalette.setColor(QPalette::Mid,             QColor( 37,  37,  37));
    newPalette.setColor(QPalette::Shadow,          QColor( 0,    0,   0));

    newPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));

    this->setPalette(newPalette);

    //*/

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
    parentIOH->scale(ui->comboBox->currentIndex(), ui->comboBox_2->currentIndex());
    QMainWindow *qmw = dynamic_cast<QMainWindow*>(this->parent());
    QSize qs = parentIOH->fullBounds();
    int X = qmw->x(), Y = qmw->y();
    qmw->setGeometry(X, Y, qs.width(), qs.height());
    qmw->move(X, Y);
    qmw->setEnabled(true);
    qmw->repaint();
}

