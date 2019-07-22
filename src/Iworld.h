#ifndef IWORLD_H
#define IWORLD_H

#include "material.h"
#include "universal.h"

template<class E, class T>
class Iworld
{
public:
	Iworld() = default;
	virtual ~Iworld() = default;

	// World functions
	virtual Coordinate maxCord() = 0;
	virtual DisplayArrayElement getDisplayInfoForTile(Coordinate cord) = 0;
	virtual uint getTickNumber() = 0;

	// Entity functions
	virtual bool moveEntity(E* entityPtr, Coordinate cord) = 0;
	virtual bool addEntity(E* entityToAdd, Coordinate cord) = 0;
    virtual E* getEntityOnTile(Coordinate cord) = 0;
    virtual std::vector<E*> getEntitiesInLine(Coordinate lineStart, Coordinate lineEnd) = 0;
    virtual std::vector<E*> getEntitiesInRect(Coordinate rectStart, uint height, uint width) = 0;
    virtual std::vector<E*> getEntitiesInCircle(Coordinate circleCenter, uint Radius) = 0;

	// Tile functions
	virtual T* getTileAtPos(Coordinate cord) = 0;
	virtual bool setFloorMaterial(Coordinate cord, Material desiredMaterial) = 0;
	virtual bool setWallMaterial(Coordinate cord, Material desiredMaterial, uint startingHealth) = 0;
};

#endif