TARGET="Fortune Avenue"

#Application version
VERSION_MAJOR = 2
VERSION_MINOR = 5
VERSION_BUILD = 0

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

QMAKE_TARGET_BUNDLE_PREFIX = com.fortunestreetmodding

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

INCLUDEPATH += muparser/include

DEFINES += MUPARSER_STATIC
DEFINES += _UNICODE

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    autoassignshopmodelsdialog.cpp \
    autopath.cpp \
    fortuneavenuegraphicsscene.cpp \
    fortunestreetdata.cpp \
    hungarian.cpp \
    main.cpp \
    mainwindow.cpp \
    muparser/src/muParser.cpp \
    muparser/src/muParserBase.cpp \
    muparser/src/muParserBytecode.cpp \
    muparser/src/muParserCallback.cpp \
    muparser/src/muParserError.cpp \
    muparser/src/muParserTokenReader.cpp \
    screenshotdialog.cpp \
    squareaddcmd.cpp \
    squarechangecmd.cpp \
    squareitem.cpp \
    squaremovecmd.cpp \
    squareremovecmd.cpp \
    updateboardmetacmd.cpp

HEADERS += \
    autoassignshopmodelsdialog.h \
    autopath.h \
    darkdetect.h \
    directions.h \
    fortuneavenuegraphicsscene.h \
    fortunestreetdata.h \
    hungarian.h \
    mainwindow.h \
    orderedmap.h \
    screenshotdialog.h \
    squareaddcmd.h \
    squarechangecmd.h \
    squareitem.h \
    squaremovecmd.h \
    squareremovecmd.h \
    updateboardmetacmd.h \
    util.h

FORMS += \
    autoassignshopmodelsdialog.ui \
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

QMAKE_INFO_PLIST = Info.plist

DISTFILES += \
    Info.plist
