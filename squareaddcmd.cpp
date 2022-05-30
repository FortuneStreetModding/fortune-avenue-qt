#include "squareaddcmd.h"



SquareAddCmd::SquareAddCmd(FortuneAvenueGraphicsScene *scene) : scene(scene)
{
}

void SquareAddCmd::undo()
{
    auto sqItems = scene->squareItems();
    scene->removeItem(sqItems.back());
    delete sqItems.back();
    scene->update();
}

void SquareAddCmd::redo()
{
    scene->addItem(new SquareItem(SquareData(scene->squareItems().size() /* add next index */)));
    scene->update();
}
