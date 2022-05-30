#include "squaremovecmd.h"


SquareMoveCmd::SquareMoveCmd(FortuneAvenueGraphicsScene *scene, int squareId, const QPointF &oldLoc, const QPointF &newLoc)
    : scene(scene), squareId(squareId), oldLoc(oldLoc), newLoc(newLoc)
{
}

void SquareMoveCmd::undo()
{
    auto sqItems = scene->squareItems();
    sqItems[squareId]->setPos(oldLoc);
    scene->update();
}

void SquareMoveCmd::redo()
{
    auto sqItems = scene->squareItems();
    sqItems[squareId]->setPos(newLoc);
    scene->update();
}
