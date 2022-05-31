#include "squaremovecmd.h"


SquareMoveCmd::SquareMoveCmd(FortuneAvenueGraphicsScene *scene, const QMap<int, QPointF> &oldPositions, const QMap<int, QPointF> &newPositions)
    : scene(scene), oldPositions(oldPositions), newPositions(newPositions)
{
}

void SquareMoveCmd::undo()
{
    auto sqItems = scene->squareItems();
    for (auto it=oldPositions.begin(); it!=oldPositions.end(); ++it) {
        sqItems[it.key()]->setPos(it.value());
    }
    scene->update();
}

void SquareMoveCmd::redo()
{
    auto sqItems = scene->squareItems();
    for (auto it=newPositions.begin(); it!=newPositions.end(); ++it) {
        sqItems[it.key()]->setPos(it.value());
    }
    scene->update();
}
