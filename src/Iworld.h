#ifndef IWORLD_H
#define IWORLD_H

#include "material.h"
#include "universal.h"
#include <vector>
#include <utility>

using namespace std;

template<class Object, class ID_Type>
class ObjectAndData {
public:
    explicit ObjectAndData(Object *pointer, bool *lockPointer, bool isPlaceholder, ID_Type ID_Number,
                           Coordinate position) : _pointer(pointer), _lockPointer(lockPointer),
                                                  _isPlaceholder(isPlaceholder),
                                                  _position(position), _ID_Number(ID_Number) {};

private:
    Object *_pointer;
    ID_Type _ID_Number;
    Coordinate _position;
    bool *_lockPointer;
    bool _isPlaceholder;

public:
    Object &object() { return *_pointer; }

    ID_Type id() const { return _ID_Number; }

    Coordinate coordinate() const { return _position; }

    Coordinate &mutCoordinate() {
        //TODO: Change this to a custom exception.
        if (*_lockPointer)
            throw "Attempted to access a lock ObjectAndPointer.";
        else
            return _position;
    }

    bool isPlaceholder() const { return _isPlaceholder; }
};

// For determining if two ObjectAndData objects are the same.
template<class Object, typename ID_Type>
inline bool operator==(const ObjectAndData<Object, ID_Type> &op1, const ObjectAndData<Object, ID_Type> &op2) {
    return (op1.id() == op2.id()) && (op1.coordinate() == op2.coordinate());
}

template<class EntityType, class ItemType>
struct SearchResult {
    vector<ObjectAndData<EntityType, EID>*> entitiesFound;
    vector<ObjectAndData<ItemType, IID>*> itemsFound;
};

template<class EntityType, class ItemType>
class Iworld {
public:
    Iworld() = default;

    virtual ~Iworld() = default;

    virtual uint getTickNumber() = 0;

    virtual bool moveEntity(const ObjectAndData<EntityType, EID> &entityData, Coordinate desiredPosition) = 0;

    virtual bool addEntity(EntityType *entityToAdd, Coordinate cord) = 0;

    virtual SearchResult<EntityType, ItemType> getObjectsOnTile(Coordinate cord, bool getEntities, bool getItems) = 0;

    virtual SearchResult<EntityType, ItemType>
    getObjectsInLine(Coordinate lineStart, Coordinate lineEnd, bool getEntities, bool getItems) = 0;

    virtual SearchResult<EntityType, ItemType>
    getObjectsInRect(Coordinate rectStart, uint height, uint width, bool getEntities, bool getItems) = 0;

    virtual SearchResult<EntityType, ItemType>
    getObjectsInCircle(Coordinate circleCenter, uint Radius, bool getEntities, bool getItems) = 0;

    virtual bool addItem(ItemType *itemPtr, Coordinate cord) = 0;

    virtual bool deleteItem(IID itemToDelete) = 0;
};

#endif