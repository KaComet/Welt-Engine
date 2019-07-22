#include "entity.h"


Entity::Entity() {
    selfID = 0;
    objectType = 0;
    selfHealth = 1;
    selfPosition = Coordinate{0, 0};
    entityDisplay = 5;
}

EffectedType Entity::tick(Iworld<Entity, Tile> *worldPointer) {
    return EffectedType::NONE;
}

EffectedType Entity::takeDamage(OID attacker, uint damageAmount, DamageType type) {
    return EffectedType::NONE;
}

