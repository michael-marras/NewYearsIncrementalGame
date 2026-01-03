#include "definitions/resource_definitions.h"

void SetupResources(ResourceManager* resourceManager, TextureManager* textureManager) {
    textureManager->LoadImageFromRes("resources", "resources.png");

    resourceManager->RegisterResource(1, "resources", 0, 0, 16, 16, true, "raw_gold");
    resourceManager->RegisterResource(2, "resources", 16, 0, 16, 16, true, "gold_ingot");
    resourceManager->RegisterResource(3, "resources", 32, 0, 16, 16, true, "gold_coin");

    resourceManager->RegisterResource(4, "resources", 0, 16, 16, 16, true, "raw_iron");
    resourceManager->RegisterResource(5, "resources", 16, 16, 16, 16, true, "iron_ingot");
    resourceManager->RegisterResource(6, "resources", 32, 16, 16, 16, true, "iron_coin");

    resourceManager->RegisterResource(7, "resources", 0, 32, 16, 16, true, "coal");
    resourceManager->RegisterResource(8, "resources", 16, 32, 16, 16, true, "stone");

    resourceManager->RegisterResource(9, "resources", 0, 48, 16, 16, true, "log");
    resourceManager->RegisterResource(10, "resources", 16, 48, 16, 16, true, "plank");
    resourceManager->RegisterResource(11, "resources", 32, 48, 16, 16, true, "leaf");

    resourceManager->RegisterResource(12, "resources", 0, 64, 16, 16, true, "empty_bottle");
    resourceManager->RegisterResource(13, "resources", 16, 64, 16, 16, true, "water_bottle");
}