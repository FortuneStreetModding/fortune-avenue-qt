#include "squarechangecmd.h"


SquareChangeCmd::SquareChangeCmd(FortuneAvenueGraphicsScene *scene, const QMap<int, SquareData> &oldData, const QMap<int, SquareData> &newData, const QString &text, const std::function<void ()> &updateFn)
    : scene(scene), oldData(oldData), newData(newData), updateFn(updateFn)
{
    setText(text);
}

void SquareChangeCmd::undo()
{
    auto sqItems = scene->squareItems();
    for (auto it = oldData.begin(); it != oldData.end(); ++it) {
        auto item = sqItems[it.key()];
        item->getData() = it.value();
        item->update();
    }
    updateFn();
}

void SquareChangeCmd::redo()
{
    auto sqItems = scene->squareItems();
    for (auto it = newData.begin(); it != newData.end(); ++it) {
        auto item = sqItems[it.key()];
        item->getData() = it.value();
        item->update();
    }
    updateFn();
}
