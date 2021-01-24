TARGET="Fortune Avenue"

# update this for release
VERSION = 2.0.0.0

QMAKE_TARGET_BUNDLE_PREFIX = com.fortunestreetmodding

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fortuneavenuegraphicsscene.cpp \
    fortunestreetdata.cpp \
    main.cpp \
    mainwindow.cpp \
    squareitem.cpp

HEADERS += \
    darkdetect.h \
    fortuneavenuegraphicsscene.h \
    fortunestreetdata.h \
    mainwindow.h \
    orderedmap.h \
    squareitem.h \
    static_block.hpp \
    util.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    fortune-avenue-port.qrc

win32:RC_ICONS = AppIcon.ico
macos:ICON=AppIcon.icns
