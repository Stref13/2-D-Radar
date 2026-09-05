#include "mainwindow.h"

#include <QApplication>
#include "Radar/RadarWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RadarWidget radar;
    radar.resize(600, 600);
    radar.show();
    return a.exec();
}
