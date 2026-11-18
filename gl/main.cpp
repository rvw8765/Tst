#include "mainwindow.h"
#include <QSurfaceFormat>
#include <QApplication>

int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    //QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
