//
// Created by Bot_SpaceCat on 6/22/2019.
//

#ifndef WELT_P_C_SPRITESET_H
#define WELT_P_C_SPRITESET_H

#include <SDL.h>
#include <string>
#include "LTexture.h"
#include "universal.h"
#include <SDL_image.h>
#include "DisplayIDdef.h"


class SpriteSet {
public:
    SpriteSet();

    ~SpriteSet();

    // Loads image at specified path. Returns the number of tiles loaded.
    uint loadFromFile(SDL_Renderer *renderer, const std::string &path, uint tileWidth, uint tileHeight);

    // Renders texture at given point.
    bool render(int x, int y, displayID valueToDisplay) const;

    // Renders texture at given point in the given color.
    bool render(int x, int y, displayID valueToDisplay, SDL_Color renderColor);

    uint getFontWidth() const;

    uint getFontHeight() const;

    void setColor(const SDL_Color &color);

    uint getNTiles() const;

private:
    //Deallocates texture
    void free();

    //The font texture
    LTexture *mBitmap;
    uint fontWidth, fontHeight, nTiles;
};


#endif //WELT_P_C_SPRITESET_H
