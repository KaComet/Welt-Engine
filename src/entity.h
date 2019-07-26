#ifndef ENTITY_H
#define ENTITY_H

#include "DisplayIDdef.h"
#include "universal.h"
#include "Iworld.h"
#include "tile.h"
#include "Item.h"

class Entity {
public:
    Entity();

    virtual ~Entity() = default;

    virtual EffectedType tick(Iworld<Entity, Tile, Item> *worldPointer);

    virtual EffectedType takeDamage(OID attacker, uint damageAmount, DamageType type);

    OID selfID;            // The ID of the entity.
    uint objectType;       // The type of the object.
    uint selfHealth;
    Material selfMaterial;
    DisplayID entityDisplay;
    Coordinate selfPosition;      // The position of the entity.
};

#endif