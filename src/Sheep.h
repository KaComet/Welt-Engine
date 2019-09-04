#ifndef WELT_P_C_SHEEP_H
#define WELT_P_C_SHEEP_H

#include "Ientity.h"
#include <cmath>
#include <vector>

class Sheep : public Ientity {
public:
    Sheep();

    ~Sheep() override = default;

    std::vector<std::size_t> getEntityTypeHash() override;

    EffectedType tick(Iworld<Ientity, EID, Iitem, IID> *worldPointer, TileMap *map,
                      const ObjectAndData<Ientity, EID> &selfReference, uint energy) override;

    EffectedType takeDamage(EID attacker, uint damageAmount, DamageType type) override;

    uint getHealth() override;

    uint getObjectType() override;

    Material getMaterial() override;

    DisplayID getDisplayID() override;

protected:
    std::vector<std::size_t> sheepHash();

private:
    const uint energyNeededForMove = 100;
    const uint maxEnergy = 200;
    uint objectType, selfHealth, selfEnergy;
    Material selfMaterial{};
    DisplayID entityDisplay;
};


#endif //WELT_P_C_TEST_ENTITY_H
