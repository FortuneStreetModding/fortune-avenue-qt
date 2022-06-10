#ifndef SQUARECHANGECMD_H
#define SQUARECHANGECMD_H

#include "fortuneavenuegraphicsscene.h"
#include "fortunestreetdata.h"
#include <QUndoCommand>

class SquareChangeCmd : public QUndoCommand
{
public:
    SquareChangeCmd(FortuneAvenueGraphicsScene *scene, const QMap<int, SquareData> &oldData, const QMap<int, SquareData> &newData, const std::function<void()> &updateFn);
    void undo() override;
    void redo() override;
private:
    FortuneAvenueGraphicsScene *scene;
    QMap<int, SquareData> oldData;
    QMap<int, SquareData> newData;
    std::function<void()> updateFn;
};

#endif // SQUARECHANGECMD_H
