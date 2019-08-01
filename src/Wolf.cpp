#include "Wolf.h"

Wolf::Wolf() {
    selfMaterial = M_ENTITY;
    selfMaterial.color = 6;
    objectType = 2;
}

EffectedType Wolf::tick(Iworld<Entity, Tile, Item> *worldPointer, TileMap * map) {
    if (selfHealth == 0)
        return EffectedType::DELETED;

    if ((worldPointer->getTickNumber() % 3) != 0)
        return EffectedType::NONE;

    std::vector<Entity *> entities = worldPointer->getObjectsInCircle(selfPosition, 100, true, false).first;
    Entity *target = nullptr;
    uint targetDistance = 0;
    for (const auto &ent : entities) {
        if ((ent != this) && (ent != nullptr) && (ent->objectType == 1)) {
            uint entDistance = (uint) ceil(distance(this->selfPosition, ent->selfPosition));
            if (target == nullptr) {
                target = ent;
                targetDistance = entDistance;
            } else if (entDistance < targetDistance) {
                target = ent;
                targetDistance = entDistance;
            }

            if (targetDistance <= 1) {
                target->takeDamage(selfID, 10, DamageType::KINETIC);
                return EffectedType::NONE;
            }
        }
    }

    if (target == nullptr) {
        return EffectedType::NONE;
    }

    Coordinate delta = Coordinate{1, 1};
    if (target->selfPosition.x > selfPosition.x)
        delta.x = 2;
    else if (target->selfPosition.x < selfPosition.x)
        delta.x = 0;

    if (target->selfPosition.y > selfPosition.y)
        delta.y = 2;
    else if (target->selfPosition.y < selfPosition.y)
        delta.y = 0;

    Coordinate nextPos = Coordinate{(selfPosition.x + delta.x) - 1, (selfPosition.y + delta.y) - 1};
    Tile *nextTile = map->at(nextPos);
    if ((nextTile == nullptr) || (nextTile->wallMaterial.materialType != MaterialType::GAS))
        return EffectedType::NONE;

    bool wasMoved = worldPointer->moveEntity(this, nextPos);
    if (wasMoved)
        return EffectedType::MOVED;

    return EffectedType::NONE;
}

EffectedType Wolf::takeDamage(OID attacker, uint damageAmount, DamageType type) {
    return EffectedType::NONE;
}
