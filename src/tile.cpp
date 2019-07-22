#include "tile.h"


Tile::Tile() {
    // Initialize the wall health with the default value.
    wallMaterial= M_AIR;
    floorMaterial= M_AIR;
    wallDisplay= M_AIR.defaultDisplayWall;
    floorDisplay= M_AIR.defaultDisplayFloor;
    wallHealth = 0;
}

