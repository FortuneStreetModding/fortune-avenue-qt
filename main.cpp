#include "mainwindow.h"

#include <QApplication>
#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QWindow>
#include "darkdetect.h"
#include "fortuneavenueapp.h"

int main(int argc, char *argv[]) {
    FortuneAvenueApp a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    QFontDatabase::addApplicationFont(":/fonts/Lato-Regular.ttf");
    initDarkThemeSettings();
    MainWindow w(a);
    w.setWindowTitle(QString("Fortune Avenue %1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));
    w.show();
    return a.exec();
}
