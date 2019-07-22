#ifndef WELT_P_C_DISPLAYIDDEF_H
#define WELT_P_C_DISPLAYIDDEF_H

#include <string>
#include <vector>
#include "FlatVector.h"
#include "universal.h"
#include "FlatVector.h"

typedef unsigned int displayID; // For specifying a char/pic that is used to display an object.

// A single element of a display array. Contains all the info needed to represent a tile.
struct DisplayArrayElement {
    DisplayID BackgroundInfo, ForegroundInfo;
    colorID BackgroundColor, ForegroundColor;
};

struct DisplayArray {
    uint width, height;
    DisplayArrayElement *displayData;
    DisplayArray(): width(0), height(0), displayData(nullptr)
    {
    }
};

// Contains all the displayIDs for every possible state of a tile. Used to allow for tiles to
//   connect (like wall or fences) or other "fancy" effects.
struct SpriteInteraction {
    std::string name;
    displayID dDefault,
            W,
            E,
            EW,
            S,
            SW,
            SE,
            SEW,
            N,
            NW,
            NE,
            NEW,
            NS,
            NSW,
            NSE,
            NSEW;
};

#endif //WELT_P_C_DISPLAYIDDEF_H
