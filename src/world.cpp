#include "world.h"


World::World(uint height, uint width) {
    // Make sure the World has valid dimensions. If something is wrong, throw invalid_argument.
    if ((height == 0) || (width == 0))
        throw std::invalid_argument("Cannot create a World with any dimension that is zero");

    // Create the tiles in the World. If enough memory cannot be allocated, throw bad_alloc.
    try {
        map = new TileMap(width, height);
    } catch (std::bad_alloc &bad) {
        throw bad;
    }

    // Start the OID, IID, and tick counters.
    nextAvailableOID = 0;
    nextAvailableIID = 0;
    tickNumber = 0;
    chunkSize = 16;

    // Initialize the entity and item chunks and calculate the max chunk number.
    Coordinate maxChunkCord;
    uint nChunksPerRow = (uint) std::ceil((double) width / (double) chunkSize);
    maxChunkCord.y = height / chunkSize;
    maxChunkCord.x = width / chunkSize;
    maxChunkNumber = (maxChunkCord.y * nChunksPerRow) + maxChunkCord.x;
    entitiesInChunks.resize(maxChunkNumber + 1);
    itemsInChunks.resize(maxChunkNumber + 1);

    assert(chunkSize != 0);
    assert(width != 0);
    assert(height != 0);
    assert(map);
}

World::~World() {
    // Delete all of the tiles, entities, and items in the World.
    delete map;
    for (Entity *ent : entitiesInWorld)
        delete ent;

    for (Item *itm : itemsInWorld)
        delete itm;
}

// Returns a pointer to the TileMap used by the world.
TileMap *World::getMap() {
    return map;
}

void World::loadDisplayArray(DisplayArray &displayArray) {
    map->loadDisplayArray(displayArray);

    for (const auto &entity : entitiesInWorld) {
        if (!cordOutsideBound(map->maxCord(), entity->selfPosition)) {
            DisplayArrayElement *tmp = &displayArray.displayData[entity->selfPosition.x +
                                                                 (entity->selfPosition.y * displayArray.width)];

            tmp->ForegroundInfo = entity->entityDisplay;
            tmp->ForegroundColor = entity->selfMaterial.color;
        }
    }
}

// Calls each entity's tick function.
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
        EffectedType returnState = entityPtr->tick(this, map);

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
    if (cordOutsideBound(map->maxCord(), cord) || (entityPtr == nullptr))
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

// Returns the entity and/or the items at the given tile.
std::pair<Entity *, std::vector<Item *>>
World::getObjectsOnTile(Coordinate cord, const bool getEntities, const bool getItems) {
    std::pair<Entity *, std::vector<Item *>> result;
    result.first = nullptr;

    // If the given coordinate is outside the bounds of the world, return;
    if (cordOutsideBound(map->maxCord(), cord))
        return result;

    // Find the chunk number for the coordinate.
    uint chunkNumber = getChunkNumberForCoordinate(cord);

    // If it was specified to look for entities, scan through the the entities in the
    //   chunk, looking for an entity that is on the specified tile.
    if (getEntities) {
        auto chunkIt = entitiesInChunks.at(chunkNumber).begin();

        while (chunkIt != entitiesInChunks.at(chunkNumber).end()) {
            if ((*chunkIt)->selfPosition == cord) {
                result.first = *chunkIt;
            }

            chunkIt++;
        }
    }

    // If it was specified to look for items, scan through the the items in the
    //   chunk, looking for items that are on the specified tile.
    if (getItems) {
        for (auto itmPtr : itemsInChunks.at(chunkNumber))
            if (itmPtr->selfPosition == cord)
                result.second.push_back(itmPtr);
    }

    return result;
}

// Adds the provided entity to the World at the indicated tile. Returns true if successful.
bool World::addEntity(Entity *entityToAdd, Coordinate cord) {
    // If the desired coordinate is outsize the World or the entity pointer is NULL, return false.
    if (cordOutsideBound(map->maxCord(), cord) || (entityToAdd == nullptr))
        if (cordOutsideBound(map->maxCord(), cord) || (entityToAdd == nullptr))
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

// Returns the number of the chunk for the given coordinate.
uint World::getChunkNumberForCoordinate(const Coordinate &cord) {
    uint nChunksPerRow, chunkNumber;

    nChunksPerRow = map->width() / chunkSize;
    if ((map->width() % chunkSize) > 0)
        nChunksPerRow += 1;

    if (cordOutsideBound(map->maxCord(), cord))
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
    if (cordOutsideBound(map->maxCord(), rectStart)) {
        result.push_back(maxChunkNumber);
        return result;
    }

    // If the max value of the chunk is outside the bounds of the world, resize it so it
    //   is, and set a flag so the overflow chunk can be included later.
    if ((rectStart.y + height) > map->maxCord().y) {
        height = map->maxCord().y - rectStart.y + 1;
        outside = true;
    }

    if ((rectStart.x + width) > map->maxCord().x) {
        width = map->maxCord().x - rectStart.x + 1;
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
std::pair<std::vector<Entity *>, std::vector<Item *>>
World::getObjectsInLine(Coordinate lineStart, Coordinate lineEnd, bool getEntities, bool getItems) {
    std::pair<std::vector<Entity *>, std::vector<Item *>> result;

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

        auto tileResult = this->getObjectsOnTile(currentPos, getEntities, getItems);

        if (tileResult.first)
            result.first.push_back(tileResult.first);

        if (!tileResult.second.empty())
            result.second.insert(result.second.end(), tileResult.second.begin(), tileResult.second.end());

        error -= dy;
        if (error < 0) {
            y += yStep;
            error += dx;
        }
    }

    return result;
}

// Returns vectors containing pointers to any entities and/or items found in the given rectangle.
std::pair<std::vector<Entity *>, std::vector<Item *>>
World::getObjectsInRect(Coordinate rectStart, uint height, uint width, bool getEntities, bool getItems) {
    std::pair<std::vector<Entity *>, std::vector<Item *>> result;

    // If the width and/or the height is zero, return.
    if ((height == 0) || (width == 0))
        return result;

    // Find the numbers of the chunks in the rectangle.
    const std::vector<uint> cChunksInRect = this->getChunksInRect(rectStart, height, width);

    for (const auto &currentChunk : cChunksInRect) {
        if (getEntities) {
            for (const auto &currentEntity : entitiesInChunks.at(currentChunk)) {
                if (cordInsideRect(rectStart, height, width, currentEntity->selfPosition))
                    result.first.push_back(currentEntity);
            }
        }

        if (getItems) {
            for (const auto &currentItem : itemsInChunks.at(currentChunk)) {
                if (cordInsideRect(rectStart, height, width, currentItem->selfPosition))
                    result.second.push_back(currentItem);
            }
        }
    }

    return result;
}

// Returns vectors containing pointers to any entities and/or items found in the given circle.
std::pair<std::vector<Entity *>, std::vector<Item *>>
World::getObjectsInCircle(Coordinate circleCenter, uint radius, bool getEntities, bool getItems) {
    std::pair<std::vector<Entity *>, std::vector<Item *>> result;

    const uint rectSideLength = (radius * 2) + 1;
    Coordinate rectEquivalent = Coordinate{circleCenter.x - radius, circleCenter.y - radius};

    if (circleCenter.x < radius)
        rectEquivalent.x = 0;

    if (circleCenter.y < radius)
        rectEquivalent.y = 0;

    const std::pair<std::vector<Entity *>, std::vector<Item *>> tmpVec = getObjectsInRect(rectEquivalent,
                                                                                          rectSideLength,
                                                                                          rectSideLength, getEntities,
                                                                                          getItems);

    if (getEntities) {
        for (const auto &it : tmpVec.first) {
            if ((uint) ceil(distance(circleCenter, it->selfPosition) <= radius))
                result.first.push_back(it);
        }
    }

    if (getItems) {
        for (const auto &it : tmpVec.second) {
            if ((uint) ceil(distance(circleCenter, it->selfPosition) <= radius))
                result.second.push_back(it);
        }
    }

    return result;
}

// Adds an item to the world. Returns true if successful.
bool World::addItem(Item *itemPtr, Coordinate cord, bool wasPreviouslyAdded) {
    // If the item pointer is a null pointer or the desired position is
    //   outside the bounds of the world, return false.
    if ((!itemPtr) || cordOutsideBound(map->maxCord(), cord))
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

