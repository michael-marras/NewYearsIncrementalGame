#include "ui/inventory.h"
#include "ui/text_renderer.h"
#include "core/textures.h"
#include "core/input_manager.h"
#include "world/planet.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <SDL3/SDL.h>

Inventory::Inventory(SDL_Renderer* renderer) : textRenderer(nullptr), isOpen(false), yScroll(0.0f), mode(InventoryMode::NoValues), currentTotalValue(0.0f) {
    // Create text renderer
    textRenderer = new TextRenderer(renderer);
    
    // Get base path for font loading
    const char* basePath = SDL_GetBasePath();
    char* fontPath = nullptr;
    if (basePath) {
        SDL_asprintf(&fontPath, "%sfonts/pixelfont.ttf", basePath);
    } else {
        SDL_asprintf(&fontPath, "fonts/pixelfont.ttf");
    }
    
    if (!textRenderer->LoadFont(fontPath, 14)) {
        SDL_Log("Warning: Failed to load font for inventory, text rendering may not work");
    }
    
    SDL_free(fontPath);
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
    yScroll = 0.0f;
}

void Inventory::Close() {
    isOpen = false;
}

void Inventory::SetMode(InventoryMode newMode) {
    mode = newMode;
    inputTexts.clear();
    focusedResourceId = -1;
    if (newMode == InventoryMode::NoValues) {
        yScroll = 0.0f;
    }
}

void Inventory::Update(SDL_Renderer* renderer, InputManager* inputManager, Player* player, ResourceManager* resourceManager) {
    if (!renderer || !inputManager || !isOpen || !player || !resourceManager) {
        return;
    }

    // Check if clicking outside inventory closes it (only for NoValues mode)
    if (mode == InventoryMode::NoValues && inputManager->IsMouseButtonPressed(1)) {
        float mouseX = inputManager->GetMouseX();
        float mouseY = inputManager->GetMouseY();
        
        float mouseRenderX, mouseRenderY;
        SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &mouseRenderX, &mouseRenderY);
        if (!IsMouseInInventory(mouseRenderX, mouseRenderY)) {
            Toggle();
            return;
        }
    }
    
    if (mode == InventoryMode::WithValues && inputManager->IsMouseButtonPressed(1)) {
        float mouseX = inputManager->GetMouseX();
        float mouseY = inputManager->GetMouseY();
        
        float mouseRenderX, mouseRenderY;
        SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &mouseRenderX, &mouseRenderY);
        
        std::unordered_map<int, int>* playerInventory = player->getInventory();
        if (playerInventory) {
            if (resourceManager) {
                float count = 0.0f;
                for (const auto& entry : *playerInventory) {
                    int resourceId = entry.first;
                    ResourceInfo* resourceInfo = resourceManager->GetResource(resourceId);
                    if (resourceInfo && resourceInfo->value > 0.0f) {
                        count += 1.0f;
                    }
                }
                
                const float panelStartX = 48.0f;
                const float panelStartY = 27.0f;
                const float panelHeight = 126.0f;
                const float panelWidth = 224.0f;
                const float padding = 8.0f;
                const float submitButtonWidth = 50.0f;
                const float submitButtonHeight = 14.0f;

                // Ensure count is valid for button position calculation
                if (count >= 0.0f) {
                    const float submitButtonY = 27.0f + count * 24.0f + 8.0f - yScroll;
                    const float submitButtonX = panelStartX + panelWidth - submitButtonWidth - padding;
                    
                    if (mouseRenderX >= submitButtonX && mouseRenderX <= submitButtonX + submitButtonWidth &&
                        mouseRenderY >= submitButtonY && mouseRenderY <= submitButtonY + submitButtonHeight) {
                        float totalValue = 0.0f;
                        std::unordered_map<int, int> resourcesToConsume; // resourceId -> quantity to consume
                        
                        Planet* currentPlanet = nullptr;
                        if (player) {
                            currentPlanet = player->GetPlanet();
                        }
                        
                        float targetValue = 0.0f;
                        bool hasLimit = false;
                        if (currentPlanet) {
                            float energyCost = currentPlanet->GetEnergyCost();
                            float currentEnergy = currentPlanet->GetCurrentEnergy();
                            // Ensure we don't get negative targetValue
                            targetValue = (energyCost > currentEnergy) ? (energyCost - currentEnergy) : 0.0f;
                            hasLimit = (targetValue > 0.0f);
                        }
                        
                        // Calculate totalValue and track which resources to consume
                        for (const auto& entry : *playerInventory) {
                            int resourceId = entry.first;
                            ResourceInfo* resourceInfo = resourceManager->GetResource(resourceId);
                            if (resourceInfo && resourceInfo->value > 0.0f) {
                                int inputValue = GetInputValue(resourceId);
                                if (inputValue > 0) {
                                    float resourceValue = resourceInfo->value * static_cast<float>(inputValue);
                                    
                                    if (hasLimit && totalValue + resourceValue > targetValue) {
                                        // Partial consumption needed - calculate how much of this resource to use
                                        float remaining = targetValue - totalValue;
                                        if (remaining > 0.0f) {
                                            // Calculate partial quantity needed
                                            float resourceValuePerUnit = resourceInfo->value;
                                            int partialQuantity = static_cast<int>(remaining / resourceValuePerUnit);
                                            // Ensure we don't consume more than the user input
                                            if (partialQuantity > inputValue) {
                                                partialQuantity = inputValue;
                                            }
                                            if (partialQuantity > 0) {
                                                resourcesToConsume[resourceId] = partialQuantity;
                                                totalValue += resourceValuePerUnit * static_cast<float>(partialQuantity);
                                            }
                                        }
                                        break; // Reached target, stop
                                    } else {
                                        // Full consumption
                                        resourcesToConsume[resourceId] = inputValue;
                                        totalValue += resourceValue;
                                        
                                        if (hasLimit && totalValue >= targetValue) {
                                            // Exactly reached target
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        
                        // Clamp totalValue to target if it slightly exceeded
                        if (hasLimit && totalValue > targetValue) {
                            totalValue = targetValue;
                        }
                        
                        if (submitCallback) {
                            submitCallback(totalValue, resourcesToConsume);
                        }
                        Toggle();
                        
                        return;
                    }
                }
            }
            // Match the exact values from Render function
            const float itemSize = 16.0f;
            const float padding = 8.0f;
            const float panelStartX = 48.0f;
            const float panelStartY = 27.0f;
            const float startX = panelStartX + padding;
            const float startY = panelStartY + padding;
            const float textSpacing = 4.0f;
            const float inputBoxWidth = 40.0f;
            const float inputBoxHeight = 16.0f;
            const float inputBoxSpacing = 4.0f;
            
            int itemIndex = 0;
            for (const auto& entry : *playerInventory) {
                int resourceId = entry.first;
                int quantity = entry.second;
                
                if (quantity <= 0) {
                    continue;
                }
                
                if (!resourceManager) {
                    continue;
                }
                ResourceInfo* resourceInfo = resourceManager->GetResource(resourceId);
                if (!resourceInfo) {
                    continue;
                }
                if (mode == InventoryMode::WithValues && resourceInfo->value == 0.0f) {
                    continue;
                }
                
                float itemX = startX + padding;
                float baseItemY = startY + (itemIndex * (itemSize + padding)) + (itemSize / 2.0f);
                float itemY = baseItemY - GetScrollY();
                
                float itemTop = itemY - itemSize / 2.0f;
                float itemBottom = itemY + itemSize / 2.0f;

                const float panelHeight = 126.0f;
                const float panelEndY = panelStartY + panelHeight;
                
                if (itemBottom < panelStartY) {
                    itemIndex++;
                    continue;
                }
                if (itemTop > panelEndY) {
                    break;
                }
                
                const float textSpacing = 4.0f;
                float textX = itemX + itemSize + textSpacing;
                const float inputBoxX = textX + 100.0f;
                const float inputBoxY = itemY - inputBoxHeight / 2.0f;
                
                if (CheckAndFocusInputBox(mouseRenderX, mouseRenderY, resourceId, inputBoxX, inputBoxY, inputBoxWidth, inputBoxHeight)) {
                    break;
                }
                
                itemIndex++;
            }
        }
    }
    
    if (mode == InventoryMode::WithValues && focusedResourceId >= 0) {
        if (inputManager->IsKeyPressed(SDLK_BACKSPACE)) {
            HandleBackspace();
        }
        
        for (int i = 0; i <= 9; i++) {
            SDL_Keycode key = (SDL_Keycode)(SDLK_0 + i);
            if (inputManager->IsKeyPressed(key)) {
                char digit = '0' + i;
                HandleTextInput(&digit);
            }
        }
    }
    
    if (mode == InventoryMode::WithValues && focusedResourceId >= 0 && player) {
        std::unordered_map<int, int>* playerInventory = player->getInventory();
        if (playerInventory) {
            auto it = playerInventory->find(focusedResourceId);
            if (it != playerInventory->end()) {
                int maxQuantity = it->second;
                int inputValue = GetInputValue(focusedResourceId);
                if (inputValue > maxQuantity) {
                    char buffer[32];
                    snprintf(buffer, sizeof(buffer), "%d", maxQuantity);
                    inputTexts[focusedResourceId] = buffer;
                }
            }
        }
    }
    
    if (!player) {
        return;
    }
    std::unordered_map<int, int>* playerInventory = player->getInventory();
    if (!playerInventory) {
        return;
    }
    
    // Count valid items based on mode
    int validItemCount = 0;
    if (resourceManager) {
        for (const auto& entry : *playerInventory) {
            int resourceId = entry.first;
            int quantity = entry.second;
            if (quantity > 0) {
                ResourceInfo* resourceInfo = resourceManager->GetResource(resourceId);
                if (resourceInfo) {
                    if (mode == InventoryMode::NoValues || resourceInfo->value > 0.0f) {
                        validItemCount++;
                    }
                }
            }
        }
    }
    
    // Calculate max scrollY
    const float itemSize = 16.0f;
    const float padding = 6.0f;
    const float panelHeight = 126.0f;
    float itemHeight = itemSize + padding;
    float visibleHeight = panelHeight - (padding * 2.0f);
    
    if (mode == InventoryMode::WithValues) {
        const float totalSubmitSpace = 40.0f;
        visibleHeight -= totalSubmitSpace;
    }
    
    float maxScrollY = CalculateMaxScrollY(validItemCount, itemHeight, visibleHeight);
    
    // Handle scroll wheel inputm
    float scrollDelta = inputManager->GetMouseWheelY();
    if (scrollDelta != 0.0f) {
        HandleScroll(scrollDelta, maxScrollY);
    }
}

void Inventory::Render(SDL_Renderer* renderer, Player* player, ResourceManager* resourceManager, TextureManager* textureManager) {
    if (!isOpen) {
        return;
    }
    
    if (!renderer || !player || !resourceManager || !textureManager) {
        return;
    }
    
    // Lazy load inventory texture if not already loaded
    if (!textureManager->HasTexture("inventory")) {
        textureManager->LoadImageFromRes("inventory", "inventory.png");
    }
    
    // Lazy load input box texture if not already loaded
    if (!textureManager->HasTexture("inputbox")) {
        textureManager->LoadImageFromRes("inputbox", "inputsbox.png");
    }
    
    // Render the inventory background panel
    textureManager->RenderInventory(1.0f);
    
    std::unordered_map<int, int>* playerInventory = player->getInventory();
    if (!playerInventory) {
        SDL_Log("Inventory: player->getInventory() returned nullptr");
        return;
    }
    
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
    const float padding = 8.0f;
    const float startX = panelStartX + padding;
    const float startY = panelStartY + padding;

    int validItemCount = 0;
    for (const auto& entry : *playerInventory) {
        int resourceId = entry.first;
        int quantity = entry.second;
        if (quantity > 0) {
            ResourceInfo* resourceInfo = resourceManager->GetResource(resourceId);
            if (resourceInfo) {
                if (mode == InventoryMode::NoValues || resourceInfo->value > 0.0f) {
                    validItemCount++;
                }
            }
        }
    }
    
    float itemHeight = itemSize + padding;
    float visibleHeight = panelHeight - (padding * 2.0f);
    
    // In WithValues mode, reserve space for total display and submit button at the bottom
    if (mode == InventoryMode::WithValues) {
        const float totalSubmitSpace = 40.0f; // Space for total display + submit button
        visibleHeight -= totalSubmitSpace;
    }
    
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
        
        if (mode == InventoryMode::WithValues && resourceInfo->value == 0.0f) {
            continue;
        }
        
        float itemX = startX + padding;
        float baseItemY = startY + (itemIndex * (itemSize + padding)) + (itemSize / 2.0f);
        float itemY = baseItemY - yScroll;
        
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
        }
        
        const float textSpacing = 4.0f;
        float textX = itemX + itemSize + textSpacing;
        float textY = itemY;

        if (mode == InventoryMode::WithValues && resourceInfo->value > 0.0f) {
            RenderWithValue(renderer, textureManager, resourceInfo, resourceId, quantity, textX, textY);
        } else {
            RenderNoValue(renderer, resourceInfo, quantity, textX, textY);
        }
        itemIndex++;
    }

    if (mode == InventoryMode::WithValues) {
        currentTotalValue = 0.0f;
        float count = 0;
        if (playerInventory) {
            for (const auto& entry : *playerInventory) {
                int resourceId = entry.first;
                ResourceInfo* resourceInfo = resourceManager->GetResource(resourceId);
                if (resourceInfo && resourceInfo->value > 0.0f) {
                    int inputValue = GetInputValue(resourceId);
                    currentTotalValue += resourceInfo->value * static_cast<float>(inputValue);
                    count += 1;
                }
            }
        }
        
        const float submitButtonWidth = 50.0f;
        const float submitButtonHeight = 14.0f;
        // Fixed position at bottom (not affected by scroll)
        const float panelEndY = panelStartY + panelHeight;
        const float submitButtonY = 27.0f + count * 24 + 8.0f - yScroll;
        const float submitButtonX = panelStartX + panelWidth - submitButtonWidth - padding;
        
        SDL_Color totalTextColor = {255, 255, 255, 255};
        char totalTextBuffer[64];
        snprintf(totalTextBuffer, sizeof(totalTextBuffer), "Total Value: $%.0f", currentTotalValue);
        RenderText(renderer, totalTextBuffer, 56, submitButtonY + 7, "left-center", totalTextColor);
        
        SDL_SetRenderDrawColor(renderer, 100, 150, 100, 255);
        SDL_FRect submitButtonRect = {submitButtonX, submitButtonY, submitButtonWidth, submitButtonHeight};
        SDL_RenderFillRect(renderer, &submitButtonRect);
        
        // Draw button border
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderRect(renderer, &submitButtonRect);
        
        // Draw button text
        const float submitTextX = submitButtonX + submitButtonWidth / 2.0f;
        const float submitTextY = submitButtonY + submitButtonHeight / 2.0f;
        SDL_Color submitTextColor = {255, 255, 255, 255};
        RenderText(renderer, "Submit", submitTextX, submitTextY, "center-center", submitTextColor);
    }
    
    SDL_SetRenderClipRect(renderer, nullptr);
    
}

void Inventory::RenderNoValue(SDL_Renderer* renderer, ResourceInfo* resourceInfo, int quantity, float textX, float textY) {
    SDL_Color textColor = {255, 255, 255, 255};
    const char* displayName = resourceInfo->displayName.empty() ? resourceInfo->name.c_str() : resourceInfo->displayName.c_str();
    RenderText(renderer, displayName, textX, textY, "left-center", textColor);
    
    const float quantityAlignX = textX + 180.0f;
    
    char quantityBuffer[32];
    snprintf(quantityBuffer, sizeof(quantityBuffer), "x%d", quantity);
    RenderText(renderer, quantityBuffer, quantityAlignX, textY, "right-center", textColor);
}

void Inventory::RenderWithValue(SDL_Renderer* renderer, TextureManager* textureManager, ResourceInfo* resourceInfo, int resourceId, int quantity, float textX, float textY) {
    if (!renderer || !textureManager || !resourceInfo || resourceInfo->value == 0.0f) {
        return;
    }

    SDL_Color textColor = {255, 255, 255, 255};
    const char* displayName = resourceInfo->displayName.empty() ? resourceInfo->name.c_str() : resourceInfo->displayName.c_str();
    RenderText(renderer, displayName, textX, textY, "left-center", textColor);
    
    // Render input box to the left of the value
    const float inputBoxWidth = 40.0f;
    const float inputBoxHeight = 16.0f;
    const float inputBoxSpacing = 4.0f;
    const float valueAlignX = textX + 180.0f;
    const float inputBoxX = textX + 100.0f;
    const float inputBoxY = textY - inputBoxHeight / 2.0f;
    
    // Render input box background
    TextureInfo* inputBoxTex = textureManager->GetTexture("inputbox");
    if (inputBoxTex && inputBoxTex->texture) {
        SDL_FRect inputBoxRect = {inputBoxX, inputBoxY, inputBoxWidth, inputBoxHeight};
        SDL_RenderTexture(renderer, inputBoxTex->texture, nullptr, &inputBoxRect);
    } else {
        // Fallback: draw a simple rectangle
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_FRect inputBoxRect = {inputBoxX, inputBoxY, inputBoxWidth, inputBoxHeight};
        SDL_RenderFillRect(renderer, &inputBoxRect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderRect(renderer, &inputBoxRect);
    }
    
    // Render input text or placeholder
    std::string inputText;
    auto it = inputTexts.find(resourceId);
    if (it != inputTexts.end()) {
        inputText = it->second;
    }
    if (inputText.empty() && focusedResourceId != resourceId) {
        inputText = "0";
    }
    
    // Validate input doesn't exceed quantity
    int inputValue = GetInputValue(resourceId);
    if (inputValue > quantity) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%d", quantity);
        inputText = buffer;
        inputTexts[resourceId] = buffer;
    }
    
    const float inputTextX = inputBoxX + 4.0f;
    const float inputTextY = inputBoxY + inputBoxHeight / 2.0f;
    SDL_Color inputTextColor = {255, 255, 255, 255};
    if (focusedResourceId == resourceId) {
        inputTextColor = {200, 255, 200, 255};
    }
    RenderText(renderer, inputText.c_str(), inputTextX, inputTextY, "left-center", inputTextColor);
    
    const float quantityAlignX = textX + 50.0f;
    
    char quantityBuffer[32];
    snprintf(quantityBuffer, sizeof(quantityBuffer), "x%d", quantity);
    RenderText(renderer, quantityBuffer, quantityAlignX, textY, "left-center", textColor);

    int inputValueForCalc = GetInputValue(resourceId);
    float totalValue = resourceInfo->value * static_cast<float>(inputValueForCalc);
    char valueBuffer[32];
    snprintf(valueBuffer, sizeof(valueBuffer), "$%.0f", totalValue);
    RenderText(renderer, valueBuffer, valueAlignX, textY, "right-center", textColor);
}

void Inventory::RenderText(SDL_Renderer* renderer, const char* text, float x, float y, const std::string& alignment, SDL_Color color) {
    if (!text || !textRenderer || !textRenderer->IsFontLoaded()) {
        return;
    }
    
    textRenderer->RenderText(text, x, y, alignment, color);
}


bool Inventory::CheckAndFocusInputBox(float mouseX, float mouseY, int resourceId, float inputBoxX, float inputBoxY, float inputBoxWidth, float inputBoxHeight) {
    // Check if mouse is within input box bounds
    bool inBounds = (mouseX >= inputBoxX && mouseX <= inputBoxX + inputBoxWidth &&
                     mouseY >= inputBoxY && mouseY <= inputBoxY + inputBoxHeight);
    
    if (inBounds) {
        focusedResourceId = resourceId;
        // Initialize input text if empty
        if (inputTexts.find(resourceId) == inputTexts.end()) {
            inputTexts[resourceId] = "";
        }
        return true;
    }
    return false;
}

void Inventory::HandleTextInput(const char* textInput) {
    if (focusedResourceId < 0) {
        return;
    }
    
    // Only allow numeric characters
    if (textInput && textInput[0] >= '0' && textInput[0] <= '9') {
        // Ensure entry exists in map
        if (inputTexts.find(focusedResourceId) == inputTexts.end()) {
            inputTexts[focusedResourceId] = "";
        }
        std::string& currentText = inputTexts[focusedResourceId];
        // Limit to reasonable length (e.g., 6 digits max)
        if (currentText.length() < 6) {
            currentText += textInput[0];
        }
    }
}

void Inventory::HandleBackspace() {
    if (focusedResourceId < 0) {
        return;
    }
    
    // Ensure entry exists in map
    if (inputTexts.find(focusedResourceId) == inputTexts.end()) {
        inputTexts[focusedResourceId] = "";
    }
    std::string& currentText = inputTexts[focusedResourceId];
    if (!currentText.empty()) {
        currentText.pop_back();
    }
}

int Inventory::GetInputValue(int resourceId) const {
    auto it = inputTexts.find(resourceId);
    if (it == inputTexts.end() || it->second.empty()) {
        return 0;
    }
    
    const std::string& text = it->second;
    if (text.empty()) {
        return 0;
    }
    
    int value = std::atoi(text.c_str());
    return value;
}

std::unordered_map<int, int> Inventory::GetInputValues() const {
    std::unordered_map<int, int> values;
    for (const auto& entry : inputTexts) {
        int resourceId = entry.first;
        int inputValue = GetInputValue(resourceId);
        if (inputValue > 0) {
            values[resourceId] = inputValue;
        }
    }
    return values;
}

void Inventory::ClearFocus() {
    focusedResourceId = -1;
}

bool Inventory::IsMouseInInventory(float mouseX, float mouseY) const {
    return mouseX > 48 && mouseX < 272 && mouseY > 27 && mouseY < 153;
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

