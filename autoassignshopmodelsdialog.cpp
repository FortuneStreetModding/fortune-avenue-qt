#include "autoassignshopmodelsdialog.h"
#include "ui_autoassignshopmodelsdialog.h"

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QPushButton>
#include <QQueue>
#include <QRandomGenerator>
#include "fortunestreetdata.h"
#include "squareitem.h"
#include "hungarian.h"
#include "muParser.h"

AutoAssignShopModelsDialog::AutoAssignShopModelsDialog(QWidget *parent, QVector<SquareItem *> *squares, QString autoCalcFunction) :
    QDialog(parent),
    ui(new Ui::AutoAssignShopModelsDialog),
    squares(squares),
    autoCalcFunction(autoCalcFunction)
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

int AutoAssignShopModelsDialog::calcShopPriceFromValue(int value) {
    double x = (qreal) value;
    mu::Parser p;
    p.DefineVar(QString("x").toStdWString(), &x);
    p.SetExpr(autoCalcFunction.toStdWString());
    int price = qRound(p.Eval());
    if(price < 0)
        return 0;
    return price;
}

bool isVanillaShopModel(int shopModel) {
    if(shopTypeToText(shopModel) == "Unused")
        return false;
    return true;
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

    double cost = 0;
    if (preventDuplicates) {
        // we need to use Hungarian algorithm for weighted bipartite matching where the weight is modeled as cost
        // to avoid duplicates
        // cost = |value - model*10|
        vector< vector<double> > costMatrix;
        costMatrix.resize(squares->size());
        for(int i=0;i<squares->size();i++) {
            costMatrix[i].resize(maxShopModel + 1);
            SquareItem *squareItem = (SquareItem *)squares->at(i);
            if(squareItem->getData().squareType == Property) {
                for(int j=0;j<=maxShopModel;j++) {
                    cost = qAbs(squareItem->getData().value - j*10);
                    if(randomizedAssign) {
                        cost = QRandomGenerator::global()->bounded(10, (maxShopModel+1)*10);
                    }
                    if((!allowNonVanilla && !isVanillaShopModel(j)) || j == 0) {
                        cost = 99999999999;
                    }
                    costMatrix[i][j] = cost;
                }
            }
        }
        HungarianAlgorithm HungAlgo;
        vector<int> assignment;
        cost = HungAlgo.Solve(costMatrix, assignment);
        for (unsigned int i = 0; i < costMatrix.size(); i++) {
            SquareItem *squareItem = (SquareItem *)squares->at(i);
            squareItem->getData().shopModel = assignment[i];
        }
    } else {
        for(int i=0;i<squares->size();i++) {
            SquareItem *squareItem = (SquareItem *)squares->at(i);
            if(squareItem->getData().squareType == Property) {
                if(randomizedAssign) {
                    do {
                        squareItem->getData().shopModel = QRandomGenerator::global()->bounded(1, maxShopModel+1)*10;
                    } while(!allowNonVanilla && !isVanillaShopModel(squareItem->getData().shopModel));
                } else {
                    int shopValue = squareItem->getData().value;
                    int newShopModel = qRound(shopValue/10.0);
                    for(int i=0;i<1000;i++) {
                        if(!allowNonVanilla && !isVanillaShopModel(newShopModel)) {
                            newShopModel = qRound((shopValue+i)/10.0);
                        } else {
                            break;
                        }
                        if(!allowNonVanilla && !isVanillaShopModel(newShopModel)) {
                            newShopModel = qRound((shopValue-i)/10.0);
                        } else {
                            break;
                        }
                    }
                    squareItem->getData().shopModel = newShopModel;
                }
            }
        }
    }

    for(int i=0;i<squares->size();i++) {
        SquareItem *squareItem = (SquareItem *)squares->at(i);
        if(allowReadjustShopValues) {
            squareItem->getData().updateValueFromShopModel();
            if(allowReadjustShopPrices) {
                try {
                    squareItem->getData().price = calcShopPriceFromValue(squareItem->getData().value);
                } catch (mu::Parser::exception_type &e) {}
            }
        }
        if(squareItem->getData().squareType != Property) {
            squareItem->getData().shopModel = 0;
        }
    }

    QStringList builder;
    builder << "Shop Models have been assigned.";
    if(preventDuplicates && !randomizedAssign) {
        builder << QString("Absolute Error: %1").arg(QString::number(cost));
    }
    QMessageBox::information(this, "Auto Shop Model Assignment", builder.join("\n"));
    close();
}
