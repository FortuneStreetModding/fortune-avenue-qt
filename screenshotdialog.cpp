#include "screenshotdialog.h"
#include "ui_screenshotdialog.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include "fortunestreetdata.h"
#include "squareitem.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

ScreenshotDialog::ScreenshotDialog(const QString &frbFilename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenshotDialog),
    scene(new FortuneAvenueGraphicsScene(-1600 + 32, -1600 + 32, 3200, 3200, this))
{
    ui->setupUi(this);
    this->setWindowTitle("Take Board Screenshots");
    scene->setAxesVisible(false);

    connect(ui->addFrbsButton, &QPushButton::clicked, this, [this](bool) { browseFrb(); });
    connect(ui->removeFrbsButton, &QPushButton::clicked, this, [this](bool) { removeFrb(); });

    if (!frbFilename.isEmpty()) {
        QFileInfo info(frbFilename);
        QRegularExpression re("(\\d)$");
        QString baseWithoutNumber = info.baseName().replace(re, "");
        QVector<QString> frbFiles;
        QString frbFile = info.path() + "/" + baseWithoutNumber + ".frb";
        if(QFile(frbFile).exists())
            frbFiles.append(frbFile);
        for(int i=0; i<94; i++) {
            QString frbFile = info.path() + "/" + QString("%1%2").arg(baseWithoutNumber).arg(i) + ".frb";
            if(QFile(frbFile).exists())
                frbFiles.append(frbFile);
            if(frbFiles.length()>=94)
                break;
        }

        // actually add the items to the widget
        ui->frbsListWidget->addItems(frbFiles);
    }
}

ScreenshotDialog::~ScreenshotDialog()
{
    delete ui;
}

void ScreenshotDialog::browseFrb() {
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Open File", QString(), "Fortune Street Boards (*.frb)");
    if(!filenames.isEmpty()) {
        ui->frbsListWidget->addItems(filenames);
    }
}

void ScreenshotDialog::removeFrb() {
    for (auto item: ui->frbsListWidget->selectedItems()) {
        delete item;
    }
}

BoardFile ScreenshotDialog::readBoardFile(const QString &filename, QRectF &rect) {
    QFile file(filename);
    BoardFile boardFile;
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        stream >> boardFile;
        if (stream.status() != QDataStream::Status::ReadCorruptData) {
            for (auto &square: boardFile.boardData.squares) {
                if (square.positionX < rect.left()) {
                    rect.setLeft(square.positionX);
                }
                if (square.positionX + 64 > rect.right()) {
                    rect.setRight(square.positionX + 64);
                }
                if (square.positionY < rect.top()) {
                    rect.setTop(square.positionY);
                }
                if (square.positionY + 64 > rect.bottom()) {
                    rect.setBottom(square.positionY + 64);
                }
            }
            return boardFile;
        } else {
            goto badFile;
        }
    } else {
        badFile:
        QMessageBox::critical(this, "Error opening file", "An error occurred while trying to open the file " + filename);
    }
    return boardFile;
}

extern bool makeScreenshot(QString* filename, QString* ext, BoardFile* boardFile, QRectF* rect){
    auto scene = new FortuneAvenueGraphicsScene(-1600 + 32, -1600 + 32, 3200, 3200);
    scene->setAxesVisible(false);
    scene->deleteLater();
    scene->clearSquares();
    for (auto &square: boardFile->boardData.squares) {
        scene->addItem(new SquareItem(square));
    }
    QFileInfo info(*filename);
    QString screenshotFilename = info.path() + "/" + info.baseName() + *ext;
    scene->clearSelection();
    scene->setSceneRect(*rect);
    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    scene->render(&painter);
    return image.save(screenshotFilename);
}

void ScreenshotDialog::accept() {
    if(ui->frbsListWidget->count() != 0){
        // disable the buttons so users can't mash while the operation is performed.
        ui->buttonBox->setEnabled(false);
        QRectF rect(1600, 1600, -3200, -3200);
        QStringList filenames;
        QList<BoardFile> boardFiles;
        QString ext = "." + ui->screenshotFormatLineEdit->text();
        int count = ui->frbsListWidget->count();

        for(int i=0; i<count; i++){
            auto filename = ui->frbsListWidget->item(i)->text();
            QFileInfo info(filename);
            QString screenshotFilename = info.path() + "/" + info.baseName() + ext;
            filenames += screenshotFilename;
            boardFiles.append(readBoardFile(filename, rect));
        }
        // do this work asynchronously, because at 93 possible board states, it's a heavy task!
        QList<QFuture<bool>> futures;
        for(int i = 0; i<count; i++){
            QFuture<bool> future = QtConcurrent::run(&makeScreenshot, &filenames[i], &ext, &boardFiles[i], &rect);
            futures.append(future);
        }

        bool anyStatusesFalse = false;
        for(int i=0; i<futures.count(); i++){
            if(!futures[i].isFinished()){
                futures[i].waitForFinished();
            }
            if(!futures[i].result()){
                anyStatusesFalse = true;
            }
        }

        if(!anyStatusesFalse){
            QMessageBox::information(this, "Success!", "All screenshots saved successfully.");
        }
        else{
            QMessageBox::critical(this, "Failure", "One or more screenshots could not be saved.");
        }
        ui->buttonBox->setEnabled(true);
    }
    else {
        // nothing to do!
    }

    close();
}
