#include "squarechangecmd.h"


SquareChangeCmd::SquareChangeCmd(FortuneAvenueGraphicsScene *scene, const QMap<int, SquareData> &oldData, const QMap<int, SquareData> &newData, const std::function<void ()> &updateFn)
    : scene(scene), oldData(oldData), newData(newData), updateFn(updateFn)
{
    setText("Change Square Data");
}

void SquareChangeCmd::undo()
{
    auto sqItems = scene->squareItems();
    for (auto &item: sqItems) {
        item->getData() = oldData[item->getData().id];
        item->update();
    }
    updateFn();
}

void SquareChangeCmd::redo()
{
    auto sqItems = scene->squareItems();
    for (auto &item: sqItems) {
        item->getData() = newData[item->getData().id];
        item->update();
    }
    updateFn();
}
