#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <QString>

namespace AutoPath {

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
        return "north";
    case Northwest:
        return "northwest";
    case Northeast:
        return "northeast";
    case South:
        return "south";
    case Southwest:
        return "southwest";
    case Southeast:
        return "southeast";
    case West:
        return "west";
    case East:
        return "east";
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
