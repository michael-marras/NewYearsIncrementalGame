#include "definitions/frame_definitions.h"

void SetupAnimations(Player* playerManager, TextureManager* textureManager) {
    // Load sprite sheets
    textureManager->LoadImageFromRes("incremental_character", "player/incremental_character.png");
    
    // Auto-generate tiles from incremental_character.png
    PlayerAnimations nextId = playerManager->RegisterPlayerAnimationsFromGrid("incremental_character", 4, 8);
}