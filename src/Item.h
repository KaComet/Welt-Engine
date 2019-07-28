#ifndef WELT_ITEM_H
#define WELT_ITEM_H

#include "universal.h"
#include "material.h"
#include <typeinfo>
#include <vector>

class Item {
public:
    Item();

    virtual ~Item() = default;

    std::size_t GetItemTypeHash();

    virtual std::vector<std::size_t> getComposition();

    IID selfID;
    Material selfMaterial;
    Coordinate selfPosition;
    DisplayID itemDisplay;
};


#endif //WELT_ITEM_H
