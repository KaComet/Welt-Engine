#ifndef WORLD_H
#define WORLD_H

#include "ObjectSearchCircle.h"
#include "IObjectSearch.h"
#include "DisplayIDdef.h"
#include "universal.h"
#include "material.h"
#include "TileMap.h"
#include "Iworld.h"
#include "Ientity.h"
#include "tile.h"
#include "Iitem.h"
#include <list>
#include <vector>
#include <memory>
#include <utility>
#include <cassert>
#include <algorithm>
#include <stdexcept>

using namespace std;

class World : public Iworld<Ientity, EID, Iitem, IID> {
public:
    World(uint height, uint width, uint energyPerTick);

    ~World() override;

    TileMap *getMap();

    // Returns how many ticks have passed since the creation of the world.
    inline uint getTickNumber() override { return tickNumber; }

    uint &energyPerTick() { return givenEnergyPerTick; }

    void loadDisplayArray(DisplayArray &displayArray);

    void tick();

    bool moveEntity(const ObjectAndData<Ientity, EID> &entityData, Coordinate desiredPosition) override;

    bool addEntity(Ientity *entityToAdd, Coordinate cord) override;

    bool deleteEntity(list<ObjectAndData<Ientity, EID>>::iterator *it);

    bool deleteEntity(EID objectID);

    SearchResult<Ientity, EID, Iitem, IID> getObjectsOnTile(Coordinate cord, bool getEntities, bool getItems) override;

    SearchResult<Ientity, EID, Iitem, IID>
    getObjectsInLine(Coordinate lineStart, Coordinate lineEnd, bool getEntities, bool getItems) override;

    SearchResult<Ientity, EID, Iitem, IID>
    getObjectsInRect(Coordinate rectStart, uint height, uint width, bool getEntities, bool getItems) override;

    SearchResult<Ientity, EID, Iitem, IID>
    getObjectsInCircle(Coordinate circleCenter, uint radius, bool getEntities, bool getItems) override;

    bool addItem(Iitem *itemPtr, Coordinate cord) override;

    bool deleteItem(IID itemToDelete) override;

private:
    uint getChunkNumberForCoordinate(const Coordinate &cord);

    vector<uint> getChunksInRect(const Coordinate &rectStart, uint height, uint width);

    TileMap *map;
    uint givenEnergyPerTick, tickNumber, chunkSize, maxChunkNumber;
    EID nextAvailableOID;
    EID nextAvailableIID;
    bool isDataLocked;
    list<ObjectAndData<Ientity, EID>> entitiesInWorld;
    vector<list<ObjectAndData<Ientity, EID> *>> entitiesInChunks;
    list<ObjectAndData<Iitem, IID>> itemsInWorld;
    vector<list<ObjectAndData<Iitem, IID> *>> itemsInChunks;
};

#endif