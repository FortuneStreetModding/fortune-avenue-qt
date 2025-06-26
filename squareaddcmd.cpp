#include "squareaddcmd.h"



SquareAddCmd::SquareAddCmd(FortuneAvenueGraphicsScene *scene, const std::function<void (SquareItem *)> &updateFn) : scene(scene), updateFn(updateFn)
{
    setText(FortuneAvenueGraphicsScene::tr("Add Square"));
}

void SquareAddCmd::undo()
{
    auto sqItems = scene->squareItems();
    scene->removeItem(sqItems.back());
    delete sqItems.back();
    scene->update();
    updateFn(nullptr);
}

void SquareAddCmd::redo()
{
    auto item = new SquareItem(SquareData(scene->squareItems().size() /* add next index */));
    scene->addItem(item);
    scene->update();
    updateFn(item);
}
