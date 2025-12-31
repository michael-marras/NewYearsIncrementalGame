#include "tile_definitions.h"

void SetupTiles(TileManager* tileManager, TextureManager* textureManager) {
    // Load sprite sheets
    textureManager->LoadImageFromRes("winter_floors", "winter_floors.png");
    
    // Auto-generate tiles from winter_floors.png
    int nextId = tileManager->RegisterTilesFromGrid("winter_floors", 7, 5);

}

