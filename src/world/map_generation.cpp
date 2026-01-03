#include "world/map_generation.h"
#include <cstdlib>
#include <vector>
#include <string>

// Structure to define object placement with density (0.0 to 1.0)
struct ObjectPlacement {
    std::string objectName;
    float density;  // 0.0 = none, 1.0 = every possible spot
    int fixedCount; // If > 0, use this exact count instead of density
};

GeneratedMap GenerateMapFromSeed(unsigned int seed, 
                                 TileManager* tileManager, 
                                 ObjectManager* objectManager,
                                 int width, int height) {
    // Seed the random number generator for reproducible generation
    std::srand(seed);
    
    // Generate tile grid with grass patches (seeded)
    int tileGridId = tileManager->CreateTileGridwGroupingsSeeded(width, height, seed);
    
    // Create object grid
    int objectGridId = objectManager->CreateObjectGrid(width, height);
    ObjectGrid* objGrid = objectManager->GetObjectGrid(objectGridId);
    
    if (!objGrid) {
        GeneratedMap result;
        result.tileGridId = tileGridId;
        result.objectGridId = objectGridId;
        return result;
    }
    
    // Define all objects with their density (0.0 to 1.0)
    // Density represents the fraction of total tiles that should have this object
    std::vector<ObjectPlacement> objectPlacements = {
        {"winter_tree", 0.25f, 0},
        {"winter_big_rock", 0.01f, 0},
        {"winter_medium_rock", 0.0025f, 0},
        {"winter_big_iron_rock", 0.00125f, 0},
        {"winter_medium_iron_rock", 0.00083f, 0},
        {"winter_big_gold_rock", 0.00083f, 0},
        {"winter_medium_gold_rock", 0.000625f, 0},
        {"winter_bush", 0.0056f, 0},
        {"winter_well", 0.0f, 20},
    };
    
    // Loop through all object placements
    for (const auto& placement : objectPlacements) {
        if (placement.density == 0.0f && placement.fixedCount == 0) {
            continue;
        }
        
        ObjectInfo* objInfo = objectManager->GetObjectByName(placement.objectName.c_str());
        if (!objInfo) {
            continue;
        }

        int numObjects;
        if (placement.fixedCount > 0) {
            numObjects = placement.fixedCount;
        } else {
            numObjects = static_cast<int>(placement.density * width * height);
        }
        
        for (int i = 0; i < numObjects; i++) {
            int x = std::rand() % width;
            int y = std::rand() % height;
            
            if (!objGrid->HasObject(x, y)) {
                objGrid->SetObject(x, y, objInfo->id);
            }
        }
    }
    
    GeneratedMap result;
    result.tileGridId = tileGridId;
    result.objectGridId = objectGridId;
    return result;
}

