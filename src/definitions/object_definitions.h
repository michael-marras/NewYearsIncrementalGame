#ifndef OBJECT_DEFINITIONS_H
#define OBJECT_DEFINITIONS_H

#include "world/objects.h"
#include "core/textures.h"

// Function to set up all object definitions
void SetupObjects(ObjectManager* objectManager, TextureManager* textureManager, ResourceManager* resourceManager);

enum class ObjectDefinitions: uint8_t {
    WINTER_BIG_ROCK = 1,
    WINTER_MEDIUM_ROCK = 2,
    WINTER_SMALL_ROCK = 3,
    WINTER_BIG_IRON_ROCK = 4,
    WINTER_MEDIUM_IRON_ROCK = 5,
    WINTER_SMALL_IRON_ROCK = 6,
    WINTER_BIG_GOLD_ROCK = 7,
    WINTER_MEDIUM_GOLD_ROCK = 8,
    WINTER_SMALL_GOLD_ROCK = 9,
    WINTER_TREE = 10,
    WINTER_BUSH = 11,
    WINTER_WELL = 12
};

#endif // OBJECT_DEFINITIONS_H

