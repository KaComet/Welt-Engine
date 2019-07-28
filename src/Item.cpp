#include "Item.h"

std::size_t Item::GetItemTypeHash() {
    return typeid(this).hash_code();
}

std::vector<std::size_t> Item::getComposition() {
    return std::vector<std::size_t>{this->GetItemTypeHash()};
}

Item::Item() {
    selfID = 0;
    selfMaterial = M_AIR;
    itemDisplay = DCID_AIR;
    selfPosition = Coordinate{0, 0};
}
