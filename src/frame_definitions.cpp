#include "frame_definitions.h"

void SetupAnimations(player* playerManager, TextureManager* textureManager) {
    // Load sprite sheets
    textureManager->LoadImageFromRes("incremental_character", "incremental_character.png");
    
    // Auto-generate tiles from incremental_character.png
    PlayerAnimations nextId = playerManager->RegisterPlayerAnimationsFromGrid("incremental_character", 4, 8);
}