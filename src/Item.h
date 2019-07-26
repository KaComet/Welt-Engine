#ifndef WELT_ITEM_H
#define WELT_ITEM_H

#include "universal.h"
#include <typeinfo>
#include <vector>

class Item {
public:
    virtual ~Item() = default;

    std::size_t GetItemTypeHash();

    virtual std::vector<std::size_t> getComposition();

    IID selfID;
    Coordinate selfPosition;
};


#endif //WELT_ITEM_H
