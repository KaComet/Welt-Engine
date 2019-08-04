#ifndef IWORLD_H
#define IWORLD_H

#include "material.h"
#include "universal.h"
#include <vector>
#include <utility>

using namespace std;

template<class Object, class ID_Type>
struct ObjectAndPosition {
    explicit ObjectAndPosition(Object *pointer = nullptr,
                               Coordinate position = Coordinate{0, 0}, ID_Type ID_Number = 0)
            : pointer(pointer), position(position), ID_Number(ID_Number) {};

    Object *pointer;
    ID_Type ID_Number;
    Coordinate position;
};

// For determining if two ObjectAndPosition objects are the same.
template<class Object, typename ID_Type>
inline bool operator==(const ObjectAndPosition<Object, ID_Type> &op1, const ObjectAndPosition<Object, ID_Type> &op2) {
    if ((op1.position == op2.position) && (op1.pointer == op2.pointer))
        return true;
    else
        return false;
}

template<class EntityType, class ItemType>
struct SearchResult {
    vector<ObjectAndPosition<EntityType, EID>> entitiesFound;
    vector<ObjectAndPosition<ItemType, IID>> itemsFound;
};

template<class EntityType, class ItemType>
class Iworld {
public:
    Iworld() = default;

    virtual ~Iworld() = default;

    virtual uint getTickNumber() = 0;

    virtual bool moveEntity(const ObjectAndPosition<EntityType, EID> &entityData, Coordinate desiredPosition) = 0;

    virtual bool addEntity(EntityType *entityToAdd, Coordinate cord) = 0;

    virtual SearchResult<EntityType, ItemType> getObjectsOnTile(Coordinate cord, bool getEntities, bool getItems) = 0;

    virtual SearchResult<EntityType, ItemType>
    getObjectsInLine(Coordinate lineStart, Coordinate lineEnd, bool getEntities, bool getItems) = 0;

    virtual SearchResult<EntityType, ItemType>
    getObjectsInRect(Coordinate rectStart, uint height, uint width, bool getEntities, bool getItems) = 0;

    virtual SearchResult<EntityType, ItemType>
    getObjectsInCircle(Coordinate circleCenter, uint Radius, bool getEntities, bool getItems) = 0;

    virtual bool addItem(ItemType *itemPtr, Coordinate cord, bool wasPreviouslyAdded) = 0;

    virtual bool unLinkItem(IID itemToUnlink) = 0;

    virtual bool deleteItem(IID itemToDelete) = 0;
};

#endif