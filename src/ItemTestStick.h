#ifndef WELT_ITEMTESTSTICK_H
#define WELT_ITEMTESTSTICK_H

#include "universal.h"
#include "material.h"
#include "Iitem.h"
#include <typeinfo>
#include <vector>

class ItemTestStick : public Iitem {
public:
    ItemTestStick();

    ~ItemTestStick() override = default;

    std::size_t GetItemTypeHash() override;

    std::vector<std::size_t> getComposition() override;

    Material getMaterial() override ;

    DisplayID getDisplayID() override ;

private:

    Material selfMaterial;
    DisplayID itemDisplay;
};


#endif //WELT_ITEMTESTSTICK_H
