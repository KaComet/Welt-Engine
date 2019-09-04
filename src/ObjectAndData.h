#ifndef WELT_OBJECTANDDATA_H
#define WELT_OBJECTANDDATA_H

#include "universal.h"

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


#endif //WELT_OBJECTANDDATA_H
