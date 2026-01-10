#include "definitions/object_definitions.h"
#include "definitions/resource_definitions.h"
#include "items/resources.h"
#include <vector>
#include <string>

// Helper structure for drops using string names (converted to IDs during registration)
struct DropDefinition {
    const char* resourceName;
    int quantity;
};

// Structure to hold all object definition data
struct ObjectDefinitionData {
    int id;
    const char* sheetName;
    int sheetX;
    int sheetY;
    int width;
    int height;
    bool interactable;
    const char* name;
    int maxHealth;
    const char* deathReplacement;
    const char* requiredToolType;
    std::vector<DropDefinition> drops;
};

void SetupObjects(ObjectManager* objectManager, TextureManager* textureManager, ResourceManager* resourceManager) {
    // Load object sprite sheets
    textureManager->LoadImageFromRes("summer_objects", "summer_objects.png");
    textureManager->LoadImageFromRes("fall_objects", "fall_objects.png");
    textureManager->LoadImageFromRes("winter_objects", "winter_objects.png");
    textureManager->LoadImageFromRes("spring_objects", "spring_objects.png");
    textureManager->LoadImageFromRes("fantasy_objects", "fantasy_objects.png");

    // Define all objects in a data structure
    std::vector<ObjectDefinitionData> objectDefinitions = {

        // Summer (IDs 13-24)
        // Regular Rocks
        {13, "summer_objects", 2, 31, 16, 23, true, "summer_big_rock", 100, "summer_medium_rock", "pickaxe", {{"stone", 5}}},
        {14, "summer_objects", 19, 38, 16, 16, true, "summer_medium_rock", 50, "summer_small_rock", "pickaxe", {{"stone", 3}}},
        {15, "summer_objects", 37, 40, 16, 16, true, "summer_small_rock", 25, "", "pickaxe", {{"stone", 1}}},
        
        // Iron Rocks
        {16, "summer_objects", 2, 56, 16, 23, true, "summer_big_iron_rock", 150, "summer_medium_iron_rock", "pickaxe", {{"raw_iron", 3}}},
        {17, "summer_objects", 35, 56, 16, 16, true, "summer_medium_iron_rock", 75, "summer_small_iron_rock", "pickaxe", {{"raw_iron", 2}}},
        {18, "summer_objects", 69, 56, 16, 16, true, "summer_small_iron_rock", 37, "", "pickaxe", {{"raw_iron", 1}}},
        
        // Gold Rocks
        {19, "summer_objects", 18, 56, 16, 23, true, "summer_big_gold_rock", 200, "summer_medium_gold_rock", "pickaxe", {{"raw_gold", 3}}},
        {20, "summer_objects", 51, 56, 16, 16, true, "summer_medium_gold_rock", 100, "summer_small_gold_rock", "pickaxe", {{"raw_gold", 2}}},
        {21, "summer_objects", 85, 58, 16, 16, true, "summer_small_gold_rock", 50, "", "pickaxe", {{"raw_gold", 1}}},
        
        // Trees and Plants
        {22, "summer_objects", 2, 81, 16, 24, true, "summer_tree", 100, "", "axe", {{"log", 3}}},
        {23, "summer_objects", 54, 89, 16, 16, true, "summer_bush", 50, "", "axe", {{"leaf", 4}}},
        {24, "summer_objects", 2, 2, 15, 27, true, "summer_well", 200, "", "", {}},

        // Fall (IDs 25-36)
        // Regular Rocks
        {25, "fall_objects", 2, 31, 16, 23, true, "fall_big_rock", 100, "fall_medium_rock", "pickaxe", {{"stone", 5}}},
        {26, "fall_objects", 19, 38, 16, 16, true, "fall_medium_rock", 50, "fall_small_rock", "pickaxe", {{"stone", 3}}},
        {27, "fall_objects", 37, 40, 16, 16, true, "fall_small_rock", 25, "", "pickaxe", {{"stone", 1}}},
        
        // Iron Rocks
        {28, "fall_objects", 2, 56, 16, 23, true, "fall_big_iron_rock", 150, "fall_medium_iron_rock", "pickaxe", {{"raw_iron", 3}}},
        {29, "fall_objects", 35, 56, 16, 16, true, "fall_medium_iron_rock", 75, "fall_small_iron_rock", "pickaxe", {{"raw_iron", 2}}},
        {30, "fall_objects", 69, 56, 16, 16, true, "fall_small_iron_rock", 37, "", "pickaxe", {{"raw_iron", 1}}},
        
        // Gold Rocks
        {31, "fall_objects", 18, 56, 16, 23, true, "fall_big_gold_rock", 200, "fall_medium_gold_rock", "pickaxe", {{"raw_gold", 3}}},
        {32, "fall_objects", 51, 56, 16, 16, true, "fall_medium_gold_rock", 100, "fall_small_gold_rock", "pickaxe", {{"raw_gold", 2}}},
        {33, "fall_objects", 85, 58, 16, 16, true, "fall_small_gold_rock", 50, "", "pickaxe", {{"raw_gold", 1}}},
        
        // Trees and Plants
        {34, "fall_objects", 2, 81, 16, 24, true, "fall_tree", 100, "", "axe", {{"log", 3}}},
        {35, "fall_objects", 54, 89, 16, 16, true, "fall_bush", 50, "", "axe", {{"leaf", 4}}},
        {36, "fall_objects", 2, 2, 15, 27, true, "fall_well", 200, "", "", {}},

        // Winter (IDs 37-48)
        // Regular Rocks
        {37, "winter_objects", 2, 31, 16, 23, true, "winter_big_rock", 100, "winter_medium_rock", "pickaxe", {{"stone", 5}}},
        {38, "winter_objects", 19, 38, 16, 16, true, "winter_medium_rock", 50, "winter_small_rock", "pickaxe", {{"stone", 3}}},
        {39, "winter_objects", 37, 40, 16, 16, true, "winter_small_rock", 25, "", "pickaxe", {{"stone", 1}}},
        
        // Iron Rocks
        {40, "winter_objects", 2, 56, 16, 23, true, "winter_big_iron_rock", 150, "winter_medium_iron_rock", "pickaxe", {{"raw_iron", 3}}},
        {41, "winter_objects", 35, 56, 16, 16, true, "winter_medium_iron_rock", 75, "winter_small_iron_rock", "pickaxe", {{"raw_iron", 2}}},
        {42, "winter_objects", 69, 56, 16, 16, true, "winter_small_iron_rock", 37, "", "pickaxe", {{"raw_iron", 1}}},
        
        // Gold Rocks
        {43, "winter_objects", 18, 56, 16, 23, true, "winter_big_gold_rock", 200, "winter_medium_gold_rock", "pickaxe", {{"raw_gold", 3}}},
        {44, "winter_objects", 51, 56, 16, 16, true, "winter_medium_gold_rock", 100, "winter_small_gold_rock", "pickaxe", {{"raw_gold", 2}}},
        {45, "winter_objects", 85, 58, 16, 16, true, "winter_small_gold_rock", 50, "", "pickaxe", {{"raw_gold", 1}}},
        
        // Trees and Plants
        {46, "winter_objects", 2, 81, 16, 24, true, "winter_tree", 100, "", "axe", {{"log", 3}}},
        {47, "winter_objects", 54, 89, 16, 16, true, "winter_bush", 50, "", "axe", {{"leaf", 4}}},
        {48, "winter_objects", 2, 2, 15, 27, true, "winter_well", 200, "", "", {}},

        // Spring (IDs 49-60)
        // Regular Rocks
        {49, "spring_objects", 2, 31, 16, 23, true, "spring_big_rock", 100, "spring_medium_rock", "pickaxe", {{"stone", 5}}},
        {50, "spring_objects", 19, 38, 16, 16, true, "spring_medium_rock", 50, "spring_small_rock", "pickaxe", {{"stone", 3}}},
        {51, "spring_objects", 37, 40, 16, 16, true, "spring_small_rock", 25, "", "pickaxe", {{"stone", 1}}},
        
        // Iron Rocks
        {52, "spring_objects", 2, 56, 16, 23, true, "spring_big_iron_rock", 150, "spring_medium_iron_rock", "pickaxe", {{"raw_iron", 3}}},
        {53, "spring_objects", 35, 56, 16, 16, true, "spring_medium_iron_rock", 75, "spring_small_iron_rock", "pickaxe", {{"raw_iron", 2}}},
        {54, "spring_objects", 69, 56, 16, 16, true, "spring_small_iron_rock", 37, "", "pickaxe", {{"raw_iron", 1}}},
        
        // Gold Rocks
        {55, "spring_objects", 18, 56, 16, 23, true, "spring_big_gold_rock", 200, "spring_medium_gold_rock", "pickaxe", {{"raw_gold", 3}}},
        {56, "spring_objects", 51, 56, 16, 16, true, "spring_medium_gold_rock", 100, "spring_small_gold_rock", "pickaxe", {{"raw_gold", 2}}},
        {57, "spring_objects", 85, 58, 16, 16, true, "spring_small_gold_rock", 50, "", "pickaxe", {{"raw_gold", 1}}},
        
        // Trees and Plants
        {58, "spring_objects", 2, 81, 16, 24, true, "spring_tree", 100, "", "axe", {{"log", 3}}},
        {59, "spring_objects", 54, 89, 16, 16, true, "spring_bush", 50, "", "axe", {{"leaf", 4}}},
        {60, "spring_objects", 2, 2, 15, 27, true, "spring_well", 200, "", "", {}},

        // Fantasy (IDs 61-72)
        // Regular Rocks
        {61, "fantasy_objects", 2, 31, 16, 23, true, "fantasy_big_rock", 100, "fantasy_medium_rock", "pickaxe", {{"stone", 5}}},
        {62, "fantasy_objects", 19, 38, 16, 16, true, "fantasy_medium_rock", 50, "fantasy_small_rock", "pickaxe", {{"stone", 3}}},
        {63, "fantasy_objects", 37, 40, 16, 16, true, "fantasy_small_rock", 25, "", "pickaxe", {{"stone", 1}}},
        
        // Iron Rocks
        {64, "fantasy_objects", 2, 56, 16, 23, true, "fantasy_big_iron_rock", 150, "fantasy_medium_iron_rock", "pickaxe", {{"raw_iron", 3}}},
        {65, "fantasy_objects", 35, 56, 16, 16, true, "fantasy_medium_iron_rock", 75, "fantasy_small_iron_rock", "pickaxe", {{"raw_iron", 2}}},
        {66, "fantasy_objects", 69, 56, 16, 16, true, "fantasy_small_iron_rock", 37, "", "pickaxe", {{"raw_iron", 1}}},
        
        // Gold Rocks
        {67, "fantasy_objects", 18, 56, 16, 23, true, "fantasy_big_gold_rock", 200, "fantasy_medium_gold_rock", "pickaxe", {{"raw_gold", 3}}},
        {68, "fantasy_objects", 51, 56, 16, 16, true, "fantasy_medium_gold_rock", 100, "fantasy_small_gold_rock", "pickaxe", {{"raw_gold", 2}}},
        {69, "fantasy_objects", 85, 58, 16, 16, true, "fantasy_small_gold_rock", 50, "", "pickaxe", {{"raw_gold", 1}}},
        
        // Trees and Plants
        {70, "fantasy_objects", 2, 81, 16, 24, true, "fantasy_tree", 100, "", "axe", {{"log", 3}}},
        {71, "fantasy_objects", 54, 89, 16, 16, true, "fantasy_bush", 50, "", "axe", {{"leaf", 4}}},
        {72, "fantasy_objects", 2, 2, 15, 27, true, "fantasy_well", 200, "", "", {}},
    };

    // Automatically register all objects
    for (const auto& objDef : objectDefinitions) {
        std::vector<DropInstance> dropInstances;
        for (const auto& dropDef : objDef.drops) {
            ResourceInfo* resourceInfo = resourceManager->GetResourceByName(dropDef.resourceName);
            if (resourceInfo) {
                dropInstances.push_back({resourceInfo->id, dropDef.quantity});
            }
        }
        
        objectManager->RegisterObject(
            objDef.id,
            objDef.sheetName,
            objDef.sheetX,
            objDef.sheetY,
            objDef.width,
            objDef.height,
            objDef.interactable,
            objDef.name,
            objDef.maxHealth,
            objDef.deathReplacement,
            objDef.requiredToolType,
            dropInstances
        );
    }
}