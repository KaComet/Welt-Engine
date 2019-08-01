#ifndef MATERIAL_H
#define MATERIAL_H

#include "DisplayIDdef.h"
#include "universal.h"
#include <SDL.h>

enum MaterialType {
    SOLID,
    GAS,
    LIQUID
};

struct Material {
    MaterialType materialType;  // Type of the material.
    uint baseHealth;            // The base health any wall of the specified material will have.
    colorID color;
    DisplayID defaultDisplayWall, defaultDisplayFloor;
};

const Material M_VOID   = Material{  MaterialType::GAS,     0, COLOR_VOID, DCID_AIR, DCID_AIR  };
const Material M_AIR    = Material{  MaterialType::GAS,     0, COLOR_AIR, DCID_AIR, DCID_AIR  };
const Material M_STONE  = Material{  MaterialType::SOLID, 200, COLOR_STONE, DCID_SLDWALL_CONNECT, DCID_GROUND_OUTSIDE  };
const Material M_GRASS  = Material{  MaterialType::SOLID, 200, COLOR_GRASS,   DCID_GROUND_OUTSIDE,  DCID_GROUND_OUTSIDE  };
const Material M_ENTITY = Material{  MaterialType::SOLID, 200, COLOR_ENTITY_NICE, DCID_ENTITY_SIMPLE,   DCID_ENTITY_SIMPLE   };


#endif

