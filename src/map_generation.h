#ifndef MAP_GENERATION_H
#define MAP_GENERATION_H

#include "tiles.h"
#include "objects.h"

// Structure to hold generated map grid IDs
struct GeneratedMap {
    int tileGridId;
    int objectGridId;
};

// Generate both tile map and objects from a seed
// Returns the grid IDs for both tile and object grids
GeneratedMap GenerateMapFromSeed(unsigned int seed, 
                                 TileManager* tileManager, 
                                 ObjectManager* objectManager,
                                 int width, int height);

#endif // MAP_GENERATION_H

