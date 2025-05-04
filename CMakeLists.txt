cmake_minimum_required(VERSION 3.19)

set(PROJECT_VERSION 2.8.3)
project(FortuneAvenue VERSION ${PROJECT_VERSION} LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets Concurrent LinguistTools)
qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} languages/en_US.ts languages/it_IT.ts languages/ja_JP.ts)
qt6_add_lupdate(FortuneAvenue ${QM_FILES})

qt_standard_project_setup()

set(PROJECT_SOURCES
    autoassignshopmodelsdialog.cpp autoassignshopmodelsdialog.h autoassignshopmodelsdialog.ui
    autopath.cpp autopath.h
    darkdetect.h
    directions.h

    fortuneavenuegraphicsscene.cpp fortuneavenuegraphicsscene.h
    fortunestreetdata.cpp fortunestreetdata.h
    hungarian.cpp hungarian.h
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    muparser/src/muParser.cpp
    muparser/src/muParserBase.cpp
    muparser/src/muParserBytecode.cpp
    muparser/src/muParserCallback.cpp
    muparser/src/muParserError.cpp
    muparser/src/muParserTokenReader.cpp
    orderedmap.h
    preferencesdialog.h preferencesdialog.cpp preferencesdialog.ui
    screenshotdialog.cpp screenshotdialog.h screenshotdialog.ui
    squareaddcmd.cpp squareaddcmd.h
    squarechangecmd.cpp squarechangecmd.h
    squareitem.cpp squareitem.h
    squaremovecmd.cpp squaremovecmd.h
    squareremovecmd.cpp squareremovecmd.h
    squareshiftidscommand.cpp squareshiftidscommand.h
    squareswapidscommand.cpp squareswapidscommand.h
    updateboardmetacmd.cpp updateboardmetacmd.h
    usersettings.h usersettings.cpp
    util.h
)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    # MACOSX_BUNDLE_ICON_FILE gets added to Info.plist and
    # contains only the .icns file name, without the path.
    set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)
    # the following two commands, however, tell CMake where to find
    # and install this file.
    set(app_icon_macos ${CMAKE_CURRENT_SOURCE_DIR}/AppIcon.icns)
    set_source_files_properties(${app_icon_macos} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    # this is where the app icon gets attached to the Mac OS bundle...
    qt_add_executable(FortuneAvenue MACOSX_BUNDLE ${app_icon_macos} ${PROJECT_SOURCES} ${QM_FILES})
elseif(WIN32)
    set(app_icon_windows "${CMAKE_CURRENT_SOURCE_DIR}/AppIcon.rc")
    # ...and this is where it is attached to the Windows executable.
    qt_add_executable(FortuneAvenue WIN32 ${app_icon_windows} ${PROJECT_SOURCES} ${QM_FILES})
else()
    qt_add_executable(FortuneAvenue ${PROJECT_SOURCES} ${QM_FILES}
        region.h region.cpp
        fortune-avenue.qrc)
endif()

target_include_directories(FortuneAvenue PRIVATE
    muparser/include
)

target_compile_definitions(FortuneAvenue PRIVATE
    MUPARSER_STATIC
    _UNICODE
    FORTUNE_AVENUE_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(FortuneAvenue PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

install(TARGETS FortuneAvenue
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_target_properties(FortuneAvenue PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.fortunestreetmodding
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
