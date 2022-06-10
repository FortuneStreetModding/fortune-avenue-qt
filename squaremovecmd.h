#ifndef SQUAREMOVECMD_H
#define SQUAREMOVECMD_H

#include <QPoint>
#include <QUndoCommand>
#include "fortuneavenuegraphicsscene.h"

class SquareMoveCmd : public QUndoCommand
{
public:
    SquareMoveCmd(FortuneAvenueGraphicsScene *scene, const QMap<int, QPointF> &oldPositions, const QMap<int, QPointF> &newPositions, bool snap, const std::function<void(const QMap<int, QPointF> &)> &updateFn);
    void undo() override;
    void redo() override;
private:
    FortuneAvenueGraphicsScene *scene;
    QMap<int, QPointF> oldPositions;
    QMap<int, QPointF> newPositions;
    bool snap;
    std::function<void(const QMap<int, QPointF> &)> updateFn;
};

#endif // SQUAREMOVECMD_H
