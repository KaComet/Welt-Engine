#include "ColorList.h"

// Loads the colors defined from the given file name into the given ColorList.
// If the file could not be opened, the function returns false.
bool ColorList::loadColorsFromFile(const std::string &fileName, const SDL_Color &defaultColor, const uint &maxColorNumber) {
    std::string path = getResourcePath("") + fileName;
    if (path.empty())
        return false;

    colorDefinitions.setDefault(defaultColor);
    colorDefinitions.setCapacity(maxColorNumber);

    printf("Loading colors...\n");

    std::ifstream inputFile;
    inputFile.open(path);

    if (inputFile.is_open()) {
        uint lineNumber = 0;
        uint nLoadedTI = 0;
        while (!inputFile.eof()) {
            lineNumber++;
            bool lineLoaded = true;
            char currentLine[1024];
            std::string SCurrentLine;
            SDL_Color newColor = defaultColor;
            uint ColorNumber = 0;

            inputFile.getline(currentLine, 1024);
            SCurrentLine = currentLine;

            uint delemitCount = std::count(SCurrentLine.begin(), SCurrentLine.end(), ',');

            if (delemitCount != 2) {
                lineLoaded = false;
            } else {
                for (uint i = 0; i < 3; i++) {
                    std::string tmp = getDelimitedContents(SCurrentLine, ',', i);
                    tmp.shrink_to_fit();

                    if (tmp.empty())
                        lineLoaded = false;

                    switch (tmp.at(tmp.size() - 1)) {
                        case '\r':
                            tmp.pop_back();
                            break;
                        case '\n':
                            tmp.pop_back();
                            break;
                        default:;
                    }

                    switch (i) {
                        case 0:
                            ColorNumber = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 1:
                            continue;
                            break;
                        case 2:
                            if ((tmp.size() != 7) || !stringIsHex(tmp.substr(1, 6))) {
                                lineLoaded = false;
                                break;
                            }
                            newColor = getSDL_ColorFromCSS_Color(tmp, defaultColor);
                            break;
                        default:;
                    }
                }
            }

            if (lineLoaded) {
                nLoadedTI++;
                const flat::SetType definitionType = colorDefinitions.set(ColorNumber, newColor);
                switch (definitionType) {
                    case flat::SetType::Defined:
                        break;
                    case flat::SetType::Redefinition:
                        printf("   * Line %u is redefining def number %u. Skipping\n", lineNumber, ColorNumber);
                        break;
                    case flat::SetType::Discarded:
                        printf("   * Line %u is out of range of 0..%u. Skipping\n", lineNumber, maxColorNumber);
                        break;
                }
            } else {
                printf("   * Line %u is not formatted correctly. Skipping\n", lineNumber);
            }
        }

        printf("   Loaded %u colors.\n", nLoadedTI);

        return true;
    } else {
        printf("   * Could not open the file \"%s\"\n", path.c_str());
        return false;
    }
}

SDL_Color ColorList::get(const uint &colorNumber) {
    return colorDefinitions.get(colorNumber);
}

// Converts a CSS color formatted string to SDL_Color.
// Returns the value provided for ErrorColor if the given string is not a valid CSS color.
SDL_Color ColorList::getSDL_ColorFromCSS_Color(const std::string &input, SDL_Color errorColor) {
    // If the string is not the right size or does not begin with '#', return errorColor.
    if ((input.size() != 7) || (input.at(0) != '#'))
        return errorColor;

    // Check the string to make sure that it is formatted correctly (contains characters only in the range of 0-F.)
    if (!stringIsHex(input.substr(1, 6)))
        return errorColor;

    Uint8 red, blue, green;
    red = std::stoi(input.substr(1, 2), nullptr, 16);
    blue = std::stoi(input.substr(3, 4), nullptr, 16);
    green = std::stoi(input.substr(5, 6), nullptr, 16);

    return SDL_Color{red, blue, green};
}

// Returns true if the provided string is a valid hexadecimal number.
bool ColorList::stringIsHex(const std::string &input) {
    // If the input is empty, return false.
    if (input.empty())
        return false;

    // Scan through the string, returning false if any of the
    //   characters are not in the range of 0-F.
    for (auto c : input) {
        if (!std::isxdigit(c))
            return false;
    }

    return true;
}