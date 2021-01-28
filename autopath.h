#ifndef AUTOPATH_H
#define AUTOPATH_H

#include "directions.h"
#include "squareitem.h"

namespace AutoPath {

SquareItem *getSquareInDirection(SquareItem *square, const QVector<SquareItem *> &squares, Direction dir);
bool pathSquare(SquareItem *square, const QMap<Direction, SquareItem *> &touchingSquares);

}

#endif // AUTOPATH_H
