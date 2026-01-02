#include "object_definitions.h"

void SetupObjects(ObjectManager* objectManager, TextureManager* textureManager) {
    // Load object sprite sheets
    textureManager->LoadImageFromRes("winter_objects", "winter_objects.png");

    objectManager->RegisterObject(1, "winter_objects", 16, 25, 16, 23, true, "winter_big_rock", 100, "winter_medium_rock");
    objectManager->RegisterObject(2, "winter_objects", 32, 32, 16, 16, true, "winter_medium_rock", 50, "winter_small_rock");
    objectManager->RegisterObject(3, "winter_objects", 48, 32, 16, 16, true, "winter_small_rock", 25, "");

    objectManager->RegisterObject(4, "winter_objects", 0, 72, 16, 24, true, "winter_tree", 100);
    objectManager->RegisterObject(5, "winter_objects", 96, 80, 16, 16, true, "winter_bush", 50);
    objectManager->RegisterObject(6, "winter_objects", 1, 20, 15, 28, true, "winter_well", 200);

}