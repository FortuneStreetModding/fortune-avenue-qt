#ifndef FORTUNEAVENUEAPP_H
#define FORTUNEAVENUEAPP_H

#include <QApplication>

class FortuneAvenueApp : virtual public QApplication
{
public:
    FortuneAvenueApp(int argc, char **argv);
    bool event(QEvent *event) override;
};

#endif // FORTUNEAVENUEAPP_H
