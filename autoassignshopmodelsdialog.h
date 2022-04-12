#ifndef AUTOASSIGNSHOPMODELSDIALOG_H
#define AUTOASSIGNSHOPMODELSDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QGraphicsView>
#include <QLineEdit>
#include "fortuneavenuegraphicsscene.h"

namespace Ui { class AutoAssignShopModelsDialog; }

class AutoAssignShopModelsDialog : public QDialog
{
    Q_OBJECT

public:
    AutoAssignShopModelsDialog(QWidget *parent = nullptr, const QVector<SquareItem *> *squares = nullptr);
    ~AutoAssignShopModelsDialog();
    void accept();

private:
    Ui::AutoAssignShopModelsDialog *ui;
    const QVector<SquareItem *> *squares;
};

#endif // AUTOASSIGNSHOPMODELSDIALOG_H
