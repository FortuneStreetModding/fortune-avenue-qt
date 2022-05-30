#ifndef SQUAREADDCMD_H
#define SQUAREADDCMD_H

#include "fortuneavenuegraphicsscene.h"
#include <QUndoCommand>

class SquareAddCmd : public QUndoCommand
{
public:
    SquareAddCmd(FortuneAvenueGraphicsScene *scene);
    void undo() override;
    void redo() override;
private:
    FortuneAvenueGraphicsScene *scene;
};

#endif // SQUAREADDCMD_H
