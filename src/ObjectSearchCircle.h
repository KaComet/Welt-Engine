#ifndef WELT_OBJECTSEARCHCIRCLE_H
#define WELT_OBJECTSEARCHCIRCLE_H


#include <vector>
#include <list>
#include "universal.h"
#include "IObjectSearch.h"
#include "ObjectAndData.h"

using namespace std;

template<class ObjectType, class ID_Type>
class ObjectSearchCircle : public IObjectSearch<ObjectType, ID_Type> {
public:
    explicit ObjectSearchCircle(vector<list<ObjectAndData<ObjectType, ID_Type> *> *> chunksReference,
                                Coordinate center, uint radius);

    virtual ~ObjectSearchCircle() = default;

    void next() override;

    ID_Type id() const override;

    Coordinate position() const override;

    ObjectType &object() override;

    ObjectAndData<ObjectType, ID_Type> ObjectAndDataCopy() const override;

protected:
    uint _radius;
    Coordinate _center;
    typename vector<list<ObjectAndData<ObjectType, ID_Type> *> *>::iterator _chunksIterator;
    typename list<ObjectAndData<ObjectType, ID_Type> *>::iterator _objectIterator;


};

template<class ObjectType, class ID_Type>
ObjectSearchCircle<ObjectType, ID_Type>::ObjectSearchCircle(
        vector<list<ObjectAndData<ObjectType, ID_Type> *> *> chunksReference, Coordinate center,
        uint radius) : IObjectSearch<ObjectType, ID_Type>(chunksReference), _center(center),
                       _radius(radius) {
    this->_isAtEnd = true;

    if (this->_chunksReference.size() == 0)
        return;

    bool foundViableEntity = false;
    _chunksIterator = this->_chunksReference.begin();
    while (_chunksIterator != this->_chunksReference.end()) {
        if ((**_chunksIterator).size() == 0) {
            _chunksIterator++;
            continue;
        } else {
            _objectIterator = (**_chunksIterator).begin();
            while (_objectIterator != (**_chunksIterator).end()) {
                if (distanceFast(this->_center, (*_objectIterator)->coordinate(), _radius)) {
                    foundViableEntity = true;
                    break;
                }
                _objectIterator++;
            }
            if (foundViableEntity) {
                this->_isAtEnd = false;
                break;
            }
        }
        _chunksIterator++;
    }
}

template<class ObjectType, class ID_Type>
void ObjectSearchCircle<ObjectType, ID_Type>::next() {
    if (this->_isAtEnd)
        throw std::out_of_range("oof"); //TODO: Add custom exception class instead of "oof."

    _objectIterator++;
    while (!this->_isAtEnd) {
        if (_objectIterator == (**_chunksIterator).end()) {
            _chunksIterator++;
            while (_chunksIterator != this->_chunksReference.end()) {
                if ((**_chunksIterator).size() == 0) {
                    _chunksIterator++;
                    continue;
                }

                _objectIterator = (**_chunksIterator).begin();
                break;
            }

            if (_chunksIterator == this->_chunksReference.end()) {
                this->_isAtEnd = true;
            }
        }

        if ((this->_isAtEnd) || (distanceFast(this->_center, (*_objectIterator)->coordinate(), _radius)))
            break;
        else
            _objectIterator++;
    }

}

template<class ObjectType, class ID_Type>
ID_Type ObjectSearchCircle<ObjectType, ID_Type>::id() const {
    return (*_objectIterator)->id();
}

template<class ObjectType, class ID_Type>
Coordinate ObjectSearchCircle<ObjectType, ID_Type>::position() const {
    return (*_objectIterator)->coordinate();
}

template<class ObjectType, class ID_Type>
ObjectType &ObjectSearchCircle<ObjectType, ID_Type>::object() {
    return (*_objectIterator)->object();
}

template<class ObjectType, class ID_Type>
ObjectAndData<ObjectType, ID_Type> ObjectSearchCircle<ObjectType, ID_Type>::ObjectAndDataCopy() const {
    return **_objectIterator;
}

#endif //WELT_OBJECTSEARCHCIRCLE_H
