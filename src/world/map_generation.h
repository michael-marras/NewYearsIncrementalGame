#ifndef MAP_GENERATION_H
#define MAP_GENERATION_H

#include "world/tiles.h"
#include "world/objects.h"

// Forward declaration
class ResourceManager;

// Structure to hold generated map grid IDs
struct GeneratedMap {
    int tileGridId;
    int objectGridId;
    int resourceArrayId;
};

// Generate both tile map and objects from a seed
// Returns the grid IDs for both tile and object grids
GeneratedMap GenerateMapFromSeed(unsigned int seed, 
                                 TileManager* tileManager, 
                                 ObjectManager* objectManager,
                                 ResourceManager* resourceManager,
                                 int width, int height);

#endif // MAP_GENERATION_H

