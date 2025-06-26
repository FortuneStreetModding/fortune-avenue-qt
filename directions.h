#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include "qobject.h"
#include <QString>

namespace AutoPath {

class Directions : public QObject
{
    Q_OBJECT
};

enum Direction {
    North, South, West, East, Northwest, Northeast, Southwest, Southeast,

    _UnrecognizedDirection = -1
};
const Direction DIRECTIONS[] = {
    North,
    South,
    West,
    East,
    Northwest,
    Northeast,
    Southwest,
    Southeast
};

inline int getXOffset(Direction dir) {
    switch (dir) {
    case West:
    case Northwest:
    case Southwest:
        return -1;
    case East:
    case Northeast:
    case Southeast:
        return 1;
    default:
        return 0;
    }
}
inline int getYOffset(Direction dir) {
    switch (dir) {
    case North:
    case Northwest:
    case Northeast:
        return -1;
    case South:
    case Southwest:
    case Southeast:
        return 1;
    default:
        return 0;
    }
}
inline QString getDirectionName(Direction dir) {
    switch (dir) {
    case North:
        return Directions::tr("north");
    case Northwest:
        return Directions::tr("northwest");
    case Northeast:
        return Directions::tr("northeast");
    case South:
        return Directions::tr("south");
    case Southwest:
        return Directions::tr("southwest");
    case Southeast:
        return Directions::tr("southeast");
    case West:
        return Directions::tr("west");
    case East:
        return Directions::tr("east");
    default:
        return "";
    }
}
inline Direction getOppositeDirection(Direction dir) {
    switch (dir) {
    case North:
        return South;
    case Northwest:
        return Southeast;
    case Northeast:
        return Southwest;
    case South:
        return North;
    case Southwest:
        return Northeast;
    case Southeast:
        return Northwest;
    case West:
        return East;
    case East:
        return West;
    default:
        return _UnrecognizedDirection;
    }
}

}


#endif // DIRECTIONS_H
