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
#include "mainwindow.h"

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
    setPaletteLabel();

    bool useHighlightColorSetting = settings.value("window_palette/use_highlight_colors", 1).toBool();
    ui->usePaletteHighlightColorCheckbox->setChecked(useHighlightColorSetting);

    QString localeCode = settings.value("programLanguage","").toString();
    QLocale locale(localeCode);

    rebuildLanguageComboBox();

    bool shouldUseAdvancedAutoPath = settings.value("use_advanced_auto_path", false).toBool();
    bool autoPathSelectedShouldAddEntryIdsToNearbySquares = settings.value("autopath_selected_should_add_entry_ids_to_nearby_squares", false).toBool();
    ui->useAdvancedAutoPathingSystemCheckbox->setChecked(shouldUseAdvancedAutoPath);
    ui->autoPathSelectedShouldAddEntryIdsToNearbySquaresCheckBox->setChecked(autoPathSelectedShouldAddEntryIdsToNearbySquares);

    int currentLanguageIndex = ui->languageComboBox->findText(locale.nativeLanguageName());
    if(currentLanguageIndex == -1){
        // if we didn't find it, we're likely set to English -- see the comments in the rebuildLanguageComboBox()
        // function for more details as to why this is necessary.
        currentLanguageIndex = ui->languageComboBox->findText(QLocale::languageToString(locale.language()));
    }
    if (currentLanguageIndex != -1) {
        ui->languageComboBox->setCurrentIndex(currentLanguageIndex);  // Select the item if found
    }

    connect(ui->usePaletteHighlightColorCheckbox, &QCheckBox::checkStateChanged, this, [this](bool value){
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

    connect(ui->useAdvancedAutoPathingSystemCheckbox, &QCheckBox::checkStateChanged, this, [this](bool value){
        toggleAdvancedAutoPath(value);
    });

    connect(ui->autoPathSelectedShouldAddEntryIdsToNearbySquaresCheckBox, &QCheckBox::checkStateChanged, this, [this](bool value){
        toggleAutoPathSelectedShouldAddEntryIdsToNearbySquares(value);
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

        QSettings settings;
        QString currentLanguageLocaleCode = settings.value("programLanguage","").toString();

        // creating the variables first, just in case
        QString name = "not set";
        QString englishName = "not set";
        QString category = "not set";

        if(rootObj.contains("name") && rootObj["name"].isObject()){
            // just being extra careful that the json is in the proper format
            QJsonObject nameObj = rootObj["name"].toObject();

            if(nameObj.contains(currentLanguageLocaleCode)){
                name = nameObj.value(currentLanguageLocaleCode).toString();
                englishName = nameObj.value("en_US").toString();
            }
            else{
                name = nameObj.value("en_US").toString();
                englishName = name;
            }
        }

        if(rootObj.contains("category") && rootObj["category"].isObject()){
            // just being extra careful that the json is in the proper format
            QJsonObject categoryObj = rootObj["category"].toObject();

            if(categoryObj.contains(currentLanguageLocaleCode)){
                category = categoryObj.value(currentLanguageLocaleCode).toString();
            }
            else{
                category = categoryObj.value("en_US").toString();
            }
        }

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
        // set the English name so we can set this value in Settings
        action->setData(englishName);

        connect(action, &QAction::triggered, this, &PreferencesDialog::paletteActionTriggered);
        submenus.value(category)->addAction(action);

        palette_files.insert(englishName, colors);
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
        QString paletteEnglishName = action->data().toString();

        // set the window palette label with the name of the new palette
        ui->windowPaletteLabel->setText(paletteName);

        // check whether or not to use highlight colors
        bool useHighlightColors = ui->usePaletteHighlightColorCheckbox->isChecked();

        // apply the palette
        setChosenPalette(palette_files.value(paletteEnglishName), useHighlightColors);

        // set the palette as chosen in QSettings
        saveUserWindowPalette(paletteEnglishName, palette_files.value(paletteEnglishName), useHighlightColors);
    }
}

QString PreferencesDialog::returnPaletteNameInCurrentLanguage(QString currentLocaleCode, QString englishPaletteName){
    // get the list of JSON palette files
    QString palettePath = ":/palettes/";
    QDir paletteDir = palettePath;
    QStringList paletteFiles = paletteDir.entryList(QStringList() << "*.json", QDir::Files);

    // return the english palette name if we cannot otherwise find it;
    // if we can find it we'll overwrite it in the loop below.
    QString nameInCurrentLanguage = englishPaletteName;

    // iterate over each JSON file
    for (const QString& jsonFile : paletteFiles) {
        QJsonDocument doc = readJsonFile(palettePath, jsonFile);

        // grab the name and category and whatever other data from these palettes
        QJsonObject rootObj = doc.object();

        if(rootObj.contains("name") && rootObj["name"].isObject()){
            // just being extra careful that the json is in the proper format
            QJsonObject nameObj = rootObj["name"].toObject();

            if(englishPaletteName == nameObj.value("en_US").toString()){
                if(nameObj.contains(currentLocaleCode)){
                    nameInCurrentLanguage = nameObj.value(currentLocaleCode).toString();
                }
            }
        }
    }
    return nameInCurrentLanguage;
}

void PreferencesDialog::setPaletteLabel(){
    QSettings settings;
    QString localeCode = settings.value("programLanguage","").toString();
    QString englishPaletteName = settings.value("window_palette/name", "not set").toString();

    // if language is not english, set the palette name in the appropriate language
    if(localeCode == "en_US"){
        ui->windowPaletteLabel->setText(englishPaletteName);
    }
    else{
        ui->windowPaletteLabel->setText(returnPaletteNameInCurrentLanguage(localeCode, englishPaletteName));
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

void PreferencesDialog::toggleAdvancedAutoPath(int status)
{
    bool boolStatus = status;
    QSettings settings;
    settings.setValue("use_advanced_auto_path", boolStatus);
    emit advancedAutoPathChanged();
}

void PreferencesDialog::toggleAutoPathSelectedShouldAddEntryIdsToNearbySquares(int status)
{
    bool boolStatus = status;
    QSettings settings;
    settings.setValue("autopath_selected_should_add_entry_ids_to_nearby_squares", boolStatus);
}

void PreferencesDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        qInfo() << "A PreferencesDialog::changeEvent() has fired!";

        ui->retranslateUi(this);
        setPaletteLabel();
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
