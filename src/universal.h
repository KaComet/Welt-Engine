#ifndef UNIVERSAL_H
#define UNIVERSAL_H

#include <list>
#include <SDL.h>
#include <string>
#include <vector>
#include <cmath>
#include "../examples/01-Wolf_and_Sheep/LTexture.h"

// ---------------- Typedefs ----------------
typedef unsigned int DisplayID;
typedef unsigned int colorID;
typedef unsigned int uint; // Short form for unsigned int.
typedef unsigned int EID;  // Entity ID type.
typedef unsigned int IID;  // Item ID type.

const DisplayID DCID_VOID = 1;
const DisplayID DCID_AIR = 2;
const DisplayID DCID_GROUND_OUTSIDE = 3;
const DisplayID DCID_SLDWALL_CONNECT = 4;
const DisplayID DCID_ENTITY_SIMPLE = 5;
const DisplayID DCID_ITEM_TEST_1 = 6;

const colorID COLOR_VOID = 1;
const colorID COLOR_AIR = 2;
const colorID COLOR_GRASS = 3;
const colorID COLOR_STONE = 4;
const colorID COLOR_ENTITY_NICE = 5;
const colorID COLOR_ENTITY_HOSTILE = 6;
const colorID COLOR_ITEM_TEST_1 = 7;

// ---------------- Structs ----------------
struct Coordinate {
    explicit Coordinate(uint x = 0, uint y = 0) : x(x), y(y) {
    };

    uint x;
    uint y;
};

// ---------------- Enums ----------------
enum class EffectedType {
    NONE,
    DELETED,
    MOVED
};

enum class DamageType {
    KINETIC,
};

// ---------------- Functions ----------------

// If the given coordinate is outside the given bound (defined by its max coordinate,) the function returns true.
inline bool cordOutsideBound(const Coordinate &maxCord, const Coordinate &cord) {
    return (maxCord.x < cord.x) || (maxCord.y < cord.y);
}

// Returns true if the given point is inside the given rectangle.
inline bool
cordInsideRect(const Coordinate &rrectStart, const uint &height, const uint &width, const Coordinate &point) {
    return (point.x >= rrectStart.x) && (point.y >= rrectStart.y) &&
           (point.x <= (rrectStart.x + width)) && (point.y <= (rrectStart.y + width));

}

// For determining if two coordinates are the same.
inline bool operator==(const Coordinate &c1, const Coordinate &c2) {
    return (c1.x == c2.x) && (c1.y == c2.y);
}

inline double distance(const Coordinate &c1, const Coordinate &c2) {
    return std::sqrt(pow((int) c1.x - (int) c2.x, 2) + pow((int) c1.y - (int) c2.y, 2));
}

inline bool distanceFast(const Coordinate &c1, const Coordinate &c2, const uint &radius) {
    const uint dx = (int) c1.x - (int) c2.x;
    const uint dy = (int) c1.y - (int) c2.y;
    const uint distanceSqrd = radius * radius;
    return (uint) ((dx * dx) + (dy * dy)) <= distanceSqrd;
}

inline uint getArrayIndex(const Coordinate &coordinate, const uint arrayWidth) {
    return coordinate.x + (coordinate.y * arrayWidth);
}

#endif