TARGET="Fortune Avenue"

#Application version
VERSION_MAJOR = 2
VERSION_MINOR = 0
VERSION_BUILD = 4

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

QMAKE_TARGET_BUNDLE_PREFIX = com.fortunestreetmodding

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    autopath.cpp \
    fortuneavenuegraphicsscene.cpp \
    fortunestreetdata.cpp \
    main.cpp \
    mainwindow.cpp \
    screenshotdialog.cpp \
    squareitem.cpp

HEADERS += \
    autopath.h \
    darkdetect.h \
    directions.h \
    fortuneavenuegraphicsscene.h \
    fortunestreetdata.h \
    mainwindow.h \
    orderedmap.h \
    screenshotdialog.h \
    squareitem.h \
    static_block.hpp \
    util.h

FORMS += \
    mainwindow.ui \
    screenshotdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    fortune-avenue-port.qrc

win32:RC_ICONS = AppIcon.ico
macos:ICON=AppIcon.icns

DISTFILES += \
    .travis.yml
