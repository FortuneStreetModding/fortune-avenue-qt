#ifndef AUTOASSIGNSHOPMODELSDIALOG_H
#define AUTOASSIGNSHOPMODELSDIALOG_H

#include "squareitem.h"
#include <QDialog>
#include <QCheckBox>
#include <QGraphicsView>
#include <QLineEdit>

namespace Ui { class AutoAssignShopModelsDialog; }

class AutoAssignShopModelsDialog : public QDialog
{
    Q_OBJECT

public:
    AutoAssignShopModelsDialog(QWidget *parent = nullptr, QVector<SquareItem *> *squares = nullptr, QString autoCalcFunction = "");
    ~AutoAssignShopModelsDialog();
    void accept();

private:
    Ui::AutoAssignShopModelsDialog *ui;
    QVector<SquareItem *> *squares;
    QString autoCalcFunction;
    int calcShopPriceFromValue(int value);
};

#endif // AUTOASSIGNSHOPMODELSDIALOG_H
