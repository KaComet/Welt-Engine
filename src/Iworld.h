#ifndef IWORLD_H
#define IWORLD_H

#include "material.h"
#include "universal.h"
#include <vector>
#include <utility>

using namespace std;

template<class Object>
struct ObjectAndPosition {
    explicit ObjectAndPosition(Object *pointer = nullptr, Coordinate position = Coordinate{0, 0})
            : pointer(pointer), position(position) {
    };

    Object *pointer;
    Coordinate position;
};

// For determining if two ObjectAndPosition objects are the same.
template<class Object>
inline bool operator==(const ObjectAndPosition<Object> &op1, const ObjectAndPosition<Object> &op2) {
    if ((op1.position == op2.position) && (op1.pointer == op2.pointer))
        return true;
    else
        return false;
}

template<class E, class I>
struct SearchResult {
    vector<ObjectAndPosition<E> *> entitiesFound;
    vector<ObjectAndPosition<I> *> itemsFound;
};

template<class E, class I>
class Iworld {
public:
    Iworld() = default;

    virtual ~Iworld() = default;

    virtual uint getTickNumber() = 0;

    virtual bool moveEntity(const ObjectAndPosition<E> &entityData, Coordinate desiredPosition) = 0;

    virtual bool addEntity(E *entityToAdd, Coordinate cord) = 0;

    virtual SearchResult<E, I> getObjectsOnTile(Coordinate cord, bool getEntities, bool getItems) = 0;

    virtual SearchResult<E, I>
    getObjectsInLine(Coordinate lineStart, Coordinate lineEnd, bool getEntities, bool getItems) = 0;

    virtual SearchResult<E, I>
    getObjectsInRect(Coordinate rectStart, uint height, uint width, bool getEntities, bool getItems) = 0;

    virtual SearchResult<E, I>
    getObjectsInCircle(Coordinate circleCenter, uint Radius, bool getEntities, bool getItems) = 0;

    virtual bool addItem(I *itemPtr, Coordinate cord, bool wasPreviouslyAdded) = 0;

    virtual bool unLinkItem(IID itemToUnlink) = 0;

    virtual bool deleteItem(IID itemToDelete) = 0;
};

#endif