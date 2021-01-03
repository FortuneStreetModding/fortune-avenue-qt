#ifndef UTIL_H
#define UTIL_H

#include <QAbstractButton>

QString addShortcutToText(QString text, QKeySequence shortcut) {
    return QString("%1 %2").arg(text).arg(shortcut.toString(QKeySequence::NativeText));
}

void addShortcutTextToButton(QAbstractButton *button) {
    auto shortcut = button->shortcut();
    button->setText(addShortcutToText(button->text(), button->shortcut()));
    button->setShortcut(shortcut);
}

#endif // UTIL_H
