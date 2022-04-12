#include "autopath.h"
//#include <QDebug>
#include <QtMath>
#include <QSet>

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

bool hasCycle_(QVector<QPair<double, QPair<int, int>>> &edges, QVector<bool> marked, int currentPathLength, int currentNode, int startNode, int maxCycleLength) {
    marked[currentNode] = true;

    if (currentPathLength + 1 >= maxCycleLength) {
        // check if we can reach the start node again
        for(auto edge : edges) {
            int u = edge.second.first;
            int v = edge.second.second;
            if(currentNode == u && startNode == v) {
                return true;
            }
            if(currentNode == v && startNode == u) {
                return true;
            }
        }
        return false;
    }

    // check with all possible neighbors of currentNode
    for(auto edge : edges) {
        int u = edge.second.first;
        int v = edge.second.second;
        if(currentNode == u && !marked[v]) {
            if(hasCycle_(edges, marked, currentPathLength + 1, v, startNode, maxCycleLength))
                return true;
        }
        if(currentNode == v && !marked[u]) {
            if(hasCycle_(edges, marked, currentPathLength + 1, u, startNode, maxCycleLength))
                return true;
        }
    }

    marked[currentNode] = false;
    return false;
}

bool hasCycle(QVector<QPair<double, QPair<int, int>>> edges, int nodeCount, int maxCycleLength) {
    // marked nodes we do not need to check again
    QVector<bool> marked(nodeCount);

    for (int i = 0; i < nodeCount - (maxCycleLength - 1); i++) {
        if(hasCycle_(edges, marked, 0, i, i, maxCycleLength))
            return true;
        marked[i] = true;
    }

    return false;
}

bool isTransportingSquareType(SquareType squareType) {
    return squareType == OneWayAlleyDoorA
            || squareType == OneWayAlleyDoorB
            || squareType == OneWayAlleyDoorC
            || squareType == OneWayAlleyDoorD
            || squareType == LiftMagmaliceSquareStart
            || squareType == MagmaliceSquare
            || squareType == OneWayAlleySquare
            || squareType == LiftSquareEnd;
}

bool isConnected(SquareData &square1, SquareData &square2) {
    for(auto waypoint : square1.waypoints) {
        if(std::any_of(std::begin(waypoint.destinations), std::end(waypoint.destinations), [&](int i) { return i == square2.id; }))
            return true;
    }
    for(auto waypoint : square2.waypoints) {
        if(std::any_of(std::begin(waypoint.destinations), std::end(waypoint.destinations), [&](int i) { return i == square1.id; }))
            return true;
    }
    return false;
}

bool canConnectSquareTypes(SquareType squareType1, SquareType squareType2) {
    switch(squareType1) {
    case OneWayAlleyDoorA:
    case OneWayAlleyDoorB:
    case OneWayAlleyDoorC:
    case OneWayAlleyDoorD:
        return squareType2 != LiftMagmaliceSquareStart && squareType2 != MagmaliceSquare && squareType2 != LiftSquareEnd;
    case LiftMagmaliceSquareStart:
        return squareType2 == MagmaliceSquare || squareType2 == LiftSquareEnd || !isTransportingSquareType(squareType2);
    case MagmaliceSquare:
        return squareType2 == LiftMagmaliceSquareStart || !isTransportingSquareType(squareType2);
    case OneWayAlleySquare:
    case LiftSquareEnd:
        return !isTransportingSquareType(squareType2);
    default:
        return squareType2 != OneWayAlleySquare && squareType2 != LiftSquareEnd;
    }
}

void connect(SquareData &square1, SquareData &square2) {
    if(!canConnectSquareTypes(square1.squareType, square2.squareType)) {
        return;
    }
    // Find a free entry id in square 2
    int square2FreeEntryId = -1;
    for (int i=0; i<4; ++i) {
        bool square1IdInDestinations = std::any_of(std::begin(square2.waypoints[i].destinations), std::end(square2.waypoints[i].destinations), [&](int i) { return i == square1.id; });
        if(square2.waypoints[i].entryId == 255 && square2FreeEntryId == -1 && !square1IdInDestinations) {
            square2FreeEntryId = i;
        }
        if(square2.waypoints[i].entryId == square1.id) {
            square2FreeEntryId = i;
            break;
        }
    }
    if(square2FreeEntryId != -1) {
        QSet<int> destinations;
        // Collect all other destinations in square2
        for (int i=0; i<4; ++i) {
            for(int j=0; j<3; j++) {
                int destination = square2.waypoints[i].destinations[j];
                if(destination != 255 && destination != square1.id) {
                    destinations.insert(destination);
                }
            }
        }
        // add the square1 id as entry
        square2.waypoints[square2FreeEntryId].entryId = square1.id;
        // add the other destinations to it
        int destinationIndex = 0;
        for(int destination : destinations) {
            if(destination != square1.id && destinationIndex < 3) {
                square2.waypoints[square2FreeEntryId].destinations[destinationIndex] = destination;
                destinationIndex++;
            }
        }
    }
    // add the square2 id to all destinations in square1
    bool hasAdded = false;
    for (int i=0; i<4; ++i) {
        if(square1.waypoints[i].entryId != 255 && square1.waypoints[i].entryId != square2.id) {
            // do not add the destination if it already exists
            auto waypoint = square1.waypoints[i];
            bool square2IdInDestinations = std::any_of(std::begin(waypoint.destinations), std::end(waypoint.destinations), [&](int i) { return i == square2.id; });
            if(square2IdInDestinations) {
                 hasAdded = true;
            } else {
                for(int j=0; j<3; j++) {
                    int destination = square1.waypoints[i].destinations[j];
                    if(destination == 255) {
                        square1.waypoints[i].destinations[j] = square2.id;
                        hasAdded = true;
                        break;
                    }
                }
            }
        }
    }
    // if it was not possible to add the square2 id as destination in square1, then there is no entry ids defined yet.
    // -> lets force add it somewhere
    if(!hasAdded) {
        for (int i=0; i<4; ++i) {
            if(square1.waypoints[i].entryId == 255) {
                for(int j=0; j<3; j++) {
                    int destination = square1.waypoints[i].destinations[j];
                    if(destination == 255) {
                        square1.waypoints[i].destinations[j] = square2.id;
                        hasAdded = true;
                        break;
                    }
                }
            }
            if(hasAdded)
                break;
        }
    }
}

void kruskalDfsAutoPathAlgorithm(const QVector<SquareItem *> &squares, int maxManhattanDistance) {
    // Construct edges first
    QVector<QPair<double, QPair<int, int>>> edges;
    for (int i=0; i<squares.size(); ++i) {
        auto &square = ((SquareItem *)squares[i])->getData();
        for (int j=i+1; j<squares.size(); ++j) {
            auto &otherSquare = ((SquareItem *)squares[j])->getData();

            // Special case handling for transporting squares
            bool isTransportingEdge = isConnected(square, otherSquare)
                    && isTransportingSquareType(square.squareType)
                    && isTransportingSquareType(otherSquare.squareType);

            if (isTransportingEdge || (qAbs(square.positionX - otherSquare.positionX) <= maxManhattanDistance
                    && qAbs(square.positionY - otherSquare.positionY) <= maxManhattanDistance)) {
                auto xDistance = square.positionX - otherSquare.positionX;
                auto yDistance = square.positionY - otherSquare.positionY;
                auto euclideanDistance = qSqrt(xDistance*xDistance + yDistance*yDistance);
                edges.append({euclideanDistance, {square.id, otherSquare.id}});
            }
        }
    }

    // Sort edges
    std::sort(edges.begin(), edges.end());

    // Run modified Kruskal algorithm
    QVector<QPair<double, QPair<int, int>>> autoPathEdges;
    for(auto& edge : edges) {
        // Normally with Kruskals algorithm we do not create cycles.
        // In this modified variant, we will allow cycles which have a
        // path longer than 4
        auto autoPathCandidate(autoPathEdges);
        autoPathCandidate.append(edge);
        if (!hasCycle(autoPathCandidate, squares.size(), 3))
        {
            autoPathEdges = autoPathCandidate;
        }
    }

    // Clear waypoints
    for (int i=0; i<squares.size(); ++i) {
        auto &square = ((SquareItem *)squares[i])->getData();
        for (auto &waypoint: square.waypoints) {
            waypoint.entryId = 255;
            for (auto &dest: waypoint.destinations) {
                dest = 255;
            }
        }
    }

    // Apply pathing
    for(auto& edge : autoPathEdges) {
        int u = edge.second.first;
        int v = edge.second.second;
        auto& square = ((SquareItem *)squares[u])->getData();
        auto& otherSquare = ((SquareItem *)squares[v])->getData();
        connect(square, otherSquare);
        connect(otherSquare, square);
    }

    // Cleanup waypoints (set all destinations to 255 if entry id is still 255)
    for (int i=0; i<squares.size(); ++i) {
        auto &square = ((SquareItem *)squares[i])->getData();
        for (auto &waypoint: square.waypoints) {
            if (waypoint.entryId == 255) {
                for (auto &dest: waypoint.destinations) {
                    dest = 255;
                }
            }
        }
    }

    // Sort waypoints
    for (int i=0; i<squares.size(); ++i) {
        sortWaypoints(squares[i]);
    }
}

}
