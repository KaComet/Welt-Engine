#include "ColorList.h"

// Loads the colors defined from the given file name into the given ColorList.
// If the file could not be opened, the function returns false.
bool ColorList::loadColorsFromFile(const std::string &fileName, const SDL_Color &defaultColor, const uint &maxColorNumber) {
    // Determine the path of the resource.
    std::string path = getResourcePath("") + fileName;

    // If the path is empty, return false.
    if (path.empty())
        return false;

    // Set the capacity and default element of the Color list.
    colorDefinitions.setDefault(defaultColor);
    colorDefinitions.setCapacity(maxColorNumber);

    printf("Loading colors...\n");

    // Open the input file using the previously determined path.
    std::ifstream inputFile;
    inputFile.open(path);

    // If the file could be opened, start loading the file. If not, return false.
    if (inputFile.is_open()) {
        uint lineNumber = 0;
        uint nLoadedTI = 0;
        while (!inputFile.eof()) {
            bool lineLoaded = true;
            std::string currentLine;
            SDL_Color newColor = defaultColor;
            uint ColorNumber = 0;

            // NOTE: line numbers start at one.
            lineNumber++;

            // Load the current line.
            std::getline(inputFile, currentLine);
            uint delemitCount = std::count(currentLine.begin(), currentLine.end(), ',');

            // If there are not the right number of delmit characters, the line must not be formatted correctly.
            //   If this is the case, skip the current line.
            if (delemitCount != 2) {
                lineLoaded = false;
            } else {
                // Trim any newline/carriage return characters from the string.
                auto c = currentLine.begin();
                while (c != currentLine.end()) {
                    switch (*c) {
                        case '\r':
                        case '\n':
                            c = currentLine.erase(c);
                            continue;
                    }
                    c++;
                }

                // Scan through the current line and load its contents.
                for (uint i = 0; i < 3; i++) {
                    std::string tmp = getDelimitedContents(currentLine, ',', i);

                    if (tmp.empty())
                        lineLoaded = false;

                    // Try to convert the current element to the desired format.
                    //   If something could not be converted, cancel loading the line.
                    switch (i) {
                        case 0:
                            ColorNumber = (uint) std::stol(tmp, nullptr, 10);
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

            // If the line was loaded, load the line. If the line redefined something or is
            //   outside the range of the SI List, notify the user.
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
                // If the line was not formatted correctly, notify the user.
                printf("   * Line %u is not formatted correctly. Skipping\n", lineNumber);
            }
        }

        // When done loading the file, print how many colors were loaded and return true.
        printf("   Loaded %u colors.\n", nLoadedTI);

        return true;
    } else {
        // If the file could not be loaded, notify the user and return false.
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
    std::string redS, blueS, greenS;
    red = std::stoi(input.substr(1, 2), nullptr, 16);
    blue = std::stoi(input.substr(3, 2), nullptr, 16);
    green = std::stoi(input.substr(5, 2), nullptr, 16);

    return SDL_Color{red, blue, green};
    //#FFBBCC
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