#include "world/map_generation.h"
#include "items/resources.h"
#include "world/planet.h"
#include "world/object_node.h"
#include <cstdlib>
#include <vector>
#include <string>
#include <cmath>

GeneratedMap GenerateMapFromSeed(unsigned int seed, 
                                 TileManager* tileManager, 
                                 ObjectManager* objectManager,
                                 ResourceManager* resourceManager,
                                 int width, int height, PlanetBiome planetBiome, PlanetType planetType,
                                 Planet* planet, PlanetFace face) {
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
    
    // Create object nodes instead of directly placing objects
    // This allows for regenerating objects dynamically
    if (planet) {
        ObjectNodeManager* nodeManager = planet->GetOrCreateObjectNodeManager(face);
        
        // Determine which node types to create based on planet type
        std::vector<ObjectNodeType> nodeTypes;
        int numNodes = 0;
        
        if (planetType == PlanetType::TREE_PLANET) {
            nodeTypes.push_back(ObjectNodeType::TREE_NODE);
            numNodes = 15 + (width * width / 500);
        } else if (planetType == PlanetType::ROCK_PLANET) {
            nodeTypes.push_back(ObjectNodeType::ROCK_NODE);
            numNodes = 12 + (width * width / 600);
        } else if (planetType == PlanetType::IRON_PLANET) {
            nodeTypes.push_back(ObjectNodeType::IRON_NODE);
            numNodes = 10 + (width * width / 600);
        } else if (planetType == PlanetType::GOLD_PLANET) {
            nodeTypes.push_back(ObjectNodeType::GOLD_NODE);
            numNodes = 8 + (width * width / 700);
        }
        
        // Create nodes at random positions
        for (int i = 0; i < numNodes && !nodeTypes.empty(); i++) {
            ObjectNodeType nodeTypeToCreate = nodeTypes[std::rand() % nodeTypes.size()];
            
            // Random position (avoid edges by a margin)
            int margin = 5;
            int x = margin + std::rand() % (width - 2 * margin);
            int y = margin + std::rand() % (height - 2 * margin);

            int tier = planet->GetTier();
            float tierMultiplier = 1.0f + (tier * 0.4f);
            
            ObjectNode newNode;
            float range = (8.0f + (std::rand() % 5)) * tierMultiplier;
            float spawnRate = (0.2f + (std::rand() % 30) / 100.0f) * tierMultiplier;
            float initialEnergy = (50.0f + (std::rand() % 50)) * tierMultiplier;
            float maxEnergy = (100.0f + (std::rand() % 100)) * tierMultiplier;
            float energyRegen = (0.3f + (std::rand() % 40) / 100.0f) * tierMultiplier;
            int maxObjects = static_cast<int>((15 + (std::rand() % 15)) * tierMultiplier);
            
            newNode.Initialize(nodeTypeToCreate, face, x, y, range, spawnRate, initialEnergy, 
                              maxEnergy, energyRegen, maxObjects);
            
            if (objGrid) {
                newNode.SpawnInitialObjects(objectManager, objectGridId, width, height, planetBiome);
            }
            
            nodeManager->AddNode(newNode);
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

    // Determine biome (FANTASY has 1/20 chance, others share remaining 95%)
    std::srand(seed + 3000);
    int biomeRoll = std::rand() % 100;  // 0-99 for percentage-based selection
    PlanetBiome planetBiome;
    
    if (biomeRoll < 5) {
        // 5% chance for FANTASY (0-4)
        planetBiome = PlanetBiome::FANTASY;
    } else {
        // 95% chance split among 4 biomes (23.75% each)
        // Remap 5-99 to 0-3 for the 4 regular biomes
        int regularBiomeRoll = (biomeRoll - 5) % 4;
        switch (regularBiomeRoll) {
            case 0:
                planetBiome = PlanetBiome::SUMMER;
                break;
            case 1:
                planetBiome = PlanetBiome::FALL;
                break;
            case 2:
                planetBiome = PlanetBiome::WINTER;
                break;
            case 3:
                planetBiome = PlanetBiome::SPRING;
                break;
            default:
                planetBiome = PlanetBiome::SUMMER;
                break;
        }
    }
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
        GeneratedMap faceMap = GenerateMapFromSeed(faceSeed, tileManager, objectManager, resourceManager, width, width, planet->GetPlanetBiome(), planet->GetPlanetType(), planet, faces[i]);
        planet->SetFaceData(faces[i], faceMap.tileGridId, faceMap.objectGridId, faceMap.resourceArrayId);
    }
    
    return planet;
}

