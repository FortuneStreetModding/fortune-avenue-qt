#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "squareitem.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), scene(new QGraphicsScene(-1600, -1600, 3200, 3200, this))
{
    ui->setupUi(this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->centerOn(0, 0);
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(ui->actionSave_As, SIGNAL(triggered()), this, SLOT(saveFileAs()));
    connect(scene, SIGNAL(selectionChanged()), this, SLOT(updateSquareSidebar()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newFile() {
    qDebug() << "new file";
}

void MainWindow::openFile() {
    qDebug() << "open file";
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
    qDebug() << "save file";
    // todo expand this stub
}

void MainWindow::saveFileAs() {
    qDebug() << "save file as";
    // todo expand this stub
}

void MainWindow::loadFile(BoardFile file) {
    qDebug() << "load file";
    ui->initialCash->setText(QString::number(file.boardInfo.initialCash));
    ui->targetAmount->setText(QString::number(file.boardInfo.targetAmount));
    for (auto &square: file.boardData.squares) {
        //qDebug() << square.positionX << square.positionY;
        scene->addItem(new SquareItem(square));
    }
    QCoreApplication::processEvents();
}

BoardFile MainWindow::exportFile() {
    BoardFile file;
    // todo add board file stuff
    return file;
}

void MainWindow::updateSquareSidebar() {
    auto selectedItems = scene->selectedItems();
    if (selectedItems.size() == 1) {
        SquareItem *item = (SquareItem *)selectedItems[0];
        ui->id->setText(QString::number(item->getData().id));
    } else {
        ui->id->setText("");
    }
}
