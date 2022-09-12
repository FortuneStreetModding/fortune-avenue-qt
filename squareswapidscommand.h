#ifndef SQUARESWAPIDSCOMMAND_H
#define SQUARESWAPIDSCOMMAND_H

#include "fortuneavenuegraphicsscene.h"
#include <QUndoCommand>

class SquareSwapIDsCommand : public QUndoCommand
{
public:
    SquareSwapIDsCommand(FortuneAvenueGraphicsScene *scene, int oldId, int newId, const std::function<void()> &updateFn);
    void undo() override;
    void redo() override;
private:
    FortuneAvenueGraphicsScene *scene;
    int oldId;
    int newId;
    std::function<void()> updateFn;
};

#endif // SQUARESWAPIDSCOMMAND_H
