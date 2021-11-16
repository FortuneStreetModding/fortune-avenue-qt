#ifndef AUTOPATH_H
#define AUTOPATH_H

#include "directions.h"
#include "squareitem.h"

namespace AutoPath {

SquareItem *getSquareInDirection(SquareItem *square, const QVector<SquareItem *> &squares, Direction dir, int directionCheckRange, int allowedVariance);
QMap<Direction, SquareItem *> getTouchingSquares(SquareItem *square, const QVector<SquareItem *> &squares, int directionCheckRange, int allowedVariance);
bool pathSquare(SquareItem *square, const QMap<Direction, SquareItem *> &touchingSquares);
void enumerateAutopathingRules(SquareItem *square, const QMap<Direction, SquareItem *> &touchingSquares);
void sortWaypoints(SquareItem *square);

}

#endif // AUTOPATH_H
