#ifndef WELT_IOBJECTSEARCH_H
#define WELT_IOBJECTSEARCH_H

#include <vector>
#include <list>
#include "universal.h"
#include "ObjectAndData.h"

using namespace std;

template<class ObjectType, class ID_Type>
class IObjectSearch {
public:
    IObjectSearch(vector<list<ObjectAndData<ObjectType, ID_Type> *>*> chunks)
            : _chunksReference(chunks) {
        _isAtEnd = true;
    }

    virtual ~IObjectSearch() = default;

    virtual void next() = 0;

    virtual ID_Type id() = 0;

    virtual Coordinate position() = 0;

    virtual ObjectType &object() = 0;

    virtual ObjectAndData<ObjectType, ID_Type> ObjectAndDataCopy() = 0;

    bool isAtEnd() {
        return _isAtEnd;
    };

protected:
    vector<list<ObjectAndData<ObjectType, ID_Type> *>*> _chunksReference;
    bool _isAtEnd;
};


#endif //WELT_IOBJECTSEARCH_H
