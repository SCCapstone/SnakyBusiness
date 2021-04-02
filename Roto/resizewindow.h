#ifndef RESIZEWINDOW_H
#define RESIZEWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <dataIOHandler.h>

using std::pair;

QT_BEGIN_NAMESPACE
namespace Ui { class resizeWindow; }
QT_END_NAMESPACE

class resizeWindow : public QMainWindow
{
    Q_OBJECT

public:

    resizeWindow(QWidget *parent, DataIOHandler *ioh);
    ~resizeWindow();
    void showRelative();
    void doClose();

private:

    Ui::resizeWindow *ui;
    DataIOHandler *parentIOH;

    bool ignoreClose;

public slots:

    void finish();

protected:

     void closeEvent(QCloseEvent *event);

};

#endif // RESIZEWINDOW_H
