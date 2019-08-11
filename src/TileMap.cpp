#include "TileMap.h"

TileMap::TileMap(uint height, uint width) {
// Make sure the World has valid dimensions. If something is wrong, throw invalid_argument.
    if ((height == 0) || (width == 0))
        throw std::invalid_argument("Cannot create a TileMap with any dimension that is zero");

    // Create the tiles in the TileMap. If enough memory cannot be allocated, throw bad_alloc.
    try {
        tiles = new Tile[width * height];
    } catch (std::bad_alloc &bad) {
        throw bad;
    }

    // Save the map's size.
    this->_height = height;
    this->_width = width;

    // Calculate and save the maximum possible coordinate of the TileMap.
    _maxCord = Coordinate{width - 1, height - 1};

    assert(this->_width != 0);
    assert(this->_height != 0);
    assert(tiles);
}

TileMap::~TileMap() {
    delete[] tiles;
}

// Returns the height of the TileMap.
uint TileMap::height() noexcept {
    return _height;
}

// Returns the width of the TileMap.
uint TileMap::width() noexcept {
    return _width;
}

// Returns a pointer to the tile at the given coordinate. If the given
//   coordinate is outside the bounds of the TileMap, the function returns nullptr.
Tile *TileMap::at(const Coordinate &coordinate) {
    if (cordOutsideBound(this->_maxCord, coordinate))
        return nullptr;
    else
        return &tiles[getArrayIndex(coordinate, _width)];
}

// Sets the floor material of the specified tile to the given material. Returns true if successful.
bool TileMap::setFloorMaterial(const Coordinate &coordinate, const Material &desiredMaterial) noexcept {
    // Get the tile at the given coordinate.
    Tile *tile = this->at(coordinate);

    // If the tile is not valid (nullptr,) return false.
    if (!tile)
        return false;

    // Set the tile's floor material and displayID.
    tile->floorMaterial = desiredMaterial;
    tile->floorDisplay = desiredMaterial.defaultDisplayFloor;

    return true;
}

// Sets the wall material of the specified tile to the given material. Returns true if successful.
bool
TileMap::setWallMaterial(const Coordinate &coordinate, const Material &desiredMaterial, uint startingHealth) noexcept {
    // Get the tile at the given coordinate.
    Tile *tile = this->at(coordinate);

    // If the tile is not valid (nullptr,) return false.
    if (!tile)
        return false;

    // Set the tile's wall material, displayID, and health.
    tile->wallMaterial = desiredMaterial;
    tile->wallDisplay = desiredMaterial.defaultDisplayFloor;
    tile->wallHealth = desiredMaterial.baseHealth;

    return true;
}

// Returns a DisplayArray representing everything in the TileMap.
DisplayArray TileMap::generateDisplayArray() {
    // Create a DisplayArray and initialize its dimensions.
    DisplayArray result;
    result.width = _width;
    result.height = _height;

    // Create an array of DisplayArrayElement and link it to the result DisplayArray.
    auto *arrayPtr = new DisplayArrayElement[result.width * result.height];
    result.displayData = arrayPtr;

    // Load the DisplayArrayElement array with the display info for every tile in the chunk.
    Coordinate pos = Coordinate{0, 0};
    for (pos.y = 0; pos.y < result.width; pos.y++) {
        for (pos.x = 0; pos.x < result.height; pos.x++) {
            arrayPtr[getArrayIndex(pos, result.width)] = this->getTileDisplayElement(pos);
        }
    }

    return result;
}

// Take an already existing DisplayArray and loads it with everything needed to represent the TileMap.
void TileMap::loadDisplayArray(DisplayArray &displayArray) {
    // If the display array is the wrong dimensions or does not exist, delete it and create a new one.
    if ((displayArray.height != _height) || (displayArray.width != _width) || !displayArray.displayData) {
        delete[] displayArray.displayData;
        displayArray = this->generateDisplayArray();
        return;
    }

    // Load the DisplayArrayElement array with the display info for every tile in the chunk.
    Coordinate pos = Coordinate{0, 0};
    for (pos.y = 0; pos.y < displayArray.width; pos.y++) {
        for (pos.x = 0; pos.x < displayArray.height; pos.x++) {
            displayArray.displayData[getArrayIndex(pos, displayArray.width)] = this->getTileDisplayElement(pos);
        }
    }
}

// Returns true if there is no tile to represent the given coordinate.
bool TileMap::isInvalidTile(const Coordinate &coordinate) noexcept {
    return cordOutsideBound(this->_maxCord, coordinate);
}

// Returns a display element loaded with the information for the given coordinate.
DisplayArrayElement TileMap::getTileDisplayElement(Coordinate coordinate) noexcept {
    DisplayArrayElement result = DisplayArrayElement{DCID_VOID, DCID_VOID, COLOR_VOID, COLOR_VOID};

    // If the tile is not valid (nullptr,) return.
    if (isInvalidTile(coordinate))
        return result;

    result.BackgroundInfo = this->at(coordinate)->floorDisplay;
    result.ForegroundInfo = this->at(coordinate)->wallDisplay;
    result.ForegroundColor = this->at(coordinate)->wallMaterial.color;
    result.BackgroundColor = this->at(coordinate)->floorMaterial.color;

    return result;
}

// Returns the max coordinate for the TileMap.
const Coordinate &TileMap::maxCord() const {
    return _maxCord;
}
