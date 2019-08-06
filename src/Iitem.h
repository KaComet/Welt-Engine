#ifndef WELT_IITEM_H
#define WELT_IITEM_H

#include "universal.h"
#include "material.h"
#include <typeinfo>
#include <vector>

class Iitem {
public:
    Iitem() = default;

    virtual ~Iitem() = default;

    virtual std::size_t GetItemTypeHash() = 0;

    virtual std::vector<std::size_t> getComposition() = 0;

    virtual Material getMaterial() = 0;

    virtual DisplayID getDisplayID() = 0;
};


#endif //WELT_IITEM_H
