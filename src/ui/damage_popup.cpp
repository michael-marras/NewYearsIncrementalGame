#include "ui/damage_popup.h"
#include "core/camera.h"
#include "ui/text_renderer.h"
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

void DamagePopupManager::Render(TextRenderer* textRenderer, Camera* camera, int virtualWidth, int virtualHeight) {
    if (!textRenderer || !camera) {
        return;
    }
    for (auto& popup : popups) {
        float progress = popup.age / popup.lifetime;
        float offsetY = -30.0f * progress;
        float currentY = popup.worldY + offsetY;
        
        float renderX, renderY;
        camera->WorldToRender(popup.worldX, currentY, renderX, renderY, virtualWidth, virtualHeight);

        textRenderer->RenderText(popup.text.c_str(), renderX, renderY, 0.35, "center", "center");
    }
    
    (void)textRenderer; (void)camera; (void)virtualWidth; (void)virtualHeight;
}

void DamagePopupManager::Clear() {
    popups.clear();
}

