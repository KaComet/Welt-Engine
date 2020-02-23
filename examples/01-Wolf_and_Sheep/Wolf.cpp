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
Wolf::tick(Iworld<Ientity, EID, Iitem, IID> *worldPointer, TileMap *map,
           const ObjectAndData<Ientity, EID> &selfReference, uint energy) {
    if (selfHealth == 0)
        return EffectedType::DELETED;

    bool wasMoved = false;

    selfEnergy += energy;
    if (selfEnergy > maxEnergy)
        selfEnergy = maxEnergy;

    while (selfEnergy >= energyNeededForMoveAndAttack) {
        selfEnergy -= energyNeededForMoveAndAttack;

        SearchResult<Ientity, EID, Iitem, IID> searchResult = worldPointer->getObjectsInCircle(selfReference.coordinate(), 500, true, false);

        ObjectAndData<Ientity, EID> target(nullptr, nullptr, true, 0, Coordinate());
        uint targetDistance = 0;

        for ( ; !searchResult.entitiesFound->isAtEnd(); searchResult.entitiesFound->next()) {
            if ((searchResult.entitiesFound->id() != selfReference.id()) && (searchResult.entitiesFound->object().getObjectType() == 1)) {
                uint entDistance = (uint) ceil(distance(selfReference.coordinate(), searchResult.entitiesFound->position()));
                if (target.isPlaceholder()) {
                    target = searchResult.entitiesFound->ObjectAndDataCopy();
                    targetDistance = entDistance;
                } else if ((entDistance < targetDistance) && (searchResult.entitiesFound->object().getHealth() != 0)) {
                    target = searchResult.entitiesFound->ObjectAndDataCopy();
                    targetDistance = entDistance;
                }

                if (targetDistance <= 1) {
                    target.object().takeDamage(selfReference.id(), 10, DamageType::KINETIC);
                    continue;
                }
            }
        }

        if (target.isPlaceholder()) {
            return EffectedType::NONE;
        }

        Coordinate delta = Coordinate{1, 1};
        if (target.coordinate().x > selfReference.coordinate().x)
            delta.x = 2;
        else if (target.coordinate().x < selfReference.coordinate().x)
            delta.x = 0;

        if (target.coordinate().y > selfReference.coordinate().y)
            delta.y = 2;
        else if (target.coordinate().y < selfReference.coordinate().y)
            delta.y = 0;

        Coordinate nextPos = Coordinate{(selfReference.coordinate().x + delta.x) - 1,
                                        (selfReference.coordinate().y + delta.y) - 1};
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

