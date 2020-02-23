#include "LTexture.h"

LTexture::LTexture() {
    //Initialize
    mTexture = nullptr;
    mRenderer = nullptr;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture() {
    //Deallocate the SpriteSet
    free();
}

bool LTexture::loadFromFile(const std::string &path) {
    //Get rid of preexisting texture
    free();

    //The final texture
    SDL_Texture *newTexture = nullptr;

    //Load image at specified path
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());

    if (loadedSurface == nullptr) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return false;
    } else {
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0, 0));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
        if (newTexture == nullptr) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        } else {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //Return success
    mTexture = newTexture;
    return true;
}

void LTexture::free() {
    //Free texture if it exists
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y) const {
    //Set rendering space and render to screen
    SDL_Rect renderRectangle = {x, y, mWidth, mHeight};
    SDL_RenderCopy(mRenderer, mTexture, nullptr, &renderRectangle);
}

int LTexture::getWidth() const {
    return mWidth;
}

int LTexture::getHeight() const {
    return mHeight;
}

// Returns the current color mod of the texture.
// If there is no texture loaded/no color mod, the function returns white (255, 255, 255.)
SDL_Color LTexture::getModColor() const {
    // If there is no texture currently loaded, return white.
    if (!mTexture)
        return SDL_Color{0xFF, 0xFF, 0xFF};

    // Get the current color mod. If SDL_GetTextureColorMod() returns anything
    //   besides zero, this indicates an error. In this case, return white.
    Uint8 red, blue, green;
    if (SDL_GetTextureColorMod(mTexture, &red, &green, &blue) == 0)
        return SDL_Color{red, green, blue};
    else
        return SDL_Color{0xFF, 0xFF, 0xFF};
}


bool LTexture::setRenderer(SDL_Renderer *renderer) {
    if (renderer == nullptr) {
        return false;
    }

    mRenderer = renderer;

    return true;
}

void LTexture::setModColor(const SDL_Color &color) {
    //Modulate texture
    SDL_SetTextureColorMod(mTexture, color.r, color.g, color.b);
}

void LTexture::renderPortion(SDL_Rect &portion, SDL_Rect &renderSection) const {
    SDL_RenderCopyEx(mRenderer, mTexture, &portion, &renderSection, 0.00, nullptr, SDL_FLIP_NONE);
}