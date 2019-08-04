#ifndef WORLD_H
#define WORLD_H

#include "DisplayIDdef.h"
#include "universal.h"
#include "material.h"
#include "TileMap.h"
#include "Iworld.h"
#include "entity.h"
#include "tile.h"
#include "Item.h"
#include <list>
#include <vector>
#include <utility>
#include <cassert>
#include <algorithm>
#include <stdexcept>

using namespace std;

class World : public Iworld<Entity, Item> {
public:
    World(uint height, uint width);

    ~World() override;

    TileMap *getMap();

    // Returns how many ticks have passed since the creation of the world.
    inline uint getTickNumber() override { return tickNumber; }

    void loadDisplayArray(DisplayArray &displayArray);

    void tick();

    bool moveEntity(const ObjectAndPosition<Entity, EID> &entityData, Coordinate desiredPosition) override;

    bool addEntity(Entity *entityToAdd, Coordinate cord) override;

    bool deleteEntity(list<ObjectAndPosition<Entity, EID>>::iterator *it);

    bool deleteEntity(EID objectID);

    SearchResult<Entity, Item> getObjectsOnTile(Coordinate cord, bool getEntities, bool getItems) override;

    SearchResult<Entity, Item>
    getObjectsInLine(Coordinate lineStart, Coordinate lineEnd, bool getEntities, bool getItems) override;

    SearchResult<Entity, Item>
    getObjectsInRect(Coordinate rectStart, uint height, uint width, bool getEntities, bool getItems) override;

    SearchResult<Entity, Item>
    getObjectsInCircle(Coordinate circleCenter, uint radius, bool getEntities, bool getItems) override;

    bool addItem(Item *itemPtr, Coordinate cord, bool wasPreviouslyAdded) override;

    bool unLinkItem(IID itemToUnlink) override;

    bool deleteItem(IID itemToDelete) override;

private:
    uint getChunkNumberForCoordinate(const Coordinate &cord);

    vector<uint> getChunksInRect(const Coordinate &rectStart, uint height, uint width);

    TileMap *map;
    uint tickNumber, chunkSize, maxChunkNumber;
    EID nextAvailableOID;
    EID nextAvailableIID;
    list<ObjectAndPosition<Entity, EID>> entitiesInWorld;
    vector<list<ObjectAndPosition<Entity, EID> *>> entitiesInChunks;
    list<ObjectAndPosition<Item, EID>> itemsInWorld;
    vector<list<ObjectAndPosition<Item, EID> *>> itemsInChunks;
};

#endif