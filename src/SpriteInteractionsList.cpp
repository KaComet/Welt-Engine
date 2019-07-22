#include "SpriteInteractionsList.h"

// Clears the current definitions and loads the definitions in the given file. Returns true if successfull.
bool SpriteInteractionsList::loadSpriteInteractionsFromFile(const std::string &fileName,
                                                            const SpriteInteraction &defaultValue,
                                                            const uint maxSI_number) {
    std::string path = getResourcePath("") + fileName;
    if (path.empty())
        return false;

    SI_list.setDefault(defaultValue);
    SI_list.setCapacity(maxSI_number);

    printf("Loading sprite interactions...\n");

    std::ifstream inputFile;
    inputFile.open(path);

    if (inputFile.is_open()) {
        uint lineNumber = 0;
        uint nLoadedSI = 0;
        while (!inputFile.eof()) {
            lineNumber++;
            bool lineLoaded = true;
            char currentLine[1024];
            std::string SCurrentLine;
            SpriteInteraction newSI = defaultValue;
            uint SI_number = 0;

            inputFile.getline(currentLine, 1024);
            SCurrentLine = currentLine;

            uint delemitCount = std::count(SCurrentLine.begin(), SCurrentLine.end(), ',');

            if (delemitCount != 17) {
                lineLoaded = false;
            } else {
                for (uint i = 0; i < 18; i++) {
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

                    if (i == 1) {
                        newSI.name = tmp;
                        continue;
                    }

                    if (!stringIsUint(tmp)) {
                        lineLoaded = false;
                        break;
                    }

                    switch (i) {
                        case 0:
                            SI_number = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 2:
                            newSI.dDefault = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 3:
                            newSI.W = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 4:
                            newSI.E = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 5:
                            newSI.EW = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 6:
                            newSI.S = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 7:
                            newSI.SW = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 8:
                            newSI.SE = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 9:
                            newSI.SEW = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 10:
                            newSI.N = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 11:
                            newSI.NW = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 12:
                            newSI.NE = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 13:
                            newSI.NEW = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 14:
                            newSI.NS = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 15:
                            newSI.NSW = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 16:
                            newSI.NSE = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        case 17:
                            newSI.NSEW = (uint) std::strtol(tmp.c_str(), nullptr, 10);
                            break;
                        default:;
                    }
                }
            }

            if (lineLoaded) {
                nLoadedSI++;
                const flat::SetType definitionType = SI_list.set(SI_number, newSI);
                switch (definitionType) {
                    case flat::SetType::Defined:
                        break;
                    case flat::SetType::Redefinition:
                        printf("   * Line %u is redefining def number %u. Skipping\n", lineNumber, SI_number);
                        break;
                    case flat::SetType::Discarded:
                        printf("   * Line %u is out of range of 0..%u. Skipping\n", lineNumber, maxSI_number);
                        break;
                }
            } else {
                printf("   * Line %u is not formatted correctly. Skipping\n", lineNumber);
            }
        }

        printf("   Loaded %u sprite interactions.\n", nLoadedSI);

        return true;
    } else {
        printf("   * Could not open the file \"%s\"\n", path.c_str());
        return false;
    }
}

// Returns true is the provided string is a valid unsigned integer number.
bool SpriteInteractionsList::stringIsUint(const std::string &input) const {
    // If the input is empty, return false.
    if (input.empty())
        return false;

    // Scan through the string, returning false if any of the
    //   characters are not in the range of 0-9.
    for (auto c : input) {
        if (!std::isdigit(c))
            return false;
    }

    return true;
}

// Returns the SpriteInteraction for the given number.
SpriteInteraction SpriteInteractionsList::get(const uint elementNumber) const {
    return SI_list.get(elementNumber);
}

// When given a DisplayArray and a coordinate, this function will
//   determine the correct sprite to display in the background for the given tile.
uint SpriteInteractionsList::getBackgroundTileFromDisplayArray(uint col, uint row, DisplayArray &dis) {
    if ((col >= dis.width) || (row >= dis.height))
        return 0;

    displayID result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).dDefault;

    bool N, S, E, W;
    N = (row != 0) && (dis.displayData[col + (dis.width * row)].BackgroundInfo ==
                       dis.displayData[col + (dis.width * (row - 1))].BackgroundInfo);

    S = (col != (dis.width) - 1) && (dis.displayData[col + (dis.width * row)].BackgroundInfo ==
                                     dis.displayData[(col + 1) + (dis.width * row)].BackgroundInfo);

    W = (col != 0) && (dis.displayData[col + (dis.width * row)].BackgroundInfo ==
                       dis.displayData[(col - 1) + (dis.width * row)].BackgroundInfo);

    E = (col != (dis.width) - 1) && (dis.displayData[col + (dis.width * row)].BackgroundInfo ==
                                     dis.displayData[(col + 1) + (dis.width * row)].BackgroundInfo);

    if (!N && !S && !E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).W;
    if (!N && !S && E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).E;
    if (!N && !S && E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).EW;
    if (!N && S && !E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).S;
    if (!N && S && !E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).SW;
    if (!N && S && E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).SE;
    if (!N && S && E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).SEW;
    if (N && !S && !E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).N;
    if (N && !S && !E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).NW;
    if (N && !S && E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).NE;
    if (N && !S && E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).NEW;
    if (N && S && !E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).NS;
    if (N && S && !E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).NSW;
    if (N && S && E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).NSE;
    if (N && S && E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].BackgroundInfo).NSEW;

    return result;
}

// When given a DisplayArray and a coordinate, this function will
//   determine the correct sprite to display in the foreground for the given tile.
uint SpriteInteractionsList::getForegroundTileFromDisplayArray(uint col, uint row, DisplayArray &dis) {
    if ((col >= dis.width) || (row >= dis.height))
        return 0;

    displayID result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).dDefault;

    bool N, S, E, W;
    N = (row != 0) && (dis.displayData[col + (dis.width * row)].ForegroundInfo ==
                       dis.displayData[col + (dis.width * (row - 1))].ForegroundInfo);

    S = (row != (dis.height - 1)) && (dis.displayData[col + (dis.width * row)].ForegroundInfo ==
                                      dis.displayData[col + (dis.width * (row + 1))].ForegroundInfo);

    W = (col != 0) && (dis.displayData[col + (dis.width * row)].ForegroundInfo ==
                       dis.displayData[(col - 1) + (dis.width * row)].ForegroundInfo);

    E = (col != (dis.width) - 1) && (dis.displayData[col + (dis.width * row)].ForegroundInfo ==
                                     dis.displayData[(col + 1) + (dis.width * row)].ForegroundInfo);

    if (!N && !S && !E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).W;
    if (!N && !S && E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).E;
    if (!N && !S && E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).EW;
    if (!N && S && !E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).S;
    if (!N && S && !E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).SW;
    if (!N && S && E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).SE;
    if (!N && S && E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).SEW;
    if (N && !S && !E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).N;
    if (N && !S && !E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).NW;
    if (N && !S && E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).NE;
    if (N && !S && E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).NEW;
    if (N && S && !E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).NS;
    if (N && S && !E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).NSW;
    if (N && S && E && !W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).NSE;
    if (N && S && E && W)
        result = SI_list.get(dis.displayData[col + (dis.width * row)].ForegroundInfo).NSEW;

    return result;
}