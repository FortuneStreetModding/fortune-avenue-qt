#ifndef SQUARESHIFTIDSCOMMAND_H
#define SQUARESHIFTIDSCOMMAND_H

#include "fortuneavenuegraphicsscene.h"
#include <QUndoCommand>

class SquareShiftIDsCommand : public QUndoCommand
{
public:
    SquareShiftIDsCommand(FortuneAvenueGraphicsScene *scene, int oldId, int newId, const std::function<void()> &updateFn);
    void undo() override;
    void redo() override;
private:
    FortuneAvenueGraphicsScene *scene;
    int oldId;
    int newId;
    std::function<void()> updateFn;
};

#endif // SQUARESHIFTIDSCOMMAND_H
