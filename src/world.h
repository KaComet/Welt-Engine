#ifndef WORLD_H
#define WORLD_H

#include "universal.h"
#include "material.h"
#include "Iworld.h"
#include "entity.h"
#include <cassert>
#include "tile.h"
#include <list>
#include <vector>
#include <algorithm>
#include <stdexcept>

class World : public Iworld<Entity, Tile> {
public:
    World(uint height, uint width); //NU
    ~World() override;

    // World functions
    Coordinate maxCord() override;
    DisplayArrayElement getDisplayInfoForTile(Coordinate cord) override; //NU
    DisplayArray getDisplayArrayForWorld(); //NU
    void getDisplayArrayForWorld(DisplayArray &dis);
    // Returns how many ticks have passed since the creation of the chunk.
    inline uint getTickNumber() override { return tickNumber; }

    // Entity functions
    void tick();
    bool moveEntity(Entity *entityPtr, Coordinate cord) override;
    bool addEntity(Entity *entityToAdd, Coordinate cord) override;
    bool deleteEntity(Entity *entityPtr);
    bool deleteEntity(std::list<Entity*>::iterator *it);
    bool deleteEntity(OID objectID);
    bool deleteEntityOnTile(Coordinate cord);
    Entity *getEntityOnTile(Coordinate cord) override;
    std::vector<Entity *> getEntitiesInLine(Coordinate lineStart, Coordinate lineEnd) override; //NU
    std::vector<Entity *> getEntitiesInRect(Coordinate rectStart, uint height, uint width) override;
    std::vector<Entity *> getEntitiesInCircle(Coordinate circleCenter, uint radius) override;
    // Tile functions
    // Returns a pointer to a tile at the specified coordinate.
    // Returns nullptr if the Coordinate is out of the bounds of the World.
    inline Tile *getTileAtPos(Coordinate cccord) override {
        return (cordOutsideBound(this->maxCord(), cccord)) ? nullptr : &tilesInChunk[cccord.x +
                                                                                     (cccord.y * worldWidth)];
    }

    bool setFloorMaterial(Coordinate cord, Material desiredMaterial) override;
    bool setWallMaterial(Coordinate cord, Material desiredMaterial, uint startingHealth) override;

private:
    uint getChunkNumberForCoordinate(const Coordinate &cord);
    std::vector<uint> getChunksInRect(Coordinate rectStart, uint height, uint width);

    Tile *tilesInChunk;
    uint worldHeight, worldWidth, tickNumber, chunkSize, maxChunkNumber;
    OID nextAvailableOID;
    std::list<Entity *> entitiesInWorld;
    std::vector<std::list<Entity *>> entitiesInChunks;
};

#endif