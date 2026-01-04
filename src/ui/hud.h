#ifndef HUD_H
#define HUD_H

#include <SDL3/SDL.h>
#include "entities/player.h"
#include "items/resources.h"

// Forward declarations
class TextureManager;

// HUD (Heads-Up Display) for gameplay overlay
class HUD {
public:
    HUD(SDL_Renderer* renderer);
    ~HUD();
    
    // Render the HUD overlay (resource counters, etc.)
    void Render(Player* player, ResourceManager* resourceManager, TextureManager* textureManager);
    
private:
    SDL_Renderer* renderer;
    
    // Helper function to render text (simple numeric font)
    void RenderText(const char* text, float x, float y, SDL_Color color);
    
    // Render a single digit (0-9) using simple geometric shapes
    void RenderDigit(char digit, float x, float y, SDL_Color color);
    
    // Render resource counter
    void RenderResourceCounter(Player* player, ResourceManager* resourceManager, TextureManager* textureManager, float startX, float startY);
};

#endif // HUD_H

