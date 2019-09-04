#ifndef IWORLD_H
#define IWORLD_H

#include "universal.h"
#include "ObjectAndData.h"
#include "IObjectSearch.h"
#include <vector>
#include <utility>

using namespace std;

// For determining if two ObjectAndData objects are the same.
template<class Object, typename ID_Type>
inline bool operator==(const ObjectAndData<Object, ID_Type> &op1, const ObjectAndData<Object, ID_Type> &op2) {
    return (op1.id() == op2.id()) && (op1.coordinate() == op2.coordinate());
}

template<class EntityType, class EntityID_Type, class ItemType, class ItemID_Type>
class SearchResult {
public:
    SearchResult(IObjectSearch<EntityType, EntityID_Type> *entitiesFound = nullptr,
                 IObjectSearch<ItemType, IID> *itemsFound = nullptr) : entitiesFound(entitiesFound),
                                                                       itemsFound(itemsFound) {

    }

    ~SearchResult() {
        delete entitiesFound;
        delete itemsFound;
    }

    IObjectSearch<EntityType, EntityID_Type> *entitiesFound;
    IObjectSearch<ItemType, ItemID_Type> *itemsFound;
};

template<class EntityType, class EntityID_Type, class ItemType, class ItemID_Type>
class Iworld {
public:
    Iworld() = default;

    virtual ~Iworld() = default;

    virtual uint getTickNumber() = 0;

    virtual bool moveEntity(const ObjectAndData<EntityType, EID> &entityData, Coordinate desiredPosition) = 0;

    virtual bool addEntity(EntityType *entityToAdd, Coordinate cord) = 0;

    virtual SearchResult<EntityType, EntityID_Type, ItemType, ItemID_Type>
    getObjectsOnTile(Coordinate cord, bool getEntities, bool getItems) = 0;

    virtual SearchResult<EntityType, EntityID_Type, ItemType, ItemID_Type>
    getObjectsInLine(Coordinate lineStart, Coordinate lineEnd, bool getEntities, bool getItems) = 0;

    virtual SearchResult<EntityType, EntityID_Type, ItemType, ItemID_Type>
    getObjectsInRect(Coordinate rectStart, uint height, uint width, bool getEntities, bool getItems) = 0;

    virtual SearchResult<EntityType, EntityID_Type, ItemType, ItemID_Type>
    getObjectsInCircle(Coordinate circleCenter, uint Radius, bool getEntities, bool getItems) = 0;

    virtual bool addItem(ItemType *itemPtr, Coordinate cord) = 0;

    virtual bool deleteItem(IID itemToDelete) = 0;
};

#endif