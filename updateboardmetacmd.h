#ifndef UPDATEBOARDMETACMD_H
#define UPDATEBOARDMETACMD_H

#include "fortunestreetdata.h"
#include <QUndoCommand>

class UpdateBoardMetaCmd : public QUndoCommand
{
public:
    UpdateBoardMetaCmd(const BoardInfo &oldBoardInfo, const BoardInfo &newBoardInfo, const QString &text, const std::function<void(const BoardInfo &)> &updateFn);
    void undo() override;
    void redo() override;
private:
    BoardInfo oldBoardInfo;
    BoardInfo newBoardInfo;
    std::function<void(const BoardInfo &)> updateFn;
};

#endif // UPDATEBOARDMETACMD_H
