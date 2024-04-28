cmake_minimum_required(VERSION 3.16)
project(FortuneAvenue VERSION 2.7.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)

qt_standard_project_setup()

qt_add_executable(FortuneAvenue WIN32 MACOSX_BUNDLE
    autoassignshopmodelsdialog.cpp autoassignshopmodelsdialog.h autoassignshopmodelsdialog.ui
    autopath.cpp autopath.h
    darkdetect.h
    directions.h
    fortune-avenue.qrc
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
    fortune-avenue.qrc
)
target_include_directories(FortuneAvenue PRIVATE
    muparser/include
)

target_compile_definitions(FortuneAvenue PRIVATE
    MUPARSER_STATIC
    VERSION_BUILD=0
    VERSION_MAJOR=2
    VERSION_MINOR=7
    _UNICODE
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

qt_generate_deploy_app_script(
    TARGET FortuneAvenue
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
