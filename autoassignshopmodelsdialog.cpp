#include "autoassignshopmodelsdialog.h"
#include "ui_autoassignshopmodelsdialog.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QPushButton>
#include "fortunestreetdata.h"
#include "squareitem.h"

AutoAssignShopModelsDialog::AutoAssignShopModelsDialog(QWidget *parent, const QVector<SquareItem *> *squares) :
    QDialog(parent),
    ui(new Ui::AutoAssignShopModelsDialog),
    squares(squares)
{
    ui->setupUi(this);
    this->setWindowTitle("Auto Shop Model Assignment Options");

    connect(ui->allowNonVanilla, &QCheckBox::clicked, this, [&](bool checked) {
        ui->maxShopModelLabel->setEnabled(checked);
        ui->maxShopModelSpinBox->setEnabled(checked);
    });

    connect(ui->allowReadjustingShopValues, &QCheckBox::clicked, this, [&](bool checked) {
        ui->allowReadjustingShopPrices->setEnabled(checked);
    });
}

AutoAssignShopModelsDialog::~AutoAssignShopModelsDialog()
{
    delete ui;
}

void AutoAssignShopModelsDialog::accept() {
    bool allowNonVanilla = ui->allowNonVanilla->isChecked();
    bool preventDuplicates = ui->preventDuplicates->isChecked();
    bool randomizedAssign = ui->assignmentModeComboBox->currentText().contains("random", Qt::CaseInsensitive);
    int maxShopModel = 98;
    if (allowNonVanilla) {
        maxShopModel = ui->maxShopModelSpinBox->value();
    }
    bool allowReadjustShopValues = ui->allowReadjustingShopValues->isChecked();
    bool allowReadjustShopPrices = allowReadjustShopValues && ui->allowReadjustingShopPrices->isChecked();

    QVector<bool> usedShopModels;

    for (auto squareItem : *squares) {
        auto square = squareItem->getData();

    }

    QMessageBox::information(this, "Auto Shop Model Assignment", "Shop Models have been assigned.");
    close();
}
