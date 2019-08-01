#ifndef WELT_TILEMAP_H
#define WELT_TILEMAP_H

#include "DisplayIDdef.h"
#include "universal.h"
#include "tile.h"
#include "cassert"

class TileMap {
public:
    TileMap(uint height, uint width);

    virtual ~TileMap();

    // Returns the height of the TileMap.
    uint height() noexcept;

    uint width() noexcept;

    Tile *at(const Coordinate &coordinate);

    bool setFloorMaterial(const Coordinate &coordinate, const Material &desiredMaterial) noexcept;

    bool setWallMaterial(const Coordinate &cord, const Material &desiredMaterial, uint startingHealth) noexcept;

    DisplayArray generateDisplayArray();

    void loadDisplayArray(DisplayArray &displayArray);

    const Coordinate &maxCord() const;

private:
    Tile *tiles;
    Coordinate _maxCord;
    uint _width, _height;

    bool isInvalidTile(const Coordinate &coordinate) noexcept;

    DisplayArrayElement getTileDisplayElement(Coordinate coordinate) noexcept;
};


#endif //WELT_TILEMAP_H
