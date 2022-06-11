#include "updateboardmetacmd.h"

UpdateBoardMetaCmd::UpdateBoardMetaCmd(const BoardInfo &oldBoardInfo, const BoardInfo &newBoardInfo, const std::function<void (const BoardInfo &)> &updateFn)
    : oldBoardInfo(oldBoardInfo), newBoardInfo(newBoardInfo), updateFn(updateFn)
{
    setText("Update Board Metadata");
}


void UpdateBoardMetaCmd::undo()
{
    updateFn(oldBoardInfo);
}

void UpdateBoardMetaCmd::redo()
{
    updateFn(newBoardInfo);
}
