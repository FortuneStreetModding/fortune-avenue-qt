#ifndef SQUAREADDCMD_H
#define SQUAREADDCMD_H

#include "fortuneavenuegraphicsscene.h"
#include <QUndoCommand>

class SquareAddCmd : public QUndoCommand
{
public:
    SquareAddCmd(FortuneAvenueGraphicsScene *scene, const std::function<void(SquareItem *)> &updateFn);
    void undo() override;
    void redo() override;
private:
    FortuneAvenueGraphicsScene *scene;
    std::function<void(SquareItem *)> updateFn;
};

#endif // SQUAREADDCMD_H
