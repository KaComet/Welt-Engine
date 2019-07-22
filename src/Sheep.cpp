#include "Sheep.h"

Sheep::Sheep() {
    selfMaterial = M_ENTITY;
    objectType = 1;
    selfHealth = 100;
}

EffectedType Sheep::tick(Iworld<Entity, Tile> *worldPointer) {
    if (selfHealth == 0)
        return EffectedType::DELETED;

    if ((worldPointer->getTickNumber() % 5) != 0)
        return EffectedType::NONE;

    std::vector<Entity*> entitiesNear = worldPointer->getEntitiesInCircle(selfPosition, 100);
    bool fearful = false;
    Entity *enemyPtr = nullptr;
    uint enemyDistance = 0;

    for (const auto& entPtr : entitiesNear)
    {
        if ((entPtr != this) && (entPtr != nullptr) && (entPtr->objectType == 2)) {
            uint entDistance = (uint)ceil(distance(this->selfPosition, entPtr->selfPosition));
            if (enemyPtr == nullptr) {
                enemyPtr = entPtr;
                enemyDistance = entDistance;
            } else if (entDistance < enemyDistance) {
                enemyPtr = entPtr;
                enemyDistance = entDistance;
            }
        }
    }

    if (enemyPtr == nullptr) {
        return EffectedType::NONE;
    }

    Coordinate delta = Coordinate{1, 1};
    if (enemyPtr->selfPosition.x > selfPosition.x)
        delta.x = 0;
    else if (enemyPtr->selfPosition.x < selfPosition.x)
        delta.x = 2;

    if (enemyPtr->selfPosition.y > selfPosition.y)
        delta.y = 0;
    else if (enemyPtr->selfPosition.y < selfPosition.y)
        delta.y = 2;

    Coordinate nextPos = Coordinate{(selfPosition.x + delta.x) - 1, (selfPosition.y + delta.y) - 1};
    Tile *nextTile = worldPointer->getTileAtPos(nextPos);
    Entity* nxtPtr = worldPointer->getEntityOnTile(nextPos);
    if ((nxtPtr != nullptr) || (nextTile == nullptr) || (nextTile->wallMaterial.materialType != MaterialType::GAS))
        return EffectedType::NONE;

    bool wasMoved = worldPointer->moveEntity(this, nextPos);
    if (wasMoved)
        return EffectedType::MOVED;

    return EffectedType::NONE;
}

EffectedType Sheep::takeDamage(OID attacker, uint damageAmount, DamageType type) {
    if (damageAmount >= selfHealth)
    {
        selfHealth = 0;
        return EffectedType::DELETED;
    }

    selfHealth -= damageAmount;

    return EffectedType::NONE;
}
