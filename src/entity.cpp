#include "entity.h"


Entity::Entity() {
    selfID = 0;
    objectType = 0;
    selfHealth = 1;
    selfPosition = Coordinate{0, 0};
    selfMaterial = M_AIR;
    entityDisplay = 5;
}

EffectedType Entity::tick(Iworld<Entity, Tile, Item> *worldPointer) {
    return EffectedType::NONE;
}

EffectedType Entity::takeDamage(OID attacker, uint damageAmount, DamageType type) {
    return EffectedType::NONE;
}

