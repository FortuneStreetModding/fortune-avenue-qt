#ifndef AUTOPATH_H
#define AUTOPATH_H

#include "directions.h"
#include "squareitem.h"

namespace AutoPath {

SquareItem *getSquareInDirection(SquareItem *square, const QVector<SquareItem *> &squares, Direction dir);
QMap<Direction, SquareItem *> getTouchingSquares(SquareItem *square, const QVector<SquareItem *> &squares);
bool pathSquare(SquareItem *square, const QMap<Direction, SquareItem *> &touchingSquares);
void enumerateAutopathingRules(SquareItem *square, const QMap<Direction, SquareItem *> &touchingSquares);

}

#endif // AUTOPATH_H
