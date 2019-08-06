#include "ItemTestStick.h"
std::size_t ItemTestStick::GetItemTypeHash() {
    return typeid(this).hash_code();
}

std::vector<std::size_t> ItemTestStick::getComposition() {
    return std::vector<std::size_t>{this->GetItemTypeHash()};
}

ItemTestStick::ItemTestStick() : Iitem() {
    selfMaterial = M_AIR;
    selfMaterial.color = COLOR_ITEM_TEST_1;
    itemDisplay = DCID_ITEM_TEST_1;
}

Material ItemTestStick::getMaterial() {
    return selfMaterial;
}

DisplayID ItemTestStick::getDisplayID() {
    return itemDisplay;
}
