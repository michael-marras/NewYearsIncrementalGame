#include "map_generation.h"
#include <cstdlib>

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
    
    // Get object IDs by name (adjust names based on your registered objects)
    ObjectInfo* treeObj = objectManager->GetObjectByName("winter_tree");
    ObjectInfo* bigRockObj = objectManager->GetObjectByName("winter_big_rock");
    ObjectInfo* mediumRockObj = objectManager->GetObjectByName("winter_medium_rock");
    ObjectInfo* bushObj = objectManager->GetObjectByName("winter_bush");
    ObjectInfo* wellObj = objectManager->GetObjectByName("winter_well");
    
    // Procedurally place objects
    // Adjust these parameters to control object density and placement
    const int numTrees = (width * height) / 200;        // Tree density
    const int numBigRocks = (width * height) / 100;     // Big rock density
    const int numMediumRocks = (width * height) / 400;  // Medium rock density
    const int numBushes = (width * height) / 180;       // Bush density
    const int numWells = 10;
    
    // Place trees
    if (treeObj) {
        for (int i = 0; i < numTrees; i++) {
            int x = std::rand() % width;
            int y = std::rand() % height;
            // Only place if no object already there
            if (!objGrid->HasObject(x, y)) {
                objGrid->SetObject(x, y, treeObj->id);
            }
        }
    }
    
    // Place big rocks
    if (bigRockObj) {
        for (int i = 0; i < numBigRocks; i++) {
            int x = std::rand() % width;
            int y = std::rand() % height;
            if (!objGrid->HasObject(x, y)) {
                objGrid->SetObject(x, y, bigRockObj->id);
            }
        }
    }
    
    // Place medium rocks
    if (mediumRockObj) {
        for (int i = 0; i < numMediumRocks; i++) {
            int x = std::rand() % width;
            int y = std::rand() % height;
            if (!objGrid->HasObject(x, y)) {
                objGrid->SetObject(x, y, mediumRockObj->id);
            }
        }
    }
    
    // Place bushes
    if (bushObj) {
        for (int i = 0; i < numBushes; i++) {
            int x = std::rand() % width;
            int y = std::rand() % height;
            if (!objGrid->HasObject(x, y)) {
                objGrid->SetObject(x, y, bushObj->id);
            }
        }
    }

    // Place wells
    if (wellObj) {
        for (int i = 0; i < numWells; i++) {
            int x = std::rand() % width;
            int y = std::rand() % height;
            if (!objGrid->HasObject(x, y)) {
                objGrid->SetObject(x, y, wellObj->id);
            }
        }
    }
    
    GeneratedMap result;
    result.tileGridId = tileGridId;
    result.objectGridId = objectGridId;
    return result;
}

