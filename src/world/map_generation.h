#ifndef MAP_GENERATION_H
#define MAP_GENERATION_H

#include "world/tiles.h"
#include "world/objects.h"
#include "world/planet.h"

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
                                 int width, int height, PlanetBiome planetBiome, PlanetType planetType);

// Generate a cube planet with 6 faces from a seed
// Returns a Planet with all 6 faces initialized
// Uses the tier to determine the planet size (radius)
Planet* GeneratePlanetFromSeed(unsigned int seed,
                                TileManager* tileManager,
                                ObjectManager* objectManager,
                                ResourceManager* resourceManager,
                                int tier);

#endif // MAP_GENERATION_H

