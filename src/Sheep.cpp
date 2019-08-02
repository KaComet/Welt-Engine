#include "Sheep.h"

Sheep::Sheep() {
    selfMaterial = M_ENTITY;
    objectType = 1;
    selfHealth = 100;
}

EffectedType
Sheep::tick(Iworld<Entity, Item> *worldPointer, TileMap *map, const ObjectAndPosition<Entity> &selfReference) {
    if (selfHealth == 0)
        return EffectedType::DELETED;

    if ((worldPointer->getTickNumber() % 5) != 0)
        return EffectedType::NONE;

    auto foundEntities = worldPointer->getObjectsInCircle(selfReference.position, 100, true, false).entitiesFound;

    ObjectAndPosition<Entity> *enemyPtr = nullptr;
    uint enemyDistance = 0;

    for (const auto &entPtr : foundEntities) {
        if ((entPtr->pointer != this) && (entPtr->pointer) && (entPtr->pointer->objectType == 2)) {
            uint entDistance = (uint) ceil(distance(selfReference.position, entPtr->position));
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
    if (enemyPtr->position.x > selfReference.position.x)
        delta.x = 0;
    else if (enemyPtr->position.x < selfReference.position.x)
        delta.x = 2;

    if (enemyPtr->position.y > selfReference.position.y)
        delta.y = 0;
    else if (enemyPtr->position.y < selfReference.position.y)
        delta.y = 2;

    Coordinate nextPos = Coordinate{(selfReference.position.x + delta.x) - 1, (selfReference.position.y + delta.y) - 1};
    Tile *nextTile = map->at(nextPos);
    bool isNextTileClear = worldPointer->getObjectsOnTile(nextPos, true, false).entitiesFound.empty();
    if ((!isNextTileClear) || (!nextTile) || (nextTile->wallMaterial.materialType != MaterialType::GAS))
        return EffectedType::NONE;

    bool wasMoved = worldPointer->moveEntity(selfReference, nextPos);
    if (wasMoved)
        return EffectedType::MOVED;

    return EffectedType::NONE;
}

EffectedType Sheep::takeDamage(OID attacker, uint damageAmount, DamageType type) {
    if (damageAmount >= selfHealth) {
        selfHealth = 0;
        return EffectedType::DELETED;
    }

    selfHealth -= damageAmount;

    return EffectedType::NONE;
}
