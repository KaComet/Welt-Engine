// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "world.h"
#include "Iitem.h"
#include "Sheep.h"
#include "Wolf.h"
#include "ltimer.h"
#include "SpriteSet.h"
#include "ColorList.h"
#include "ItemTestStick.h"
#include "SpriteInteractionsList.h"
#include "material.h"
#include "universal.h"
#include "resource.h"
#include <SDL.h>
#include <vector>
#include <fstream>
#include <SDL_image.h>

const uint WORLD_HEIGHT = 50;
const uint WORLD_WIDTH = 50;

// Sprite, SI, color, and file name constants
const SpriteInteraction TI_ERROR = SpriteInteraction{"TI_ERROR", 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
const SDL_Color COLOR_ERROR = SDL_Color{0xFF, 0x00, 0xFF};
const std::string SIFileName = "spritedefinitions_default.txt";
const std::string colorSetFileName = "colorset_default.txt";
const std::string spriteSetFileName = "derived_luculent_15x15.png";
const uint TILE_WIDTH = 15;
const uint TILE_HEIGHT = 15;
const uint MAX_SI = 500;
const uint MAX_COLOR = 500;

//Screen constants
const int SCREEN_FPS = 45;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
const int SCREEN_WIDTH = TILE_WIDTH * WORLD_WIDTH;
const int SCREEN_HEIGHT = TILE_HEIGHT * WORLD_HEIGHT;

//The window we'll be rendering to
SDL_Window *gWindow = nullptr;

//The window renderer
SDL_Renderer *gRenderer = nullptr;

void setWorldFloorToMaterial(TileMap &map, Material m);

void setWorldWallToMaterial(TileMap &map, Material m, uint h);

void drawLineOfWalls(TileMap &map, Coordinate start, Coordinate end, Material m, uint health);

bool init();

bool loadSpriteSetFromFile(SDL_Renderer *renderer, SpriteSet &spriteSet, const std::string &fileName);

void close();

int main(int argc, char *args[]) {
    printf("--WELT--\n");

    // Start SDL and create window
    if (!init()) {
        printf("!Failed to initialize SDL! Closing...\n");
    } else {
        // Create and load the resources. If something fails to load, allResourcesLoaded will be
        //   set to false.
        SpriteSet sprites;
        SpriteInteractionsList tileI;
        ColorList colors;
        bool allResourcesLoaded = true;
        if (!tileI.loadSpriteInteractionsFromFile(SIFileName, TI_ERROR, MAX_SI))
            allResourcesLoaded = false;
        if (!colors.loadColorsFromFile(colorSetFileName, COLOR_ERROR, MAX_COLOR))
            allResourcesLoaded = false;
        if (!loadSpriteSetFromFile(gRenderer, sprites, spriteSetFileName))
            allResourcesLoaded = false;

        // If all resources were not loaded, exit the application.
        if (!allResourcesLoaded) {
            printf("!Failed to load required resources! Closing...\n");
        } else {
            // Main loop flag
            bool quit = false;

            // Event handler
            SDL_Event eventH;

            SDL_Rect screenRect;
            screenRect.h = SCREEN_HEIGHT;
            screenRect.w = SCREEN_WIDTH;
            screenRect.x = 0;
            screenRect.y = 0;

            // Setup the viewport for the map's renderer.
            SDL_Rect mapViewRect;
            mapViewRect.x = 0;
            mapViewRect.y = 0;
            mapViewRect.w = TILE_WIDTH * WORLD_WIDTH;
            mapViewRect.h = TILE_HEIGHT * WORLD_HEIGHT;
            SDL_RenderSetViewport(gRenderer, &mapViewRect);

            // Create a timer to regulate the FPS. Without this, the application
            //   will render as fast as possible. This would waste system resources.
            LTimer fpsReg;

            // Create a DisplayArray so we can get data out of the world.
            DisplayArray dis;

            // Create a chunk and load it with some test data.
            World a(WORLD_HEIGHT, WORLD_WIDTH);

            // Create a wolf.
            auto *wolf = new Wolf;

            // Create 40 sheep.
            std::vector<Ientity *> theHerd;
            for (uint i = 0; i < 40; i++)
                theHerd.push_back(new Sheep);

            // Add the wolf to the center of the world.
            a.addEntity(wolf, Coordinate{WORLD_WIDTH / 2, WORLD_HEIGHT / 2});

            // Set the floor material to grass, and the wall material to air.
            Material grassTmp = M_GRASS;
            Material airTmp = M_AIR;
            setWorldFloorToMaterial(*(a.getMap()), grassTmp);
            setWorldWallToMaterial(*(a.getMap()), airTmp, airTmp.baseHealth);

            // Place the sheep in a rough square around the wolf.
            for (uint i = 0; i < 10; i++)
                a.addEntity(theHerd.at(i), Coordinate{(WORLD_WIDTH / 2) - 10, ((WORLD_HEIGHT / 2) - 10) + i});

            for (uint i = 10; i < 20; i++)
                a.addEntity(theHerd.at(i), Coordinate{(WORLD_WIDTH / 2) + 10, ((WORLD_HEIGHT / 2) - 10) + i});

            for (uint i = 20; i < 30; i++)
                a.addEntity(theHerd.at(i), Coordinate{((WORLD_HEIGHT / 2) - 15) + i, (WORLD_HEIGHT / 2) - 10});

            for (uint i = 30; i < 40; i++)
                a.addEntity(theHerd.at(i), Coordinate{((WORLD_WIDTH / 2) + 15) + i, (WORLD_HEIGHT / 2) - 10});

            // Create a test item and add it to the test world.
            auto *testItem = new ItemTestStick;

            a.addItem(testItem, Coordinate{5, 5});

            //While application is running
            while (!quit) {
                fpsReg.start();

                // If there are events in the que, try to process them,
                while (SDL_PollEvent(&eventH) != 0) {
                    // If the user wants to quit the app, signal to exit.
                    if (eventH.type == SDL_QUIT) {
                        quit = true;
                    }
                }

                // Clear the renderer.
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                // Draw the background as solid black.
                SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
                SDL_RenderFillRect(gRenderer, &screenRect);
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                // Load the DisplayArray with the display data for the chunk.
                a.loadDisplayArray(dis);

                // Draw the background elements to the renderer.
                for (uint row = 0; row < dis.height; row++) {
                    for (uint col = 0; col < dis.width; col++) {
                        DisplayArrayElement currentElement = dis.displayData[col + (dis.width * row)];
                        // Draw the background elements.
                        // If the Sprite is not in the SpriteSet or is defined as invalid, print the error sprite. (sprite 0)
                        if (sprites.getNTiles() <= currentElement.BackgroundInfo) {
                            sprites.setColor(COLOR_ERROR);
                            sprites.render(col * TILE_WIDTH, row * TILE_HEIGHT, tileI.get(0).dDefault);
                        } else {
                            SDL_Color backgroundColor = colors.get(currentElement.BackgroundColor);

                            sprites.setColor(backgroundColor);
                            sprites.render(col * TILE_WIDTH, row * TILE_HEIGHT,
                                           tileI.getBackgroundTileFromDisplayArray(col, row, dis));
                        }

                        // Draw the foreground elements.
                        // If the Sprite is not in the SpriteSet or is defined as invalid, print the error sprite. (sprite 0)
                        if (sprites.getNTiles() <= currentElement.ForegroundInfo) {
                            sprites.setColor(COLOR_ERROR);
                            sprites.render(col * TILE_WIDTH, row * TILE_HEIGHT, tileI.get(0).dDefault);
                        } else {
                            SDL_Color foregroundColor = colors.get(currentElement.ForegroundColor);

                            sprites.setColor(foregroundColor);
                            sprites.render(col * TILE_WIDTH, row * TILE_HEIGHT,
                                           tileI.getForegroundTileFromDisplayArray(col, row, dis));
                        }
                    }
                }

                // Update screen with the contents of the renderer.
                SDL_RenderPresent(gRenderer);

                // Wait out the rest of the frame.
                int elapsedTicks = fpsReg.getTicks();
                if (elapsedTicks < SCREEN_TICKS_PER_FRAME)
                    SDL_Delay((Uint32) (SCREEN_TICKS_PER_FRAME - elapsedTicks));

                // Tick the chunk.
                a.tick();

                //printf("Tick: %u\n", a.getTickNumber());
            }
        }
    }

    // If the user has chosen to exit, free resources and close SDL.
    close();

    return 0;
}

// Initializes SDL2, creates a window, and creates a renderer.
// Call before using any SDL2 resources.
bool init() {
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("!SDL could not initialize! SDL Error: %s\n !", SDL_GetError());
        success = false;
    } else {
        //Try to set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
            printf("!Linear texture filtering could not be enabled!");
        }

        //Create window
        gWindow = SDL_CreateWindow("Welt Engine Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                   SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == nullptr) {
            printf("!Window could not be created. SDL Error: %s\n !", SDL_GetError());
            success = false;
        } else {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == nullptr) {
                printf("!Renderer could not be created. SDL Error: %s\n !", SDL_GetError());
                success = false;
            } else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags)) {
                    printf("!SDL_image could not initialize. SDL_image Error: %s\n !", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

// Closes any SDL2/SDL2_Image resources. Call before exiting the application.
void close() {
    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

// Sets the floor material of all tile in a chunk to the specified material.
void setWorldFloorToMaterial(TileMap &map, Material m) {
    for (uint row = 0; row < WORLD_HEIGHT; row++) {
        for (uint column = 0; column < WORLD_WIDTH; column++) {
            map.setFloorMaterial(Coordinate{column, row}, m);
        }
    }
}

// Sets the wall material of all tile in a chunk to the specified material.
void setWorldWallToMaterial(TileMap &map, Material m, uint h) {
    for (uint row = 0; row < WORLD_HEIGHT; row++) {
        for (uint column = 0; column < WORLD_WIDTH; column++) {
            map.setWallMaterial(Coordinate{column, row}, m, h);
        }
    }
}

void drawLineOfWalls(TileMap &map, Coordinate start, Coordinate end, Material m, uint health) {
    // Bresenham's line algorithm. Adapted from https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm
    bool steep = (fabs(end.y - start.y) > fabs(end.x - start.x));
    if (steep) {
        std::swap(start.x, start.y);
        std::swap(end.x, end.y);
    }

    if (start.x > end.x) {
        std::swap(start.x, end.x);
        std::swap(start.y, end.y);
    }

    const double dx = end.x - start.x;
    const double dy = fabs(end.y - start.y);
    const int yStep = (start.y < end.y) ? 1 : -1;
    double error = dx / 2.0f;
    uint y = start.y;

    for (uint x = start.x; x < end.x; x++) {
        Coordinate currentPos = steep ? Coordinate{y, x} : Coordinate{x, y};

        // TODO: FIX
        map.setWallMaterial(currentPos, m, health);

        error -= dy;
        if (error < 0) {
            y += yStep;
            error += dx;
        }
    }
}

bool loadSpriteSetFromFile(SDL_Renderer *renderer, SpriteSet &spriteSet, const std::string &fileName) {
    // Check if all the provided pointers are valid. If not, return false.
    if (!renderer || fileName.empty())
        return false;

    const std::string path = getResourcePath("tileset") + fileName;
    const uint NTiles = spriteSet.loadFromFile(renderer, path, TILE_WIDTH, TILE_HEIGHT);

    // Print the number of elements loaded.
    printf("Loaded %u tiles\n", NTiles);

    // If no elements could not be loaded, return false.
    if (NTiles)
        return true;
    else
        return false;
}

