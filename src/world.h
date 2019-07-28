#ifndef WORLD_H
#define WORLD_H

#include "universal.h"
#include "material.h"
#include "Iworld.h"
#include "entity.h"
#include "tile.h"
#include "Item.h"
#include <list>
#include <vector>
#include <cassert>
#include <algorithm>
#include <stdexcept>

class World : public Iworld<Entity, Tile, Item> {
public:
    World(uint height, uint width); //NU
    ~World() override;

    // World functions
    Coordinate maxCord() override;

    DisplayArrayElement getDisplayInfoForTile(Coordinate cord) override;

    DisplayArray getDisplayArrayForWorld();

    void getDisplayArrayForWorld(DisplayArray &dis);

    // Returns how many ticks have passed since the creation of the world.
    inline uint getTickNumber() override { return tickNumber; }

    void tick();

    bool moveEntity(Entity *entityPtr, Coordinate cord) override;

    bool addEntity(Entity *entityToAdd, Coordinate cord) override;

    bool deleteEntity(std::list<Entity *>::iterator *it);

    bool deleteEntity(OID objectID);

    Entity *getEntityOnTile(Coordinate cord) override;

    std::vector<Entity *> getEntitiesInLine(Coordinate lineStart, Coordinate lineEnd) override;

    std::vector<Entity *> getEntitiesInRect(Coordinate rectStart, uint height, uint width) override;

    std::vector<Entity *> getEntitiesInCircle(Coordinate circleCenter, uint radius) override;

    // Returns a pointer to a tile at the specified coordinate.
    // Returns nullptr if the Coordinate is out of the bounds of the World.
    inline Tile *getTileAtPos(Coordinate cccord) override {
        return (cordOutsideBound(this->maxCord(), cccord)) ? nullptr : &tilesInWorld[cccord.x +
                                                                                     (cccord.y * worldWidth)];
    }

    bool setFloorMaterial(Coordinate cord, Material desiredMaterial) override;

    bool setWallMaterial(Coordinate cord, Material desiredMaterial, uint startingHealth) override;

    bool addItem(Item *itemPtr, Coordinate cord, bool wasPreviouslyAdded) override;

    std::vector<Item *> getItemsAtPos(Coordinate cord) override;

    bool unLinkItem(IID itemToUnlink) override;

    bool deleteItem(IID itemToDelete) override;

private:
    uint getChunkNumberForCoordinate(const Coordinate &cord);

    std::vector<uint> getChunksInRect(Coordinate rectStart, uint height, uint width);

    Tile *tilesInWorld;
    uint worldHeight, worldWidth, tickNumber, chunkSize, maxChunkNumber;
    OID nextAvailableOID;
    OID nextAvailableIID;
    std::list<Entity *> entitiesInWorld;
    std::vector<std::list<Entity *>> entitiesInChunks;
    std::list<Item *> itemsInWorld;
    std::vector<std::list<Item *>> itemsInChunks;
};

#endif