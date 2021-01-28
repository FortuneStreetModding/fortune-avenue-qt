#include "autopath.h"

namespace AutoPath {

static constexpr int DIRECTION_CHECK_RANGE = 100;
static constexpr int ALLOWED_VARIANCE = 20;

static bool squareInRange(SquareItem *square, SquareItem *other) {
    return qAbs(square->getData().positionX - other->getData().positionX) <= DIRECTION_CHECK_RANGE
            && qAbs(square->getData().positionY - other->getData().positionY) <= DIRECTION_CHECK_RANGE;
}

static int coordinateOnDesiredOffset(int val, int otherVal, int offset) {
    if (offset < 0) {
        return otherVal < val;
    } else if (offset > 0) {
        return otherVal > val;
    } else {
        return qAbs(otherVal - val) <= ALLOWED_VARIANCE;
    }
}

SquareItem *getSquareInDirection(SquareItem *square, const QVector<SquareItem *> &squares, Direction dir) {
    for (auto other: squares) {
        if (squareInRange(square, other)
                && coordinateOnDesiredOffset(square->getData().positionX, other->getData().positionX, getXOffset(dir))
                && coordinateOnDesiredOffset(square->getData().positionY, other->getData().positionY, getYOffset(dir))) {
            return other;
        }
    }
    return nullptr;
}

bool pathSquare(SquareItem *square, const QMap<Direction, SquareItem *> &touchingSquares) {
    if (touchingSquares.size() > 4) {
        return false;
    }

    auto &data = square->getData();
    for (auto &waypoint: data.waypoints) {
        waypoint.entryId = 255;
        for (auto &waypointDest: waypoint.destinations) {
            waypointDest = 255;
        }
    }

    int i=0;
    for (auto it=touchingSquares.begin(); it!=touchingSquares.end(); ++it) {
        int j=0;
        for (auto jt=touchingSquares.begin(); jt!=touchingSquares.end(); ++jt) {
            if (it != jt && data.validDirections.contains(it.key(), jt.key())) {
                data.waypoints[i].entryId = it.value()->getData().id;
                data.waypoints[i].destinations[j] = jt.value()->getData().id;
                ++j;
            }
        }
        if (data.waypoints[i].entryId != 255) {
            ++i;
        }
    }
    return true;
}

}
