#include "region.h"
#include "qapplication.h"
#include "qdir.h"
#include "qsettings.h"
#include "qtranslator.h"

Region::Region(QObject *parent)
    : QObject{parent}
{}

Region& Region::instance() {
    static Region instance;
    return instance;
}

QStringList Region::availableProgramLanguages() const
{
    QDir dir(":/translations");

    QStringList translationFileNames = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);
    QStringList languagesAvailable;

    // then add the others before returning the list
    for (QString &fileName : translationFileNames){
        auto localeCode = QFileInfo(fileName).baseName();
        languagesAvailable.append(localeCode);
    }

    return languagesAvailable;
}

QString Region::currentSystemLanguage() const
{
    QLocale locale = QLocale();
    return locale.name();
}

bool Region::applyProgramLanguage(QString string)
{
    qApp->removeTranslator(&translator);

    qInfo() << QString("Installing %1").arg(string);

    QDir dir(":/translations");

    bool was_translation_successful = false;

    QFileInfo translationFile(dir.filePath(string + ".qm"));
    if(translationFile.exists() && translationFile.isFile()){
        was_translation_successful = translator.load(dir.filePath(string + ".qm"));
    }
    else{
        was_translation_successful = false;
    }

    if(was_translation_successful){
        qApp->installTranslator(&translator);
        qInfo() << QString("Translation was successful! %1 has been loaded.").arg(string);
    }
    else{
        qInfo() << "Translation was not successful.";
    }
    return was_translation_successful;
}

void Region::setProgramLanguage(QString string)
{
    QSettings settings;
    settings.setValue("programLanguage", string);
}

void Region::initializeRegionSettings()
{
    QSettings settings;

    if (!settings.contains("programLanguage")){
        // get the system language
        auto systemLanguageCode = currentSystemLanguage();
        auto languageCodesAvailable = availableProgramLanguages();

        if(languageCodesAvailable.contains(systemLanguageCode)){
            setProgramLanguage(systemLanguageCode);
        }
        else {
            // if we don't, set it to English and move on.
            setProgramLanguage("en_US");
        }
        applyProgramLanguage(settings.value("programLanguage").toString());
    }
    else {
        // if programLanguage is set, simply apply it.
        applyProgramLanguage(settings.value("programLanguage").toString());
    }
}
