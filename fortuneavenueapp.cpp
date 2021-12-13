#include "fortuneavenueapp.h"

#include <QWindow>

FortuneAvenueApp::FortuneAvenueApp(int argc, char **argv) : QApplication(argc, argv)
{

}


bool FortuneAvenueApp::event(QEvent *event) {
    if (event && event->type() == QEvent::FileOpen) {
        //QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
        auto windows = topLevelWindows();
        for (auto window: qAsConst(windows)) {
            sendEvent(window, event);
        }
    }
    return QApplication::event(event);
}
