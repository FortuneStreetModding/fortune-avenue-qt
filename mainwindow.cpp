#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
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
            QMessageBox::critical(this, "Error saving file file", "An error occurred while trying to save the file");
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
        QMessageBox::critical(this, "Error saving file file", "An error occurred while trying to save the file");
    }
}

void MainWindow::loadFile(BoardFile file) {
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
    auto items = scene->items(Qt::AscendingOrder);
    for (int i=0; i<items.size(); ++i) {
        ((SquareItem *)items[i])->getData().id = i;
    }

    scene->update();
}

int MainWindow::calcSnapSizeFromInput() {
    int result = ui->snapTo->text().toInt();
    return result > 0 ? result : 1;
}

void MainWindow::calcStockPrices() {
    // todo implement
}

void MainWindow::verifyBoard() {
    // todo implement
}

void MainWindow::autoPath() {
    // todo implement
}
