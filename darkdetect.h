#ifndef DARKDETECT_H
#define DARKDETECT_H

#include <QApplication>
#include <QPalette>

bool isDarkMode() {
    return QApplication::palette().color(QPalette::Base).lightnessF() < 0.5;
}

#endif // DARKDETECT_H
