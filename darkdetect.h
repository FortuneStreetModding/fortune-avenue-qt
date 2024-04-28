#ifndef DARKDETECT_H
#define DARKDETECT_H
#include "usersettings.h"

#include <QApplication>
#include <QGuiApplication>
#include <QJsonObject>
#include <QPalette>
#include <QSettings>
#include <QStyleFactory>

inline void initWindowPaletteSettings(Qt::ColorScheme scheme) {
    QSettings settings;
    QString saved_palette = settings.value("window_palette/name", "not set").toString();
    if(saved_palette == "not set"){
        // if a palette is not set, try to match the OS setting.

        // we bake these two palettes in to ensure the system works
        // even if the user never interacts with the theme system,
        // or the palette files are not present, or something like that.
        QJsonObject light_palette;
        light_palette.insert("name", "Classic Light");
        light_palette.insert("window", "#c3c6c8");
        light_palette.insert("window_text", "#111111");
        light_palette.insert("base", "#9598a6");
        light_palette.insert("alternate_base", "#111111");
        light_palette.insert("text", "#111111");
        light_palette.insert("button", "#c3c6c8");
        light_palette.insert("button_text", "#111111");
        light_palette.insert("bright_text", "#111111");
        light_palette.insert("link", "#2a82da");
        light_palette.insert("highlight", "#172b89");
        light_palette.insert("highlighted_text", "#111111");
        light_palette.insert("tooltip_base", "#000000");
        light_palette.insert("tooltip_text", "#111111");
        light_palette.insert("disabled_button", "#aaaaaa");
        light_palette.insert("disabled_highlighted_text", "#aaaaaa");
        light_palette.insert("disabled_text", "#aaaaaa");

        QJsonObject dark_palette;
        dark_palette.insert("name", "Classic Dark");
        dark_palette.insert("window", "#2d2d2d");
        dark_palette.insert("window_text", "#eeeeee");
        dark_palette.insert("base", "#121212");
        dark_palette.insert("alternate_base", "＃111111");
        dark_palette.insert("text", "#eeeeee");
        dark_palette.insert("button", "#2d2d2d");
        dark_palette.insert("button_text", "#eeeeee");
        dark_palette.insert("bright_text", "#eeeeee");
        dark_palette.insert("link", "#2a82da");
        dark_palette.insert("highlight", "#d31779");
        dark_palette.insert("highlighted_text", "#eeeeee");
        dark_palette.insert("tooltip_base", "#000000");
        dark_palette.insert("tooltip_text", "#eeeeee");
        dark_palette.insert("disabled_button", "#555555");
        dark_palette.insert("disabled_highlighted_text", "#555555");
        dark_palette.insert("disabled_text", "#555555");

        if(scheme == Qt::ColorScheme::Light){
            setChosenPalette(light_palette);
            saveUserWindowPalette("Classic Light", light_palette);
        }
        else if(scheme == Qt::ColorScheme::Dark){
            setChosenPalette(dark_palette);
            saveUserWindowPalette("Classic Dark", dark_palette);
        }
        else{
            // if it's unknown, just set Classic Dark
            setChosenPalette(dark_palette);
            saveUserWindowPalette("Classic Dark", dark_palette);
        }
    }
    else{
        // if a palette is set
        setChosenPalette(getSavedUserWindowPalette());
    }
}


inline bool isDarkMode() {
    return QApplication::palette().color(QPalette::Base).lightnessF() < 0.5;
}

#endif // DARKDETECT_H
