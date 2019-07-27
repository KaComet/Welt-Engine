#include "SpriteInteractionsList.h"

// Clears the current definitions and loads the definitions in the given file. Returns true if successfull.
bool SpriteInteractionsList::loadSpriteInteractionsFromFile(const std::string &fileName,
                                                            const SpriteInteraction &defaultValue,
                                                            const uint maxSI_number) {
    // Determine the path of the resource.
    std::string path = getResourcePath("") + fileName;

    // If the path is empty, return false.
    if (path.empty())
        return false;

    // Set the capacity and default element of the SI list.
    SI_list.setDefault(defaultValue);
    SI_list.setCapacity(maxSI_number);

    printf("Loading sprite interactions...\n");

    // Open the input file using the previously determined path.
    std::ifstream inputFile;
    inputFile.open(path);

    // If the file could be opened, start loading the file. If not, return false.
    if (inputFile.is_open()) {
        uint lineNumber = 0;
        uint nLoadedSI = 0;

        while (!inputFile.eof()) {
            bool lineLoaded = true;
            std::string currentLine;
            SpriteInteraction newSI = defaultValue;
            uint SI_number = 0;

            // NOTE: line numbers start at one.
            lineNumber++;

            // Load the current line.
            std::getline(inputFile, currentLine);
            uint delemitCount = std::count(currentLine.begin(), currentLine.end(), ',');

            // If there are not the right number of delmit characters, the line must not be formatted correctly.
            //   If this is the case, skip the current line.
            if (delemitCount != 17) {
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
                for (uint i = 0; i < 18; i++) {
                    std::string tmp = getDelimitedContents(currentLine, ',', i);

                    // If the current element is empty, cancel loading the line.
                    if (tmp.empty())
                        lineLoaded = false;

                    // Try to convert the current element to the desired format.
                    //   If something could not be converted, cancel loading the line.
                    try {
                        switch (i) {
                            case 0:
                                SI_number = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 1:
                                newSI.name = tmp;
                                break;
                            case 2:
                                newSI.dDefault = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 3:
                                newSI.W = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 4:
                                newSI.E = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 5:
                                newSI.EW = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 6:
                                newSI.S = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 7:
                                newSI.SW = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 8:
                                newSI.SE = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 9:
                                newSI.SEW = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 10:
                                newSI.N = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 11:
                                newSI.NW = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 12:
                                newSI.NE = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 13:
                                newSI.NEW = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 14:
                                newSI.NS = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 15:
                                newSI.NSW = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 16:
                                newSI.NSE = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            case 17:
                                newSI.NSEW = (uint) std::stol(tmp, nullptr, 10);
                                break;
                            default:;
                        }
                    }
                    catch (const std::invalid_argument &e) {
                        lineLoaded = false;
                    }
                    catch (const std::out_of_range &e) {
                        lineLoaded = false;
                    }
                }
            }

            // If the line was loaded, load the line. If the line redefined something or is
            //   outside the range of the SI List, notify the user.
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
                // If the line was not formatted correctly, notify the user.
                printf("   * Line %u is not formatted correctly. Skipping\n", lineNumber);
            }
        }

        // When done loading the file, print how many SIs were loaded and return true.
        printf("   Loaded %u sprite interactions.\n", nLoadedSI);

        return true;
    } else {

        // If the file could not be loaded, notify the user and return false.
        printf("   * Could not open the file \"%s\"\n", path.c_str());
        return false;
    }
}

// Returns the SpriteInteraction for the given number.
SpriteInteraction SpriteInteractionsList::get(const uint elementNumber) const {
    return SI_list.get(elementNumber);
}

// When given a DisplayArray and a coordinate, this function will
//   determine the correct sprite to display in the background for the given tile.
uint SpriteInteractionsList::getBackgroundTileFromDisplayArray(uint col, uint row, DisplayArray &dis) const {
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
uint SpriteInteractionsList::getForegroundTileFromDisplayArray(uint col, uint row, DisplayArray &dis) const {
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