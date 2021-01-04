#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QSet>
#include "squareitem.h"
#include "util.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), scene(new FortuneAvenueGraphicsScene(-1600, -1600, 3200, 3200, this))
{
    ui->setupUi(this);
    waypointStarts = {ui->waypoint1Start, ui->waypoint2Start, ui->waypoint3Start, ui->waypoint4Start};
    waypointDests = {ui->waypoint1Dests, ui->waypoint2Dests, ui->waypoint3Dests, ui->waypoint4Dests};
    ui->graphicsView->setScene(scene);
    ui->graphicsView->centerOn(0, 0);
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

    connect(ui->actionCalculate_Stock_Prices, &QAction::triggered, this, &MainWindow::calcStockPrices);
    connect(ui->actionVerify_Board, &QAction::triggered, this, &MainWindow::verifyBoard);
    connect(ui->actionAuto_Path, &QAction::triggered, this, &MainWindow::autoPath);

    connect(ui->actionFortune_Avenue_Help, &QAction::triggered, this, [&]() {
        QDesktopServices::openUrl(QUrl("https://github.com/FortuneStreetModding/fortune-avenue-qt/wiki"));
    });

    connect(ui->addSquare, &QPushButton::clicked, this, &MainWindow::addSquare);
    connect(ui->removeSquare, &QPushButton::clicked, this, &MainWindow::removeSquare);
    connect(scene, &QGraphicsScene::selectionChanged, this, &MainWindow::updateSquareSidebar);
    connect(ui->snapToCheck, &QCheckBox::clicked, this, [&](bool) {
        scene->setSnapSize(ui->snapToCheck->isChecked() ? calcSnapSizeFromInput() : 1);
    });
    connect(ui->snapTo, &QLineEdit::textEdited, this, [&](const QString &) {
        scene->setSnapSize(ui->snapToCheck->isChecked() ? calcSnapSizeFromInput() : 1);
    });
    connect(ui->snapAll, &QPushButton::clicked, this, [&](bool) {
        int oldSnapSize = scene->getSnapSize();
        scene->setSnapSize(calcSnapSizeFromInput());
        auto items = scene->items();
        for (auto item: items) {
            item->setPos(((SquareItem *)item)->getSnapLocation(item->pos()));
        }
        scene->setSnapSize(oldSnapSize);
    });

    registerSquareSidebarEvents();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFile() {
    loadFile(BoardFile(true));
}

void MainWindow::openFile() {
    QString filename = QFileDialog::getOpenFileName(this, "Open File", "", "Fortune Street Boards (*.frb)");
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

void MainWindow::saveFile() {
    if (windowFilePath().isEmpty()) {
        saveFileAs();
    } else {
        QFile saveFile(windowFilePath());
        if (saveFile.open(QIODevice::WriteOnly)) {
            QDataStream stream(&saveFile);
            stream << exportFile();
        } else {
            QMessageBox::critical(this, "Error saving file", "An error occurred while trying to save the file");
        }
    }
}

void MainWindow::saveFileAs() {
    QString saveFileName = QFileDialog::getSaveFileName(this, "Save File", "", "Fortune Street Boards (*.frb)");
    QFile saveFile(saveFileName);
    if (saveFile.open(QIODevice::WriteOnly)) {
        QDataStream stream(&saveFile);
        stream << exportFile();
        setWindowFilePath(saveFileName);
    } else {
        QMessageBox::critical(this, "Error saving file", "An error occurred while trying to save the file");
    }
}

void MainWindow::loadFile(const BoardFile &file) {
    ui->boardEdit->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionSave_As->setEnabled(true);
    ui->menuTools->setEnabled(true);
    ui->initialCash->setText(QString::number(file.boardInfo.initialCash));
    ui->targetAmount->setText(QString::number(file.boardInfo.targetAmount));
    ui->baseSalary->setText(QString::number(file.boardInfo.baseSalary));
    ui->salaryIncrement->setText(QString::number(file.boardInfo.salaryIncrement));
    ui->maxDiceRoll->setText(QString::number(file.boardInfo.maxDiceRoll));
    ui->loopingMode->button(file.boardInfo.galaxyStatus)->setChecked(true);
    scene->clear();
    for (auto &square: file.boardData.squares) {
        //qDebug() << square.positionX << square.positionY;
        scene->addItem(new SquareItem(square));
    }
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
    auto items = scene->items(Qt::AscendingOrder);
    for (auto item: qAsConst(items)) {
        //qDebug() << ((SquareItem *)item)->getData().id;
        file.boardData.squares.append(((SquareItem *)item)->getData());
    }
    return file;
}

void MainWindow::updateSquareSidebar() {
    auto selectedItems = scene->selectedItems();
    if (selectedItems.size() == 1) {
        ui->squareEdit->setEnabled(true);
        SquareItem *item = (SquareItem *)selectedItems[0];
        ui->id->setText(QString::number(item->getData().id));
        ui->type->setCurrentText(squareTypeToText(item->getData().squareType));
        ui->districtDestinationId->setText(QString::number(item->getData().districtDestinationId));
        ui->shopModel->setCurrentText(shopTypeToText(item->getData().shopModel));
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
    } else {
        ui->squareEdit->setEnabled(false);
        ui->id->setText("");
    }
}

void MainWindow::registerSquareSidebarEvents() {
    connect(ui->type, &QComboBox::textActivated, this, [&](const QString &) { updateSquareData(); });
    connect(ui->districtDestinationId, &QLineEdit::textEdited, this, [&](const QString &) { updateSquareData(); });
    connect(ui->shopModel, &QComboBox::textActivated, this, [&](const QString &) {
        updateSquareData(true, true);
    });
    connect(ui->initialValue, &QLineEdit::textEdited, this, [&](const QString &) {
        updateSquareData(false, true);
    });
    connect(ui->initialPrice, &QLineEdit::textEdited, this, [&](const QString &) { updateSquareData(); });
    connect(ui->isLift, &QCheckBox::clicked, this, [&](bool) { updateSquareData(); });
    for (auto &waypointStart: waypointStarts) {
        connect(waypointStart, &QLineEdit::textEdited, this, [&](const QString &) { updateSquareData(); });
    }
    for (auto &waypointDest: waypointDests) {
        for (auto &child: waypointDest->findChildren<QLineEdit *>()) {
            connect(child, &QLineEdit::textEdited, this, [&](const QString &) { updateSquareData(); });
        }
    }
}

void MainWindow::updateSquareData(bool calcValue, bool calcPrice) {
    auto selectedItems = scene->selectedItems();
    if (selectedItems.size() == 1) {
        SquareItem *item = (SquareItem *)selectedItems[0];
        item->getData().squareType = textToSquareType(ui->type->currentText());
        item->getData().districtDestinationId = ui->districtDestinationId->text().toUInt();
        item->getData().shopModel = textToShopType(ui->shopModel->currentText());
        item->getData().value = ui->initialValue->text().toUShort();
        item->getData().price = ui->initialPrice->text().toUShort();
        item->getData().oneWayLift = ui->isLift->isChecked();
        for (int i=0; i<4; ++i) {
            item->getData().waypoints[i].entryId = waypointStarts[i]->text().toUInt();
            auto children = waypointDests[i]->findChildren<QLineEdit *>();
            for (int j=0; j<3; ++j) {
                item->getData().waypoints[i].destinations[j] = children[j]->text().toUInt();
            }
        }
        if (calcValue) {
            item->getData().updateValueFromShopModel();
        }
        if (calcPrice) {
            item->getData().updatePriceFromValue();
        }
        if (calcValue || calcPrice) {
            updateSquareSidebar();
        }
        item->update();
    }
}

void MainWindow::addSquare() {
    scene->addItem(new SquareItem(SquareData(scene->items().size() /* add next index */)));
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
    auto items = scene->items(Qt::AscendingOrder);
    for (int i=0; i<items.size(); ++i) {
        oldToNewIDs[((SquareItem *)items[i])->getData().id] = i;
        ((SquareItem *)items[i])->getData().id = i;
    }

    // and waypoints
    for (auto item: items) {
        for (auto &waypoint: ((SquareItem *)item)->getData().waypoints) {
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

void MainWindow::calcStockPrices() {
    QStringList builder;
    QVector<int> districtCount(12);
    QVector<int> districtSum(12);
    int highestDistrict = -1;
    auto items = scene->items(Qt::AscendingOrder);
    for (auto item: qAsConst(items)) {
        auto &square = ((SquareItem *)item)->getData();
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
    int highestDistrict;

    BoardFile file = exportFile();
    if (file.boardData.squares.size() > 0 && file.boardData.squares[0].squareType != Bank) {
        warnings << "There should be a bank at ID 0.";
    }
    if (file.boardData.squares.size() < 3) {
        errors << "Board must have at least 3 spaces.";
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
        } else if (districtCount[i] < 3) {
            warnings << QString("District %1 has %2 shops which is less than the recommended minimum of 3").arg(i).arg(districtCount[i]);
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
    auto items = scene->items(Qt::AscendingOrder);
    for (auto item: qAsConst(items)) {
        auto &square = ((SquareItem *)item)->getData();
        // ignore waypoints for one-way alleys for now
        if (square.squareType == OneWayAlleyDoorA
                || square.squareType == OneWayAlleyDoorB
                || square.squareType == OneWayAlleyDoorC
                || square.squareType == OneWayAlleyDoorD
                || square.squareType == OneWayAlleySquare) {
            continue;
        }
        for (auto &waypoint: square.waypoints) {
            waypoint.entryId = 255;
            for (auto &dest: waypoint.destinations) {
                dest = 255;
            }
        }
        QVector<int> touchingSquares;
        for (int i=0; i<items.size(); ++i) {
            auto &otherSquare = ((SquareItem *)items[i])->getData();
            if (otherSquare.id != square.id
                    && qAbs(square.positionX - otherSquare.positionX) <= 64
                    && qAbs(square.positionY - otherSquare.positionY) <= 64) {
                touchingSquares.append(i);
            }
        }
        int touchingSquaresNo = qMin(touchingSquares.size(), 4);
        for (int i=0; i<touchingSquaresNo; ++i) {
            square.waypoints[i].entryId = touchingSquares[i];
            int k=0;
            for (int j=0; j<touchingSquaresNo; ++j) {
                if (j != i) {
                    square.waypoints[i].destinations[k++] = touchingSquares[j];
                }
            }
        }
    }
    QMessageBox::information(this, "Auto-pathing", "Auto-pathed entire map");
    updateSquareSidebar();
}
