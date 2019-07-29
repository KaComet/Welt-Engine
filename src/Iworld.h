#ifndef IWORLD_H
#define IWORLD_H

#include "material.h"
#include "universal.h"
#include <vector>
#include <utility>

template<class E, class T, class I>
class Iworld {
public:
    Iworld() = default;

    virtual ~Iworld() = default;

    virtual Coordinate maxCord() = 0;

    virtual DisplayArrayElement getDisplayInfoForTile(Coordinate cord) = 0;

    virtual uint getTickNumber() = 0;

    virtual bool moveEntity(E *entityPtr, Coordinate cord) = 0;

    virtual bool addEntity(E *entityToAdd, Coordinate cord) = 0;

    virtual std::pair<E *, std::vector<I *>> getObjectsOnTile(Coordinate cord, bool getEntities, bool getItems) = 0;

    virtual std::pair<std::vector<E *>, std::vector<I *>>
    getObjectsInLine(Coordinate lineStart, Coordinate lineEnd, bool getEntities, bool getItems) = 0;

    virtual std::pair<std::vector<E *>, std::vector<I *>>
    getObjectsInRect(Coordinate rectStart, uint height, uint width, bool getEntities, bool getItems) = 0;

    virtual std::pair<std::vector<E *>, std::vector<I *>>
    getObjectsInCircle(Coordinate circleCenter, uint Radius, bool getEntities, bool getItems) = 0;

    virtual T *getTileAtPos(Coordinate cord) = 0;

    virtual bool setFloorMaterial(Coordinate cord, Material desiredMaterial) = 0;

    virtual bool setWallMaterial(Coordinate cord, Material desiredMaterial, uint startingHealth) = 0;

    virtual bool addItem(I *itemPtr, Coordinate cord, bool wasPreviouslyAdded) = 0;

    virtual bool unLinkItem(IID itemToUnlink) = 0;

    virtual bool deleteItem(IID itemToDelete) = 0;
};

#endif