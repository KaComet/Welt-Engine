#include "world.h"


World::World(uint height, uint width) {
    // Make sure the World has valid dimensions. If something is wrong, throw invalid_argument.
    if ((height == 0) || (width == 0))
        throw std::invalid_argument("Cannot create a World with any dimension that is zero");

    // Create the tiles in the World. If enough memory cannot be allocated, throw bad_alloc.
    try {
        tilesInWorld = new Tile[width * height];
    } catch (std::bad_alloc &bad) {
        throw bad;
    }

    // Save the World's size.
    worldHeight = height;
    worldWidth = width;

    // Start the OID, IID, and tick counters.
    nextAvailableOID = 0;
    nextAvailableIID = 0;
    tickNumber = 0;
    chunkSize = 16;

    // Initialize the entity chunks and calculate the max chunk number.
    Coordinate maxChunkCord;
    uint nChunksPerRow = (uint) std::ceil((double) worldWidth / (double) chunkSize);
    maxChunkCord.y = worldHeight / chunkSize;
    maxChunkCord.x = worldWidth / chunkSize;
    maxChunkNumber = (maxChunkCord.y * nChunksPerRow) + maxChunkCord.x;
    entitiesInChunks.resize(maxChunkNumber + 1);

    assert(chunkSize != 0);
    assert(worldWidth != 0);
    assert(worldHeight != 0);
}

World::~World() {
    // Delete all of the tiles, entities, and items in the World.
    delete[] tilesInWorld;
    for (Entity *ent : entitiesInWorld)
        delete ent;

    for (Item *itm : itemsInWorld)
        delete itm;
}

// Returns the max coordinate an entity can have.
Coordinate World::maxCord() {
    return Coordinate{worldWidth - 1, worldHeight - 1};
}

// Returns the DisplayArrayElement for the indicated tile. If the given position is
//  outside the world, the function will return a DisplayArrayElement that is loaded
//  with the values and colors for VOID.
DisplayArrayElement World::getDisplayInfoForTile(Coordinate cord) {
    // If the cord is outside the chunk, return air.
    if (cordOutsideBound(this->maxCord(), cord))
        return DisplayArrayElement{DCID_VOID, DCID_VOID, M_VOID.color, M_VOID.color};

    DisplayArrayElement result;

    Tile *tmp = &tilesInWorld[(cord.y * worldWidth) + cord.x];

    result.BackgroundInfo = tmp->floorDisplay;
    result.ForegroundInfo = tmp->wallDisplay;
    result.BackgroundColor = tmp->floorMaterial.color;
    result.ForegroundColor = tmp->wallMaterial.color;

    //REFACTOR: move to new function to speed up drawing.
    uint chunkNumber = getChunkNumberForCoordinate(cord);
    for (const auto &it : entitiesInChunks.at(chunkNumber)) {
        if (it->selfPosition == cord) {
            result.ForegroundInfo = it->entityDisplay;
            result.ForegroundColor = it->selfMaterial.color;
            break;
        }
    }

    return result;
}

// Returns a DisplayArray representing everything in the chunk.
// Don't forget to call delete and nullptr DisplayArrayElement* in the output after use.
DisplayArray World::getDisplayArrayForWorld() {
    // Create a DisplayArray and initialize its dimensions.
    DisplayArray result;
    result.width = worldWidth;
    result.height = worldHeight;

    // Create an array of DisplayArrayElement and link it to the result DisplayArray.
    auto *arrayPtr = new DisplayArrayElement[result.width * result.height];
    result.displayData = arrayPtr;

    // Load the DisplayArrayElement array with the display info for every tile in the chunk.
    for (uint r = 0; r < worldWidth; r++) {
        for (uint c = 0; c < worldWidth; c++) {
            arrayPtr[c + (worldWidth * r)] = getDisplayInfoForTile(Coordinate{c, r});
        }
    }

    return result;
}

// Don't forget to call delete and nullptr DisplayArrayElement* in the output after use.
void World::getDisplayArrayForWorld(DisplayArray &dis) {
    // If the display array is the wrong dimensions or does not exit, delete it and create a new one.
    if ((dis.height != worldHeight) || (dis.width != worldWidth) || (dis.displayData == nullptr)) {
        delete[] dis.displayData;
        dis = this->getDisplayArrayForWorld();
        return;
    }

    // Load the DisplayArrayElement array with the display info for every tile in the chunk.
    for (uint r = 0; r < worldWidth; r++) {
        for (uint c = 0; c < worldWidth; c++) {
            dis.displayData[c + (worldWidth * r)] = getDisplayInfoForTile(Coordinate{c, r});
        }
    }
}

// Calls each entity's tick function. The order in which the tick functions are called are determined
//   by which entities have the highest initiative.
void World::tick() {
    auto it = entitiesInWorld.begin();

    // Iterate through the list, executing every entity's tick function.
    while (it != entitiesInWorld.end()) {
        Entity *entityPtr = *it;

        // If the entity* is null, skip this element.
        if (entityPtr == nullptr) {
            ++it;
            continue;
        }

        // Call the entity's tick function and store its returned state.
        EffectedType returnState = entityPtr->tick(this);

        // If the entity indicated that it needs to be deleted, delete it.
        if (returnState == EffectedType::DELETED) {
            bool wasDeleted = this->deleteEntity(&it);
            if (!wasDeleted)
                ++it;
        } else {
            // Move to the next entity.
            ++it;
        }
    }

    tickNumber++;
}

// Moves an entity from one position to another. Returns true is successful.
// If the entity does not exist or the cord is outside the World, return false.
bool World::moveEntity(Entity *entityPtr, Coordinate cord) {
    // If the desired coordinate is outsize the World or the entity pointer is NULL, return false.
    if (cordOutsideBound(this->maxCord(), cord) || (entityPtr == nullptr))
        return false;

    // Set the entity to the desired coordinate.
    uint newChunkNumber = getChunkNumberForCoordinate(cord);
    uint oldChunkNumber = getChunkNumberForCoordinate(entityPtr->selfPosition);
    if (newChunkNumber != oldChunkNumber) {
        entitiesInChunks.at(oldChunkNumber).remove(entityPtr);
        entitiesInChunks.at(newChunkNumber).push_back(entityPtr);
    }
    entityPtr->selfPosition = cord;

    return true;
}

// Returns the entity on the specified tile. If the tile contains no entity, nullptr is returned.
Entity *World::getEntityOnTile(Coordinate cord) {
    if (cordOutsideBound(this->maxCord(), cord))
        return nullptr;

    uint chunkNumber = getChunkNumberForCoordinate(cord);
    auto chunkIt = entitiesInChunks.at(chunkNumber).begin();

    while (chunkIt != entitiesInChunks.at(chunkNumber).end()) {
        if ((*chunkIt)->selfPosition == cord) {
            return *chunkIt;
        }

        chunkIt++;
    }

    return nullptr;
}

// Adds the provided entity to the World at the indicated tile. Returns true if successful.
bool World::addEntity(Entity *entityToAdd, Coordinate cord) {
    // If the desired coordinate is outsize the World or the entity pointer is NULL, return false.
    if (cordOutsideBound(this->maxCord(), cord) || (entityToAdd == nullptr))
        return false;

    for (const auto &it : entitiesInWorld) {
        // If we have found a entity that is currently occupying the tile we want to add an entity to, return false.
        if (it->selfPosition == cord)
            return false;
    }

    // Set the entity's position and OID and add it to the World.
    entityToAdd->selfPosition = cord;
    entityToAdd->selfID = nextAvailableOID;
    entitiesInWorld.push_back(entityToAdd);
    entitiesInChunks.at(getChunkNumberForCoordinate(cord)).push_back(entityToAdd);

    // Increment the OID counter
    nextAvailableOID++;

    return true;
}

// Deletes an entity from the world based on it's pointer. Returns true if the entity was found and deleted.
bool World::deleteEntity(std::list<Entity *>::iterator *it) {
    auto globalIt = entitiesInWorld.begin();
    bool foundInGlobal = false;

    while (globalIt != entitiesInWorld.end()) {
        if (*globalIt == **it) {
            foundInGlobal = true;
            break;
        }

        ++globalIt;
    }

    if (!foundInGlobal)
        return false;

    uint chunkNumber = getChunkNumberForCoordinate((**it)->selfPosition);
    auto chunkIt = entitiesInChunks.at(chunkNumber).begin();
    bool foundInChunk = false;

    while (chunkIt != entitiesInChunks.at(chunkNumber).end()) {
        if (*chunkIt == **it) {
            foundInChunk = true;
            break;
        }

        ++chunkIt;
    }

    if (foundInChunk && foundInGlobal) {
        delete **it;
        *it = entitiesInWorld.erase(*it);
        entitiesInChunks.at(chunkNumber).erase(chunkIt);

        return true;
    }

    // If we could not find an entity with the given pointer, return false.
    return false;
}

// Deletes the entity with the specified objectID. Returns true if the entity was found and deleted.
bool World::deleteEntity(OID objectID) {
    auto globalIt = entitiesInWorld.begin();
    bool foundInGlobal = false;

    while (globalIt != entitiesInWorld.end()) {
        if ((*globalIt)->selfID == objectID) {
            foundInGlobal = true;
            break;
        }

        ++globalIt;
    }

    if (!foundInGlobal)
        return false;

    uint chunkNumber = getChunkNumberForCoordinate((*globalIt)->selfPosition);
    auto chunkIt = entitiesInChunks.at(chunkNumber).begin();
    bool foundInChunk = false;

    while (chunkIt != entitiesInChunks.at(chunkNumber).end()) {
        if ((*chunkIt)->selfID == objectID) {
            foundInChunk = true;
            break;
        }

        ++chunkIt;
    }

    if (foundInChunk && foundInGlobal) {
        delete *globalIt;
        entitiesInWorld.erase(globalIt);
        entitiesInChunks.at(chunkNumber).erase(chunkIt);

        return true;
    }

    // If we could not find an entity with the given pointer, return false.
    return false;
}

// Sets the floor material of the specified tile to the given material. Returns true if successful.
bool World::setFloorMaterial(Coordinate cord, Material desiredMaterial) {
    if (cordOutsideBound(this->maxCord(), cord))
        return false;

    tilesInWorld[(cord.y * worldWidth) + cord.x].floorMaterial = desiredMaterial;
    tilesInWorld[(cord.y * worldWidth) + cord.x].floorDisplay = desiredMaterial.defaultDisplayFloor;

    return true;
}

// Sets the wall material of the specified tile to the given material. Returns true if successful.
bool World::setWallMaterial(Coordinate cord, Material desiredMaterial, uint startingHealth) {
    if (cordOutsideBound(this->maxCord(), cord))
        return false;

    tilesInWorld[(cord.y * worldWidth) + cord.x].wallMaterial = desiredMaterial;
    tilesInWorld[(cord.y * worldWidth) + cord.x].wallDisplay = desiredMaterial.defaultDisplayWall;
    tilesInWorld[(cord.y * worldWidth) + cord.x].wallHealth = startingHealth;

    return true;
}

// Returns the number of the chunk for the given coordinate.
uint World::getChunkNumberForCoordinate(const Coordinate &cord) {
    uint nChunksPerRow, chunkNumber;

    nChunksPerRow = worldWidth / chunkSize;
    if ((worldWidth % chunkSize) > 0)
        nChunksPerRow += 1;

    if (cordOutsideBound(this->maxCord(), cord))
        return maxChunkNumber;

    Coordinate chunkCord;
    chunkCord.y = cord.y / chunkSize;
    chunkCord.x = cord.x / chunkSize;
    chunkNumber = (chunkCord.y * nChunksPerRow) + chunkCord.x;

    assert(chunkNumber <= maxChunkNumber);

    return chunkNumber;
}

// Returns a vector of the numbers of the chunks in a given rectangle.
std::vector<uint> World::getChunksInRect(Coordinate rectStart, uint height, uint width) {
    std::vector<uint> result;
    bool outside = false;

    // If the minimum value of the rect is outside the world, return the overflow chunk (the max chunk.)
    if (cordOutsideBound(this->maxCord(), rectStart)) {
        result.push_back(maxChunkNumber);
        return result;
    }

    // If the max value of the chunk is outside the bounds of the world, resize it so it
    //   is, and set a flag so the overflow chunk can be included later.
    if ((rectStart.y + height) > maxCord().y) {
        height = maxCord().y - rectStart.y + 1;
        outside = true;
    }

    if ((rectStart.x + width) > maxCord().x) {
        width = maxCord().x - rectStart.x + 1;
        outside = true;
    }

    Coordinate rectEnd = Coordinate{(rectStart.x + width) - 1, (rectStart.y + height) - 1};
    Coordinate chunkEnd = Coordinate{rectEnd.x / chunkSize, rectEnd.y / chunkSize};
    Coordinate chunkStart = Coordinate{rectStart.x / chunkSize, rectStart.y / chunkSize};

    std::vector<Coordinate> chunkCordsInRect;

    while (chunkStart.x != (chunkEnd.x + 1)) {
        for (Coordinate tmpCord = chunkStart; tmpCord.y != (chunkEnd.y + 1); tmpCord.y += 1) {
            chunkCordsInRect.push_back(tmpCord);
        }
        chunkStart.x += 1;
    }

    for (const auto &it : chunkCordsInRect) {
        Coordinate tmpCrd = Coordinate{it.x * chunkSize, it.y * chunkSize};
        result.push_back(this->getChunkNumberForCoordinate(tmpCrd));
    }

    // If any portion of the rect was outside the chunk, verify the the overflow chunk (the max chunk) is in the vector.
    // If it is not, add it.
    if (outside) {
        uint tmpLast = result.at(result.size() - 1);
        if (tmpLast != maxChunkNumber) {
            result.push_back(maxChunkNumber);
        }
    }

    assert(result.size() <= (maxChunkNumber + 1));

    return result;
}

// Note: the order of the entities in the returned vector is not in any specific order.
std::vector<Entity *> World::getEntitiesInLine(Coordinate lineStart, Coordinate lineEnd) {
    std::vector<Entity *> result;

    // Bresenham's line algorithm. Adapted from https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm
    bool steep = (fabs(lineEnd.y - lineStart.y) > fabs(lineEnd.x - lineStart.x));
    if (steep) {
        std::swap(lineStart.x, lineStart.y);
        std::swap(lineEnd.x, lineEnd.y);
    }

    if (lineStart.x > lineEnd.x) {
        std::swap(lineStart.x, lineEnd.x);
        std::swap(lineStart.y, lineEnd.y);
    }

    const double dx = lineEnd.x - lineStart.x;
    const double dy = fabs(lineEnd.y - lineStart.y);
    const int yStep = (lineStart.y < lineEnd.y) ? 1 : -1;
    double error = dx / 2.0f;
    uint y = lineStart.y;

    for (uint x = lineStart.x; x < lineEnd.x; x++) {
        Coordinate currentPos = steep ? Coordinate{y, x} : Coordinate{x, y};

        Entity *tmpPtr = this->getEntityOnTile(currentPos);

        if (tmpPtr != nullptr)
            result.push_back(tmpPtr);

        error -= dy;
        if (error < 0) {
            y += yStep;
            error += dx;
        }
    }

    return result;
}

// Returns a vector containing pointers to any entities found in the given rectangle.
std::vector<Entity *> World::getEntitiesInRect(Coordinate rectStart, uint height, uint width) {
    std::vector<Entity *> result;

    if ((height == 0) || (width == 0))
        return result;

    std::vector<uint> cChunksInRect = this->getChunksInRect(rectStart, height, width);

    for (const auto &currentChunk : cChunksInRect) {
        for (const auto &currentEntity : entitiesInChunks.at(currentChunk)) {
            if (cordInsideRect(rectStart, height, width, currentEntity->selfPosition))
                result.push_back(currentEntity);
        }
    }

    return result;
}

// Returns a vector containing pointers to any entities found in the given circle.
std::vector<Entity *> World::getEntitiesInCircle(Coordinate circleCenter, uint radius) {
    std::vector<Entity *> result;

    uint rectSideLength = (radius * 2) + 1;
    Coordinate rectEquivalent = Coordinate{circleCenter.x - radius, circleCenter.y - radius};

    if (circleCenter.x < radius)
        rectEquivalent.x = 0;

    if (circleCenter.y < radius)
        rectEquivalent.y = 0;

    std::vector<Entity *> tmpVec = getEntitiesInRect(rectEquivalent, rectSideLength, rectSideLength);

    for (const auto &it : tmpVec) {
        if ((uint) ceil(distance(circleCenter, it->selfPosition) <= radius))
            result.push_back(it);
    }

    return result;
}

// Adds an item to the world. Returns true if successful.
bool World::addItem(Item *itemPtr, Coordinate cord, bool wasPreviouslyAdded) {
    // If the item pointer is a null pointer or the desired position is
    //   outside the bounds of the world, return false.
    if ((!itemPtr) || cordOutsideBound(this->maxCord(), cord))
        return false;

    // Set the IID and the position of the item.
    if (!wasPreviouslyAdded)
        itemPtr->selfID = nextAvailableIID++;
    itemPtr->selfPosition = cord;

    // Add the item to the world and its chunk.
    itemsInWorld.push_back(itemPtr);
    itemsInChunks.at(this->getChunkNumberForCoordinate(cord)).push_back(itemPtr);

    return true;
}

// Returns the items at the given coordinate.
std::vector<Item *> World::getItemsAtPos(Coordinate cord) {
    // If the given coordinate is outside the bounds of the world, return false.
    if (cordOutsideBound(this->maxCord(), cord))
        return std::vector<Item *>{};

    std::vector<Item *> result;
    uint chunkNumber = this->getChunkNumberForCoordinate(cord);
    for (auto itmPtr : itemsInChunks.at(chunkNumber))
        if (itmPtr->selfPosition == cord)
            result.push_back(itmPtr);

    return result;

}

// Unlinks the indicated item from world so that an entity may possess it. Returns true if successful.
bool World::unLinkItem(IID itemToUnlink) {
    // If the given IID has not been assigned yes, return false;
    if (itemToUnlink >= nextAvailableIID)
        return false;

    bool itemWasFoundInWorld = false;
    auto worldIterator = itemsInWorld.begin();
    while (worldIterator != itemsInWorld.end()) {
        if ((*worldIterator)->selfID == itemToUnlink) {
            itemWasFoundInWorld = true;
            break;
        }

    }

    if (!itemWasFoundInWorld)
        return false;

    uint chunkNumber = this->getChunkNumberForCoordinate((*worldIterator)->selfPosition);
    auto chunkIterator = itemsInChunks.at(chunkNumber).begin();
    while (chunkIterator != itemsInChunks.at(chunkNumber).end()) {
        if ((*chunkIterator)->selfID == itemToUnlink) {
            itemsInWorld.erase(worldIterator);
            itemsInChunks.at(chunkNumber).erase(chunkIterator);
            return true;
        }

    }

    return false;
}

// Deletes a linked item completely from the world. Returns true if successful.
bool World::deleteItem(IID itemToDelete) {
    // If the given IID has not been assigned yes, return false;
    if (itemToDelete >= nextAvailableIID)
        return false;

    bool itemWasFoundInWorld = false;
    auto worldIterator = itemsInWorld.begin();
    while (worldIterator != itemsInWorld.end()) {
        if ((*worldIterator)->selfID == itemToDelete) {
            itemWasFoundInWorld = true;
            break;
        }

    }

    if (!itemWasFoundInWorld)
        return false;

    uint chunkNumber = this->getChunkNumberForCoordinate((*worldIterator)->selfPosition);
    auto chunkIterator = itemsInChunks.at(chunkNumber).begin();
    while (chunkIterator != itemsInChunks.at(chunkNumber).end()) {
        if ((*chunkIterator)->selfID == itemToDelete) {
            delete *worldIterator;
            itemsInWorld.erase(worldIterator);
            itemsInChunks.at(chunkNumber).erase(chunkIterator);
            return true;
        }

    }

    return false;
}
