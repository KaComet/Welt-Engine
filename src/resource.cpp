#include "resource.h"

// Get the path to a file in the resource directory.
// NOTE: all paths assume that the executable is contained in a directory named "bin".
std::string getResourcePath(const std::string &subDir) {
#ifdef _WIN32
    const char PATH_SEP = '\\';
#else
    const char PATH_SEP = '/';
#endif
    //This will hold the base resource path: Lessons/res/
    //We give it static lifetime so that we'll only need to call
    //SDL_GetBasePath once to get the executable path
    static std::string baseRes;
    if (baseRes.empty()) {
        //SDL_GetBasePath will return nullptr if something went wrong in retrieving the path
        char *basePath = SDL_GetBasePath();
        if (basePath != nullptr) {
            baseRes = basePath;
            SDL_free(basePath);
        } else {
            std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
            return "";
        }
        //We replace the last bin/ with res/ to get the the resource path
        size_t pos = baseRes.rfind("bin");
        baseRes = baseRes.substr(0, pos) + "resource" + PATH_SEP;
    }
    //If we want a specific subdirectory path in the resource directory
    //append it to the base path. This would be something like Lessons/res/Lesson0
    return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}

std::string getDelimitedContents(const std::string &input, const char DelimitingChar, const uint start) {
    uint delmitCount = 0;
    size_t beginPos = 0, endPos = 0;
    for (size_t i = 0; i < input.size(); i++) {
        if (DelimitingChar == input.at(i)) {
            delmitCount++;

            if (delmitCount == start) {
                beginPos = i + 1;
            }
        }

        if (delmitCount == (start + 1)) {
            endPos = i;
            break;
        }
    }

    if (delmitCount < start)
        return " ";

    if (endPos < beginPos)
        endPos = input.size();

    return input.substr(beginPos, endPos - beginPos);
}