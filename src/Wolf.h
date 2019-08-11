#ifndef WELT_P_C_WOLF_H
#define WELT_P_C_WOLF_H

#include "Ientity.h"
#include <vector>
#include <cmath>

class Wolf : public Ientity {
public:
    Wolf();

    ~Wolf() override = default;

    std::vector<std::size_t> getEntityTypeHash() override;

    EffectedType tick(Iworld<Ientity, Iitem> *worldPointer, TileMap *map,
                      const ObjectAndData<Ientity, EID> &selfReference, uint energy) override;

    EffectedType takeDamage(EID attacker, uint damageAmount, DamageType type) override;

    uint getHealth() override;

    uint getObjectType() override;

    Material getMaterial() override;

    DisplayID getDisplayID() override;

protected:
    std::vector<std::size_t> wolfHash();

private:
    const uint energyNeededForMoveAndAttack = 60;
    uint objectType, selfHealth, selfEnergy;
    Material selfMaterial;
    DisplayID entityDisplay;
};


#endif //WELT_P_C_TEST_ENTITY_H
