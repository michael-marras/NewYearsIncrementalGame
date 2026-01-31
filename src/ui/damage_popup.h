#ifndef DAMAGE_POPUP_H
#define DAMAGE_POPUP_H

#include "ui/text_renderer.h"
#include <SDL3/SDL.h>
#include <string>
#include <vector>

struct DamagePopup {
    float worldX;
    float worldY;
    std::string text;
    float lifetime;
    float age;
    SDL_Color color;
    
    DamagePopup(float x, float y, const std::string& txt, SDL_Color col) 
        : worldX(x), worldY(y), text(txt), lifetime(1.0f), age(0.0f), color(col) {}
};

// Manager for damage popups
class DamagePopupManager {
public:
    DamagePopupManager();
    ~DamagePopupManager();
    
    // Add a new damage popup at world position
    void AddPopup(float worldX, float worldY, int damage, SDL_Color color = {255, 255, 255, 255});
    
    // Update all popups (call each frame with deltaTime)
    void Update(float deltaTime);
    
    // Render all popups (requires TextRenderer and Camera)
    void Render(TextRenderer* textRenderer, class Camera* camera, int virtualWidth, int virtualHeight);
    
    // Clear all popups
    void Clear();
    
private:
    std::vector<DamagePopup> popups;
};

#endif // DAMAGE_POPUP_H

