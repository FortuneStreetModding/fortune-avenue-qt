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

void setChosenPalette(QJsonObject colors){
    // read the colors from the JSON object
    QColor alternate_base_color = colors.value("alternate_base_color").toString();
    QColor bright_text_color = colors.value("bright_text_color").toString();
    QColor canvas_color = colors.value("canvas_color").toString();
    QColor disabled_color = colors.value("disabled_color").toString();
    QColor highlight_color = colors.value("accent_highlight_color").toString();
    QColor highlighted_text_color = colors.value("highlighted_text_color").toString();
    QColor link_color = colors.value("link_color").toString();
    QColor outside_color = colors.value("outside_color").toString();
    QColor text_color = colors.value("main_text_color").toString();
    QColor tooltip_base = colors.value("tooltip_base").toString();
    QColor tooltip_text = colors.value("tooltip_text").toString();

    // now create the palette
    QPalette palette;
    palette.setColor(QPalette::Window, outside_color);
    palette.setColor(QPalette::WindowText, text_color);
    palette.setColor(QPalette::Base, canvas_color);
    palette.setColor(QPalette::AlternateBase, alternate_base_color);
    palette.setColor(QPalette::Text, text_color);

    palette.setColor(QPalette::Button, outside_color);
    palette.setColor(QPalette::ButtonText, text_color);
    palette.setColor(QPalette::BrightText, bright_text_color);
    palette.setColor(QPalette::Link, link_color);
    palette.setColor(QPalette::Highlight, highlight_color);
    palette.setColor(QPalette::HighlightedText, highlighted_text_color);
    palette.setColor(QPalette::ToolTipBase, tooltip_base);
    palette.setColor(QPalette::ToolTipText, tooltip_text);

    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
    palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);

    // apply the palette immediately
    qApp->setStyle(QStyleFactory::create("Fusion"));
    qApp->setPalette(palette);
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
}

// The key is the palette name, and the value is a QJsonObject representing the colors of the palette
QMap<QString, QJsonObject> palette_files;

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

        file.close();

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

        // TODO: set the palette as chosen in QSettings
    }
}
