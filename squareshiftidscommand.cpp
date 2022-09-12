#include "squareshiftidscommand.h"

SquareShiftIDsCommand::SquareShiftIDsCommand(FortuneAvenueGraphicsScene *scene,
                                           int oldId, int newId, const std::function<void ()> &updateFn)
        : scene(scene), oldId(oldId), newId(newId), updateFn(updateFn)
{
}

static void shiftSquaresHelper(FortuneAvenueGraphicsScene *scene, int oldId, int newId) {
    auto squares = scene->squareItems();
    squares[oldId]->getData().id = newId;
    squares[oldId]->updateZValueFromData();
    if (oldId < newId) {
        for (int i=oldId+1; i<=newId; ++i) {
            squares[i]->getData().id = i-1;
            squares[i]->updateZValueFromData();
        }
    } else {
        for (int i=newId; i<oldId; ++i) {
            squares[i]->getData().id = i+1;
            squares[i]->updateZValueFromData();
        }
    }
}

void SquareShiftIDsCommand::undo()
{
    shiftSquaresHelper(scene, newId, oldId);
    updateFn();
}

void SquareShiftIDsCommand::redo()
{
    shiftSquaresHelper(scene, oldId, newId);
    updateFn();
}
