#ifndef DARKDETECT_H
#define DARKDETECT_H

#include <QApplication>
#include <QPalette>
#include <QSettings>
#include <QStyleFactory>

enum Theme {
    amethyst,
    chocolate,
    citrine,
    classic_light,
    classic_dark,
    desert,
    forest,
    kiwi,
    midnight,
    ruby,
    snowfall,
    strawberry
};

inline void applyTheme(QPalette palette){
    qApp->setStyle(QStyleFactory::create("Fusion"));
    qApp->setPalette(palette);
    qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
}

inline void setTheme(Theme style) {
    switch(style){
    case amethyst: {
        QPalette palette;
        QColor outside_color = QColor("#20112f");
        QColor canvas_color = QColor("#160725");
        QColor highlight_color = QColor("#9381FF");
        QColor alternate_base_color = QColor(18,18,18);
        QColor disabled_color = QColor(30,0,0);
        QColor text_color = Qt::white;
        QColor highlighted_text_color = Qt::black;
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, text_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case chocolate:
    {
        QPalette palette;
        QColor outside_color = QColor("#362001");
        QColor canvas_color = QColor("#2c0f01");
        QColor highlight_color = QColor("#934501");
        QColor alternate_base_color = QColor(18,18,18);
        QColor disabled_color = QColor(30,0,0);
        QColor text_color = Qt::white;
        QColor highlighted_text_color = Qt::black;
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, text_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case citrine:
    {
        QPalette palette;
        QColor outside_color = QColor("#e3843e");
        QColor canvas_color = QColor("#804E2C");
        QColor highlight_color = QColor("#D0AE8E");
        QColor alternate_base_color = QColor(18,18,18);
        QColor disabled_color = QColor(30,0,0);
        QColor text_color = Qt::black;
        QColor highlighted_text_color = Qt::black;
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, text_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case classic_light: {
        QPalette palette;
        QColor outside_color = QColor("#c3c6c8");
        QColor canvas_color = QColor("#9598a6");
        QColor accent_highlight_color = QColor("#172b89");
        QColor alternate_base_color = QColor("#111111");
        QColor disabled_color = QColor("#eeeeee");
        QColor main_text_color = QColor("#111111");
        QColor accent_text_color = QColor("#111111");
        QColor highlighted_text_color = QColor("#eeeeee");
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, main_text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, accent_text_color);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, main_text_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, accent_highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case classic_dark:
    {
        QPalette palette;
        QColor outside_color = QColor("#2d2d2d");
        QColor canvas_color = QColor("#121212");
        QColor accent_highlight_color = QColor("#d31779");
        QColor alternate_base_color = QColor("#111111");;
        QColor disabled_color = QColor("#111111");
        QColor main_text_color = QColor("#eeeeee");
        QColor accent_text_color = QColor("#eeeeee");
        QColor highlighted_text_color = QColor("#eeeeee");

        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, main_text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, main_text_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, accent_highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case desert:
    {
        QPalette palette;
        QColor outside_color = QColor("#ECC23E");
        QColor canvas_color = QColor("#66583d");
        QColor highlight_color = QColor("#DBC2B1");
        QColor alternate_base_color = QColor(18,18,18);
        QColor disabled_color = QColor(30,0,0);
        QColor text_color = Qt::black;
        QColor highlighted_text_color = Qt::black;
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, Qt::black);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case forest:
    {
        QPalette palette;
        QColor outside_color = QColor("#0F3E02");
        QColor canvas_color = QColor("#062501");
        QColor highlight_color = QColor("#499135");
        QColor text_color = Qt::white;
        QColor highlighted_text_color = QColor("#062501");
        QColor disabled_color = QColor(30,0,0);
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, QColor(30,0,0));
        palette.setColor(QPalette::ToolTipBase, Qt::white);
        palette.setColor(QPalette::ToolTipText, Qt::white);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, highlight_color);
        palette.setColor(QPalette::HighlightedText, Qt::black);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case kiwi: {
        QPalette palette;
        QColor outside_color = QColor("#62963D");
        QColor canvas_color = QColor("#44712F");
        QColor accent_highlight_color = QColor("#87d15d");
        QColor alternate_base_color = QColor(18,18,18);
        QColor disabled_color = QColor(30,0,0);
        QColor main_text_color = Qt::black;
        QColor accent_text_color = Qt::white;
        QColor highlighted_text_color = Qt::black;
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, main_text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, accent_text_color);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, main_text_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, accent_highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case midnight: {
        QPalette palette;
        QColor outside_color = QColor("#0B163B");
        QColor canvas_color = QColor("#08001E");
        QColor highlight_color = QColor("#acbdd8");
        QColor alternate_base_color = QColor(18,18,18);
        QColor disabled_color = QColor(30,0,0);
        QColor text_color = Qt::white;
        QColor highlighted_text_color = Qt::black;
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, text_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case ruby:
    {
        QPalette palette;
        QColor outside_color = QColor("#320000");
        QColor canvas_color = QColor("#1e0000");
        QColor highlight_color = QColor("#ff6060");
        QColor alternate_base_color = QColor(18,18,18);
        QColor disabled_color = QColor(30,0,0);
        QColor text_color = Qt::white;
        QColor highlighted_text_color = Qt::black;
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, text_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case snowfall:
    {
        QPalette palette;
        QColor outside_color = QColor("#60a7bf");
        QColor canvas_color = QColor("#4d638c");
        QColor highlight_color = QColor("#809ed5");
        QColor alternate_base_color = QColor(18,18,18);
        QColor accent_text_color = Qt::black;
        QColor disabled_color = QColor(30,0,0);
        QColor text_color = Qt::black;
        QColor highlighted_text_color = Qt::black;
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, Qt::white);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, Qt::black);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
    case strawberry:
    {
        QPalette palette;
        QColor outside_color = QColor("#F4ACB7");
        QColor canvas_color = QColor("#9D8189");
        QColor accent_highlight_color = QColor("#FF5AB8");
        QColor alternate_base_color = QColor(18,18,18);
        QColor disabled_color = QColor(30,0,0);
        QColor main_text_color = Qt::black;
        QColor accent_text_color = Qt::black;
        QColor highlighted_text_color = Qt::black;
        palette.setColor(QPalette::Window, outside_color);
        palette.setColor(QPalette::WindowText, main_text_color);
        palette.setColor(QPalette::Base, canvas_color);
        palette.setColor(QPalette::AlternateBase, alternate_base_color);
        palette.setColor(QPalette::ToolTipBase, Qt::black);
        palette.setColor(QPalette::ToolTipText, Qt::black);
        palette.setColor(QPalette::Text, accent_text_color);
        palette.setColor(QPalette::Disabled, QPalette::Text, disabled_color);
        palette.setColor(QPalette::Button, outside_color);
        palette.setColor(QPalette::ButtonText, main_text_color);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabled_color);
        palette.setColor(QPalette::BrightText, Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, accent_highlight_color);
        palette.setColor(QPalette::HighlightedText, highlighted_text_color);
        palette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabled_color);
        applyTheme(palette);
        break;
    }
}
}
inline void initDarkThemeSettings() {
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",QSettings::NativeFormat);
    if(settings.value("AppsUseLightTheme")==0){
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        QColor darkColor = QColor(45,45,45);
        QColor disabledColor = QColor(127,127,127);
        darkPalette.setColor(QPalette::Window, darkColor);
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(18,18,18));
        darkPalette.setColor(QPalette::AlternateBase, darkColor);
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
        darkPalette.setColor(QPalette::Button, darkColor);
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));

        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

        qApp->setPalette(darkPalette);

        qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");
    }
#else
    QPalette darkPalette;
    QColor darkColor = QColor(45,45,45);
    QColor darkDisabledColor = QColor(127,127,127);
    darkPalette.setColor(QPalette::Window, darkColor);
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(18,18,18));
    darkPalette.setColor(QPalette::AlternateBase, darkColor);
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, darkDisabledColor);
    darkPalette.setColor(QPalette::Button, darkColor);
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, darkDisabledColor);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, darkDisabledColor);
    applyTheme(darkPalette);
#endif
}

inline bool isDarkMode() {
    return QApplication::palette().color(QPalette::Base).lightnessF() < 0.5;
}

#endif // DARKDETECT_H
