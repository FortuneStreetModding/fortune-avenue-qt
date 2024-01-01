cmake_minimum_required(VERSION 3.16)
project(FortuneAvenue VERSION 2.7.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Widgets)

qt_standard_project_setup()

qt_add_executable(FortuneAvenue WIN32 MACOSX_BUNDLE
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
    screenshotdialog.cpp screenshotdialog.h screenshotdialog.ui
    squareaddcmd.cpp squareaddcmd.h
    squarechangecmd.cpp squarechangecmd.h
    squareitem.cpp squareitem.h
    squaremovecmd.cpp squaremovecmd.h
    squareremovecmd.cpp squareremovecmd.h
    squareshiftidscommand.cpp squareshiftidscommand.h
    squareswapidscommand.cpp squareswapidscommand.h
    updateboardmetacmd.cpp updateboardmetacmd.h
    util.h
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
    Qt::Core
    Qt::Gui
    Qt::Widgets
)


# Resources:
set(fortune-avenue-port_resource_files
    "buttonicons/arrow_east.svg"
    "buttonicons/arrow_east_green.svg"
    "buttonicons/arrow_east_red.svg"
    "buttonicons/arrow_north.svg"
    "buttonicons/arrow_north_green.svg"
    "buttonicons/arrow_north_red.svg"
    "buttonicons/arrow_northeast.svg"
    "buttonicons/arrow_northeast_green.svg"
    "buttonicons/arrow_northeast_red.svg"
    "buttonicons/arrow_northwest.svg"
    "buttonicons/arrow_northwest_green.svg"
    "buttonicons/arrow_northwest_red.svg"
    "buttonicons/arrow_south.svg"
    "buttonicons/arrow_south_green.svg"
    "buttonicons/arrow_south_red.svg"
    "buttonicons/arrow_southeast.svg"
    "buttonicons/arrow_southeast_green.svg"
    "buttonicons/arrow_southeast_red.svg"
    "buttonicons/arrow_southwest.svg"
    "buttonicons/arrow_southwest_green.svg"
    "buttonicons/arrow_southwest_red.svg"
    "buttonicons/arrow_west.svg"
    "buttonicons/arrow_west_green.svg"
    "buttonicons/arrow_west_red.svg"
    "buttonicons/dark/arrow_east.svg"
    "buttonicons/dark/arrow_east_green.svg"
    "buttonicons/dark/arrow_east_red.svg"
    "buttonicons/dark/arrow_north.svg"
    "buttonicons/dark/arrow_north_green.svg"
    "buttonicons/dark/arrow_north_red.svg"
    "buttonicons/dark/arrow_northeast.svg"
    "buttonicons/dark/arrow_northeast_green.svg"
    "buttonicons/dark/arrow_northeast_red.svg"
    "buttonicons/dark/arrow_northwest.svg"
    "buttonicons/dark/arrow_northwest_green.svg"
    "buttonicons/dark/arrow_northwest_red.svg"
    "buttonicons/dark/arrow_south.svg"
    "buttonicons/dark/arrow_south_green.svg"
    "buttonicons/dark/arrow_south_red.svg"
    "buttonicons/dark/arrow_southeast.svg"
    "buttonicons/dark/arrow_southeast_green.svg"
    "buttonicons/dark/arrow_southeast_red.svg"
    "buttonicons/dark/arrow_southwest.svg"
    "buttonicons/dark/arrow_southwest_green.svg"
    "buttonicons/dark/arrow_southwest_red.svg"
    "buttonicons/dark/arrow_west.svg"
    "buttonicons/dark/arrow_west_green.svg"
    "buttonicons/dark/arrow_west_red.svg"
    "squares/EventSquare.png"
    "squares/EventSquare/eventsquare_000.png"
    "squares/EventSquare/eventsquare_001.png"
    "squares/EventSquare/eventsquare_002.png"
    "squares/EventSquare/eventsquare_003.png"
    "squares/EventSquare/eventsquare_004.png"
    "squares/EventSquare/eventsquare_005.png"
    "squares/EventSquare/eventsquare_006.png"
    "squares/EventSquare/eventsquare_007.png"
    "squares/EventSquare/eventsquare_008.png"
    "squares/EventSquare/eventsquare_009.png"
    "squares/EventSquare/eventsquare_010.png"
    "squares/EventSquare/eventsquare_011.png"
    "squares/EventSquare/eventsquare_012.png"
    "squares/EventSquare/eventsquare_013.png"
    "squares/EventSquare/eventsquare_014.png"
    "squares/EventSquare/eventsquare_015.png"
    "squares/EventSquare/eventsquare_016.png"
    "squares/EventSquare/eventsquare_017.png"
    "squares/EventSquare/eventsquare_018.png"
    "squares/EventSquare/eventsquare_019.png"
    "squares/EventSquare/eventsquare_020.png"
    "squares/EventSquare/eventsquare_021.png"
    "squares/EventSquare/eventsquare_022.png"
    "squares/EventSquare/eventsquare_023.png"
    "squares/EventSquare/eventsquare_024.png"
    "squares/EventSquare/eventsquare_025.png"
    "squares/EventSquare/eventsquare_026.png"
    "squares/EventSquare/eventsquare_027.png"
    "squares/EventSquare/eventsquare_028.png"
    "squares/EventSquare/eventsquare_029.png"
    "squares/EventSquare/eventsquare_030.png"
    "squares/EventSquare/eventsquare_031.png"
    "squares/EventSquare/eventsquare_032.png"
    "squares/EventSquare/eventsquare_033.png"
    "squares/EventSquare/eventsquare_034.png"
    "squares/EventSquare/eventsquare_035.png"
    "squares/EventSquare/eventsquare_036.png"
    "squares/EventSquare/eventsquare_037.png"
    "squares/EventSquare/eventsquare_038.png"
    "squares/EventSquare/eventsquare_039.png"
    "squares/EventSquare/eventsquare_040.png"
    "squares/EventSquare/eventsquare_041.png"
    "squares/EventSquare/eventsquare_042.png"
    "squares/EventSquare/eventsquare_043.png"
    "squares/EventSquare/eventsquare_044.png"
    "squares/EventSquare/eventsquare_045.png"
    "squares/EventSquare/eventsquare_046.png"
    "squares/EventSquare/eventsquare_047.png"
    "squares/EventSquare/eventsquare_048.png"
    "squares/EventSquare/eventsquare_049.png"
    "squares/EventSquare/eventsquare_050.png"
    "squares/EventSquare/eventsquare_051.png"
    "squares/EventSquare/eventsquare_052.png"
    "squares/EventSquare/eventsquare_053.png"
    "squares/EventSquare/eventsquare_054.png"
    "squares/EventSquare/eventsquare_055.png"
    "squares/EventSquare/eventsquare_056.png"
    "squares/EventSquare/eventsquare_057.png"
    "squares/EventSquare/eventsquare_058.png"
    "squares/EventSquare/eventsquare_059.png"
    "squares/EventSquare/eventsquare_060.png"
    "squares/EventSquare/eventsquare_061.png"
    "squares/EventSquare/eventsquare_062.png"
    "squares/EventSquare/eventsquare_063.png"
    "squares/EventSquare/eventsquare_064.png"
    "squares/EventSquare/eventsquare_065.png"
    "squares/EventSquare/eventsquare_066.png"
    "squares/EventSquare/eventsquare_067.png"
    "squares/EventSquare/eventsquare_068.png"
    "squares/EventSquare/eventsquare_069.png"
    "squares/EventSquare/eventsquare_070.png"
    "squares/EventSquare/eventsquare_071.png"
    "squares/EventSquare/eventsquare_072.png"
    "squares/EventSquare/eventsquare_073.png"
    "squares/EventSquare/eventsquare_074.png"
    "squares/EventSquare/eventsquare_075.png"
    "squares/EventSquare/eventsquare_076.png"
    "squares/EventSquare/eventsquare_077.png"
    "squares/EventSquare/eventsquare_078.png"
    "squares/EventSquare/eventsquare_079.png"
    "squares/EventSquare/eventsquare_080.png"
    "squares/EventSquare/eventsquare_081.png"
    "squares/EventSquare/eventsquare_082.png"
    "squares/EventSquare/eventsquare_083.png"
    "squares/EventSquare/eventsquare_084.png"
    "squares/EventSquare/eventsquare_085.png"
    "squares/EventSquare/eventsquare_086.png"
    "squares/EventSquare/eventsquare_087.png"
    "squares/EventSquare/eventsquare_088.png"
    "squares/EventSquare/eventsquare_089.png"
    "squares/EventSquare/eventsquare_090.png"
    "squares/EventSquare/eventsquare_091.png"
    "squares/EventSquare/eventsquare_092.png"
    "squares/EventSquare/eventsquare_093.png"
    "squares/EventSquare/eventsquare_094.png"
    "squares/EventSquare/eventsquare_095.png"
    "squares/EventSquare/eventsquare_096.png"
    "squares/EventSquare/eventsquare_097.png"
    "squares/EventSquare/eventsquare_098.png"
    "squares/EventSquare/eventsquare_099.png"
    "squares/EventSquare/eventsquare_100.png"
    "squares/EventSquare/eventsquare_101.png"
    "squares/EventSquare/eventsquare_102.png"
    "squares/EventSquare/eventsquare_103.png"
    "squares/EventSquare/eventsquare_104.png"
    "squares/EventSquare/eventsquare_105.png"
    "squares/EventSquare/eventsquare_106.png"
    "squares/EventSquare/eventsquare_107.png"
    "squares/EventSquare/eventsquare_108.png"
    "squares/EventSquare/eventsquare_109.png"
    "squares/EventSquare/eventsquare_110.png"
    "squares/EventSquare/eventsquare_111.png"
    "squares/EventSquare/eventsquare_112.png"
    "squares/EventSquare/eventsquare_113.png"
    "squares/EventSquare/eventsquare_114.png"
    "squares/EventSquare/eventsquare_115.png"
    "squares/EventSquare/eventsquare_116.png"
    "squares/EventSquare/eventsquare_117.png"
    "squares/EventSquare/eventsquare_118.png"
    "squares/EventSquare/eventsquare_119.png"
    "squares/EventSquare/eventsquare_120.png"
    "squares/EventSquare/eventsquare_121.png"
    "squares/EventSquare/eventsquare_122.png"
    "squares/EventSquare/eventsquare_123.png"
    "squares/EventSquare/eventsquare_124.png"
    "squares/EventSquare/eventsquare_125.png"
    "squares/EventSquare/eventsquare_126.png"
    "squares/EventSquare/eventsquare_127.png"
    "squares/EventSquare/eventsquare_128.png"
    "squares/GroundArcade.png"
    "squares/GroundBank.png"
    "squares/GroundBoom.png"
    "squares/GroundBoon.png"
    "squares/GroundCSuit01.png"
    "squares/GroundCSuit02.png"
    "squares/GroundCSuit03.png"
    "squares/GroundCSuit04.png"
    "squares/GroundCannon.png"
    "squares/GroundCasino.png"
    "squares/GroundDefault.png"
    "squares/GroundDoorBlue.png"
    "squares/GroundDoorDQ.png"
    "squares/GroundDoorGreen.png"
    "squares/GroundDoorMario.png"
    "squares/GroundDoorRed.png"
    "squares/GroundDoorYellow.png"
    "squares/GroundLift.png"
    "squares/GroundLiftMagmaliceStart.png"
    "squares/GroundMagmalice.png"
    "squares/GroundPipe.png"
    "squares/GroundProperty.png"
    "squares/GroundRollOn.png"
    "squares/GroundStockBroker.png"
    "squares/GroundStone.png"
    "squares/GroundSuit01.png"
    "squares/GroundSuit02.png"
    "squares/GroundSuit03.png"
    "squares/GroundSuit04.png"
    "squares/GroundSwitch.png"
    "squares/GroundTakeABreak.png"
    "squares/GroundVacant.png"
    "squares/GroundVenture.png"
    "squares/GroundWarp.png"
    "squares/GroundWarpA.png"
    "squares/GroundWarpB.png"
    "squares/GroundWarpC.png"
    "squares/GroundWarpD.png"
    "squares/GroundWarpE.png"
    "squares/Path.png"
)

qt_add_resources(FortuneAvenue "fortune-avenue-port"
    PREFIX
        "/"
    FILES
        ${fortune-avenue-port_resource_files}
)
set(fortune-avenue-port1_resource_files
    "Lato-Regular.ttf"
)

qt_add_resources(FortuneAvenue "fortune-avenue-port1"
    PREFIX
        "/fonts"
    FILES
        ${fortune-avenue-port1_resource_files}
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
