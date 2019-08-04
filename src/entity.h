#ifndef ENTITY_H
#define ENTITY_H

#include "DisplayIDdef.h"
#include "universal.h"
#include "TileMap.h"
#include "Iworld.h"
#include "tile.h"
#include "Item.h"

class Entity {
public:
    Entity();

    virtual ~Entity() = default;

    virtual EffectedType
    tick(Iworld<Entity, Item> *worldPointer, TileMap *map, const ObjectAndPosition<Entity, EID> &selfReference);

    virtual EffectedType takeDamage(EID attacker, uint damageAmount, DamageType type);

    uint objectType;       // The type of the object.
    uint selfHealth;
    Material selfMaterial;
    DisplayID entityDisplay;
};

#endif