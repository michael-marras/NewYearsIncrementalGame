#include "ui/hud.h"
#include "core/textures.h"
#include <cstdio>
#include <cstring>

HUD::HUD(SDL_Renderer* renderer) : renderer(renderer) {
}

HUD::~HUD() {
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
            float textY = iconCenterY - 5.0f;
            RenderText(buffer, textX, textY, color);
            
            yOffset += lineHeight;
        }
    }
}

void HUD::RenderText(const char* text, float x, float y, SDL_Color color) {
    if (!text || !renderer) {
        return;
    }
    
    float charX = x;
    const float charWidth = 6.0f;
    
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] >= '0' && text[i] <= '9') {
            RenderDigit(text[i], charX, y, color);
        }
        charX += charWidth;
    }
}

void HUD::RenderDigit(char digit, float x, float y, SDL_Color color) {
    if (!renderer || digit < '0' || digit > '9') {
        return;
    }
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    const float w = 8.0f;
    const float h = 10.0f;
    const float t = 2.0f;
    
    bool segments[10][7] = {
        {true,  true,  true,  false, true,  true,  true },  // 0
        {false, false, true,  false, false, true,  false},  // 1
        {true,  false, true,  true,  true,  false, true },  // 2
        {true,  false, true,  true,  false, true,  true },  // 3
        {false, true,  true,  true,  false, true,  false},  // 4
        {true,  true,  false, true,  false, true,  true },  // 5
        {true,  true,  false, true,  true,  true,  true },  // 6
        {true,  false, true,  false, false, true,  false},  // 7
        {true,  true,  true,  true,  true,  true,  true },  // 8
        {true,  true,  true,  true,  false, true,  true }   // 9
    };
    
    int d = digit - '0';
    bool* seg = segments[d];
    
    // Draw segments as rectangles
    // Top
    if (seg[0]) { SDL_FRect r = {x+1, y, w-2, t}; SDL_RenderFillRect(renderer, &r); }
    // Top-left
    if (seg[1]) { SDL_FRect r = {x, y+1, t, h/2-1}; SDL_RenderFillRect(renderer, &r); }
    // Top-right
    if (seg[2]) { SDL_FRect r = {x+w-t, y+1, t, h/2-1}; SDL_RenderFillRect(renderer, &r); }
    // Middle
    if (seg[3]) { SDL_FRect r = {x+1, y+h/2-t/2, w-2, t}; SDL_RenderFillRect(renderer, &r); }
    // Bottom-left
    if (seg[4]) { SDL_FRect r = {x, y+h/2+1, t, h/2-1}; SDL_RenderFillRect(renderer, &r); }
    // Bottom-right
    if (seg[5]) { SDL_FRect r = {x+w-t, y+h/2+1, t, h/2-1}; SDL_RenderFillRect(renderer, &r); }
    // Bottom
    if (seg[6]) { SDL_FRect r = {x+1, y+h-t, w-2, t}; SDL_RenderFillRect(renderer, &r); }
}

