#include "world.h"

#include <memory>


World::World(uint height, uint width, uint energyPerTick = 100) {
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

    // Set the energy to be given to every entity per tick.
    givenEnergyPerTick = energyPerTick;

    isDataLocked = true;

    assert(chunkSize != 0);
    assert(width != 0);
    assert(height != 0);
    assert(map);
}

World::~World() {
    // Delete the TileMap.
    delete map;

    for (auto entity : entitiesInWorld)
        delete &entity.object();

    for (auto item : itemsInWorld)
        delete &item.object();
}

// Returns a pointer to the TileMap used by the world.
TileMap *World::getMap() {
    return map;
}

// Loads a preexisting DisplayArray with all the data needed to display the world.
void World::loadDisplayArray(DisplayArray &displayArray) {

    // Load the DisplayArray with the info for the tiles.
    map->loadDisplayArray(displayArray);

    // Scan through and load all items' info into the DisplayArray.
    for (auto &itemData : itemsInWorld) {
        if (!cordOutsideBound(map->maxCord(), itemData.coordinate())) {
            DisplayArrayElement &tmp = displayArray.displayData[getArrayIndex(itemData.coordinate(),
                                                                              displayArray.width)];

            tmp.ForegroundInfo = itemData.object().getDisplayID();
            tmp.ForegroundColor = itemData.object().getMaterial().color;
        }
    }

    // Scan through and load all entities' info into the DisplayArray.
    for (auto &entityData : entitiesInWorld) {
        if (!cordOutsideBound(map->maxCord(), entityData.coordinate())) {
            DisplayArrayElement &tmp = displayArray.displayData[getArrayIndex(entityData.coordinate(),
                                                                              displayArray.width)];

            tmp.ForegroundInfo = entityData.object().getDisplayID();
            tmp.ForegroundColor = entityData.object().getMaterial().color;
        }
    }
}

// Calls each entity's tick function.
void World::tick() {
    auto it = entitiesInWorld.begin();

    // Iterate through the list, executing every entity's tick function.
    while (it != entitiesInWorld.end()) {
        Ientity *entityPtr = &((*it).object());

        // If the entity* is null, skip this element.
        if (!entityPtr) {
            ++it;
            continue;
        }

        // Call the entity's tick function and store its returned state.
        const EffectedType returnState = entityPtr->tick(this, map, *it, givenEnergyPerTick);

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

    ++tickNumber;
}

// Moves an entity from one position to another. Returns true is successful.
// If the entity does not exist or the cord is outside the World, return false.
bool World::moveEntity(const ObjectAndData<Ientity, EID> &entityData, Coordinate desiredPosition) {
    // If the desired coordinate is outsize the World or the entity pointer is NULL, return false.
    if (cordOutsideBound(map->maxCord(), desiredPosition))
        return false;

    bool state = false;

    // Calculate what chunk the new and old positions are in.
    const uint newChunkNumber = getChunkNumberForCoordinate(desiredPosition);
    const uint oldChunkNumber = getChunkNumberForCoordinate(entityData.coordinate());

    // Create reference variables to the new and old chunks to simplify later code.
    list<ObjectAndData<Ientity, EID> *> &oldChunk = entitiesInChunks[oldChunkNumber];
    list<ObjectAndData<Ientity, EID> *> &newChunk = entitiesInChunks[newChunkNumber];

    // If the new position is not in the same chunk as the old position, we need to do some more work.
    if (newChunkNumber != oldChunkNumber) {
        // Create a variable to store the original reference (the reference to the entity in the old chunk)
        //   if we find it.
        ObjectAndData<Ientity, EID> *originalObjectDataReference = nullptr;
        bool wasOldReferenceFound = false;

        // Scan through every entity in the chunk.
        for (auto it = oldChunk.begin(); it != oldChunk.end(); ++it) {
            // If we have found the entity reference in the old chunk, store it, remove
            //   it from the old chunk, and set state to true.
            if (**it == entityData) {
                originalObjectDataReference = *it;
                oldChunk.erase(it);
                wasOldReferenceFound = true;
                break;
            }
        }

        // If the old reference was found and is valid, add the reference to the new chunk and set its position.
        if (originalObjectDataReference && wasOldReferenceFound) {
            isDataLocked = false;
            originalObjectDataReference->mutCoordinate() = desiredPosition;
            isDataLocked = true;
            newChunk.push_back(originalObjectDataReference);
            state = true;
        }
    } else {
        // If the new position is in the same chunk as the old position, find
        //   the reference and set the entity's position
        for (auto &it : newChunk) {
            if (*it == entityData) {
                isDataLocked = false;
                it->mutCoordinate() = desiredPosition;
                isDataLocked = true;
                state = true;
                break;
            }
        }
    }

    // Return state. (true if movement was successful.)
    return state;
}

// Returns the entity and/or the items at the given tile.
SearchResult<Ientity, EID, Iitem, IID>
World::getObjectsOnTile(Coordinate cord, const bool getEntities, const bool getItems) {
    SearchResult<Ientity, EID, Iitem, IID> result;

    // If the given coordinate is outside the bounds of the world, return;
    if (cordOutsideBound(map->maxCord(), cord))
        return result;

    // Find the chunk number for the coordinate and create a reference to that chunk.
    const uint chunkNumber = getChunkNumberForCoordinate(cord);
    vector<list<ObjectAndData<Ientity, EID> *> *> entityChunk;
    vector<list<ObjectAndData<Iitem, IID> *> *> itemChunk;
    entityChunk.push_back(&entitiesInChunks[chunkNumber]);
    itemChunk.push_back(&itemsInChunks[chunkNumber]);

    // If it was specified to look for entities, scan through the the entities in the
    //   chunk, looking for an entity that is on the specified tile.
    if (getEntities)
        result.entitiesFound = std::make_shared<ObjectSearchCircle<Ientity, EID>>(entityChunk, cord, 0);

    // If it was specified to look for items, scan through the the items in the
    //   chunk, looking for items that are on the specified tile.
    if (getItems)
        result.itemsFound = std::make_shared<ObjectSearchCircle<Iitem, IID>>(itemChunk, cord, 0);

    if (!result.entitiesFound)
        printf("Boi");

    return result;
}

// Adds the provided entity to the World at the indicated tile. Returns true if successful.
bool World::addEntity(Ientity *entityToAdd, Coordinate cord) {
    // If the desired coordinate is outsize the World or the entity pointer is NULL, return false.
    if (cordOutsideBound(map->maxCord(), cord) || !entityToAdd)
        return false;

    // Find the number of the chunk that the destination coordinate is in.
    const uint chunkNumber = getChunkNumberForCoordinate(cord);

    // Check every entity in the destination chunk to be sure that the destination tile is empty.
    for (const auto &entity : entitiesInChunks[chunkNumber]) {
        // If an entity is found at the destination, return false.
        if (entity->coordinate() == cord)
            return false;
    }

    // Set the entity's position and OID and add it to the World.
    entitiesInWorld.emplace_back(entityToAdd, &isDataLocked, false, nextAvailableOID++, cord);
    entitiesInChunks[chunkNumber].push_back(&entitiesInWorld.back());

    return true;
}

// Deletes an entity from the world based on it's pointer. Returns true if the entity was found and deleted.
bool World::deleteEntity(list<ObjectAndData<Ientity, EID>>::iterator *it) {
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

    const uint chunkNumber = getChunkNumberForCoordinate((**it).coordinate());
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
        delete &((**it).object());
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
        if ((*globalIt).id() == objectID) {
            foundInGlobal = true;
            break;
        }

        ++globalIt;
    }

    if (!foundInGlobal)
        return false;

    const uint chunkNumber = getChunkNumberForCoordinate((*globalIt).coordinate());
    auto chunkIt = entitiesInChunks.at(chunkNumber).begin();
    bool foundInChunk = false;

    while (chunkIt != entitiesInChunks.at(chunkNumber).end()) {
        if ((*chunkIt)->id() == objectID) {
            foundInChunk = true;
            break;
        }

        ++chunkIt;
    }

    if (foundInChunk && foundInGlobal) {
        delete &((*globalIt).object());
        entitiesInWorld.erase(globalIt);
        entitiesInChunks.at(chunkNumber).erase(chunkIt);

        return true;
    }

    // If we could not find an entity with the given pointer, return false.
    return false;
}

// Returns the number of the chunk for the given coordinate.
uint World::getChunkNumberForCoordinate(const Coordinate &cord) {

    uint nChunksPerRow = map->width() / chunkSize;
    if ((map->width() % chunkSize) > 0)
        nChunksPerRow += 1;

    if (cordOutsideBound(map->maxCord(), cord))
        return maxChunkNumber;

    Coordinate chunkCord;
    chunkCord.y = cord.y / chunkSize;
    chunkCord.x = cord.x / chunkSize;
    const uint chunkNumber = (chunkCord.y * nChunksPerRow) + chunkCord.x;

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
    chunkCordsInRect.reserve((chunkStart.x - chunkEnd.x) * (chunkStart.y - chunkEnd.y));

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
        if (result.back() != maxChunkNumber) {
            result.push_back(maxChunkNumber);
        }
    }

    return result;
}

// Returns a SearchResult containing references to any entities and/or items found in the given circle.
SearchResult<Ientity, EID, Iitem, IID>
World::getObjectsInCircle(Coordinate circleCenter, uint radius, bool getEntities, bool getItems) {
    SearchResult<Ientity, EID, Iitem, IID> result;

    const uint rectSideLength = (radius * 2) + 1;
    Coordinate rectEquivalent = Coordinate{circleCenter.x - radius, circleCenter.y - radius};

    if (circleCenter.x < radius)
        rectEquivalent.x = 0;

    if (circleCenter.y < radius)
        rectEquivalent.y = 0;

    // Find the numbers of the chunks in the rectangle.
    const vector<uint> cChunksInRect = this->getChunksInRect(rectEquivalent, rectSideLength, rectSideLength);
    vector<list<ObjectAndData<Ientity, EID> *>*> entityChunks;
    vector<list<ObjectAndData<Iitem, IID> *> *> itemChunks;
    for (const auto &chunk : cChunksInRect) {
        entityChunks.push_back(&entitiesInChunks[chunk]);
        itemChunks.push_back(&itemsInChunks[chunk]);
    }

    if (getEntities)
        result.entitiesFound = std::make_shared<ObjectSearchCircle<Ientity, EID>>(entityChunks, circleCenter, radius);

    if (getItems)
        result.itemsFound = std::make_shared<ObjectSearchCircle<Iitem, IID>>(itemChunks, circleCenter, radius);

    return result;
}

// Adds an item to the world. Returns true if successful.
bool World::addItem(Iitem *itemPtr, Coordinate cord) {
    // If the item pointer is a null pointer or the desired position is
    //   outside the bounds of the world, return false.
    if ((!itemPtr) || cordOutsideBound(map->maxCord(), cord))
        return false;

    // Add the item to the world and its chunk.
    itemsInWorld.emplace_back(itemPtr, &isDataLocked, false, nextAvailableIID++, cord);
    itemsInChunks[this->getChunkNumberForCoordinate(cord)].push_back(&itemsInWorld.back());

    return true;
}

// Deletes a linked item completely from the world. Returns true if successful.
bool World::deleteItem(IID itemToDelete) {
    // If the given IID has not been assigned yes, return false;
    if (itemToDelete >= nextAvailableIID)
        return false;

    bool itemWasFoundInWorld = false;

    auto worldIterator = itemsInWorld.begin();
    while (worldIterator != itemsInWorld.end()) {
        if ((*worldIterator).id() == itemToDelete) {
            itemWasFoundInWorld = true;
            break;
        }

    }

    if (!itemWasFoundInWorld)
        return false;

    uint chunkNumber = this->getChunkNumberForCoordinate((*worldIterator).coordinate());
    auto chunkIterator = itemsInChunks[chunkNumber].begin();
    while (chunkIterator != itemsInChunks.at(chunkNumber).end()) {
        if ((*chunkIterator)->id() == itemToDelete) {
            delete &((*worldIterator).object());
            itemsInWorld.erase(worldIterator);
            itemsInChunks[chunkNumber].erase(chunkIterator);
            return true;
        }

    }

    return false;
}
