#include "definitions/object_definitions.h"

void SetupObjects(ObjectManager* objectManager, TextureManager* textureManager) {
    // Load object sprite sheets
    textureManager->LoadImageFromRes("winter_objects", "winter_objects.png");

    // Regular Rocks
    objectManager->RegisterObject(1, "winter_objects", 16, 25, 16, 23, true, "winter_big_rock", 100, "winter_medium_rock");
    objectManager->RegisterObject(2, "winter_objects", 32, 32, 16, 16, true, "winter_medium_rock", 50, "winter_small_rock");
    objectManager->RegisterObject(3, "winter_objects", 48, 32, 16, 16, true, "winter_small_rock", 25, "");
    // Iron Rocks
    objectManager->RegisterObject(4, "winter_objects", 0, 128, 16, 23, true, "winter_big_iron_rock", 150, "winter_medium_iron_rock");
    objectManager->RegisterObject(5, "winter_objects", 32, 128, 16, 16, true, "winter_medium_iron_rockk", 75, "winter_small_iron_rock");
    objectManager->RegisterObject(6, "winter_objects", 64, 128, 16, 16, true, "winter_small_iron_rock", 37, "");
    // Gold Rocks
    objectManager->RegisterObject(7, "winter_objects", 16, 128, 16, 23, true, "winter_big_gold_rock", 200, "winter_medium_gold_rock");
    objectManager->RegisterObject(8, "winter_objects", 48, 128, 16, 16, true, "winter_medium_gold_rock", 100, "winter_small_gold_rock");
    objectManager->RegisterObject(9, "winter_objects", 80, 128, 16, 16, true, "winter_small_gold_rock", 50, "");

    objectManager->RegisterObject(10, "winter_objects", 0, 72, 16, 24, true, "winter_tree", 100);
    objectManager->RegisterObject(11, "winter_objects", 96, 80, 16, 16, true, "winter_bush", 50);
    objectManager->RegisterObject(12, "winter_objects", 1, 20, 15, 28, true, "winter_well", 200);

}