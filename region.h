#ifndef REGION_H
#define REGION_H

#include "qtranslator.h"
#include <QObject>

class Region : public QObject
{
    Q_OBJECT
public:
    static Region& instance();

    bool applyProgramLanguage(const QString string);
    QStringList availableProgramLanguages() const;
    QString currentSystemLanguage() const;
    void initializeRegionSettings();
    void setProgramLanguage(const QString string);

private:
    explicit Region(QObject *parent = nullptr);
    QTranslator translator;

signals:
};

#endif // REGION_H
