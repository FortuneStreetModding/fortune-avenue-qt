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
bool hasCycle_(QVector<QPair<double, QPair<int, int>>> &edges, QVector<bool> marked, int currentCycleLength, int currentNode, int startNode, int maxCycleLength);
bool hasCycle(QVector<QPair<double, QPair<int, int>>> edges, int nodeCount, int maxCycleLength);
bool canConnectSquareTypes(SquareType squareType1, SquareType squareType2);
bool isTransportingSquareType(SquareType squareType);
void kruskalDfsAutoPathAlgorithm(const QVector<SquareItem *> &squares);
}

#endif // AUTOPATH_H
