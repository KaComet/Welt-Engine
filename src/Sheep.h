#ifndef WELT_P_C_SHEEP_H
#define WELT_P_C_SHEEP_H

#include "entity.h"
#include <cmath>
#include <vector>

class Sheep : public Entity {
public:
    Sheep();

    EffectedType tick(Iworld<Entity, Item> *worldPointer, TileMap *map,
                      const ObjectAndPosition<Entity> &selfReference) override;

    EffectedType takeDamage(OID attacker, uint damageAmount, DamageType type) override;
};


#endif //WELT_P_C_TEST_ENTITY_H
