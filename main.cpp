#include "mainwindow.h"
#include "roulette.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    roulette r;
    r.show();
    return a.exec();
}
