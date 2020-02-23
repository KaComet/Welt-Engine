#ifndef WELT_RESOURCE_H
#define WELT_RESOURCE_H

#include <iostream>
#include <string>
#include <SDL.h>
#include "../../src/universal.h"

std::string getResourcePath(const std::string &subDir);

std::string getDelimitedContents(const std::string &input, char DelimitingChar, uint start);

#endif //WELT_RESOURCE_H
