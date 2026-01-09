#include "ui/inventory.h"
#include "ui/text_renderer.h"
#include "core/textures.h"
#include "core/input_manager.h"
#include <cstdio>
#include <cstring>
#include <SDL3/SDL.h>

Inventory::Inventory(SDL_Renderer* renderer) : textRenderer(nullptr), isOpen(false), yScroll(0.0f) {
}

Inventory::~Inventory() {
    if (textRenderer) {
        delete textRenderer;
        textRenderer = nullptr;
    }
}

void Inventory::Toggle() {
    isOpen = !isOpen;
}

bool Inventory::IsOpen() const {
    return isOpen;
}

void Inventory::Open() {
    isOpen = true;
}

void Inventory::Close() {
    isOpen = false;
}

void Inventory::Update(InputManager* inputManager, Player* player, ResourceManager* resourceManager) {
    if (!inputManager || !isOpen || !player || !resourceManager) {
        return;
    }
    
    // Calculate max scrollY based on current inventory contents
    std::unordered_map<int, int>* playerInventory = player->getInventory();
    if (!playerInventory) {
        return;
    }
    
    // Count valid items (items with quantity > 0 that exist in ResourceManager)
    int validItemCount = 0;
    for (const auto& entry : *playerInventory) {
        int resourceId = entry.first;
        int quantity = entry.second;
        if (quantity > 0 && resourceManager->GetResource(resourceId)) {
            validItemCount++;
        }
    }
    
    // Calculate max scrollY
    const float itemSize = 16.0f;
    const float padding = 6.0f;
    const float panelHeight = 126.0f;
    float itemHeight = itemSize + padding;
    float visibleHeight = panelHeight - (padding * 2.0f); // Account for top/bottom padding
    float maxScrollY = CalculateMaxScrollY(validItemCount, itemHeight, visibleHeight);
    
    // Handle scroll wheel input
    float scrollDelta = inputManager->GetMouseWheelY();
    if (scrollDelta != 0.0f) {
        HandleScroll(scrollDelta, maxScrollY);
    }
}

void Inventory::Render(SDL_Renderer* renderer, Player* player, ResourceManager* resourceManager, TextureManager* textureManager) {
    if (!isOpen) {
        return;
    }
    
    if (!player || !resourceManager || !textureManager) {
        return;
    }
    
    // Lazy load inventory texture if not already loaded
    if (!textureManager->HasTexture("inventory")) {
        textureManager->LoadImageFromRes("inventory", "inventory.png");
    }
    
    // Render the inventory background panel
    textureManager->RenderInventory(1.0f);
    
    std::unordered_map<int, int>* playerInventory = player->getInventory();
    if (!playerInventory) {
        SDL_Log("Inventory: player->getInventory() returned nullptr");
        return;
    }
    
    SDL_Log("Inventory: Rendering %zu resources", playerInventory->size());
    
    // Inventory panel position and size
    const float panelStartX = 48.0f;
    const float panelStartY = 27.0f;
    const float panelWidth = 224.0f;
    const float panelHeight = 126.0f;
    const float panelEndY = panelStartY + panelHeight;

    SDL_Rect clipRect;
    clipRect.x = (int)panelStartX;
    clipRect.y = (int)panelStartY + 2;
    clipRect.w = (int)panelWidth;
    clipRect.h = (int)panelHeight - 4;
    SDL_SetRenderClipRect(renderer, &clipRect);
    
    // Grid layout settings
    const int itemsPerRow = 8;
    const float itemSize = 16.0f;
    const float padding = 6.0f;
    const float startX = panelStartX + padding;
    const float startY = panelStartY + padding;

    int validItemCount = 0;
    for (const auto& entry : *playerInventory) {
        int resourceId = entry.first;
        int quantity = entry.second;
        if (quantity > 0 && resourceManager->GetResource(resourceId)) {
            validItemCount++;
        }
    }
    
    float itemHeight = itemSize + padding;
    float visibleHeight = panelHeight - (padding * 2.0f);
    float maxScrollY = CalculateMaxScrollY(validItemCount, itemHeight, visibleHeight);
    if (yScroll > maxScrollY) {
        yScroll = maxScrollY;
    }
    
    int itemIndex = 0;
    for (const auto& entry : *playerInventory) {
        int resourceId = entry.first;
        int quantity = entry.second;
        
        if (quantity <= 0) {
            continue;
        }
        
        ResourceInfo* resourceInfo = resourceManager->GetResource(resourceId);
        if (!resourceInfo) {
            SDL_Log("Inventory: Resource ID %d not found (quantity: %d)", resourceId, quantity);
            continue;
        }
        
        float itemX = startX + padding;
        float baseItemY = startY + (itemIndex * (itemSize + padding)) + (itemSize / 2.0f);
        
        float itemY = baseItemY - GetScrollY();
        
        float itemTop = itemY - itemSize / 2.0f;
        float itemBottom = itemY + itemSize / 2.0f;
        
        if (itemBottom < panelStartY) {
            itemIndex++;
            continue;
        }
        if (itemTop > panelEndY) {
            break;
        }
        

        bool rendered = textureManager->RenderResource(resourceManager, resourceId, itemX, itemY, 1.0f);
        if (!rendered) {
            SDL_Log("Inventory: Failed to render resource ID %d (%s) at (%.1f, %.1f)", 
                    resourceId, resourceInfo->name.c_str(), itemX, itemY);
        } else {
            SDL_Log("Inventory: Rendered resource ID %d (%s) x%d at (%.1f, %.1f)", 
                    resourceId, resourceInfo->name.c_str(), quantity, itemX, itemY);
        }
        
        // Render quantity text (optional - you can implement this later)
        // TODO: Render quantity text near the item
        
        itemIndex++;
    }
    
    SDL_SetRenderClipRect(renderer, nullptr);
}

void Inventory::RenderText(SDL_Renderer* renderer, const char* text, float x, float y, SDL_Color color) {
    // TODO: Render text using text renderer
}

void Inventory::HandleMouseClick(float mouseX, float mouseY, Player* player) {
    // TODO: Handle mouse clicks on inventory items
}

bool Inventory::IsMouseInInventory(float mouseX, float mouseY) const {
    // TODO: Check if mouse position is within inventory UI bounds
    return false;
}

float Inventory::CalculateMaxScrollY(int itemCount, float itemHeight, float visibleHeight) const {
    float totalHeight = itemCount * itemHeight;
    float maxScroll = totalHeight - visibleHeight;
    return (maxScroll > 0.0f) ? maxScroll : 0.0f;
}

void Inventory::HandleScroll(float scrollDelta, float maxScrollY) {
    const float scrollSpeed = 20.0f;
    yScroll += scrollDelta * scrollSpeed;
    
    if (yScroll < 0.0f) {
        yScroll = 0.0f;
    } else if (yScroll > maxScrollY) {
        yScroll = maxScrollY;
    }
}

