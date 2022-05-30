#ifndef SQUAREREMOVECMD_H
#define SQUAREREMOVECMD_H

#include "fortuneavenuegraphicsscene.h"
#include "squareitem.h"
#include <QUndoCommand>

class SquareRemoveCmd : public QUndoCommand
{
public:
    SquareRemoveCmd(FortuneAvenueGraphicsScene *scene);
    void undo() override;
    void redo() override;
private:
    FortuneAvenueGraphicsScene *scene;
    QVector<SquareData> removedItems;
    QMap<int, int> oldToNewIDs;
    QMap<int, int> newToOldIDs;
    QMap<int, std::array<WaypointData, 4>> oldIDToWaypoints;
};

#endif // SQUAREREMOVECMD_H
