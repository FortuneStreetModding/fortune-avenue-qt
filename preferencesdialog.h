#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

private:
    Ui::PreferencesDialog *ui;
    void buildPaletteMenu();
    void paletteActionTriggered();
    void rebuildLanguageComboBox();
private Q_SLOTS:
    void usePaletteHighlightColorCheckboxStatusChanged(int status);
protected:
    void changeEvent(QEvent *event) override;
};

#endif // PREFERENCESDIALOG_H
