#include "preferencesdialog.h"
#include "qdir.h"
#include "ui_preferencesdialog.h"
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

PreferencesDialog::PreferencesDialog(QWidget *parent)
: QDialog(parent)
    , ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    ui->windowPaletteToolButton->setMenu(new QMenu);
    connect(ui->windowPaletteToolButton, &QToolButton::clicked, this, &PreferencesDialog::buildPaletteMenu);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::buildPaletteMenu()
{
    ui->windowPaletteToolButton->menu()->clear();

    // get the list of JSON palette files
    QString palettePath = "/palettes/";
    QString rootDir = qApp->applicationDirPath();
    QDir paletteDir = rootDir + palettePath;
    QStringList paletteFiles = paletteDir.entryList(QStringList() << "*.json", QDir::Files);

    // having references to the categories encountered and submenus created
    // will be helpful when adding palettes to those category submenus
    QStringList categories;
    QMap<QString, QMenu*> submenus;

    // iterate over each JSON file
    for (const QString& jsonFile : paletteFiles) {
        QFile file(rootDir + palettePath + jsonFile);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open file:" << rootDir + palettePath + jsonFile;
            continue;
        }

        // read JSON data from file
        QByteArray jsonData = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isNull()) {
            qDebug() << "Failed to create JSON document from data";
            file.close();
            continue;
        }

        // grab the name and category and whatever other data from these palettes
        QJsonObject rootObj = doc.object();
        QString name = rootObj.value("name").toString();
        QString category = rootObj.value("category").toString();

        // build a submenu for the category if it does not already exist
        if(!categories.contains(category))
        {
            categories.append(category);
            submenus.insert(category, ui->windowPaletteToolButton->menu()->addMenu(category));
        }

        // add the palette as an action in the submenu of its category
        QAction *action = new QAction(name, this);
            connect(action, &QAction::triggered, this, &PreferencesDialog::paletteActionTriggered);
        submenus.value(category)->addAction(action);

        file.close();
    }

    // finally show the menu after building it, so we don't require
    // them to click the tiny triangle on the side of the button
    ui->windowPaletteToolButton->showMenu();
}

void PreferencesDialog::paletteActionTriggered()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        QString paletteName = action->text();
        // do stuff with the selected palette name
        ui->windowPaletteLabel->setText(paletteName);
        // apply the palette
        // set the palette as chosen in QSettings
    }
}
