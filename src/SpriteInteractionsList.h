#ifndef WELT_SPRITEINTERACTIONSLIST_H
#define WELT_SPRITEINTERACTIONSLIST_H

#include <string>
#include <cctype>
#include <fstream>
#include <algorithm>
#include "universal.h"
#include "resource.h"
#include "DisplayIDdef.h"

// Stores mutible SpriteInteractions for easy use.

class SpriteInteractionsList {
public:
    SpriteInteractionsList() = default;

    bool loadSpriteInteractionsFromFile(const std::string &fileName, const SpriteInteraction &defaultValue,
                                        uint maxSI_number);

    SpriteInteraction get(uint elementNumber) const;

    uint getBackgroundTileFromDisplayArray(uint col, uint row, DisplayArray &dis);

    uint getForegroundTileFromDisplayArray(uint col, uint row, DisplayArray &dis);

private:
    bool stringIsUint(const std::string &input) const;

    flat::FlatVector<SpriteInteraction> SI_list;
};


#endif //WELT_SPRITEINTERACTIONSLIST_H
