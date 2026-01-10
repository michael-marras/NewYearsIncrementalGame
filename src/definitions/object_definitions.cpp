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
    textureManager->LoadImageFromRes("winter_objects", "winter_objects.png");
    textureManager->LoadImageFromRes("summer_objects", "summer_objects.png");

    // Define all objects in a data structure
    std::vector<ObjectDefinitionData> objectDefinitions = {
        // Regular Rocks (require pickaxe)
        {1, "winter_objects", 16, 25, 16, 23, true, "winter_big_rock", 100, "winter_medium_rock", "pickaxe", {{"stone", 5}}},
        {2, "winter_objects", 32, 32, 16, 16, true, "winter_medium_rock", 50, "winter_small_rock", "pickaxe", {{"stone", 3}}},
        {3, "winter_objects", 48, 32, 16, 16, true, "winter_small_rock", 25, "", "pickaxe", {{"stone", 1}}},
        
        // Iron Rocks (require pickaxe)
        {4, "winter_objects", 0, 128, 16, 23, true, "winter_big_iron_rock", 150, "winter_medium_iron_rock", "pickaxe", {{"raw_iron", 3}}},
        {5, "winter_objects", 32, 128, 16, 16, true, "winter_medium_iron_rock", 75, "winter_small_iron_rock", "pickaxe", {{"raw_iron", 2}}},
        {6, "winter_objects", 64, 128, 16, 16, true, "winter_small_iron_rock", 37, "", "pickaxe", {{"raw_iron", 1}}},
        
        // Gold Rocks (require pickaxe)
        {7, "winter_objects", 16, 128, 16, 23, true, "winter_big_gold_rock", 200, "winter_medium_gold_rock", "pickaxe", {{"raw_gold", 3}}},
        {8, "winter_objects", 48, 128, 16, 16, true, "winter_medium_gold_rock", 100, "winter_small_gold_rock", "pickaxe", {{"raw_gold", 2}}},
        {9, "winter_objects", 80, 128, 16, 16, true, "winter_small_gold_rock", 50, "", "pickaxe", {{"raw_gold", 1}}},
        
        // Trees and Plants (require axe)
        {10, "winter_objects", 0, 72, 16, 24, true, "winter_tree", 100, "", "axe", {{"log", 3}}},
        {11, "winter_objects", 96, 80, 16, 16, true, "winter_bush", 50, "", "axe", {{"leaf", 4}}},
        {12, "winter_objects", 1, 20, 15, 28, true, "winter_well", 200, "", "", {}},

        // Regular Rocks (require pickaxe)
        {13, "summer_objects", 16, 25, 16, 23, true, "summer_big_rock", 100, "summer_medium_rock", "pickaxe", {{"stone", 5}}},
        {14, "summer_objects", 32, 32, 16, 16, true, "summer_medium_rock", 50, "summer_small_rock", "pickaxe", {{"stone", 3}}},
        {15, "summer_objects", 48, 32, 16, 16, true, "summer_small_rock", 25, "", "pickaxe", {{"stone", 1}}},
        
        // Iron Rocks (require pickaxe)
        {16, "summer_objects", 0, 128, 16, 23, true, "summer_big_iron_rock", 150, "summer_medium_iron_rock", "pickaxe", {{"raw_iron", 3}}},
        {17, "summer_objects", 32, 128, 16, 16, true, "summer_medium_iron_rock", 75, "summer_small_iron_rock", "pickaxe", {{"raw_iron", 2}}},
        {18, "summer_objects", 64, 128, 16, 16, true, "summer_small_iron_rock", 37, "", "pickaxe", {{"raw_iron", 1}}},
        
        // Gold Rocks (require pickaxe)
        {19, "summer_objects", 16, 128, 16, 23, true, "summer_big_gold_rock", 200, "summer_medium_gold_rock", "pickaxe", {{"raw_gold", 3}}},
        {20, "summer_objects", 48, 128, 16, 16, true, "summer_medium_gold_rock", 100, "summer_small_gold_rock", "pickaxe", {{"raw_gold", 2}}},
        {21, "summer_objects", 80, 128, 16, 16, true, "summer_small_gold_rock", 50, "", "pickaxe", {{"raw_gold", 1}}},
        
        // Trees and Plants (require axe)
        {22, "summer_objects", 0, 72, 16, 24, true, "summer_tree", 100, "", "axe", {{"log", 3}}},
        {23, "summer_objects", 96, 80, 16, 16, true, "summer_bush", 50, "", "axe", {{"leaf", 4}}},
        {24, "summer_objects", 1, 20, 15, 28, true, "summer_well", 200, "", "", {}},
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