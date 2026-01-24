#include "ui/damage_popup.h"
// #include "ui/text_renderer.h" // DISABLED: FreeType dependency removed
#include "core/camera.h"
#include <algorithm>
#include <cstdio>

DamagePopupManager::DamagePopupManager() {
}

DamagePopupManager::~DamagePopupManager() {
    Clear();
}

void DamagePopupManager::AddPopup(float worldX, float worldY, int damage, SDL_Color color) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", damage);
    popups.emplace_back(worldX, worldY, std::string(buffer), color);
}

void DamagePopupManager::Update(float deltaTime) {
    // Update all popups
    for (auto& popup : popups) {
        popup.age += deltaTime;
    }
    
    // Remove expired popups
    popups.erase(
        std::remove_if(popups.begin(), popups.end(),
            [](const DamagePopup& p) { return p.age >= p.lifetime; }),
        popups.end()
    );
}

void DamagePopupManager::Render(void* textRenderer, Camera* camera, int virtualWidth, int virtualHeight) {
    // DISABLED: FreeType dependency removed
    // if (!textRenderer || !camera) {
    //     return;
    // }
    // 
    // for (auto& popup : popups) {
    //     // Calculate current position (move upward over time)
    //     float progress = popup.age / popup.lifetime;
    //     float offsetY = -30.0f * progress; // Move up 30 pixels over lifetime
    //     float currentY = popup.worldY + offsetY;
    //     
    //     // Calculate alpha (fade out over time)
    //     float alpha = 1.0f - progress;
    //     if (alpha < 0.0f) alpha = 0.0f;
    //     if (alpha > 1.0f) alpha = 1.0f;
    //     
    //     // Convert world coordinates to render coordinates
    //     float renderX, renderY;
    //     camera->WorldToRender(popup.worldX, currentY, renderX, renderY, virtualWidth, virtualHeight);
    //     
    //     // Update color with alpha
    //     SDL_Color renderColor = popup.color;
    //     renderColor.a = (Uint8)(alpha * 255.0f);
    //     
    //     // Render the popup text centered
    //     textRenderer->RenderText(popup.text.c_str(), renderX, renderY, "center-center", renderColor);
    // }
    
    (void)textRenderer; (void)camera; (void)virtualWidth; (void)virtualHeight; // Suppress warnings
}

void DamagePopupManager::Clear() {
    popups.clear();
}

