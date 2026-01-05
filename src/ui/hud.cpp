#include "ui/hud.h"
#include "ui/text_renderer.h"
#include "core/textures.h"
#include <cstdio>
#include <cstring>
#include <SDL3/SDL.h>

HUD::HUD(SDL_Renderer* renderer) : renderer(renderer), textRenderer(nullptr) {
    // Create text renderer
    textRenderer = new TextRenderer(renderer);
    
    // Get base path for font loading
    const char* basePath = SDL_GetBasePath();
    char* fontPath = nullptr;
    if (basePath) {
        SDL_asprintf(&fontPath, "%sfonts/pixelfont.otf", basePath);
    } else {
        SDL_asprintf(&fontPath, "fonts/pixelfont.otf");
    }
    
    // Load font at size 16
    if (!textRenderer->LoadFont(fontPath, 16)) {
        SDL_Log("Warning: Failed to load font, text rendering may not work");
    }
    
    SDL_free(fontPath);
}

HUD::~HUD() {
    if (textRenderer) {
        delete textRenderer;
        textRenderer = nullptr;
    }
}

void HUD::Render(Player* player, ResourceManager* resourceManager, TextureManager* textureManager) {
    if (!player || !resourceManager || !textureManager) {
        return;
    }
    
    // Render resource counters in top-left corner
    RenderResourceCounter(player, resourceManager, textureManager, 10.0f, 10.0f);
}

void HUD::RenderResourceCounter(Player* player, ResourceManager* resourceManager, TextureManager* textureManager, float startX, float startY) {
    if (!player || !resourceManager || !textureManager) {
        return;
    }
    
    float yOffset = startY;
    const float lineHeight = 18.0f;
    const float iconSize = 16.0f;
    const float iconTextSpacing = 2.0f;
    
    const char* resourceNames[] = {
        "raw_gold", "gold_ingot", "gold_coin",
        "raw_iron", "iron_ingot", "iron_coin",
        "coal", "stone",
        "log", "plank", "leaf",
        "empty_bottle", "water_bottle"
    };
    
    int resourceCount = sizeof(resourceNames) / sizeof(resourceNames[0]);
    
    for (int i = 0; i < resourceCount; i++) {
        ResourceInfo* resourceInfo = resourceManager->GetResourceByName(resourceNames[i]);
        if (!resourceInfo) {
            continue;
        }
        
        int quantity = player->GetResourceQuantity(resourceInfo->id);
        
        if (quantity > 0) {
            float iconCenterX = startX + iconSize / 2.0f;
            float iconCenterY = yOffset + iconSize / 2.0f;
            textureManager->RenderResource(resourceManager, resourceInfo->id, iconCenterX, iconCenterY, 1.0f);

            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d", quantity);
            
            SDL_Color color = {255, 255, 255, 255};
            float textX = startX + iconSize + iconTextSpacing;
            float textY = iconCenterY;
            RenderText(buffer, textX, textY, color);
            
            yOffset += lineHeight;
        }
    }
}

void HUD::RenderText(const char* text, float x, float y, SDL_Color color) {
    if (!text || !textRenderer) {
        return;
    }
    
    textRenderer->RenderText(text, x, y, "left-center", color);
}

