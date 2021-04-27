#include "mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include "darkdetect.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    QFontDatabase::addApplicationFont(":/fonts/Lato-Regular.ttf");
    initDarkThemeSettings();
    MainWindow w;
    w.setWindowTitle(QString("CSMM %1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));
    w.show();
    return a.exec();
}
