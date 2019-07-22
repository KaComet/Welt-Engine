#ifndef TILE_H
#define TILE_H

#include "material.h"

class Tile {
public:
    Tile();
    ~Tile() = default;

    DisplayID wallDisplay, floorDisplay;
    Material wallMaterial, floorMaterial;
    uint wallHealth;
};

#endif