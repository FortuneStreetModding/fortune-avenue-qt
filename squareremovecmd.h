#ifndef SQUAREREMOVECMD_H
#define SQUAREREMOVECMD_H

#include "fortuneavenuegraphicsscene.h"
#include "squareitem.h"
#include <QUndoCommand>

class SquareRemoveCmd : public QUndoCommand
{
public:
    /**
     * removed squares and isRedo passed to updateFn
     */
    SquareRemoveCmd(FortuneAvenueGraphicsScene *scene, const std::function<void(const QVector<SquareData> &, bool)> &updateFn);
    void undo() override;
    void redo() override;
private:
    FortuneAvenueGraphicsScene *scene;
    QVector<SquareData> removedItems;
    QMap<int, int> oldToNewIDs;
    QMap<int, int> newToOldIDs;
    QMap<int, std::array<WaypointData, 4>> oldIDToWaypoints;
    std::function<void(const QVector<SquareData> &, bool)> updateFn;
};

#endif // SQUAREREMOVECMD_H
