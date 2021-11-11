#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSaveFile>
#include <QSet>
#include "autopath.h"
#include "darkdetect.h"
#include "squareitem.h"
#include "util.h"
#include "screenshotdialog.h"

//#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), scene(new FortuneAvenueGraphicsScene(-1600 + 32, -1600 + 32, 3200, 3200, this))
{
    ui->setupUi(this);
    waypointStarts = {ui->waypoint1Start, ui->waypoint2Start, ui->waypoint3Start, ui->waypoint4Start};
    waypointDests = {ui->waypoint1Dests, ui->waypoint2Dests, ui->waypoint3Dests, ui->waypoint4Dests};
    ui->graphicsView->setScene(scene);
    ui->graphicsView->centerOn(32, 32);
    ui->loopingMode->setId(ui->loopingNone, LoopingMode::None);
    ui->loopingMode->setId(ui->loopingVertical, LoopingMode::Vertical);
    ui->loopingMode->setId(ui->loopingVerticalHorizontal, LoopingMode::Both);
    ui->type->addItem("");
    ui->type->addItems(squareTexts());
    ui->shopModel->addItems(shopTexts());
    addShortcutTextToButton(ui->addSquare);
    addShortcutTextToButton(ui->removeSquare);

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSave_As, &QAction::triggered, this, &MainWindow::saveFileAs);

    connect(ui->actionZoom_In, &QAction::triggered, this, [&]() {
        zoomPercent += 10;
        updateZoom();
    });
    connect(ui->actionZoom_Out, &QAction::triggered, this, [&]() {
        if (zoomPercent > 10) {
            zoomPercent -= 10;
        }
        updateZoom();
    });
    connect(ui->actionDraw_Axes, &QAction::triggered, this, [&](bool checked) {
        scene->setAxesVisible(checked);
    });
    connect(ui->actionDrag_to_Pan, &QAction::triggered, this, [&](bool checked) {
        ui->graphicsView->setDragMode(checked ? QGraphicsView::ScrollHandDrag : QGraphicsView::RubberBandDrag);
    });

    connect(ui->actionCalculate_Stock_Prices, &QAction::triggered, this, &MainWindow::calcStockPrices);
    connect(ui->actionVerify_Board, &QAction::triggered, this, &MainWindow::verifyBoard);
    connect(ui->actionAuto_Path, &QAction::triggered, this, &MainWindow::autoPath);
    connect(ui->actionScreenshot, &QAction::triggered, this, &MainWindow::screenshot);

    connect(ui->actionFortune_Avenue_Help, &QAction::triggered, this, [&]() {
        QDesktopServices::openUrl(QUrl("https://github.com/FortuneStreetModding/fortune-avenue-qt/wiki"));
    });

    connect(ui->addSquare, &QPushButton::clicked, this, &MainWindow::addSquare);
    connect(ui->removeSquare, &QPushButton::clicked, this, &MainWindow::removeSquare);
    connect(scene, &QGraphicsScene::changed, this, [&](const QList<QRectF> &) { updateSquareSidebar(); });
    connect(ui->snapToCheck, &QCheckBox::clicked, this, [&](bool) {
        updateSnapSize();
    });
    connect(ui->snapTo, &QLineEdit::textEdited, this, [&](const QString &) {
        updateSnapSize();
    });
    updateSnapSize();
    connect(ui->snapAll, &QPushButton::clicked, this, [&](bool) {
        int oldSnapSize = scene->getSnapSize();
        scene->setSnapSize(calcSnapSizeFromInput());
        auto items = scene->squareItems();
        for (auto item: qAsConst(items)) {
            item->setPos(item->getSnapLocation(item->pos()));
        }
        scene->setSnapSize(oldSnapSize);
    });

    initialFile = exportFile();

    registerSquareSidebarEvents();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateZoom() {
    ui->graphicsView->resetTransform();
    ui->graphicsView->scale(zoomPercent / 100.0, zoomPercent / 100.0);
    ui->statusbar->showMessage(QString("Zoom: %1%").arg(zoomPercent));
}

void MainWindow::newFile() {
    setWindowFilePath("");
    loadFile(BoardFile(true));
}

void MainWindow::openFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open File", QString(), "Fortune Street Boards (*.frb)");
    if (filename.isEmpty()) {
        return;
    }
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        BoardFile boardFile;
        stream >> boardFile;
        if (stream.status() != QDataStream::Status::ReadCorruptData) {
            setWindowFilePath(filename);
            loadFile(boardFile);
        } else {
            goto badFile;
        }
    } else {
        badFile:
        QMessageBox::critical(this, "Error opening file", "An error occurred while trying to open the file");
    }
}

bool MainWindow::saveFile() {
    if (windowFilePath().isEmpty()) {
        return saveFileAs();
    } else {
        QSaveFile saveFile(windowFilePath());
        if (saveFile.open(QIODevice::WriteOnly)) {
            QDataStream stream(&saveFile);
            auto fileContents = exportFile();
            stream << fileContents;
            if (!saveFile.commit()) {
                goto fail;
            }
            initialFile = fileContents;
            return true;
        } else {
            fail:
            QMessageBox::critical(this, "Error saving file", "An error occurred while trying to save the file");
            return false;
        }
    }
}

bool MainWindow::saveFileAs() {
    QString saveFileName = QFileDialog::getSaveFileName(this, "Save File", QString(), "Fortune Street Boards (*.frb)");
    if (saveFileName.isEmpty()) {
        return false;
    }
    QSaveFile saveFile(saveFileName);
    if (saveFile.open(QIODevice::WriteOnly)) {
        QDataStream stream(&saveFile);
        auto fileContents = exportFile();
        stream << fileContents;
        if (!saveFile.commit()) {
            goto fail;
        }
        initialFile = fileContents;
        setWindowFilePath(saveFileName);
        return true;
    } else {
        fail:
        QMessageBox::critical(this, "Error saving file", "An error occurred while trying to save the file");
        return false;
    }
}

void MainWindow::loadFile(const BoardFile &file) {
    ui->subBoardEdit->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionSave_As->setEnabled(true);
    ui->menuTools->setEnabled(true);
    ui->menuView->setEnabled(true);

    ui->initialCash->setText(QString::number(file.boardInfo.initialCash));
    ui->targetAmount->setText(QString::number(file.boardInfo.targetAmount));
    ui->baseSalary->setText(QString::number(file.boardInfo.baseSalary));
    ui->salaryIncrement->setText(QString::number(file.boardInfo.salaryIncrement));
    ui->maxDiceRoll->setText(QString::number(file.boardInfo.maxDiceRoll));
    ui->loopingMode->button(file.boardInfo.galaxyStatus)->setChecked(true);
    ui->fileVersion->setText(QString::number(file.boardInfo.versionFlag));
    ui->autopathRange->setValue(file.boardInfo.autopathRange);
    ui->straightLineTolerance->setValue(file.boardInfo.straightLineTolerance);
    scene->clearSquares();
    for (auto &square: file.boardData.squares) {
        scene->addItem(new SquareItem(square));
    }

    if (file.boardInfo.versionFlag < 2) {
        auto response = QMessageBox::question(
            this,
            "Open File",
            QString("This file is version %1, but the latest version is version 2. Would you like to upgrade the file to the latest version?")
                .arg(file.boardInfo.versionFlag)
        );
        if (response == QMessageBox::Yes) {
            ui->fileVersion->setText(QString::number(2));

            // VERSION 0 -> VERSION >= 1
            if (file.boardInfo.versionFlag < 1) {
                auto items = scene->squareItems();
                for (auto item: qAsConst(items)) {
                    auto touchingSquares = AutoPath::getTouchingSquares(item, items, ui->autopathRange->value(), ui->straightLineTolerance->value());
                    AutoPath::enumerateAutopathingRules(item, touchingSquares);
                }
            }
        }
    }

    initialFile = exportFile();

    QCoreApplication::processEvents();
}

BoardFile MainWindow::exportFile() {
    BoardFile file;
    file.boardInfo.initialCash = ui->initialCash->text().toUShort();
    file.boardInfo.targetAmount = ui->targetAmount->text().toUShort();
    file.boardInfo.baseSalary = ui->baseSalary->text().toUShort();
    file.boardInfo.salaryIncrement = ui->salaryIncrement->text().toUShort();
    file.boardInfo.maxDiceRoll = ui->maxDiceRoll->text().toUShort();
    file.boardInfo.galaxyStatus = (LoopingMode)ui->loopingMode->checkedId();
    file.boardInfo.versionFlag = ui->fileVersion->text().toUInt();

    // VERSION 1
    auto items = scene->squareItems();
    for (auto item: qAsConst(items)) {
        file.boardData.squares.append(item->getData());
    }

    // VERSION 2
    file.boardInfo.autopathRange = ui->autopathRange->value();
    file.boardInfo.straightLineTolerance = ui->straightLineTolerance->value();

    return file;
}

void MainWindow::updateSquareSidebar() {
    auto selectedItems = scene->selectedItems();
    if (selectedItems.size() == 1) {
        ui->waypoints->setEnabled(true);
        SquareItem *item = (SquareItem *)selectedItems[0];
        ui->id->setText(QString::number(item->getData().id));
        ui->type->setCurrentText(squareTypeToText(item->getData().squareType));
        ui->districtDestinationId->setText(QString::number(item->getData().districtDestinationId));
        QString shopTypeStr = shopTypeToText(item->getData().shopModel);
        // register the shop type if it doesn't already exist
        if (shopTypeStr.isEmpty() && item->getData().shopModel != 0) {
            shopTypeStr = registerShopType(item->getData().shopModel);
            ui->shopModel->addItem(shopTypeStr);
        }
        ui->shopModel->setCurrentText(shopTypeStr);
        ui->initialValue->setText(QString::number(item->getData().value));
        ui->initialPrice->setText(QString::number(item->getData().price));
        ui->isLift->setChecked(item->getData().oneWayLift);
        for (int i=0; i<4; ++i) {
            waypointStarts[i]->setText(QString::number(item->getData().waypoints[i].entryId));
            auto children = waypointDests[i]->findChildren<QLineEdit *>();
            for (int j=0; j<3; ++j) {
                children[j]->setText(QString::number(item->getData().waypoints[i].destinations[j]));
            }
        }
        ui->positionX->setText(QString::number(item->getData().positionX));
        ui->positionY->setText(QString::number(item->getData().positionY));
        ui->yield->setText(QString::number(item->getData().getYield()));

        updateDestinationUI();
    } else {
        ui->waypoints->setEnabled(false);
        if(selectedItems.size() > 1) {
            ui->id->setText(QString("%1 selected").arg(selectedItems.size()));
        } else {
            ui->id->setText("");
        }
    }
}

void MainWindow::registerSquareSidebarEvents() {
    connect(ui->type, &QComboBox::textActivated, this, [&](const QString &) {
        updateSquare([&](SquareItem *item) {
            item->getData().squareType = textToSquareType(ui->type->currentText());
        });
    });
    connect(ui->positionX, &QLineEdit::textEdited, this, [&](const QString &) {
        updateSquare([&](SquareItem *item) {
            // temporarily disable snapping for setting position
            int oldSnapSize = scene->getSnapSize();
            scene->setSnapSize(1);
            item->setX(ui->positionX->text().toInt());
            scene->setSnapSize(oldSnapSize);
        });
    });
    connect(ui->positionY, &QLineEdit::textEdited, this, [&](const QString &) {
        updateSquare([&](SquareItem *item) {
            // temporarily disable snapping for setting position
            int oldSnapSize = scene->getSnapSize();
            scene->setSnapSize(1);
            item->setY(ui->positionY->text().toInt());
            scene->setSnapSize(oldSnapSize);
        });
    });
    connect(ui->districtDestinationId, &QLineEdit::textEdited, this, [&](const QString &) {
        updateSquare([&](SquareItem *item) {
            item->getData().districtDestinationId = ui->districtDestinationId->text().toUInt();
        });
    });
    connect(ui->shopModel, &QComboBox::textActivated, this, [&](const QString &) {
        updateSquare([&](SquareItem *item) {
            item->getData().shopModel = textToShopType(ui->shopModel->currentText());
            item->getData().updateValueFromShopModel();
            item->getData().updatePriceFromValue();
            updateSquareSidebar();
        });
    });
    connect(ui->initialValue, &QLineEdit::textEdited, this, [&](const QString &) {
        updateSquare([&](SquareItem *item) {
            item->getData().value = ui->initialValue->text().toUShort();
            item->getData().updatePriceFromValue();
            updateSquareSidebar();
        });
    });
    connect(ui->initialPrice, &QLineEdit::textEdited, this, [&](const QString &) {
        updateSquare([&](SquareItem *item) {
            item->getData().price = ui->initialPrice->text().toUShort();
        });
    });
    connect(ui->isLift, &QCheckBox::clicked, this, [&](bool) {
        updateSquare([&](SquareItem *item) {
            item->getData().oneWayLift = ui->isLift->isChecked();
        });
    });
    for (auto &waypointStart: waypointStarts) {
        connect(waypointStart, &QLineEdit::textEdited, this, [&](const QString &) { updateWaypoints(); });
    }
    for (auto &waypointDest: waypointDests) {
        for (auto &child: waypointDest->findChildren<QLineEdit *>()) {
            connect(child, &QLineEdit::textEdited, this, [&](const QString &) { updateWaypoints(); });
        }
    }

    ui->fromButtons->setId(ui->from_northwest, AutoPath::Northwest);
    ui->fromButtons->setId(ui->from_north, AutoPath::North);
    ui->fromButtons->setId(ui->from_northeast, AutoPath::Northeast);
    ui->fromButtons->setId(ui->from_west, AutoPath::West);
    ui->fromButtons->setId(ui->from_east, AutoPath::East);
    ui->fromButtons->setId(ui->from_southwest, AutoPath::Southwest);
    ui->fromButtons->setId(ui->from_south, AutoPath::South);
    ui->fromButtons->setId(ui->from_southeast, AutoPath::Southeast);

    auto fromButtons = ui->fromButtons->buttons();
    for (auto button: qAsConst(fromButtons)) {
        if (isDarkMode()) {
            auto icon = button->icon();
            icon.addFile(QString(":/buttonicons/dark/arrow_%1.svg")
                         .arg(AutoPath::getDirectionName(
                                  AutoPath::getOppositeDirection((AutoPath::Direction)ui->fromButtons->id(button))
                                  )));
            button->setIcon(icon);
        }
    }

    ui->toButtons->setId(ui->to_northwest, AutoPath::Northwest);
    ui->toButtons->setId(ui->to_north, AutoPath::North);
    ui->toButtons->setId(ui->to_northeast, AutoPath::Northeast);
    ui->toButtons->setId(ui->to_west, AutoPath::West);
    ui->toButtons->setId(ui->to_east, AutoPath::East);
    ui->toButtons->setId(ui->to_southwest, AutoPath::Southwest);
    ui->toButtons->setId(ui->to_south, AutoPath::South);
    ui->toButtons->setId(ui->to_southeast, AutoPath::Southeast);

    connect(ui->fromButtons, &QButtonGroup::idClicked, this, [&]() {
        auto toButtons = ui->toButtons->buttons();
        for (auto button: qAsConst(toButtons)) {
            button->setEnabled(true);
        }
        updateDestinationUI();
    });
    connect(ui->toButtons, &QButtonGroup::idClicked, this, [&](int toDir) {
        auto fromDir = ui->fromButtons->checkedId();
        auto selectedItems = scene->selectedItems();
        if (fromDir >= 0) {
            for(auto selectedItem : qAsConst(selectedItems)) {
                SquareItem *item = (SquareItem *)selectedItem;
                if (ui->toButtons->button(toDir)->isChecked()) {
                    item->getData().validDirections.insert((AutoPath::Direction)fromDir, (AutoPath::Direction)toDir);
                } else {
                    item->getData().validDirections.remove((AutoPath::Direction)fromDir, (AutoPath::Direction)toDir);
                }
            }
        }
    });

    connect(ui->resetPaths, &QPushButton::clicked, this, [&]() {
        auto selectedItems = scene->selectedItems();
            for(auto selectedItem : qAsConst(selectedItems)) {
            SquareItem *item = (SquareItem *)selectedItem;
            for (auto from: AutoPath::DIRECTIONS) {
                for (auto to: AutoPath::DIRECTIONS) {
                    item->getData().validDirections.insert(from, to);
                }
            }
            updateDestinationUI();
        }
    });

    connect(ui->allowAll, &QPushButton::clicked, this, [&]() {
        auto fromDir = ui->fromButtons->checkedId();
        auto selectedItems = scene->selectedItems();
        if (fromDir >= 0) {
            for(auto selectedItem : qAsConst(selectedItems)) {
                SquareItem *item = (SquareItem *)selectedItem;
                for (auto to: AutoPath::DIRECTIONS) {
                    item->getData().validDirections.insert((AutoPath::Direction)fromDir, to);
                }
                updateDestinationUI();
            }
        }
    });

    connect(ui->disallowAll, &QPushButton::clicked, this, [&]() {
        auto fromDir = ui->fromButtons->checkedId();
        auto selectedItems = scene->selectedItems();
        if (fromDir >= 0) {
            for(auto selectedItem : qAsConst(selectedItems)) {
                SquareItem *item = (SquareItem *)selectedItem;
                item->getData().validDirections.remove((AutoPath::Direction)fromDir);
                updateDestinationUI();
            }
        }
    });

    connect(ui->addShopType, &QPushButton::clicked, this, [&]() {
        bool ok;
        int res = QInputDialog::getInt(this, "Enter shop id", "Enter shop id to add to dropdowns", 0, 0, 255, 1, &ok);
        if (ok) {
            ui->shopModel->addItem(registerShopType(res));
        }
    });
}

template<typename Func> void MainWindow::updateSquare(Func func) {
    auto selectedItems = scene->selectedItems();
    for(auto selectedItem : qAsConst(selectedItems)) {
        SquareItem *item = (SquareItem *)selectedItem;
        func(item);
        item->update();
    }
}

void MainWindow::updateWaypoints() {
    auto selectedItems = scene->selectedItems();
    if (selectedItems.size() == 1) {
        SquareItem *item = (SquareItem *)selectedItems[0];
        for (int i=0; i<4; ++i) {
            item->getData().waypoints[i].entryId = waypointStarts[i]->text().toUInt();
            auto children = waypointDests[i]->findChildren<QLineEdit *>();
            for (int j=0; j<3; ++j) {
                item->getData().waypoints[i].destinations[j] = children[j]->text().toUInt();
            }
        }
        item->update();
    }
}

void MainWindow::addSquare() {
    scene->addItem(new SquareItem(SquareData(scene->squareItems().size() /* add next index */)));
}

void MainWindow::removeSquare() {
    auto selectedItems = scene->selectedItems();
    scene->clearSelection();
    for (auto selectedItem: qAsConst(selectedItems)) {
        scene->removeItem(selectedItem);
        delete selectedItem;
    }

    // fix square ids
    QMap<quint8, quint8> oldToNewIDs;
    auto items = scene->squareItems();
    for (int i=0; i<items.size(); ++i) {
        oldToNewIDs[items[i]->getData().id] = i;
        items[i]->getData().id = i;
    }

    // and waypoints
    for (auto item: qAsConst(items)) {
        for (auto &waypoint: item->getData().waypoints) {
            waypoint.entryId = oldToNewIDs.value(waypoint.entryId, 255);
            for (auto &dest: waypoint.destinations) {
                dest = oldToNewIDs.value(dest, 255);
            }
        }
    }

    scene->update();
}

int MainWindow::calcSnapSizeFromInput() {
    int result = ui->snapTo->text().toInt();
    return result > 0 ? result : 1;
}

void MainWindow::updateSnapSize() {
    scene->setSnapSize(ui->snapToCheck->isChecked() ? calcSnapSizeFromInput() : 1);
}

void MainWindow::calcStockPrices() {
    QStringList builder;
    QVector<int> districtCount(12);
    QVector<int> districtSum(12);
    int highestDistrict = -1;
    auto items = scene->squareItems();
    for (auto item: qAsConst(items)) {
        auto &square = item->getData();
        if (square.districtDestinationId >= 12) {
            continue;
        }
        if (square.squareType == VacantPlot) {
            districtSum[square.districtDestinationId] += 200;
        } else if (square.squareType == Property) {
            districtSum[square.districtDestinationId] += square.value;
        } else {
            continue;
        }
        ++districtCount[square.districtDestinationId];
        highestDistrict = qMax(highestDistrict, (int)square.districtDestinationId);
    }
    for (int i=0; i<=highestDistrict; ++i) {
        if (districtCount[i] == 0) {
            continue;
        }
        qint64 result = districtSum[i] / districtCount[i];
        result *= 0xB00;
        result >>= 16;
        builder << QString("District %1: %2g").arg(char('A' + i)).arg(result);
    }
    QMessageBox::information(this, "District Stock Prices", builder.join("\n"));
}

void MainWindow::verifyBoard() {
    QStringList errors, warnings;
    QVector<int> districtCount(12);
    int highestDistrict = -1;

    BoardFile file = exportFile();
    if (file.boardData.squares.size() > 0 && file.boardData.squares[0].squareType != Bank) {
        warnings << "There should be a bank at ID 0.";
    }
    if (file.boardData.squares.size() < 3) {
        errors << "Board must have at least 3 squares.";
    }
    if (file.boardData.squares.size() > 85) {
        errors << "Board must have max 85 squares.";
    }
    if (file.boardInfo.maxDiceRoll < 1 || file.boardInfo.maxDiceRoll > 9) {
        errors << "Max. die roll must be between 1 and 9 inclusive.";
    }
    for (auto &square: file.boardData.squares) {
        if (square.squareType == Property || square.squareType == VacantPlot) {
            if (square.districtDestinationId >= 12) {
                warnings << QString("Square %1 has district value %2. Maximum is 11.").arg(square.id).arg(square.districtDestinationId);
            } else {
                ++districtCount[square.districtDestinationId];
                highestDistrict = qMax(highestDistrict, (int)square.districtDestinationId);
            }
        }
        // ignore waypoints for one-way alleys
        if (square.squareType == OneWayAlleyDoorA
                || square.squareType == OneWayAlleyDoorB
                || square.squareType == OneWayAlleyDoorC
                || square.squareType == OneWayAlleyDoorD
                || square.squareType == OneWayAlleySquare) {
            continue;
        }

        QSet<quint8> destinations;
        for (int i=0; i<4; ++i) {
            if (square.waypoints[i].entryId > file.boardData.squares.size()) {
                if (square.waypoints[i].entryId != 255) {
                    errors << QString("Starting square of Waypoint %1 of Square %2 is Square %3 which does not exist")
                              .arg(i+1).arg(square.id).arg(square.waypoints[i].entryId);
                } else {
                    // Since this waypoint has an entry point of 255,
                    // we ignore it entirely.
                    continue;
                }
            } else {
                auto &otherSquare = file.boardData.squares[square.waypoints[i].entryId];
                if (qAbs(square.positionX - otherSquare.positionX) > 96
                        || qAbs(square.positionY - otherSquare.positionY) > 96) {
                    warnings << QString("Starting square of Waypoint %1 of Square %2 is Square %3 which is too far")
                              .arg(i+1).arg(square.id).arg(square.waypoints[i].entryId);
                }
            }
            for (int j=0; j<3; ++j) {
                auto dest = square.waypoints[i].destinations[j];
                destinations.insert(dest);

                if (dest > file.boardData.squares.size()) {
                    if (dest != 255) {
                        errors << QString("Destination square #%4 of Waypoint %1 of Square %2 is Square %3 which does not exist")
                                  .arg(i+1).arg(square.id).arg(dest).arg(j+1);
                    }
                } else {
                    auto &otherSquare = file.boardData.squares[dest];
                    if (qAbs(square.positionX - otherSquare.positionX) > 96
                            || qAbs(square.positionY - otherSquare.positionY) > 96) {
                        warnings << QString("Destination square #%4 of Waypoint %1 of Square %2 is Square %3 which is too far")
                                  .arg(i+1).arg(square.id).arg(dest).arg(j+1);
                    }
                }
            }
        }

        destinations.remove(255);
        if (destinations.size() > 4) {
            errors << QString("Square %1 has %2 destinations, which is more than the maximum of 4.")
                      .arg(square.id).arg(destinations.size());
        }
    }

    for (int i=0; i<=highestDistrict; ++i) {
        if (districtCount[i] == 0) {
            errors << QString("Did you skip District %1 when assigning districts?").arg(i);
        } else if (districtCount[i] > 6) {
            errors << QString("District %1 has %2 shops which is more than the maximum of 6").arg(i).arg(districtCount[i]);
        }
    }

    if (errors.isEmpty() && warnings.isEmpty()) {
        QMessageBox::information(this, "Verification", "Board passed verification.");
    } else {
        QString numErrorsWarnings = QString("%1 error(s) and %2 warning(s):\n")
                .arg(errors.size()).arg(warnings.size());
        if (!errors.isEmpty()) {
            QMessageBox::critical(this, "Verification", numErrorsWarnings + errors.join("\n"));
        } else {
            QMessageBox::warning(this, "Verification", numErrorsWarnings + warnings.join("\n"));
        }
    }
}

void MainWindow::autoPath() {
    auto items = scene->squareItems();
    for (auto item: qAsConst(items)) {
        QMap<AutoPath::Direction, SquareItem *> touchingSquares = AutoPath::getTouchingSquares(item, items, ui->autopathRange->value(), ui->straightLineTolerance->value());
        AutoPath::pathSquare(item, touchingSquares);
    }
    QMessageBox::information(this, "Auto-pathing", "Auto-pathed entire map");
    updateSquareSidebar();
}

void MainWindow::screenshot() {
    ScreenshotDialog dialog(windowFilePath(), this);
    dialog.exec();
}

void MainWindow::updateDestinationUI() {
    auto selectedItems = scene->selectedItems();
    if (selectedItems.size() >= 1) {
        SquareItem *item = (SquareItem *)selectedItems[0];
        auto allButtons = ui->toButtons->buttons();
        for (auto toButton: qAsConst(allButtons)) {
            toButton->setChecked(false);
        }
        auto allowedDirections = item->getData().validDirections.equal_range((AutoPath::Direction)ui->fromButtons->checkedId());
        for (auto it=allowedDirections.first; it!=allowedDirections.second; ++it) {
            ui->toButtons->button(it.value())->setChecked(true);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // if no unsaved changes just close without fanfare
    if (exportFile() == initialFile) {
        event->accept();
        return;
    }

    auto button = QMessageBox::warning(this, "Unsaved Changes", "You have unsaved changes. Do you want to save them?",
                                        QMessageBox::StandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel));
    //qDebug() << button;
    if (button == QMessageBox::Cancel) {
        event->ignore();
    } else if (button == QMessageBox::Save) {
        if (saveFile()) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}
