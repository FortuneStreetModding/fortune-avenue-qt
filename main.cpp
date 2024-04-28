#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFileOpenEvent>
#include <QFontDatabase>
#include <QWindow>
#include "darkdetect.h"
#include <QStyleHints>
#ifdef Q_OS_WIN
#include "Shlobj.h"
#endif

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("FortuneAvenue");
    QFontDatabase::addApplicationFont(":/fonts/Lato-Regular.ttf");
    initWindowPaletteSettings(a.styleHints()->colorScheme()); // pass through the OS light/dark theme setting

#ifdef Q_OS_WIN
    QSettings s("HKEY_CURRENT_USER\\SOFTWARE\\CLASSES", QSettings::NativeFormat);
    QString path = QDir::toNativeSeparators(qApp->applicationFilePath());
    //qDebug() << path;
    s.setValue("customstreet.fortuneavenue/DefaultIcon/.", path);
    s.setValue(".frb/.","customstreet.fortuneavenue");
    s.setValue("customstreet.fortuneavenue/shell/open/command/.", QStringLiteral("\"%1\"").arg(path) + " \"%1\"");
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
#endif

    MainWindow w(a);
#ifdef Q_OS_LINUX
    auto iconPath = QDir(QApplication::applicationDirPath()).filePath("../../AppIcon.png");
    if (QFile::exists(iconPath)) {
        w.setWindowIcon(QIcon(iconPath));
    }
#endif
    w.setWindowTitle(QString("Fortune Avenue %1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));
    w.show();
    return a.exec();
}
