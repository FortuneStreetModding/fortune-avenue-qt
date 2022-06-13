#include "updateboardmetacmd.h"

UpdateBoardMetaCmd::UpdateBoardMetaCmd(const BoardInfo &oldBoardInfo, const BoardInfo &newBoardInfo, const QString &text, const std::function<void (const BoardInfo &)> &updateFn)
    : oldBoardInfo(oldBoardInfo), newBoardInfo(newBoardInfo), updateFn(updateFn)
{
    setText(text);
}


void UpdateBoardMetaCmd::undo()
{
    updateFn(oldBoardInfo);
}

void UpdateBoardMetaCmd::redo()
{
    updateFn(newBoardInfo);
}
