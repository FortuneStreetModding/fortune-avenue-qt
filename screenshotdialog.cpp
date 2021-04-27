#include "screenshotdialog.h"
#include "ui_screenshotdialog.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include "fortunestreetdata.h"
#include "squareitem.h"

ScreenshotDialog::ScreenshotDialog(const QString &frbFilename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ScreenshotDialog),
    scene(new FortuneAvenueGraphicsScene(-1600 + 32, -1600 + 32, 3200, 3200, this))
{
    ui->setupUi(this);
    scene->setAxesVisible(false);
    connect(ui->pushButton, &QPushButton::clicked, this, [&]() {browseFrb(ui->lineEdit, ui->checkBox);});
    connect(ui->pushButton_2, &QPushButton::clicked, this, [&]() {browseFrb(ui->lineEdit_2, ui->checkBox_2);});
    connect(ui->pushButton_3, &QPushButton::clicked, this, [&]() {browseFrb(ui->lineEdit_3, ui->checkBox_3);});
    connect(ui->pushButton_4, &QPushButton::clicked, this, [&]() {browseFrb(ui->lineEdit_4, ui->checkBox_4);});

    connect(ui->checkBox, &QCheckBox::clicked, this, [&](bool checked) { ui->lineEdit->setEnabled(checked); });
    connect(ui->checkBox_2, &QCheckBox::clicked, this, [&](bool checked) { ui->lineEdit_2->setEnabled(checked); });
    connect(ui->checkBox_3, &QCheckBox::clicked, this, [&](bool checked) { ui->lineEdit_3->setEnabled(checked); });
    connect(ui->checkBox_4, &QCheckBox::clicked, this, [&](bool checked) { ui->lineEdit_4->setEnabled(checked); });

    if(!frbFilename.isEmpty()) {
        QFileInfo info(frbFilename);
        QRegularExpression re("(\\d)$");
        QString baseWithoutNumber = info.baseName().replace(re, "");
        QVector<QString> frbFiles;
        QString frbFile = info.path() + "/" + baseWithoutNumber + ".frb";
        if(QFile(frbFile).exists())
            frbFiles.append(frbFile);
        for(int i=0; i<5; i++) {
            QString frbFile = info.path() + "/" + QString("%1%2").arg(baseWithoutNumber).arg(i) + ".frb";
            if(QFile(frbFile).exists())
                frbFiles.append(frbFile);
            if(frbFiles.length()>=4)
                break;
        }
        if(frbFiles.contains(frbFilename)) {
            ui->lineEdit->setText(frbFiles.at(0));
            ui->checkBox->setChecked(true);
            if(frbFiles.length() > 1) {
                ui->lineEdit_2->setText(frbFiles.at(1));
                ui->checkBox_2->setChecked(true);
            }
            if(frbFiles.length() > 2) {
                ui->lineEdit_3->setText(frbFiles.at(2));
                ui->checkBox_3->setChecked(true);
            }
            if(frbFiles.length() > 3) {
                ui->lineEdit_4->setText(frbFiles.at(3));
                ui->checkBox_4->setChecked(true);
            }
        } else {
            ui->lineEdit->setText(frbFilename);
            ui->checkBox->setChecked(true);
        }
    }
}

ScreenshotDialog::~ScreenshotDialog()
{
    delete ui;
}

void ScreenshotDialog::browseFrb(QLineEdit *lineEdit, QCheckBox *checkBox) {
    QString filename = QFileDialog::getOpenFileName(this, "Open File", QString(), "Fortune Street Boards (*.frb)");
    if(!filename.isEmpty()) {
        lineEdit->setText(filename);
        lineEdit->setEnabled(true);
        checkBox->setChecked(true);
    }
}

void ScreenshotDialog::readBoardFile(const QString &filename, BoardFile &boardFile, QRectF &rect) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream stream(&file);
        stream >> boardFile;
        if (stream.status() != QDataStream::Status::ReadCorruptData) {
            for (auto &square: boardFile.boardData.squares) {
                if(square.positionX < rect.left()) {
                    rect.setLeft(square.positionX);
                }
                if(square.positionX + 64> rect.right()) {
                    rect.setRight(square.positionX + 64);
                }
                if(square.positionY < rect.top()) {
                    rect.setTop(square.positionY);
                }
                if(square.positionY + 64 > rect.bottom()) {
                    rect.setBottom(square.positionY + 64);
                }
            }
        } else {
            goto badFile;
        }
    } else {
        badFile:
        QMessageBox::critical(this, "Error opening file", "An error occurred while trying to open the file " + filename);
    }
}

void ScreenshotDialog::makeScreenshot(const QString &filename, BoardFile &boardFile, const QRectF &rect) {
    scene->clearSquares();
    for (auto &square: boardFile.boardData.squares) {
        scene->addItem(new SquareItem(square));
    }
    QFileInfo info(filename);
    QString screenshotFilename = info.path() + "/" + info.baseName() + ".webp";
    scene->clearSelection();
    scene->setSceneRect(rect);
    QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    scene->render(&painter);
    image.save(screenshotFilename);
}

void ScreenshotDialog::accept() {
    QString filename;
    QString filename2;
    QString filename3;
    QString filename4;
    BoardFile boardFile;
    BoardFile boardFile2;
    BoardFile boardFile3;
    BoardFile boardFile4;
    QRectF rect;
    QVector<QString> filenames;
    if(ui->checkBox->isChecked()) {
        filename = ui->lineEdit->text();
        filenames += filename;
        readBoardFile(filename, boardFile, rect);
    }
    if(ui->checkBox_2->isChecked()) {
        filename2 = ui->lineEdit_2->text();
        filenames += filename2;
        readBoardFile(filename2, boardFile2, rect);
    }
    if(ui->checkBox_3->isChecked()) {
        filename3 = ui->lineEdit_3->text();
        filenames += filename3;
        readBoardFile(filename3, boardFile3, rect);
    }
    if(ui->checkBox_4->isChecked()) {
        filename4 = ui->lineEdit_4->text();
        filenames += filename4;
        readBoardFile(filename4, boardFile4, rect);
    }
    if(ui->checkBox->isChecked()) {
        makeScreenshot(filename, boardFile, rect);
    }
    if(ui->checkBox_2->isChecked()) {
        makeScreenshot(filename2, boardFile2, rect);
    }
    if(ui->checkBox_3->isChecked()) {
        makeScreenshot(filename3, boardFile3, rect);
    }
    if(ui->checkBox_4->isChecked()) {
        makeScreenshot(filename4, boardFile4, rect);
    }
    QMessageBox::information(this, "Screenshot", "Saved: \n" + filenames.toList().join('\n'));
    close();
}
