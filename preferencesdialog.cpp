#include "preferencesdialog.h"
#include "qcombobox.h"
#include "qdir.h"
#include "region.h"
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
    this->resize(100,100);
    setWindowTitle(tr("Preferences"));

    ui->windowPaletteToolButton->setMenu(new QMenu);
    connect(ui->windowPaletteToolButton, &QToolButton::clicked, this, &PreferencesDialog::buildPaletteMenu);

    QSettings settings;
    ui->windowPaletteLabel->setText(settings.value("window_palette/name", "not set").toString());

    bool useHighlightColorSetting = settings.value("window_palette/use_highlight_colors", 1).toBool();
    ui->usePaletteHighlightColorCheckbox->setChecked(useHighlightColorSetting);

    QString localeCode = settings.value("programLanguage","").toString();
    QLocale locale(localeCode);

    rebuildLanguageComboBox();

    int currentLanguageIndex = ui->languageComboBox->findText(locale.nativeLanguageName());
    if(currentLanguageIndex == -1){
        // if we didn't find it, we're likely set to English -- see the comments in the rebuildLanguageComboBox()
        // function for more details as to why this is necessary.
        currentLanguageIndex = ui->languageComboBox->findText(QLocale::languageToString(locale.language()));
    }
    if (currentLanguageIndex != -1) {
        ui->languageComboBox->setCurrentIndex(currentLanguageIndex);  // Select the item if found
    }

    connect(ui->usePaletteHighlightColorCheckbox, &QCheckBox::stateChanged, this, [this](bool value){
        usePaletteHighlightColorCheckboxStatusChanged(value);
    });

    connect(ui->languageComboBox, &QComboBox::currentTextChanged, this, [this](QString text){
        auto selectedLanguage = ui->languageComboBox->currentData().toString(); // this should be the locale code
        qInfo() << "saving locale code to settings: " + selectedLanguage;
        Region::instance().setProgramLanguage(selectedLanguage);
        Region::instance().applyProgramLanguage(selectedLanguage);

        QLocale locale(selectedLanguage);

        qInfo() << QString("language changed to %1").arg(QLocale::languageToString(locale.language()));
    });
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
        // QString window = colors.value("window").toString();

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

        // check whether or not to use highlight colors
        bool useHighlightColors = ui->usePaletteHighlightColorCheckbox->isChecked();

        // apply the palette
        setChosenPalette(palette_files.value(paletteName), useHighlightColors);

        // set the palette as chosen in QSettings
        saveUserWindowPalette(paletteName, palette_files.value(paletteName), useHighlightColors);
    }
}

void PreferencesDialog::usePaletteHighlightColorCheckboxStatusChanged(int status)
{
    bool useHighlightColors = status;
    // if false, we're disabling the use of the palette's highlight color and highlight text color entries.
    // if true, we're enabling their use.
    QJsonObject palette = getSavedUserWindowPalette();
    setChosenPalette(palette, useHighlightColors);
    saveUserWindowPalette(palette.value("name").toString(), palette, useHighlightColors);
}

void PreferencesDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        qInfo() << "A PreferencesDialog::changeEvent() has fired!";
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void PreferencesDialog::rebuildLanguageComboBox(){
    QComboBox* combobox = ui->languageComboBox;

    auto index = combobox->currentIndex();
    auto languages = Region::instance().availableProgramLanguages();

    qInfo() << languages;

    combobox->blockSignals(true);
    combobox->clear();

    for(auto &l: languages){
        QLocale locale(l);

        qInfo() << "Native: " + locale.nativeLanguageName();
        qInfo() << "Generic: " + QLocale::languageToString(locale.language());
        qInfo() << "l: " + l;

        // if English, we want to populate the comboBox with only "English", rather
        // than regional variants like "American English" or "British English", because
        // for the sake of our program, there is no difference between regions.

        if(l == "en_US"){
            combobox->addItem(QLocale::languageToString(locale.language()), l);
        }
        else {
            // for all other languages, we want to simply populate with the native
            // language name.
            combobox->addItem(locale.nativeLanguageName(), l);
        }
    }

    combobox->setCurrentIndex(index);
    combobox->blockSignals(false);
}
