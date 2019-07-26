#ifndef UNIVERSAL_H
#define UNIVERSAL_H

#include <list>
#include <SDL.h>
#include <string>
#include <vector>
#include <cmath>
#include "LTexture.h"

// ---------------- Typedefs ----------------
typedef unsigned int DisplayID;
typedef unsigned int colorID;
typedef unsigned int uint; // Short form for unsigned int.
typedef unsigned int OID;  // Object ID.
typedef unsigned int IID;  // Item ID.

const DisplayID DCID_VOID = 1;
const DisplayID DCID_AIR = 2;
const DisplayID DCID_GROUND_OUTSIDE = 3;
const DisplayID DCID_SLDWALL_CONNECT = 4;
const DisplayID DCID_ENTITY_SIMPLE = 5;

const colorID COLOR_VOID = 1;
const colorID COLOR_AIR = 2;
const colorID COLOR_GRASS = 3;
const colorID COLOR_STONE = 4;
const colorID COLOR_ENTITY_NICE = 5;
const colorID COLOR_ENTITY_HOSTILE = 6;

// ---------------- Structs ----------------
struct Coordinate {
    Coordinate() : x(0), y(0) {
    };

    Coordinate(uint xCord, uint yCord) : x(xCord), y(yCord) {
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

// If the given coordinate is outside the given bound (defined by its max coordinate,) the funtion returns true.
inline bool cordOutsideBound(const Coordinate &maxCord, const Coordinate &cord) {
    if ((maxCord.x < cord.x) || (maxCord.y < cord.y))
        return true;
    else
        return false;
}

// Returns true if the given point is inside the given rectangle.
inline bool
cordInsideRect(const Coordinate &rrectStart, const uint &height, const uint &width, const Coordinate &point) {
    if ((point.x >= rrectStart.x) && (point.y >= rrectStart.y) &&
        (point.x <= (rrectStart.x + width)) && (point.y <= (rrectStart.y + width)))
        return true;
    else
        return false;

}

// For determining if two coordinates are the same.
inline bool operator==(const Coordinate &c1, const Coordinate &c2) {
    if ((c1.x == c2.x) && (c1.y == c2.y))
        return true;
    else
        return false;
}

inline double distance(const Coordinate &c1, const Coordinate &c2) {
    if (c1 == c2)
        return 0;
    else
        return std::sqrt(pow((int) c1.x - (int) c2.x, 2) + pow((int) c1.y - (int) c2.y, 2));
}

#endif