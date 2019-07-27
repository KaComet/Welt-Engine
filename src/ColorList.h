#ifndef WELT_COLORLIST_H
#define WELT_COLORLIST_H

#include <SDL.h>
#include <string>
#include <cctype>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "resource.h"
#include "FlatVector.h"

class ColorList {
public:
    bool loadColorsFromFile(const std::string &fileName, const SDL_Color &defaultColor, const uint &maxColorNumber);

    SDL_Color get(const uint &colorNumber);

private:
    SDL_Color getSDL_ColorFromCSS_Color(const std::string &input, SDL_Color errorColor);

    bool stringIsHex(const std::string &input);

    flat::FlatVector<SDL_Color> colorDefinitions;
};


#endif //WELT_COLORLIST_H
