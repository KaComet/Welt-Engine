#include "Wolf.h"

Wolf::Wolf() {
    selfMaterial = M_ENTITY;
    selfMaterial.color = 6;
    objectType = 2;
}

EffectedType
Wolf::tick(Iworld<Entity, Item> *worldPointer, TileMap *map, const ObjectAndPosition<Entity> &selfReference) {

    if (selfHealth == 0)
        return EffectedType::DELETED;

    if ((worldPointer->getTickNumber() % 3) != 0)
        return EffectedType::NONE;

    auto foundEntities = worldPointer->getObjectsInCircle(selfReference.position, 100, true, false).entitiesFound;

    ObjectAndPosition<Entity> *target = nullptr;
    uint targetDistance = 0;
    for (const auto &entPtr : foundEntities) {
        if ((entPtr->pointer != this) && (entPtr->pointer) && (entPtr->pointer->objectType == 1)) {
            uint entDistance = (uint) ceil(distance(selfReference.position, entPtr->position));
            if (target == nullptr) {
                target = entPtr;
                targetDistance = entDistance;
            } else if (entDistance < targetDistance) {
                target = entPtr;
                targetDistance = entDistance;
            }

            if (targetDistance <= 1) {
                target->pointer->takeDamage(selfID, 10, DamageType::KINETIC);
                return EffectedType::NONE;
            }
        }
    }

    if (target == nullptr) {
        return EffectedType::NONE;
    }

    Coordinate delta = Coordinate{1, 1};
    if (target->position.x > selfReference.position.x)
        delta.x = 2;
    else if (target->position.x < selfReference.position.x)
        delta.x = 0;

    if (target->position.y > selfReference.position.y)
        delta.y = 2;
    else if (target->position.y < selfReference.position.y)
        delta.y = 0;

    Coordinate nextPos = Coordinate{(selfReference.position.x + delta.x) - 1, (selfReference.position.y + delta.y) - 1};
    Tile *nextTile = map->at(nextPos);
    if ((nextTile == nullptr) || (nextTile->wallMaterial.materialType != MaterialType::GAS))
        return EffectedType::NONE;

    bool wasMoved = worldPointer->moveEntity(selfReference, nextPos);
    if (wasMoved)
        return EffectedType::MOVED;

    return EffectedType::NONE;
}

EffectedType Wolf::takeDamage(OID attacker, uint damageAmount, DamageType type) {
    return EffectedType::NONE;
}
