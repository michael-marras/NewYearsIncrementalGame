#include "definitions/resource_definitions.h"

void SetupResources(ResourceManager* resourceManager, TextureManager* textureManager) {
    textureManager->LoadImageFromRes("resources", "resources.png");

    resourceManager->RegisterResource(1, "resources", 0, 0, 16, 16, true, "raw_gold", 100, "Gold Ore");
    resourceManager->RegisterResource(2, "resources", 16, 0, 16, 16, true, "gold_ingot", 200, "Gold Ingot");
    resourceManager->RegisterResource(3, "resources", 32, 0, 16, 16, true, "gold_coin", 400, "Gold Coin");

    resourceManager->RegisterResource(4, "resources", 0, 16, 16, 16, true, "raw_iron", 40, "Iron Ore");
    resourceManager->RegisterResource(5, "resources", 16, 16, 16, 16, true, "iron_ingot", 80, "Iron Ingot");
    resourceManager->RegisterResource(6, "resources", 32, 16, 16, 16, true, "iron_coin", 160, "Iron Coin");

    resourceManager->RegisterResource(7, "resources", 0, 32, 16, 16, true, "coal", 20, "Coal");
    resourceManager->RegisterResource(8, "resources", 16, 32, 16, 16, true, "stone", 3, "Stone");

    resourceManager->RegisterResource(9, "resources", 0, 48, 16, 16, true, "log", 2, "Log");
    resourceManager->RegisterResource(10, "resources", 16, 48, 16, 16, true, "plank", 4, "Plank");
    resourceManager->RegisterResource(11, "resources", 32, 48, 16, 16, true, "leaf", 1, "Leaf");

    resourceManager->RegisterResource(12, "resources", 0, 64, 16, 16, true, "empty_bottle", 0, "Empty Bottle");
    resourceManager->RegisterResource(13, "resources", 16, 64, 16, 16, true, "water_bottle", 0, "Water Bottle");
}