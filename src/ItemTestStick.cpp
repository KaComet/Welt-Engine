#include "ItemTestStick.h"
std::vector<std::size_t> ItemTestStick::GetItemTypeHash() {
    return testStickHash();
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

std::vector<std::size_t> ItemTestStick::testStickHash() {
    return std::vector<std::size_t>(typeid(this).hash_code());
}
