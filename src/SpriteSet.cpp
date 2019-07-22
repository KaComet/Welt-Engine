#include "SpriteSet.h"

SpriteSet::SpriteSet() {
    //Initialize variables
    mBitmap = nullptr;
    nTiles = 0;
    fontWidth = 0;
    fontHeight = 0;
}

SpriteSet::~SpriteSet() {
    // Free any loaded textures.
    this->free();
}

uint SpriteSet::loadFromFile(SDL_Renderer *renderer, const std::string &path, uint tileWidth, uint tileHeight) {
    // Free any previously used resources.
    free();

    // Create the texture, set its renderer, and load the SpriteSet from the given path.
    mBitmap = new LTexture;
    mBitmap->setRenderer(renderer);
    if (!(mBitmap->loadFromFile(path))) {
        free();
        return 0;
    }

    // If the SpriteSet is not formatted correctly, free and return zero;
    if (((mBitmap->getHeight() % tileHeight) != 0) || ((mBitmap->getWidth() % tileWidth) != 0)) {
        std::cout << std::endl << "Bad dem. Source height % tile Height = "
                  << mBitmap->getHeight() % tileHeight << " Source width % tile width = "
                  << mBitmap->getWidth() % tileWidth << std::endl;
        free();
        return 0;
    }

    // Set the width, height, and number of tiles.
    fontWidth = tileWidth;
    fontHeight = tileHeight;
    nTiles = ((mBitmap->getWidth() / tileWidth) * (mBitmap->getHeight() / tileHeight));

    // Return the number of tiles loaded.
    return nTiles;
}

// Frees any currently loaded resources.
void SpriteSet::free() {
    // Delete any loaded SpriteSet and set the number of tiles to zero.
    delete mBitmap;
    mBitmap = nullptr;
    nTiles = 0;
}

// Renders the specified sprite. If the specified sprite is not loaded, the function returns false.
bool SpriteSet::render(int x, int y, displayID valueToDisplay) const {
    // If the SpriteSet is not loaded, return false.
    if ((mBitmap == nullptr) || (valueToDisplay >= nTiles))
        return false;

    // Calculate the specified tiles position in the SpriteSet.
    int xPos, yPos, nTilesPerRow;
    nTilesPerRow = mBitmap->getWidth() / fontWidth;
    xPos = (valueToDisplay % nTilesPerRow) * fontWidth;
    yPos = (valueToDisplay / nTilesPerRow) * fontHeight;

    // The portion of the SpriteSet image to render.
    SDL_Rect mask;
    mask.x = xPos;
    mask.y = yPos;
    mask.h = fontHeight;
    mask.w = fontWidth;

    // The portion of the renderer to write to.
    SDL_Rect limit;
    limit.h = fontHeight;
    limit.w = fontWidth;
    limit.x = x;
    limit.y = y;

    // Render the sprite to the screen.
    mBitmap->renderPortion(x, y, &mask, &limit);

    return true;
}

// Renders the specified sprite in the given color. If the specified sprite is not loaded, the function returns false.
bool SpriteSet::render(int x, int y, displayID valueToDisplay, SDL_Color renderColor) {
    if (!mBitmap)
        return false;

    // Store the old color mod so it can be restored after rendering.
    const SDL_Color oldColorMod = mBitmap->getModColor();

    // Set the texture's color mod to the new color mod.
    setColor(renderColor);

    // Render the texture, storing whether or not rendering was successful.
    const bool wasRenderSuccessful = this->render(x, y, valueToDisplay);

    // Reset the texture's color mod to it's old value.
    mBitmap->setModColor(oldColorMod);

    // Return whether or not rendering was successful.
    return wasRenderSuccessful;
}

uint SpriteSet::getFontWidth() const {
    return fontWidth;
}

uint SpriteSet::getFontHeight() const {
    return fontHeight;
}

void SpriteSet::setColor(const SDL_Color &color) {
    if (!mBitmap)
        return;

    mBitmap->setModColor(color);
}

uint SpriteSet::getNTiles() const {
    return nTiles;
}
