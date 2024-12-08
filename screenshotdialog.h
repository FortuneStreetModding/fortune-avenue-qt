#ifndef SCREENSHOTDIALOG_H
#define SCREENSHOTDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QGraphicsView>
#include <QLineEdit>
#include "fortuneavenuegraphicsscene.h"

namespace Ui { class ScreenshotDialog; }

class ScreenshotDialog : public QDialog
{
    Q_OBJECT

public:
    ScreenshotDialog(const QString &frbFilename, QWidget *parent = nullptr);
    ~ScreenshotDialog();
    void accept();
    void browseFrb();
    void removeFrb();
private:
    Ui::ScreenshotDialog *ui;
    FortuneAvenueGraphicsScene *scene;
    BoardFile readBoardFile(const QString &filename, QRectF &rect);
    bool makeScreenshot(const QString &filename, BoardFile &boardFile, const QRectF &rect);
    QList<bool> takeAllScreenshots(int count, QStringList filenames, QList<BoardFile> boardFiles, QRectF rect);
};

#endif // SCREENSHOTDIALOG_H
