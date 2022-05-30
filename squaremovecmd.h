#ifndef SQUAREMOVECMD_H
#define SQUAREMOVECMD_H

#include <QPoint>
#include <QUndoCommand>
#include "fortuneavenuegraphicsscene.h"

class SquareMoveCmd : public QUndoCommand
{
public:
    SquareMoveCmd(FortuneAvenueGraphicsScene *scene, int squareId, const QPointF &oldLoc, const QPointF &newLoc);
    void undo() override;
    void redo() override;
private:
    FortuneAvenueGraphicsScene *scene;
    int squareId;
    QPointF oldLoc;
    QPointF newLoc;
};

#endif // SQUAREMOVECMD_H
