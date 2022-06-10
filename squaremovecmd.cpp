#include "squaremovecmd.h"


SquareMoveCmd::SquareMoveCmd(FortuneAvenueGraphicsScene *scene, const QMap<int, QPointF> &oldPositions, const QMap<int, QPointF> &newPositions, bool snap, const std::function<void (const QMap<int, QPointF> &)> &updateFn)
    : scene(scene), oldPositions(oldPositions), newPositions(newPositions), snap(snap), updateFn(updateFn)
{
    setText("Move Squares");
}

void SquareMoveCmd::undo()
{
    int oldSnapSize = scene->getSnapSize();
    if (!snap) {
        scene->setSnapSize(1);
    }
    auto sqItems = scene->squareItems();
    for (auto it=oldPositions.begin(); it!=oldPositions.end(); ++it) {
        sqItems[it.key()]->setPos(it.value());
    }
    scene->setSnapSize(oldSnapSize);
    scene->update();
    updateFn(oldPositions);
}

void SquareMoveCmd::redo()
{
    int oldSnapSize = scene->getSnapSize();
    if (!snap) {
        scene->setSnapSize(1);
    }
    auto sqItems = scene->squareItems();
    for (auto it=newPositions.begin(); it!=newPositions.end(); ++it) {
        sqItems[it.key()]->setPos(it.value());
    }
    scene->setSnapSize(oldSnapSize);
    scene->update();
    updateFn(newPositions);
}
