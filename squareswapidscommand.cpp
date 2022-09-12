#include "squareswapidscommand.h"


SquareSwapIDsCommand::SquareSwapIDsCommand(FortuneAvenueGraphicsScene *scene,
                                           int oldId, int newId, const std::function<void ()> &updateFn)
        : scene(scene), oldId(oldId), newId(newId), updateFn(updateFn)
{
}


void SquareSwapIDsCommand::undo()
{
    auto squares = scene->squareItems();
    std::swap(squares[oldId]->getData().id, squares[newId]->getData().id);
    squares[oldId]->updateZValueFromData();
    squares[newId]->updateZValueFromData();
    updateFn();
}

void SquareSwapIDsCommand::redo()
{
    auto squares = scene->squareItems();
    std::swap(squares[oldId]->getData().id, squares[newId]->getData().id);
    squares[oldId]->updateZValueFromData();
    squares[newId]->updateZValueFromData();
    updateFn();
}
