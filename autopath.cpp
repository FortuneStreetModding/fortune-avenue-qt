#include "autopath.h"
//#include <QDebug>

namespace AutoPath {

static bool squareInRange(SquareItem *square, SquareItem *other, int directionCheckRange) {
    return qAbs(square->getData().positionX - other->getData().positionX) <= directionCheckRange
            && qAbs(square->getData().positionY - other->getData().positionY) <= directionCheckRange;
}

static int coordinateOnDesiredOffset(int val, int otherVal, int offset, int allowedVariance) {
    if (offset < 0) {
        return otherVal < val;
    } else if (offset > 0) {
        return otherVal > val;
    } else {
        return qAbs(otherVal - val) <= allowedVariance;
    }
}

SquareItem *getSquareInDirection(SquareItem *square, const QVector<SquareItem *> &squares, Direction dir, int directionCheckRange, int allowedVariance) {
    for (auto other: squares) {
        if (squareInRange(square, other, directionCheckRange)
                && coordinateOnDesiredOffset(square->getData().positionX, other->getData().positionX, getXOffset(dir), allowedVariance)
                && coordinateOnDesiredOffset(square->getData().positionY, other->getData().positionY, getYOffset(dir), allowedVariance)) {
            return other;
        }
    }
    return nullptr;
}

QMap<Direction, SquareItem *> getTouchingSquares(SquareItem *square, const QVector<SquareItem *> &squares, int directionCheckRange, int allowedVariance) {
    QMap<Direction, SquareItem *> touchingSquares;
    for (auto dir: AutoPath::DIRECTIONS) {
        auto squareInDir = AutoPath::getSquareInDirection(square, squares, dir, directionCheckRange, allowedVariance);
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

    sortWaypoints(square);

    return true;
}

void sortWaypoints(SquareItem *square) {
    auto &data = square->getData();

    QMultiMap<int, QList<int>> waypointData;
    for (auto &waypoint : data.waypoints) {
        QList<int> destinations;
        for (auto &waypointDest : waypoint.destinations) {
            destinations.append(waypointDest);
        }
        std::sort(destinations.begin(), destinations.end());
        waypointData.insert(waypoint.entryId, destinations);
    }
    QList<int> entryIds;
    for (auto &waypoint : data.waypoints) {
        entryIds.append(waypoint.entryId);
    }
    std::sort(entryIds.begin(), entryIds.end());
    int i=0;
    for(int entryId : entryIds) {
        QMultiMap<int, QList<int>> waypointListData;
        auto destinationsLists = waypointData.values(entryId);
        for(auto &destinations : destinationsLists) {
            waypointListData.insert(destinations.first(), destinations);
        }
        for(auto &destinations : waypointListData.values()) {
            if(i<4) {
                data.waypoints[i].entryId = entryId;
                data.waypoints[i].destinations[0] = destinations.at(0);
                data.waypoints[i].destinations[1] = destinations.at(1);
                data.waypoints[i].destinations[2] = destinations.at(2);
            }
            i++;
        }
    }
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
