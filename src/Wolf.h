#ifndef WELT_P_C_WOLF_H
#define WELT_P_C_WOLF_H

#include "entity.h"
#include <vector>
#include <cmath>

class Wolf : public Entity {
public:
    Wolf();

    EffectedType tick(Iworld<Entity, Item> *worldPointer, TileMap *map, const ObjectAndPosition<Entity> &selfReference) override;

    EffectedType takeDamage(OID attacker, uint damageAmount, DamageType type) override;
};


#endif //WELT_P_C_TEST_ENTITY_H
