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
#include <QStyleFactory>
#include <QSettings>
#include "usersettings.h"

PreferencesDialog::PreferencesDialog(QWidget *parent)
: QDialog(parent)
    , ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setWindowTitle("Preferences");

    ui->windowPaletteToolButton->setMenu(new QMenu);
    connect(ui->windowPaletteToolButton, &QToolButton::clicked, this, &PreferencesDialog::buildPaletteMenu);

    QSettings settings;
    ui->windowPaletteLabel->setText(settings.value("window_palette/name", "not set").toString());
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

// The key is the palette name, and the value is a QJsonObject representing the colors of the palette
QMap<QString, QJsonObject> palette_files;

void PreferencesDialog::buildPaletteMenu()
{
    ui->windowPaletteToolButton->menu()->clear();

    // get the list of JSON palette files
    QString palettePath = ":/palettes/";
    QDir paletteDir = palettePath;
    QStringList paletteFiles = paletteDir.entryList(QStringList() << "*.json", QDir::Files);

    // having references to the categories encountered and submenus created
    // will be helpful when adding palettes to those category submenus
    QStringList categories;
    QMap<QString, QMenu*> submenus;

    // iterate over each JSON file
    for (const QString& jsonFile : paletteFiles) {
        QJsonDocument doc = readJsonFile(palettePath, jsonFile);

        // grab the name and category and whatever other data from these palettes
        QJsonObject rootObj = doc.object();
        QString name = rootObj.value("name").toString();
        QString category = rootObj.value("category").toString();

        // this is what we'll pass to setChosenPalette
        QJsonObject colors = rootObj.value("colors").toObject();

        // ...so that function can use lines like this.
        // QString outside_color = colors.value("outside_color").toString();

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

        palette_files.insert(name, colors);
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
        setChosenPalette(palette_files.value(paletteName));

        // set the palette as chosen in QSettings
        saveUserWindowPalette(paletteName, palette_files.value(paletteName));
    }
}
