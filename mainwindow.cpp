#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSaveFile>
#include <QSet>
#include <QDebug>
#include <QTimer>

#include "autopath.h"
#include "darkdetect.h"
#include "squareitem.h"
#include "util.h"
#include "screenshotdialog.h"
#include "autoassignshopmodelsdialog.h"
#include "muParser.h"
#include "squareaddcmd.h"
#include "squareremovecmd.h"
#include "squaremovecmd.h"
#include "squarechangecmd.h"
#include "updateboardmetacmd.h"

MainWindow::MainWindow(QApplication& app)
    : QMainWindow(), ui(new Ui::MainWindow),
      scene(new FortuneAvenueGraphicsScene(-1600 + 32, -1600 + 32, 3200, 3200, this)),
      square1Scene(new FortuneAvenueGraphicsScene(0,0,64,64,this)),
      square2Scene(new FortuneAvenueGraphicsScene(0,0,64,64,this)),
      undoStack(new QUndoStack(this)),
      updateOnSquareMove([this](const QMap<int, QPointF> &positions) {
            for (auto it=positions.begin(); it!=positions.end(); ++it) {
                auto &data = oldSquaresData()[it.key()];
                data.positionX = it.value().x();
                data.positionY = it.value().y();
            }
        })
{
    app.installEventFilter(this);
    scene->installEventFilter(this);
    ui->setupUi(this);
    waypointStarts = {ui->waypoint1Start, ui->waypoint2Start, ui->waypoint3Start, ui->waypoint4Start};
    waypointDests = {ui->waypoint1Dests, ui->waypoint2Dests, ui->waypoint3Dests, ui->waypoint4Dests};
    ui->graphicsView->setScene(scene);
    ui->graphicsView->centerOn(32, 32);
    ui->squareItemView1->setScene(square1Scene);
    ui->squareItemView2->setScene(square2Scene);

    ui->loopingMode->setId(ui->loopingNone, LoopingMode::None);
    ui->loopingMode->setId(ui->loopingVertical, LoopingMode::Vertical);
    ui->loopingMode->setId(ui->loopingVerticalHorizontal, LoopingMode::Both);
    ui->type->addItem("");
    ui->type->addItems(squareTexts());
    ui->shopModel->addItems(shopTextsWithValues());
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

    connect(ui->actionVerify_Board, &QAction::triggered, this, &MainWindow::verifyBoard);
    connect(ui->actionAuto_Path, &QAction::triggered, this, &MainWindow::autoPath);
    connect(ui->actionScreenshot, &QAction::triggered, this, &MainWindow::screenshot);
    connect(ui->actionAuto_Assign_Shop_Models, &QAction::triggered, this, &MainWindow::autoAssignShopModels);

    connect(ui->actionFortune_Avenue_Help, &QAction::triggered, this, [&]() {
        QDesktopServices::openUrl(QUrl("https://github.com/FortuneStreetModding/fortunestreetmodding.github.io/wiki/Fortune-Avenue-User-Manual"));
    });
    connect(ui->actionDistrict_Simulator, &QAction::triggered, this, [&]() {
        QDesktopServices::openUrl(QUrl("https://fortunestreetmodding.github.io/simulator"));
    });

    connect(ui->actionFollowWaypoint1, &QAction::triggered, this, [&]() {
        followWaypoint(0);
    });
    connect(ui->actionFollowWaypoint2, &QAction::triggered, this, [&]() {
        followWaypoint(1);
    });
    connect(ui->actionFollowWaypoint3, &QAction::triggered, this, [&]() {
        followWaypoint(2);
    });
    connect(ui->actionNext, &QAction::triggered, this, &MainWindow::selectNext);
    connect(ui->actionPrevious, &QAction::triggered, this, &MainWindow::selectPrevious);
    connect(ui->actionSelectAll, &QAction::triggered, this, &MainWindow::selectAll);
    connect(ui->actionUse_Advanced_Auto_Path, &QAction::triggered, this, &MainWindow::toggleAdvancedAutoPath);
    toggleAdvancedAutoPath();

    connect(ui->actionAuto_Calc_Custom_Function, &QAction::triggered, this, [&]() {
        bool ok;
        QString newFunc = QInputDialog::getText(this, "Enter Function", "Enter the shop price based on shop value function (x = shop value).\nEnter empty string to restore default.", QLineEdit::Normal, priceFunction, &ok);
        if (ok) {
            if(newFunc.isEmpty()) {
                priceFunction = defaultPriceFunction;
            } else {
                try
                {
                    calcShopPriceFromValue(newFunc, 1);
                    priceFunction = newFunc;
                }
                catch (mu::Parser::exception_type &e)
                {
                    QMessageBox::critical(this, "Error Function", QString::fromStdWString(e.GetMsg()));
                }
            }
        }
    });

    connect(ui->actionSet_Max_Path_Search_Depth, &QAction::triggered, this, [&]() {
        bool ok;
        int newMaxPathSearchDepth = QInputDialog::getInt(this, "Enter Search Depth", "Enter the max path search depth.\n\n"
                                                                                     "This is only for demonstration purposes and does not have any ingame effect.\n"
                                                                                     "Sensible values are between 16 (small boards) and 28 (huge boards).\n\n"
                                                                                     "Enter 0 to restore game default behavior which is max(squareCount/3,16)", maxPathSearchDepth, 0, 50, 1, &ok);
        if (ok) {
            maxPathSearchDepth = newMaxPathSearchDepth;
        }
    });

    auto undoAction = undoStack->createUndoAction(ui->menuEdit);
    undoAction->setShortcut(QKeySequence::Undo);
    ui->menuEdit->addAction(undoAction);
    auto redoAction = undoStack->createRedoAction(ui->menuEdit);
    redoAction->setShortcut(QKeySequence::Redo);
    ui->menuEdit->addAction(redoAction);

    connect(ui->initialCash, &QLineEdit::editingFinished, this, [this]() {
        addUpdateBoardMetaAction([this](BoardInfo &info) {
            info.initialCash = ui->initialCash->text().toInt();
        });
    });
    connect(ui->baseSalary, &QLineEdit::editingFinished, this, [this]() {
        addUpdateBoardMetaAction([this](BoardInfo &info) {
            info.baseSalary = ui->baseSalary->text().toInt();
        });
    });
    connect(ui->salaryIncrement, &QLineEdit::editingFinished, this, [this]() {
        addUpdateBoardMetaAction([this](BoardInfo &info) {
            info.salaryIncrement = ui->salaryIncrement->text().toInt();
        });
    });
    connect(ui->maxDiceRoll, &QLineEdit::editingFinished, this, [this]() {
        addUpdateBoardMetaAction([this](BoardInfo &info) {
            info.maxDiceRoll = ui->maxDiceRoll->text().toInt();
        });
    });
    connect(ui->loopingMode, &QButtonGroup::idClicked, this, [this](int buttonId) {
        addUpdateBoardMetaAction([=](BoardInfo &info) {
            info.galaxyStatus = (LoopingMode)buttonId;
        });
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

    registerSquareSidebarEvents();

    connect(undoStack, &QUndoStack::cleanChanged, this, [this](bool clean) {
        setWindowModified(!clean);
    });

    // for windows auto open
    auto args = app.arguments();
    if (args.size() >= 2) {
        loadFile(args[1]);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        //QString text;
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(obj);
        if (lineEdit) {
            bool ok;
            int number = lineEdit->text().toInt(&ok, 10);
            if(ok && key->key() == Qt::Key_Up) {
                lineEdit->setText(QString::number(number+1));
                emit lineEdit->textEdited(lineEdit->text());
            }
            if(ok && key->key() == Qt::Key_Down) {
                lineEdit->setText(QString::number(number-1));
                emit lineEdit->textEdited(lineEdit->text());
            }
        }
    } else if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *fo = static_cast<QFileOpenEvent *>(event);
        loadFile(fo->file());
    } else if (event->type() == FASceneSquareMoveEvent::TYPE) {
        FASceneSquareMoveEvent *sme = static_cast<FASceneSquareMoveEvent *>(event);
        undoStack->push(new SquareMoveCmd(scene, sme->getOldPositions(), sme->getNewPositions(), updateOnSquareMove));
        return true;
    }
    return QObject::eventFilter(obj, event);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QVector<SquareData> &MainWindow::oldSquaresData()
{
    return oldBoardFile.boardData.squares;
}

void MainWindow::addChangeSquaresAction(const QVector<int> &squareIdsToUpdate)
{
    auto sqItems = scene->squareItems();
    QMap<int, SquareData> oldData, newData;
    for (int sqId: squareIdsToUpdate) {
        oldData[sqId] = oldSquaresData()[sqId];
        newData[sqId] = sqItems[sqId]->getData();
    }
    if (oldData != newData) {
        undoStack->push(new SquareChangeCmd(scene, oldData, newData, [=]() {
            auto sqItems = scene->squareItems();
            for (int sqId: squareIdsToUpdate) {
                oldSquaresData()[sqId] = sqItems[sqId]->getData();
            }
            updateSquareSidebar();
        }));
    }
}

template<class Func>
void MainWindow::addUpdateBoardMetaAction(Func &&func)
{
    auto oldBoardInfo = oldBoardFile.boardInfo;
    func(oldBoardFile.boardInfo);
    undoStack->push(new UpdateBoardMetaCmd(oldBoardInfo, oldBoardFile.boardInfo, [this](const BoardInfo &info) {
        oldBoardFile.boardInfo = info;
        updateBoardInfoSidebar();
    }));
}

void MainWindow::updateZoom() {
    ui->graphicsView->resetTransform();
    ui->graphicsView->scale(zoomPercent / 100.0, zoomPercent / 100.0);
    ui->statusbar->showMessage(QString("Zoom: %1%").arg(zoomPercent));
}

QPair<SquareItem*,SquareItem*> MainWindow::getPreviousAndCurrentSquare() {
    SquareItem *previous = nullptr;
    SquareItem *current = nullptr;
    for(auto& item : scene->items()) {
        SquareItem *squareItem = (SquareItem*) item;
        if(squareItem->getData().id == previouslyVisitedSquareId) {
            previous = squareItem;
            for(auto& selectedItem : scene->selectedItems()) {
                SquareItem *selectedSquareItem = (SquareItem*) selectedItem;
                if(selectedSquareItem->getData().id != previouslyVisitedSquareId) {
                    current = selectedSquareItem;
                }
            }
            break;
        }
    }
    return QPair<SquareItem*,SquareItem*>(previous, current);
}


void MainWindow::followWaypoint(int destinationId) {
    auto selectedItems = scene->selectedItems();
    if (selectedItems.size() == 2) {
        auto previousAndCurrentSquare = getPreviousAndCurrentSquare();
        SquareItem *previous = previousAndCurrentSquare.first;
        SquareItem *current = previousAndCurrentSquare.second;
        if(previous != nullptr && current != nullptr) {
            previouslyVisitedSquareId = current->getData().id;
            for(auto &waypoint : current->getData().waypoints) {
                if(waypoint.entryId == previous->getData().id) {
                    previous->setSelected(false);
                    int nextSquareId = waypoint.destinations[destinationId];
                    if(nextSquareId == 255) {
                        nextSquareId = -1;
                        for(auto &waypointDest : waypoint.destinations) {
                            if(waypointDest != 255 && waypointDest > nextSquareId) {
                                nextSquareId = waypointDest;
                            }
                        }
                    }
                    for(auto &item : scene->items()) {
                        SquareItem *squareItem = (SquareItem *)item;
                        if(squareItem->getData().id == nextSquareId) {
                            squareItem->setSelected(true);
                        }
                    }
                }
            }
        }
    }
}
void MainWindow::selectNext() {
    auto highestId = scene->squareItems().size() - 1;
    auto selectedItems = scene->selectedItems();
    if (selectedItems.size() == 0) {
        for(auto &item : scene->items()) {
            SquareItem *squareItem = (SquareItem *)item;
            if(squareItem->getData().id == 0) {
                squareItem->setSelected(true);
            }
        }
    } else {
        QList<int> toBeSelectedItems;
        for(auto &item : scene->selectedItems()) {
            SquareItem *squareItem = (SquareItem *)item;
            if(squareItem->getData().id == highestId) {
                toBeSelectedItems.append(0);
            } else {
                toBeSelectedItems.append(squareItem->getData().id + 1);
            }
        }
        for(auto &item : scene->items()) {
            SquareItem *squareItem = (SquareItem *)item;
            squareItem->setSelected(toBeSelectedItems.contains(squareItem->getData().id));
        }
    }
}
void MainWindow::selectPrevious() {
    if(scene->squareItems().size() <= 0) {
        return;
    }
    auto highestId = scene->squareItems().size() - 1;
    auto selectedItems = scene->selectedItems();
    if (selectedItems.size() == 0) {
        for(auto &item : scene->items()) {
            SquareItem *squareItem = (SquareItem *)item;
            if(squareItem->getData().id == highestId) {
                squareItem->setSelected(true);
            }
        }
    } else {
        QList<int> toBeSelectedItems;
        for(auto &item : scene->selectedItems()) {
            SquareItem *squareItem = (SquareItem *)item;
            if(squareItem->getData().id == 0) {
                toBeSelectedItems.append(highestId);
            } else {
                toBeSelectedItems.append(squareItem->getData().id - 1);
            }
        }
        for(auto &item : scene->items()) {
            SquareItem *squareItem = (SquareItem *)item;
            squareItem->setSelected(toBeSelectedItems.contains(squareItem->getData().id));
        }
    }
}
void MainWindow::selectAll() {
    for(auto &item : scene->items()) {
        item->setSelected(true);
    }
}

void MainWindow::toggleAdvancedAutoPath() {
    bool visible = ui->actionUse_Advanced_Auto_Path->isChecked();
    ui->autoPathCfg->setVisible(visible);
    ui->autopathArrowsWidget->setVisible(visible);
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
    loadFile(filename);
}

bool MainWindow::saveFile() {
    if (windowFilePath().isEmpty()) {
        return saveFileAs();
    } else {
        // apply any changes from text boxes when saving
        auto focusWidget = QApplication::focusWidget();
        if (focusWidget) focusWidget->clearFocus();

        QSaveFile saveFile(windowFilePath());
        if (saveFile.open(QIODevice::WriteOnly)) {
            QDataStream stream(&saveFile);
            auto fileContents = exportFile();
            stream << fileContents;
            if (!saveFile.commit()) {
                goto fail;
            }
            undoStack->setClean();
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
        setWindowFilePath(saveFileName);
        undoStack->setClean();
        return true;
    } else {
        fail:
        QMessageBox::critical(this, "Error saving file", "An error occurred while trying to save the file");
        return false;
    }
}

void MainWindow::loadFile(const QString &fpath) {
    QFile file(fpath);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        BoardFile boardFile;
        stream >> boardFile;
        if (stream.status() != QDataStream::Status::ReadCorruptData) {
            setWindowFilePath(fpath);
            QFileInfo fileInfo(file);
            QString filename(fileInfo.fileName());
            setWindowTitle(QString("Fortune Avenue %1.%2.%3 - %4[*]").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD).arg(filename));
            loadFile(boardFile);
        } else {
            goto badFile;
        }
    } else {
        badFile:
        QMessageBox::critical(this, "Error opening file", "An error occurred while trying to open the file");
    }
}

void MainWindow::loadFile(const BoardFile &file) {
    undoStack->clear();

    ui->boardEdit->setEnabled(true);
    ui->actionSave->setEnabled(true);
    ui->actionSave_As->setEnabled(true);
    ui->menuTools->setEnabled(true);
    ui->menuView->setEnabled(true);

    ui->initialCash->setText(QString::number(file.boardInfo.initialCash));
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
    if(file.boardInfo.versionFlag >= 3) {
        ui->actionUse_Advanced_Auto_Path->setChecked(file.boardInfo.useAdvancedAutoPath);
    } else if (file.boardInfo.versionFlag > 0) {
        ui->actionUse_Advanced_Auto_Path->setChecked(true);
    } else if (file.boardInfo.versionFlag == 0) {
        ui->actionUse_Advanced_Auto_Path->setChecked(false);
    }
    toggleAdvancedAutoPath();

    oldBoardFile = file;

    if (file.boardInfo.versionFlag < 3) {
        auto response = QMessageBox::question(
            this,
            "Open File",
            QString("This file is version %1, but the latest version is version 3. Would you like to upgrade the file to the latest version?")
                .arg(file.boardInfo.versionFlag)
        );
        if (response == QMessageBox::Yes) {
            ui->fileVersion->setText(QString::number(3));
            oldBoardFile.boardInfo.versionFlag = 3;

            // VERSION 0 -> VERSION >= 1
            if (file.boardInfo.versionFlag < 1) {
                auto items = scene->squareItems();
                for (int i=0; i<items.size(); ++i) {
                    auto touchingSquares = AutoPath::getTouchingSquares(items[i], items, ui->autopathRange->value(), ui->straightLineTolerance->value());
                    AutoPath::enumerateAutopathingRules(items[i], touchingSquares);
                    oldBoardFile.boardData.squares[i] = items[i]->getData();
                }
            }

            undoStack->resetClean();
        }
    }

    QCoreApplication::processEvents();
}

BoardFile MainWindow::exportFile() {
    return oldBoardFile;
}

void MainWindow::updateBoardInfoSidebar()
{
    ui->initialCash->setText(QString::number(oldBoardFile.boardInfo.initialCash));
    ui->baseSalary->setText(QString::number(oldBoardFile.boardInfo.baseSalary));
    ui->salaryIncrement->setText(QString::number(oldBoardFile.boardInfo.salaryIncrement));
    ui->maxDiceRoll->setText(QString::number(oldBoardFile.boardInfo.maxDiceRoll));
    ui->loopingMode->button(oldBoardFile.boardInfo.galaxyStatus)->setChecked(true);
}

void MainWindow::updateSquareSidebar() {
    auto selectedItems = scene->selectedItems();
    ui->squareEdit->setEnabled(!selectedItems.empty());
    ui->actionFollowWaypoint1->setEnabled(selectedItems.size() == 2);
    ui->actionFollowWaypoint2->setEnabled(selectedItems.size() == 2);
    ui->actionFollowWaypoint3->setEnabled(selectedItems.size() == 2);
    if (selectedItems.size() == 1) {
        ui->waypoint1Dests->setEnabled(true);
        ui->waypoint1Start->setEnabled(true);
        ui->waypoint2Dests->setEnabled(true);
        ui->waypoint2Start->setEnabled(true);
        ui->waypoint3Dests->setEnabled(true);
        ui->waypoint3Start->setEnabled(true);
        ui->waypoint4Dests->setEnabled(true);
        ui->waypoint4Start->setEnabled(true);
        SquareItem *item = (SquareItem *)selectedItems[0];
        ui->id->setText(QString::number(item->getData().id));
        ui->type->setCurrentText(squareTypeToText(item->getData().squareType));
        ui->districtDestinationId->setText(QString::number(item->getData().districtDestinationId));
        QString shopTypeStrWithValue = shopTypeToTextWithValue(item->getData().shopModel);
        if(ui->shopModel->findText(shopTypeStrWithValue) != -1)
            ui->shopModel->setCurrentText(shopTypeStrWithValue);
        else
            ui->shopModel->setCurrentIndex(0);
        ui->shopModelName->setText(shopTypeToText(item->getData().shopModel));
        ui->shopModelId->setText(QString::number(item->getData().shopModel));
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
        ui->yield->setText(QString::number(item->getData().getYield(), 'f', 3));

        updateDestinationUI();

        previouslyVisitedSquareId = item->getData().id;
    } else {
        ui->waypoint1Dests->setEnabled(false);
        ui->waypoint1Start->setEnabled(false);
        ui->waypoint2Dests->setEnabled(false);
        ui->waypoint2Start->setEnabled(false);
        ui->waypoint3Dests->setEnabled(false);
        ui->waypoint3Start->setEnabled(false);
        ui->waypoint4Dests->setEnabled(false);
        ui->waypoint4Start->setEnabled(false);
        if(selectedItems.size() > 1) {
            ui->id->setText(QString("%1 selected").arg(selectedItems.size()));
        } else {
            ui->id->setText("");
        }
    }
    square1Scene->clear();
    square2Scene->clear();
    if((selectedItems.size() == 1 || selectedItems.size() == 2) && previouslyVisitedSquareId != -1 && previouslyVisitedSquareId < scene->squareItems().size()) {
        ui->connectSquares->setEnabled(true);
        auto previousAndCurrentSquare = getPreviousAndCurrentSquare();
        SquareItem *previous = previousAndCurrentSquare.first;
        SquareItem *current = previousAndCurrentSquare.second;
        if(selectedItems.size() == 2) {
            SquareItem *item0 = (SquareItem *)selectedItems[0];
            SquareItem *item1 = (SquareItem *)selectedItems[1];
            if(current == item0) {
                previous = item1;
            } else if(current == item1) {
                previous = item0;
            } else {
                current = item1;
                previous = item0;
            }
        }
        if(previous != nullptr) {
            SquareData &p = previous->getData();
            square1Scene->addItem(new SquareItem(SquareData(p.id, p.squareType, p.districtDestinationId, p.value, p.price)));
        }
        if(current != nullptr) {
            SquareData &c = current->getData();
            square2Scene->addItem(new SquareItem(SquareData(c.id, c.squareType, c.districtDestinationId, c.value, c.price)));
        }
    } else {
        ui->connectSquares->setEnabled(false);
    }
    calcStockPrices();
}

void MainWindow::clearWaypoint(SquareItem *item, int waypointId) {
    auto &waypoint = item->getData().waypoints[waypointId];
    waypoint.destinations[0] = 255;
    waypoint.destinations[1] = 255;
    waypoint.destinations[2] = 255;
    waypoint.entryId = 255;
}

int MainWindow::calcShopPriceFromValue(const QString &function, int value) {
    double x = (qreal) value;
    mu::Parser p;
    p.DefineVar(QString("x").toStdWString(), &x);
    p.SetExpr(function.toStdWString());
    int price = qRound(p.Eval());
    if(price < 0)
        return 0;
    return price;
}

void MainWindow::connectSquares(bool previousToCurrent, bool currentToPrevious) {
    auto items1 = ui->squareItemView1->items();
    auto items2 = ui->squareItemView2->items();
    if(items1.size() > 0 && items2.size() > 0) {
        SquareItem *previous = (SquareItem *)items1.first();
        SquareItem *current = (SquareItem *)items2.first();
        int previousId = previous->getData().id;
        int currentId = current->getData().id;
        for(auto &item : scene->items()) {
            SquareItem *squareItem = (SquareItem *)item;
            if(squareItem->getData().id == previousId) {
                previous = squareItem;
            }
            if(squareItem->getData().id == currentId) {
                current = squareItem;
            }
        }
        if(previousToCurrent)
            AutoPath::connect(previous->getData(), current->getData());
        if(currentToPrevious)
            AutoPath::connect(current->getData(), previous->getData());
        addChangeSquaresAction({previous->getData().id, current->getData().id});
    }
}

void MainWindow::registerSquareSidebarEvents() {
    connect(ui->type, &QComboBox::textActivated, this, [&](const QString &) {
        updateSquare([&](SquareItem *item) {
            item->getData().squareType = textToSquareType(ui->type->currentText());
        });
    });
    connect(ui->positionX, &QLineEdit::editingFinished, this, [&]() {
        auto selItems = scene->selectedItems();
        QMap<int, QPointF> oldPos, newPos;
        for (auto &elem: selItems) {
            SquareItem *item = (SquareItem *)elem;
            auto pos = item->pos();
            oldPos[item->getData().id] = pos;
            pos.setX(ui->positionX->text().toInt());
            newPos[item->getData().id] = pos;
        }
        if (oldPos != newPos) undoStack->push(new SquareMoveCmd(scene, oldPos, newPos, updateOnSquareMove));
    });
    connect(ui->positionY, &QLineEdit::editingFinished, this, [&]() {
        auto selItems = scene->selectedItems();
        QMap<int, QPointF> oldPos, newPos;
        for (auto &elem: selItems) {
            SquareItem *item = (SquareItem *)elem;
            auto pos = item->pos();
            oldPos[item->getData().id] = pos;
            pos.setY(ui->positionY->text().toInt());
            newPos[item->getData().id] = pos;
        }
        if (oldPos != newPos) undoStack->push(new SquareMoveCmd(scene, oldPos, newPos, updateOnSquareMove));
    });
    connect(ui->districtDestinationId, &QLineEdit::editingFinished, this, [&]() {
        updateSquare([&](SquareItem *item) {
            item->getData().districtDestinationId = ui->districtDestinationId->text().toUInt();
        });
    });
    connect(ui->shopModel, &QComboBox::textActivated, this, [&](const QString &) {
        updateSquare([&](SquareItem *item) {
            item->getData().shopModel = textToShopType(ui->shopModel->currentText());
            item->getData().updateValueFromShopModel();
            if(ui->autoCalcPrice->isChecked()) {
                try {
                    auto newPrice = calcShopPriceFromValue(priceFunction, item->getData().value);
                    item->getData().price = newPrice;
                } catch (mu::Parser::exception_type &e) {}
            }
        });
    });
    connect(ui->shopModelId, &QLineEdit::editingFinished, this, [&]() {
        updateSquare([&](SquareItem *item) {
            item->getData().shopModel = ui->shopModelId->text().toUShort();
        });
    });
    connect(ui->initialValue, &QLineEdit::editingFinished, this, [&]() {
        updateSquare([&](SquareItem *item) {
            item->getData().value = ui->initialValue->text().toUShort();
            if(ui->autoCalcPrice->isChecked()) {
                try {
                    auto newPrice = calcShopPriceFromValue(priceFunction, item->getData().value);
                    item->getData().price = newPrice;
                } catch (mu::Parser::exception_type &e) {}
            }
        });
    });
    connect(ui->initialPrice, &QLineEdit::editingFinished, this, [&]() {
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
        connect(waypointStart, &QLineEdit::editingFinished, this, [&]() { updateWaypoints(); });
    }
    for (auto &waypointDest: waypointDests) {
        for (auto &child: waypointDest->findChildren<QLineEdit *>()) {
            connect(child, &QLineEdit::editingFinished, this, [&]() { updateWaypoints(); });
        }
    }
    connect(ui->clearAllWaypoints, &QPushButton::clicked, this, [&](bool) {
        updateSquare([&](SquareItem *item) {
            clearWaypoint(item, 0);
            clearWaypoint(item, 1);
            clearWaypoint(item, 2);
            clearWaypoint(item, 3);
        });
    });
    connect(ui->clearWaypoint1, &QPushButton::clicked, this, [&](bool) { updateSquare([&](SquareItem *item) { clearWaypoint(item, 0); }); });
    connect(ui->clearWaypoint2, &QPushButton::clicked, this, [&](bool) { updateSquare([&](SquareItem *item) { clearWaypoint(item, 1); }); });
    connect(ui->clearWaypoint3, &QPushButton::clicked, this, [&](bool) { updateSquare([&](SquareItem *item) { clearWaypoint(item, 2); }); });
    connect(ui->clearWaypoint4, &QPushButton::clicked, this, [&](bool) { updateSquare([&](SquareItem *item) { clearWaypoint(item, 3); }); });
    connect(ui->sortAllWaypoints, &QPushButton::clicked, this, [&](bool) { updateSquare([&](SquareItem *item) { AutoPath::sortWaypoints(item); }); });
    connect(ui->connectSquareItem1_2, &QPushButton::clicked, this, [&](bool) { connectSquares(true, false); });
    connect(ui->connectSquareItem2_1, &QPushButton::clicked, this, [&](bool) { connectSquares(false, true); });
    connect(ui->connectSquareItemBi, &QPushButton::clicked, this, [&](bool) { connectSquares(true, true); });

    ui->fromButtons->setId(ui->from_northwest, AutoPath::Northwest);
    ui->fromButtons->setId(ui->from_north, AutoPath::North);
    ui->fromButtons->setId(ui->from_northeast, AutoPath::Northeast);
    ui->fromButtons->setId(ui->from_west, AutoPath::West);
    ui->fromButtons->setId(ui->from_east, AutoPath::East);
    ui->fromButtons->setId(ui->from_southwest, AutoPath::Southwest);
    ui->fromButtons->setId(ui->from_south, AutoPath::South);
    ui->fromButtons->setId(ui->from_southeast, AutoPath::Southeast);

    // add dark mode icons
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
        if (fromDir >= 0) {
            updateSquare([=](SquareItem *item) {
                if (ui->toButtons->button(toDir)->isChecked()) {
                    item->getData().validDirections.insert((AutoPath::Direction)fromDir, (AutoPath::Direction)toDir);
                } else {
                    item->getData().validDirections.remove((AutoPath::Direction)fromDir, (AutoPath::Direction)toDir);
                }
            });
        }
    });

    connect(ui->resetPaths, &QPushButton::clicked, this, [&]() {
        updateSquare([=](SquareItem *item) {
            for (auto from: AutoPath::DIRECTIONS) {
                for (auto to: AutoPath::DIRECTIONS) {
                    item->getData().validDirections.insert(from, to);
                }
            }
        });
    });

    connect(ui->allowAll, &QPushButton::clicked, this, [&]() {
        auto fromDir = ui->fromButtons->checkedId();
        if (fromDir >= 0) {
            updateSquare([=](SquareItem *item) {
                for (auto to: AutoPath::DIRECTIONS) {
                    item->getData().validDirections.insert((AutoPath::Direction)fromDir, to);
                }
            });
        }
    });

    connect(ui->disallowAll, &QPushButton::clicked, this, [&]() {
        auto fromDir = ui->fromButtons->checkedId();
        if (fromDir >= 0) {
            updateSquare([=](SquareItem *item) {
                item->getData().validDirections.remove((AutoPath::Direction)fromDir);
            });
        }
    });
}

template<typename Func> void MainWindow::updateSquare(Func &&func) {
    auto selectedItems = scene->selectedItems();
    QVector<int> idsToUpdate;
    for(auto selectedItem : qAsConst(selectedItems)) {
        SquareItem *item = (SquareItem *)selectedItem;
        func(item);
        idsToUpdate.push_back(item->getData().id);
    }
    addChangeSquaresAction(idsToUpdate);
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
    undoStack->push(new SquareAddCmd(scene, [this](SquareItem *item) {
                        if (item) {
                            oldSquaresData().push_back(item->getData());
                        } else {
                            oldSquaresData().pop_back();
                        }
                    }));
}

void MainWindow::removeSquare() {
    if (!scene->selectedItems().empty()) {
        undoStack->push(new SquareRemoveCmd(scene, [this](const QVector<SquareData> &squares, bool isRedo) {
            (void)squares;
            (void)isRedo;
            oldSquaresData().clear();
            auto sqItems = scene->squareItems();
            for (auto &item: sqItems) oldSquaresData().push_back(item->getData());
        }));
    }
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
    for (int i=0; i<=12; ++i) {
        if (i>highestDistrict || districtCount[i] == 0) {
            builder << QString();
            continue;
        }
        qint64 result = districtSum[i] / districtCount[i];
        result *= 0xB00;
        result >>= 16;
        builder << QString("District %1: %2g").arg(char('A' + i)).arg(result);
    }
    builder << QString();
    int limit = 300;
    auto boardFile = exportFile();
    int searchDepth = maxPathSearchDepth;
    if(searchDepth == 0) {
        searchDepth = boardFile.boardData.squares.size() / 3;
        if (searchDepth < 16)
            searchDepth = 16;
        if (searchDepth > 28)
            searchDepth = 28;
    }
    auto result = AutoPath::getSquareIdWithMaxPathsCount(qAsConst(items), searchDepth, limit);
    auto maxPathSquareId = result.first;
    auto maxPathCount = result.second;
    QString maxPathCountStr;
    if(maxPathCount>limit) {
        maxPathCountStr = QString(">%1").arg(QString::number(limit));
    } else {
        maxPathCountStr = QString::number(maxPathCount);
    }
    if(maxPathSquareId != 255) {
        if(maxPathSearchDepth == 0) {
            builder << QString("Search Depth: %1").arg(searchDepth);
        } else {
            builder << QString("Using Fixed Depth: %1").arg(searchDepth);
        }
        builder << QString("Max Paths Square ID: %1").arg(maxPathSquareId);
        builder << QString("Possible Paths: %1").arg(maxPathCountStr);
        if(maxPathSearchDepth == 0) {
            if(maxPathCount > 150) {
                builder << QString("Crash inevitable!");
            } else if(maxPathCount > 90) {
                builder << QString("Crash possible!");
            } else if(maxPathCount > 70) {
                builder << QString("Stuttering likely!");
            } else {
                builder << QString("Ok");
            }
        } else {
            builder << QString("");
        }
    } else {
        builder << QString();
        builder << QString();
        builder << QString();
        builder << QString();
    }
    ui->stockPricesLabel->setText(builder.join("\n"));
}

void MainWindow::verifyBoard() {
    QStringList errors, warnings;

    exportFile().verify(errors, warnings);

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
    if(ui->actionUse_Advanced_Auto_Path->isChecked()) {
        for (auto item: qAsConst(items)) {
            QMap<AutoPath::Direction, SquareItem *> touchingSquares = AutoPath::getTouchingSquares(item, items, ui->autopathRange->value(), ui->straightLineTolerance->value());
            AutoPath::pathSquare(item, touchingSquares);
        }
    } else {
        AutoPath::kruskalDfsAutoPathAlgorithm(qAsConst(items));
    }
    QMessageBox::information(this, "Auto-pathing", "Auto-pathed entire map");
    updateSquareSidebar();
}

void MainWindow::screenshot() {
    ScreenshotDialog dialog(windowFilePath(), this);
    dialog.exec();
}

void MainWindow::autoAssignShopModels() {
    auto items = scene->squareItems();
    AutoAssignShopModelsDialog dialog(this, &items, priceFunction);
    dialog.exec();
    // mark all square ids as dirty
    QVector<int> squareIds(items.size());
    std::iota(squareIds.begin(), squareIds.end(), 0);
    addChangeSquaresAction(squareIds);
    QCoreApplication::processEvents();
}

void MainWindow::updateDestinationUI() {
    auto selectedItems = scene->selectedItems();
    if (!selectedItems.empty()) {
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
    if (!isWindowModified()) {
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
