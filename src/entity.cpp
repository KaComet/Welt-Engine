#include "entity.h"


Entity::Entity() {
    selfID = 0;
    objectType = 0;
    selfHealth = 1;
    selfMaterial = M_AIR;
    entityDisplay = 5;
}

EffectedType
Entity::tick(Iworld<Entity, Item> *worldPointer, TileMap *map, const ObjectAndPosition<Entity> &selfReference) {
    return EffectedType::NONE;
}

EffectedType Entity::takeDamage(OID attacker, uint damageAmount, DamageType type) {
    return EffectedType::NONE;
}

