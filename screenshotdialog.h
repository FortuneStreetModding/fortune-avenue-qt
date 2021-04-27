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
    void browseFrb(QLineEdit *lineEdit, QCheckBox *checkBox);
private:
    Ui::ScreenshotDialog *ui;
    FortuneAvenueGraphicsScene *scene;
    void readBoardFile(const QString &filename, BoardFile &boardFile, QRectF &rect);
    void makeScreenshot(const QString &filename, BoardFile &boardFile, const QRectF &rect);
};

#endif // SCREENSHOTDIALOG_H
