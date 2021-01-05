#include "mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include "darkdetect.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(":/fonts/Lato-Regular.ttf");
    initDarkThemeSettings();
    MainWindow w;
    w.show();
    return a.exec();
}
