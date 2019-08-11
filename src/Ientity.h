#ifndef IENTITY_H
#define IENTITY_H

#include "DisplayIDdef.h"
#include "universal.h"
#include "TileMap.h"
#include "Iworld.h"
#include "tile.h"
#include "Iitem.h"

class Ientity {
public:
    Ientity() = default;

    virtual ~Ientity() = default;

    virtual std::vector<std::size_t> getEntityTypeHash() = 0;

    virtual EffectedType
    tick(Iworld<Ientity, Iitem> *worldPointer, TileMap *map, const ObjectAndData<Ientity, EID> &selfReference,
         uint energy) = 0;

    virtual EffectedType takeDamage(EID attacker, uint damageAmount, DamageType type) = 0;

    virtual uint getHealth() = 0;

    virtual uint getObjectType() = 0;

    virtual Material getMaterial() = 0;

    virtual DisplayID getDisplayID() = 0;
};

#endif //IENTITY_H