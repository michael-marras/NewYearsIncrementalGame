#include "world/map_generation.h"
#include "items/resources.h"
#include "world/planet.h"
#include <cstdlib>
#include <vector>
#include <string>

struct ObjectPlacement {
    std::string objectName;
    float density;
    int fixedCount;
};

GeneratedMap GenerateMapFromSeed(unsigned int seed, 
                                 TileManager* tileManager, 
                                 ObjectManager* objectManager,
                                 ResourceManager* resourceManager,
                                 int width, int height, PlanetBiome planetBiome, PlanetType planetType) {
    // Seed the random number generator for reproducible generation
    std::srand(seed);
    
    // Generate tile grid with grass patches (seeded)
    int tileGridId = tileManager->CreateTileGridwGroupingsSeeded(width, height, seed, planetBiome);
    
    // Create object grid
    int objectGridId = objectManager->CreateObjectGrid(width, height);
    ObjectGrid* objGrid = objectManager->GetObjectGrid(objectGridId);
    
    // Create resource array
    int resourceArrayId = -1;
    if (resourceManager) {
        resourceArrayId = resourceManager->CreateResourceArray();
    }
    
    if (!objGrid) {
        GeneratedMap result;
        result.tileGridId = tileGridId;
        result.objectGridId = objectGridId;
        result.resourceArrayId = resourceArrayId;
        return result;
    }
    
    std::vector<ObjectPlacement> objectPlacements;
    
    // Determine object names based on biome
    std::string treeName = (planetBiome == PlanetBiome::SUMMER) ? "summer_tree" : "winter_tree";
    std::string bushName = (planetBiome == PlanetBiome::SUMMER) ? "summer_bush" : "winter_bush";
    std::string wellName = (planetBiome == PlanetBiome::SUMMER) ? "summer_well" : "winter_well";
    std::string bigRockName = (planetBiome == PlanetBiome::SUMMER) ? "summer_big_rock" : "winter_big_rock";
    std::string medRockName = (planetBiome == PlanetBiome::SUMMER) ? "summer_medium_rock" : "winter_medium_rock";
    std::string bigIronName = (planetBiome == PlanetBiome::SUMMER) ? "summer_big_iron_rock" : "winter_big_iron_rock";
    std::string medIronName = (planetBiome == PlanetBiome::SUMMER) ? "summer_medium_iron_rock" : "winter_medium_iron_rock";
    std::string bigGoldName = (planetBiome == PlanetBiome::SUMMER) ? "summer_big_gold_rock" : "winter_big_gold_rock";
    std::string medGoldName = (planetBiome == PlanetBiome::SUMMER) ? "summer_medium_gold_rock" : "winter_medium_gold_rock";
    
    objectPlacements = {
        {treeName, 0.0f, 0},
        {bushName, 0.0f, 0},
        {wellName, 0.0f, 0},
        {bigRockName, 0.0f, 0},
        {medRockName, 0.0f, 0},
        {bigIronName, 0.0f, 0},
        {medIronName, 0.0f, 0},
        {bigGoldName, 0.0f, 0},
        {medGoldName, 0.0f, 0},
    };
    
    if (planetType == PlanetType::TREE_PLANET) {
        for (auto& placement : objectPlacements) {
            if (placement.objectName.find("tree") != std::string::npos) {
                placement.density = 0.25f;
            } else if (placement.objectName.find("bush") != std::string::npos) {
                placement.density = 0.015f;
            }
        }
    } else if (planetType == PlanetType::ROCK_PLANET) {
        for (auto& placement : objectPlacements) {
            if (placement.objectName.find("rock") != std::string::npos && 
                placement.objectName.find("iron") == std::string::npos &&
                placement.objectName.find("gold") == std::string::npos) {
                // Regular rocks
                if (placement.objectName.find("big") != std::string::npos) {
                    placement.density = 0.15f;
                } else if (placement.objectName.find("medium") != std::string::npos) {
                    placement.density = 0.075f;
                }
            } else if (placement.objectName.find("tree") != std::string::npos) {
                placement.density = 0.1f;
            }
        }
    } else if (planetType == PlanetType::IRON_PLANET) {
        for (auto& placement : objectPlacements) {
            if (placement.objectName.find("iron") != std::string::npos) {
                if (placement.objectName.find("big") != std::string::npos) {
                    placement.density = 0.1f;
                } else if (placement.objectName.find("medium") != std::string::npos) {
                    placement.density = 0.05f;
                }
            }
        }
    } else if (planetType == PlanetType::GOLD_PLANET) {
        for (auto& placement : objectPlacements) {
            if (placement.objectName.find("gold") != std::string::npos) {
                if (placement.objectName.find("big") != std::string::npos) {
                    placement.density = 0.1f;
                } else if (placement.objectName.find("medium") != std::string::npos) {
                    placement.density = 0.05f;
                }
            }
        }
    }
    
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
    result.resourceArrayId = resourceArrayId;
    return result;
}

Planet* GeneratePlanetFromSeed(unsigned int seed,
                                TileManager* tileManager,
                                ObjectManager* objectManager,
                                ResourceManager* resourceManager,
                                int tier) {
    Planet* planet = new Planet();

    PlanetSize size;
    std::srand(seed + 1000);
    int sizeRoll = std::rand() % 100;
    
    if (tier == 0) {
        size = PlanetSize::TINY;
    } else if (tier == 1) {
        if (sizeRoll < 80) {
            size = PlanetSize::SMALL;
        } else {
            size = PlanetSize::TINY;
        }
    } else if (tier == 2) {
        if (sizeRoll < 40) {
            size = PlanetSize::MEDIUM;
        } else if (sizeRoll < 90) {
            size = PlanetSize::SMALL;
        } else {
            size = PlanetSize::LARGE;
        }
    } else if (tier == 3) {
        if (sizeRoll < 30) {
            size = PlanetSize::SMALL;
        } else if (sizeRoll < 70) {
            size = PlanetSize::MEDIUM;
        } else {
            size = PlanetSize::LARGE;
        }
    } else {
        if (sizeRoll < 10) {
            size = PlanetSize::SMALL;
        } else if (sizeRoll < 70) {
            size = PlanetSize::MEDIUM;
        } else {
            size = PlanetSize::LARGE;
        }
    }

    // Determine biome (50/50 WINTER/SUMMER)
    PlanetBiome planetBiome = (seed % 2 == 0) ? PlanetBiome::SUMMER : PlanetBiome::WINTER;
    planet->SetPlanetBiome(planetBiome);
    
    PlanetType planetType;
    std::srand(seed + 2000);
    int typeRoll = std::rand() % 100;
    
    if (tier == 0) {
        // Tier 0: Always TREE_PLANET
        planetType = PlanetType::TREE_PLANET;
    } else if (tier == 1) {
        // Tier 1: 70% TREE, 10% each of IRON/GOLD/ROCK
        if (typeRoll < 70) {
            planetType = PlanetType::TREE_PLANET;
        } else if (typeRoll < 80) {
            planetType = PlanetType::IRON_PLANET;
        } else if (typeRoll < 90) {
            planetType = PlanetType::GOLD_PLANET;
        } else {
            planetType = PlanetType::ROCK_PLANET;
        }
    } else if (tier == 2) {
        // Tier 2: 40% TREE, 20% each of IRON/GOLD/ROCK
        if (typeRoll < 40) {
            planetType = PlanetType::TREE_PLANET;
        } else if (typeRoll < 60) {
            planetType = PlanetType::IRON_PLANET;
        } else if (typeRoll < 80) {
            planetType = PlanetType::GOLD_PLANET;
        } else {
            planetType = PlanetType::ROCK_PLANET;
        }
    } else if (tier == 3) {
        // Tier 3: 20% TREE, 26.67% each of IRON/GOLD/ROCK (rounded: 27% IRON, 27% GOLD, 26% ROCK)
        if (typeRoll < 20) {
            planetType = PlanetType::TREE_PLANET;
        } else if (typeRoll < 47) {
            planetType = PlanetType::IRON_PLANET;
        } else if (typeRoll < 74) {
            planetType = PlanetType::GOLD_PLANET;
        } else {
            planetType = PlanetType::ROCK_PLANET;
        }
    } else {
        // Tier 4+: 10% TREE, 30% each of IRON/GOLD/ROCK
        if (typeRoll < 10) {
            planetType = PlanetType::TREE_PLANET;
        } else if (typeRoll < 40) {
            planetType = PlanetType::IRON_PLANET;
        } else if (typeRoll < 70) {
            planetType = PlanetType::GOLD_PLANET;
        } else {
            planetType = PlanetType::ROCK_PLANET;
        }
    }
    planet->SetPlanetType(planetType);
    
    planet->setRadius(GetPlanetRadius(size));
    planet->SetTier(tier);
    planet->MarkDirty();  // Mark as dirty so it renders properly
    int width = planet->getRadius() * 2;
    
    // Array of all 6 faces
    PlanetFace faces[] = {
        PlanetFace::FRONT,
        PlanetFace::BACK,
        PlanetFace::LEFT,
        PlanetFace::RIGHT,
        PlanetFace::TOP,
        PlanetFace::BOTTOM
    };
    
    // Generate each face with a slightly different seed (seed + face index)
    // This ensures each face is unique but reproducible
    for (int i = 0; i < 6; i++) {
        unsigned int faceSeed = seed + i;
        GeneratedMap faceMap = GenerateMapFromSeed(faceSeed, tileManager, objectManager, resourceManager, width, width, planet->GetPlanetBiome(), planet->GetPlanetType());
        planet->SetFaceData(faces[i], faceMap.tileGridId, faceMap.objectGridId, faceMap.resourceArrayId);
    }
    
    return planet;
}

