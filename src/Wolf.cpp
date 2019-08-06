#include "Wolf.h"

Wolf::Wolf() {
    selfMaterial = M_ENTITY;
    selfMaterial.color = 6;
    objectType = 2;
    entityDisplay = 5;
    selfHealth = 100;
}

Wolf::~Wolf() {

}

EffectedType
Wolf::tick(Iworld<Ientity, Iitem> *worldPointer, TileMap *map, const ObjectAndPosition<Ientity, EID> &selfReference) {

    if (selfHealth == 0)
        return EffectedType::DELETED;

    if ((worldPointer->getTickNumber() % 3) != 0)
        return EffectedType::NONE;

    auto foundEntities = worldPointer->getObjectsInCircle(selfReference.position, 100, true, false).entitiesFound;

    ObjectAndPosition<Ientity, EID> target;
    target.pointer = nullptr;
    uint targetDistance = 0;
    for (const auto &entPtr : foundEntities) {
        if ((entPtr.pointer != this) && (entPtr.pointer) && (entPtr.pointer->getObjectType() == 1)) {
            uint entDistance = (uint) ceil(distance(selfReference.position, entPtr.position));
            if (!target.pointer) {
                target = entPtr;
                targetDistance = entDistance;
            } else if (entDistance < targetDistance) {
                target = entPtr;
                targetDistance = entDistance;
            }

            if (targetDistance <= 1) {
                target.pointer->takeDamage(selfReference.ID_Number, 10, DamageType::KINETIC);
                return EffectedType::NONE;
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

    Coordinate nextPos = Coordinate{(selfReference.position.x + delta.x) - 1, (selfReference.position.y + delta.y) - 1};
    Tile *nextTile = map->at(nextPos);
    if ((nextTile == nullptr) || (nextTile->wallMaterial.materialType != MaterialType::GAS))
        return EffectedType::NONE;

    bool wasMoved = worldPointer->moveEntity(selfReference, nextPos);
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
