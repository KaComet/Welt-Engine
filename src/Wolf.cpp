#include "Wolf.h"

Wolf::Wolf() {
    selfMaterial = M_ENTITY;
    selfEnergy = 0;
    selfMaterial.color = 6;
    objectType = 2;
    entityDisplay = 5;
    selfHealth = 100;
}

std::vector<std::size_t> Wolf::getEntityTypeHash() {
    return wolfHash();
}

EffectedType
Wolf::tick(Iworld<Ientity, Iitem> *worldPointer, TileMap *map,
           const ObjectAndData<Ientity, EID> &selfReference, uint energy) {

    if (selfHealth == 0)
        return EffectedType::DELETED;

    bool wasMoved = false;

    selfEnergy += energy;

    while (selfEnergy >= energyNeededForMoveAndAttack) {
        selfEnergy -= energyNeededForMoveAndAttack;

        auto searchResult = worldPointer->getObjectsInCircle(selfReference.position, 500, true, false);
        auto &foundEntities = searchResult.entitiesFound;

        ObjectAndData<Ientity, EID> target(nullptr, 0, Coordinate());
        uint targetDistance = 0;

        for (const auto &entPtr : foundEntities) {
            if ((entPtr.pointer != this) && (entPtr.pointer) && (entPtr.pointer->getObjectType() == 1)) {
                uint entDistance = (uint) ceil(distance(selfReference.position, entPtr.position));
                if (!target.pointer) {
                    target = entPtr;
                    targetDistance = entDistance;
                } else if ((entDistance < targetDistance) && (entPtr.pointer->getHealth() != 0)) {
                    target = entPtr;
                    targetDistance = entDistance;
                }

                if (targetDistance <= 1) {
                    target.pointer->takeDamage(selfReference.ID_Number, 10, DamageType::KINETIC);
                    continue;
                }
            }
        }

        if (!target.pointer) {
            return EffectedType::NONE;
        }

        Coordinate delta = Coordinate{1, 1};
        if (target.position.x > selfReference.position.x)
            delta.x = 2;
        else if (target.position.x < selfReference.position.x)
            delta.x = 0;

        if (target.position.y > selfReference.position.y)
            delta.y = 2;
        else if (target.position.y < selfReference.position.y)
            delta.y = 0;

        Coordinate nextPos = Coordinate{(selfReference.position.x + delta.x) - 1,
                                        (selfReference.position.y + delta.y) - 1};
        Tile *nextTile = map->at(nextPos);
        if ((nextTile == nullptr) || (nextTile->wallMaterial.materialType != MaterialType::GAS))
            return EffectedType::NONE;

        if (worldPointer->moveEntity(selfReference, nextPos))
            wasMoved = true;
    }

    if (wasMoved)
        return EffectedType::MOVED;

    return EffectedType::NONE;
}

EffectedType Wolf::takeDamage(EID attacker, uint damageAmount, DamageType type) {
    return EffectedType::NONE;
}

uint Wolf::getHealth() {
    return selfHealth;
}

uint Wolf::getObjectType() {
    return objectType;
}

Material Wolf::getMaterial() {
    return selfMaterial;
}

DisplayID Wolf::getDisplayID() {
    return entityDisplay;
}

std::vector<std::size_t> Wolf::wolfHash() {
    return std::vector<std::size_t>(typeid(this).hash_code());
}

