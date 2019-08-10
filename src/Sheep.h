#ifndef WELT_P_C_SHEEP_H
#define WELT_P_C_SHEEP_H

#include "Ientity.h"
#include <cmath>
#include <vector>

class Sheep : public Ientity {
public:
    Sheep();

    ~Sheep() override = default;

    EffectedType tick(Iworld<Ientity, Iitem> *worldPointer, TileMap *map,
                      const ObjectAndData<Ientity, EID> &selfReference) override;

    EffectedType takeDamage(EID attacker, uint damageAmount, DamageType type) override;

    uint getHealth() override;

    uint getObjectType() override;

    Material getMaterial() override;

    DisplayID getDisplayID() override;

private:
    uint objectType;
    uint selfHealth;
    Material selfMaterial;
    DisplayID entityDisplay;
};


#endif //WELT_P_C_TEST_ENTITY_H
