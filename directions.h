#ifndef DIRECTIONS_H
#define DIRECTIONS_H

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

}


#endif // DIRECTIONS_H
