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

QMap<Direction, SquareItem *> getTouchingSquares(SquareItem *square, const QVector<SquareItem *> &squares) {
    QMap<Direction, SquareItem *> touchingSquares;
    for (auto dir: AutoPath::DIRECTIONS) {
        auto squareInDir = AutoPath::getSquareInDirection(square, squares, dir);
        if (squareInDir) {
            touchingSquares[dir] = squareInDir;
        }
    }
    if (touchingSquares.contains(AutoPath::North)) {
        touchingSquares.remove(AutoPath::Northeast);
        touchingSquares.remove(AutoPath::Northwest);
    }
    if (touchingSquares.contains(AutoPath::South)) {
        touchingSquares.remove(AutoPath::Southeast);
        touchingSquares.remove(AutoPath::Southwest);
    }
    if (touchingSquares.contains(AutoPath::West)) {
        touchingSquares.remove(AutoPath::Northwest);
        touchingSquares.remove(AutoPath::Southwest);
    }
    if (touchingSquares.contains(AutoPath::East)) {
        touchingSquares.remove(AutoPath::Northeast);
        touchingSquares.remove(AutoPath::Southeast);
    }
    return touchingSquares;
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

static Direction idToDirection(const QMap<Direction, SquareItem *> &touchingSquares, int id) {
    for (auto it=touchingSquares.begin(); it!=touchingSquares.end(); ++it) {
        if (it.value()->getData().id == id) {
            return it.key();
        }
    }
    return _UnrecognizedDirection;
}

void enumerateAutopathingRules(SquareItem *square, const QMap<Direction, SquareItem *> &touchingSquares) {
    auto &data = square->getData();
    data.validDirections.clear();
    for (auto &waypoint: data.waypoints) {
        auto fromDir = idToDirection(touchingSquares, waypoint.entryId);
        if (fromDir != _UnrecognizedDirection) {
            for (auto dest: waypoint.destinations) {
                auto toDir = idToDirection(touchingSquares, dest);
                if (toDir != _UnrecognizedDirection) {
                    data.validDirections.insert(fromDir, toDir);
                }
            }
        }
    }
}

}
