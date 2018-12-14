#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setSwapInterval(0);
    QSurfaceFormat::setDefaultFormat(format);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
