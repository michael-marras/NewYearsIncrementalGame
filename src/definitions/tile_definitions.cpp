#include "definitions/tile_definitions.h"

void SetupTiles(TileManager* tileManager, TextureManager* textureManager) {
    // Load sprite sheets
    textureManager->LoadImageFromRes("summer_floors", "summer_floors.png");
    textureManager->LoadImageFromRes("fall_floors", "fall_floors.png");
    textureManager->LoadImageFromRes("winter_floors", "winter_floors.png");
    textureManager->LoadImageFromRes("spring_floors", "spring_floors.png");
    textureManager->LoadImageFromRes("fantasy_floors", "fantasy_floors.png");
    
    // Auto-generate tiles from floor sprite sheets (3x3 grid each = 9 tiles per biome)
    int nextId = tileManager->RegisterTilesFromGrid("summer_floors", 3, 3);
    nextId = tileManager->RegisterTilesFromGrid("fall_floors", 3, 3, 0, 0, nextId);
    nextId = tileManager->RegisterTilesFromGrid("winter_floors", 3, 3, 0, 0, nextId);
    nextId = tileManager->RegisterTilesFromGrid("spring_floors", 3, 3, 0, 0, nextId);
    nextId = tileManager->RegisterTilesFromGrid("fantasy_floors", 3, 3, 0, 0, nextId);
}

