#include "Sheep.h"

Sheep::Sheep() {
    selfMaterial = M_ENTITY;
    objectType = 1;
    selfEnergy = 0;
    selfHealth = 100;
    entityDisplay = 5;
}

std::vector<std::size_t> Sheep::getEntityTypeHash() {
    return sheepHash();
}

EffectedType
Sheep::tick(Iworld<Ientity, Iitem> *worldPointer, TileMap *map,
            const ObjectAndData<Ientity, EID> &selfReference, uint energy) {
    if (selfHealth == 0)
        return EffectedType::DELETED;

    bool wasMoved = false;

    selfEnergy += energy;
    if (selfEnergy > maxEnergy)
        selfEnergy = maxEnergy;

    while (selfEnergy >= energyNeededForMove) {

        auto foundEntities = worldPointer->getObjectsInCircle(selfReference.coordinate(), 1, true, false).entitiesFound;

        if (foundEntities.size() >= 5)
            break;

        foundEntities = worldPointer->getObjectsInCircle(selfReference.coordinate(), 100, true, false).entitiesFound;

        ObjectAndData<Ientity, EID> enemyPtr(nullptr, nullptr, true, 0, Coordinate());
        uint enemyDistance = 0;

        for (auto &entPtr : foundEntities) {
            if ((entPtr->id() != selfReference.id()) && (entPtr->object().getObjectType() == 2)) {
                uint entDistance = (uint) ceil(distance(selfReference.coordinate(), entPtr->coordinate()));
                if (enemyPtr.isPlaceholder()) {
                    enemyPtr = *entPtr;
                    enemyDistance = entDistance;
                } else if (entDistance < enemyDistance) {
                    enemyPtr = *entPtr;
                    enemyDistance = entDistance;
                }
            }
        }

        if (enemyPtr.isPlaceholder())
            break;

        Coordinate delta = Coordinate{1, 1};
        if (enemyPtr.coordinate().x > selfReference.coordinate().x)
            delta.x = 0;
        else if (enemyPtr.coordinate().x < selfReference.coordinate().x)
            delta.x = 2;

        if (enemyPtr.coordinate().y > selfReference.coordinate().y)
            delta.y = 0;
        else if (enemyPtr.coordinate().y < selfReference.coordinate().y)
            delta.y = 2;

        Coordinate nextPos = Coordinate{(selfReference.coordinate().x + delta.x) - 1,
                                        (selfReference.coordinate().y + delta.y) - 1};
        Tile *nextTile = map->at(nextPos);
        bool isNextTileClear = worldPointer->getObjectsOnTile(nextPos, true, false).entitiesFound.empty();
        if ((!isNextTileClear) || (!nextTile) || (nextTile->wallMaterial.materialType != MaterialType::GAS))
            return EffectedType::NONE;

        if (worldPointer->moveEntity(selfReference, nextPos)) {
            wasMoved = true;
            selfEnergy -= energyNeededForMove;
        }
    }

    if (wasMoved)
        return EffectedType::MOVED;

    return EffectedType::NONE;
}

EffectedType Sheep::takeDamage(EID attacker, uint damageAmount, DamageType type) {
    if (damageAmount >= selfHealth) {
        selfHealth = 0;
        return EffectedType::DELETED;
    }

    selfHealth -= damageAmount;

    return EffectedType::NONE;
}

uint Sheep::getHealth() {
    return selfHealth;
}

uint Sheep::getObjectType() {
    return objectType;
}

Material Sheep::getMaterial() {
    return selfMaterial;
}

DisplayID Sheep::getDisplayID() {
    return entityDisplay;
}

std::vector<std::size_t> Sheep::sheepHash() {
    return std::vector<std::size_t>(typeid(this).hash_code());
}


