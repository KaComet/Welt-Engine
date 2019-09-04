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

    ID_Type id() override;

    Coordinate position() override;

    ObjectType &object() override;

    ObjectAndData<ObjectType, ID_Type> ObjectAndDataCopy() override;

protected:
    uint _radius;
    Coordinate _center;
    typename vector<list<ObjectAndData<ObjectType, ID_Type> *> *>::iterator chunksIterator;
    typename list<ObjectAndData<ObjectType, ID_Type> *>::iterator entityIterator;


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
    chunksIterator = this->_chunksReference.begin();
    while (chunksIterator != this->_chunksReference.end()) {
        if ((**chunksIterator).size() == 0) {
            chunksIterator++;
            continue;
        } else {
            entityIterator = (**chunksIterator).begin();
            while (entityIterator != (**chunksIterator).end()) {
                if (distanceFast(this->_center, (*entityIterator)->coordinate(), _radius)) {
                    foundViableEntity = true;
                    break;
                }
                entityIterator++;
            }
            if (foundViableEntity) {
                this->_isAtEnd = false;
                break;
            }
        }
        chunksIterator++;
    }
}

template<class ObjectType, class ID_Type>
void ObjectSearchCircle<ObjectType, ID_Type>::next() {
    if (this->_isAtEnd)
        throw std::out_of_range("oof"); //TODO: Add custom exception class instead of "oof."

    entityIterator++;
    while (!this->_isAtEnd) {
        if (entityIterator == (**chunksIterator).end()) {
            chunksIterator++;
            while (chunksIterator != this->_chunksReference.end()) {
                if ((**chunksIterator).size() == 0) {
                    chunksIterator++;
                    continue;
                }

                entityIterator = (**chunksIterator).begin();
                break;
            }

            if (chunksIterator == this->_chunksReference.end()) {
                this->_isAtEnd = true;
            }
        }

        if ((this->_isAtEnd) || (distanceFast(this->_center, (*entityIterator)->coordinate(), _radius)))
            break;
        else
            entityIterator++;
    }

}

template<class ObjectType, class ID_Type>
ID_Type ObjectSearchCircle<ObjectType, ID_Type>::id() {
    return (*entityIterator)->id();
}

template<class ObjectType, class ID_Type>
Coordinate ObjectSearchCircle<ObjectType, ID_Type>::position() {
    return (*entityIterator)->coordinate();
}

template<class ObjectType, class ID_Type>
ObjectType &ObjectSearchCircle<ObjectType, ID_Type>::object() {
    return (*entityIterator)->object();
}

template<class ObjectType, class ID_Type>
ObjectAndData<ObjectType, ID_Type> ObjectSearchCircle<ObjectType, ID_Type>::ObjectAndDataCopy() {
    //entityToAdd, &isDataLocked, false, nextAvailableOID++, cord
    return **entityIterator;
}

#endif //WELT_OBJECTSEARCHCIRCLE_H
