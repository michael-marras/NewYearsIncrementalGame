#include "ui/hud.h"
// #include "ui/text_renderer.h" // DISABLED: FreeType dependency removed
#include "core/textures.h"
#include "utils/constants.h"
#include "entities/player.h"
#include "items/resources.h"
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <SDL3/SDL.h>

HUD::HUD(SDL_Renderer* renderer) : renderer(renderer) /* textRenderer(nullptr) */ {
    // DISABLED: FreeType dependency removed
    // Create text renderer
    // textRenderer = new TextRenderer(renderer);
    
    // Get base path for font loading
    // const char* basePath = SDL_GetBasePath();
    // char* fontPath = nullptr;
    // if (basePath) {
    //     SDL_asprintf(&fontPath, "%sfonts/pixelfont.ttf", basePath);
    // } else {
    //     SDL_asprintf(&fontPath, "fonts/pixelfont.ttf");
    // }
    
    // Load font at size 16
    // if (!textRenderer->LoadFont(fontPath, 16)) {
    //     SDL_Log("Warning: Failed to load font, text rendering may not work");
    // }
    
    // SDL_free(fontPath);
}

HUD::~HUD() {
    // DISABLED: FreeType dependency removed
    // if (textRenderer) {
    //     delete textRenderer;
    //     textRenderer = nullptr;
    // }
}

void HUD::Render(Player* player, ResourceManager* resourceManager, TextureManager* textureManager) {
    if (!player || !resourceManager || !textureManager) {
        return;
    }
    
    // Render resource counters in top-left corner
    RenderResourceCounter(player, resourceManager, textureManager, 10.0f, 10.0f);
    
    RenderPickupEvents(resourceManager, textureManager);
}

void HUD::Update(float deltaTime) {
    // Update all pickup event timers
    for (auto& event : pickupEvents) {
        event.timer -= deltaTime;
    }
    
    // Remove expired events
    pickupEvents.erase(
        std::remove_if(pickupEvents.begin(), pickupEvents.end(),
            [](const ResourcePickupEvent& e) { return e.timer <= 0.0f; }),
        pickupEvents.end()
    );
}

void HUD::OnResourcePickedUp(int resourceId, int quantity) {
    // Find existing event for this resource
    auto it = std::find_if(pickupEvents.begin(), pickupEvents.end(),
        [resourceId](const ResourcePickupEvent& e) { return e.resourceId == resourceId; });
    
    if (it != pickupEvents.end()) {
        // Update existing event: restart timer and add to quantity
        it->timer = it->lifetime; 
        it->totalQuantity += quantity;
    } else {
        // Create new event
        pickupEvents.emplace_back(resourceId, quantity);
    }
}

void HUD::RenderResourceCounter(Player* player, ResourceManager* resourceManager, TextureManager* textureManager, float startX, float startY) {
    if (!player || !resourceManager || !textureManager) {
        return;
    }
}

void HUD::RenderPickupEvents(ResourceManager* resourceManager, TextureManager* textureManager) {
    // DISABLED: FreeType dependency removed
    // if (!resourceManager || !textureManager || !textRenderer) {
    if (!resourceManager || !textureManager) {
        return;
    }
    
    // Early return if no pickup events
    if (pickupEvents.empty()) {
        return;
    }
    
    // DISABLED: Text rendering removed with FreeType
    // Render in top-right corner
    // const float padding = 10.0f;
    // const float startY = 10.0f;
    // const float iconSize = 16.0f;
    // const float iconTextSpacing = -2.0f;
    // const float lineHeight = 20.0f;
    // const float virtualWidth = (float)VIRTUAL_WIDTH;
    
    // float currentY = startY;
    
    // Render each active pickup event
    // for (const auto& event : pickupEvents) {
    //     ResourceInfo* resourceInfo = resourceManager->GetResource(event.resourceId);
    //     if (!resourceInfo) {
    //         continue;
    //     }
    //     
    //     float alpha = 1.0f;
    //     if (event.timer < 1.0f) {
    //         alpha = event.timer / 1.0f;
    //         if (alpha < 0.0f) alpha = 0.0f;
    //     }
    //     Uint8 alphaByte = (Uint8)(alpha * 255.0f);
    //     
    //     // Render resource icon
    //     float iconCenterX = virtualWidth - padding - iconSize / 2.0f;
    //     float iconCenterY = currentY + iconSize / 2.0f;
    //     textureManager->RenderResource(resourceManager, event.resourceId, iconCenterX, iconCenterY, 1.0f);
    //     
    //     // Render quantity text (e.g., "+15")
    //     char buffer[32];
    //     snprintf(buffer, sizeof(buffer), "+%d", event.totalQuantity);
    //     
    //     SDL_Color textColor = {255, 255, 255, alphaByte};
    //     float textX = iconCenterX - iconSize / 2.0f - iconTextSpacing;
    //     float textY = iconCenterY;
    //     
    //     // Get text width for right alignment
    //     int textWidth = textRenderer->GetTextWidth(buffer);
    //     textX -= textWidth;
    //     
    //     RenderText(buffer, textX, textY, textColor);
    //     
    //     currentY += lineHeight;
    // }
}

void HUD::RenderText(const char* text, float x, float y, SDL_Color color) {
    // DISABLED: FreeType dependency removed
    // if (!text || !textRenderer) {
    //     return;
    // }
    // 
    // textRenderer->RenderText(text, x, y, "left-center", color);
    
    (void)text;  // Suppress unused parameter warnings
    (void)x;
    (void)y;
    (void)color;
}

