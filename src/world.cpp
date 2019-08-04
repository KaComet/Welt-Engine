#include "world.h"


World::World(uint height, uint width) {
    // Make sure the World has valid dimensions. If something is wrong, throw invalid_argument.
    if ((height == 0) || (width == 0))
        throw invalid_argument("Cannot create a World with any dimension that is zero");

    // Create the tiles in the World. If enough memory cannot be allocated, throw bad_alloc.
    try {
        map = new TileMap(height, width);
    } catch (bad_alloc &bad) {
        throw bad;
    }

    // Start the OID, IID, and tick counters.
    nextAvailableOID = 0;
    nextAvailableIID = 0;
    tickNumber = 0;
    chunkSize = 16;

    // Initialize the entity and item chunks and calculate the max chunk number.
    const uint nChunksPerRow = (uint) ceil((double) width / (double) chunkSize);
    Coordinate maxChunkCord;
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
    // Delete all of the items, entities, and the TileMap.
    delete map;
    for (ObjectAndPosition<Entity, EID> entityData : entitiesInWorld)
        delete entityData.pointer;

    for (ObjectAndPosition<Item, IID> itemData : itemsInWorld)
        delete itemData.pointer;
}

// Returns a pointer to the TileMap used by the world.
TileMap *World::getMap() {
    return map;
}

// Loads a preexisting display array with all the data needed to display the world.
void World::loadDisplayArray(DisplayArray &displayArray) {
    map->loadDisplayArray(displayArray);

    // Scan through and load all the entities's info into the DisplayArray.
    for (const auto &entityData : entitiesInWorld) {
        if (!cordOutsideBound(map->maxCord(), entityData.position)) {
            DisplayArrayElement &tmp = displayArray.displayData[entityData.position.x +
                                                                 (entityData.position.y * displayArray.width)];

            tmp.ForegroundInfo = entityData.pointer->entityDisplay;
            tmp.ForegroundColor = entityData.pointer->selfMaterial.color;
        }
    }
}

// Calls each entity's tick function.
void World::tick() {
    auto it = entitiesInWorld.begin();

    // Iterate through the list, executing every entity's tick function.
    while (it != entitiesInWorld.end()) {
        Entity *entityPtr = (*it).pointer;

        // If the entity* is null, skip this element.
        if (!entityPtr) {
            ++it;
            continue;
        }

        // Call the entity's tick function and store its returned state.
        const EffectedType returnState = entityPtr->tick(this, map, *it);

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
bool World::moveEntity(const ObjectAndPosition<Entity, EID> &entityData, Coordinate desiredPosition) {
    // If the desired coordinate is outsize the World or the entity pointer is NULL, return false.
    if (cordOutsideBound(map->maxCord(), desiredPosition) || !entityData.pointer)
        return false;

    bool state = false;

    // Calculate what chunk the new and old positions are in.
    const uint newChunkNumber = getChunkNumberForCoordinate(desiredPosition);
    const uint oldChunkNumber = getChunkNumberForCoordinate(entityData.position);

    // Create reference variables to the new and old chunks to simplify later code.
    list<ObjectAndPosition<Entity, EID> *> &oldChunk = entitiesInChunks.at(oldChunkNumber);
    list<ObjectAndPosition<Entity, EID> *> &newChunk = entitiesInChunks.at(newChunkNumber);

    // If the new position is not in the same chunk as the old position, we need to do some more work.
    if (newChunkNumber != oldChunkNumber) {
        // Create a variable to store the original reference (the reference to the entity in the old chunk)
        //   if we find it.
        ObjectAndPosition<Entity, EID> *originalObjectDataReference = nullptr;

        // Scan through every entity in the chunk.
        for (auto it = oldChunk.begin(); it != oldChunk.end(); it++) {
            // If we have found the entity reference in the old chunk, store it, remove
            //   it from the old chunk, and set state to true.
            if (**it == entityData) {
                originalObjectDataReference = *it;
                oldChunk.erase(it);
                state = true;
                break;
            }
        }

        // If the old reference was found and is valid, add the reference to the new chunk and set its position.
        if (state && originalObjectDataReference) {
            originalObjectDataReference->position = desiredPosition;
            newChunk.push_back(originalObjectDataReference);
        }
    } else {
        // If the new position is in the same chunk as the old position, find
        //   the reference and set the entity's position
        for (auto &it : newChunk) {
            if (*it == entityData) {
                it->position = desiredPosition;
                state = true;
                break;
            }
        }
    }

    // Return state. (true if movement was successful.)
    return state;
}

// Returns the entity and/or the items at the given tile.
SearchResult<Entity, Item> World::getObjectsOnTile(Coordinate cord, const bool getEntities, const bool getItems) {
    SearchResult<Entity, Item> result;

    // If the given coordinate is outside the bounds of the world, return;
    if (cordOutsideBound(map->maxCord(), cord))
        return result;

    // Find the chunk number for the coordinate and create a reference to that chunk.
    const uint chunkNumber = getChunkNumberForCoordinate(cord);

    // If it was specified to look for entities, scan through the the entities in the
    //   chunk, looking for an entity that is on the specified tile.
    if (getEntities) {
        for (auto &entity : entitiesInChunks.at(chunkNumber)) {
            if (entity->position == cord) {
                result.entitiesFound.push_back(*entity);
                break;
            }
        }
    }

    // If it was specified to look for items, scan through the the items in the
    //   chunk, looking for items that are on the specified tile.
    if (getItems) {
        for (auto &itmPtr : itemsInChunks.at(chunkNumber))
            if (itmPtr->position == cord)
                result.itemsFound.push_back(*itmPtr);
    }

    return result;
}

// Adds the provided entity to the World at the indicated tile. Returns true if successful.
bool World::addEntity(Entity *entityToAdd, Coordinate cord) {
    // If the desired coordinate is outsize the World or the entity pointer is NULL, return false.
    if (cordOutsideBound(map->maxCord(), cord) || !entityToAdd)
        return false;

    // Find the number of the chunk that the destination coordinate is in.
    const uint chunkNumber = getChunkNumberForCoordinate(cord);

    // Check every entity in the destination chunk to be sure that the destination tile is empty.
    for (const auto & entity : entitiesInChunks.at(chunkNumber))
    {
        // If an entity is found at the destination, return false.
        if (entity->position == cord)
            return false;
    }

    // Set the entity's position and OID and add it to the World.
    ObjectAndPosition<Entity, EID> entityData = ObjectAndPosition<Entity, EID>{entityToAdd, cord};
    entityData.ID_Number = nextAvailableOID;
    entitiesInWorld.push_back(entityData);
    entitiesInChunks.at(chunkNumber).push_back(&entitiesInWorld.back());

    // Increment the OID counter
    nextAvailableOID++;

    return true;
}

// Deletes an entity from the world based on it's pointer. Returns true if the entity was found and deleted.
bool World::deleteEntity(list<ObjectAndPosition<Entity, EID>>::iterator *it) {
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

    const uint chunkNumber = getChunkNumberForCoordinate((**it).position);
    auto chunkIt = entitiesInChunks.at(chunkNumber).begin();
    bool foundInChunk = false;

    while (chunkIt != entitiesInChunks.at(chunkNumber).end()) {
        if (**chunkIt == **it) {
            foundInChunk = true;
            break;
        }

        ++chunkIt;
    }

    if (foundInChunk && foundInGlobal) {
        delete (**it).pointer;
        *it = entitiesInWorld.erase(*it);
        entitiesInChunks.at(chunkNumber).erase(chunkIt);

        return true;
    }

    // If we could not find an entity with the given pointer, return false.
    return false;
}

// Deletes the entity with the specified objectID. Returns true if the entity was found and deleted.
bool World::deleteEntity(EID objectID) {
    auto globalIt = entitiesInWorld.begin();
    bool foundInGlobal = false;

    while (globalIt != entitiesInWorld.end()) {
        if ((*globalIt).ID_Number == objectID) {
            foundInGlobal = true;
            break;
        }

        ++globalIt;
    }

    if (!foundInGlobal)
        return false;

    const uint chunkNumber = getChunkNumberForCoordinate((*globalIt).position);
    auto chunkIt = entitiesInChunks.at(chunkNumber).begin();
    bool foundInChunk = false;

    while (chunkIt != entitiesInChunks.at(chunkNumber).end()) {
        if ((*chunkIt)->ID_Number == objectID) {
            foundInChunk = true;
            break;
        }

        ++chunkIt;
    }

    if (foundInChunk && foundInGlobal) {
        delete (*globalIt).pointer;
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
vector<uint> World::getChunksInRect(const Coordinate &rectStart, uint height, uint width) {
    vector<uint> result;
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

    const Coordinate rectEnd = Coordinate{(rectStart.x + width) - 1, (rectStart.y + height) - 1};
    const Coordinate chunkEnd = Coordinate{rectEnd.x / chunkSize, rectEnd.y / chunkSize};
    Coordinate chunkStart = Coordinate{rectStart.x / chunkSize, rectStart.y / chunkSize};

    vector<Coordinate> chunkCordsInRect;

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

// Note: the order of the entities in the returned vector are not in any specific order.
SearchResult<Entity, Item>
World::getObjectsInLine(Coordinate lineStart, Coordinate lineEnd, bool getEntities, bool getItems) {
    SearchResult<Entity, Item> result;

    // Bresenham's line algorithm. Adapted from https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm
    bool steep = (fabs(lineEnd.y - lineStart.y) > fabs(lineEnd.x - lineStart.x));
    if (steep) {
        swap(lineStart.x, lineStart.y);
        swap(lineEnd.x, lineEnd.y);
    }

    if (lineStart.x > lineEnd.x) {
        swap(lineStart.x, lineEnd.x);
        swap(lineStart.y, lineEnd.y);
    }

    const double dx = lineEnd.x - lineStart.x;
    const double dy = fabs(lineEnd.y - lineStart.y);
    const int yStep = (lineStart.y < lineEnd.y) ? 1 : -1;
    double error = dx / 2.0f;
    uint y = lineStart.y;

    for (uint x = lineStart.x; x < lineEnd.x; x++) {
        Coordinate currentPos = steep ? Coordinate{y, x} : Coordinate{x, y};

        auto tileResult = this->getObjectsOnTile(currentPos, getEntities, getItems);

        if (getEntities && !tileResult.entitiesFound.empty())
            result.entitiesFound.insert(result.entitiesFound.end(), tileResult.entitiesFound.begin(),
                                        tileResult.entitiesFound.end());

        if (getItems && !tileResult.itemsFound.empty())
            result.itemsFound.insert(result.itemsFound.end(), tileResult.itemsFound.begin(),
                                     tileResult.itemsFound.end());

        error -= dy;
        if (error < 0) {
            y += yStep;
            error += dx;
        }
    }

    return result;
}

// Returns vectors containing pointers to any entities and/or items found in the given rectangle.
SearchResult<Entity, Item>
World::getObjectsInRect(Coordinate rectStart, uint height, uint width, bool getEntities, bool getItems) {
    SearchResult<Entity, Item> result;

    // If the width and/or the height is zero, return.
    if ((height == 0) || (width == 0))
        return result;

    // Find the numbers of the chunks in the rectangle.
    const vector<uint> cChunksInRect = this->getChunksInRect(rectStart, height, width);

    for (const auto &currentChunk : cChunksInRect) {
        if (getEntities) {
            for (const auto &currentEntity : entitiesInChunks.at(currentChunk)) {
                if (currentEntity->pointer && cordInsideRect(rectStart, height, width, currentEntity->position))
                    result.entitiesFound.push_back(*currentEntity);
            }
        }

        if (getItems) {
            for (const auto &currentItem : itemsInChunks.at(currentChunk)) {
                if (cordInsideRect(rectStart, height, width, currentItem->position))
                    result.itemsFound.push_back(*currentItem);
            }
        }
    }

    return result;
}

// Returns vectors containing pointers to any entities and/or items found in the given circle.
SearchResult<Entity, Item>
World::getObjectsInCircle(Coordinate circleCenter, uint radius, bool getEntities, bool getItems) {
    SearchResult<Entity, Item> result;

    const uint rectSideLength = (radius * 2) + 1;
    Coordinate rectEquivalent = Coordinate{circleCenter.x - radius, circleCenter.y - radius};

    if (circleCenter.x < radius)
        rectEquivalent.x = 0;

    if (circleCenter.y < radius)
        rectEquivalent.y = 0;

    const SearchResult<Entity, Item> searchResult = getObjectsInRect(rectEquivalent, rectSideLength, rectSideLength,
                                                                     getEntities, getItems);

    if (getEntities) {
        for (const auto &it : searchResult.entitiesFound) {
            if ((uint) ceil(distance(circleCenter, it.position) <= radius))
                result.entitiesFound.push_back(it);
        }
    }

    if (getItems) {
        for (const auto &it : searchResult.itemsFound) {
            if ((uint) ceil(distance(circleCenter, it.position) <= radius))
                result.itemsFound.push_back(it);
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

    ObjectAndPosition<Item, IID> newItem = ObjectAndPosition<Item, IID>{itemPtr, cord};
    // Set the IID and the position of the item.
    if (!wasPreviouslyAdded)
        itemPtr->selfID = nextAvailableIID++;
    itemPtr->selfPosition = cord;

    // Add the item to the world and its chunk.
    itemsInWorld.push_back(newItem);
    ObjectAndPosition<Item, IID> *tmpPtr = &itemsInWorld.back();
    itemsInChunks.at(this->getChunkNumberForCoordinate(cord)).push_back(tmpPtr);

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
        if ((*worldIterator).pointer->selfID == itemToUnlink) {
            itemWasFoundInWorld = true;
            break;
        }

    }

    if (!itemWasFoundInWorld)
        return false;

    uint chunkNumber = this->getChunkNumberForCoordinate((*worldIterator).position);
    auto chunkIterator = itemsInChunks.at(chunkNumber).begin();
    while (chunkIterator != itemsInChunks.at(chunkNumber).end()) {
        if ((*chunkIterator)->pointer->selfID == itemToUnlink) {
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
        if ((*worldIterator).pointer->selfID == itemToDelete) {
            itemWasFoundInWorld = true;
            break;
        }

    }

    if (!itemWasFoundInWorld)
        return false;

    uint chunkNumber = this->getChunkNumberForCoordinate((*worldIterator).position);
    auto chunkIterator = itemsInChunks.at(chunkNumber).begin();
    while (chunkIterator != itemsInChunks.at(chunkNumber).end()) {
        if ((*chunkIterator)->pointer->selfID == itemToDelete) {
            delete (*worldIterator).pointer;
            itemsInWorld.erase(worldIterator);
            itemsInChunks.at(chunkNumber).erase(chunkIterator);
            return true;
        }

    }

    return false;
}

