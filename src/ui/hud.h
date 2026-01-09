#ifndef HUD_H
#define HUD_H

#include <SDL3/SDL.h>
#include "entities/player.h"
#include "items/resources.h"

class TextureManager;
class TextRenderer;

class HUD {
public:
    HUD(SDL_Renderer* renderer);
    ~HUD();
    
    void Render(Player* player, ResourceManager* resourceManager, TextureManager* textureManager);
    
private:
    SDL_Renderer* renderer;
    TextRenderer* textRenderer;
    
    void RenderText(const char* text, float x, float y, SDL_Color color);
    
    void RenderResourceCounter(Player* player, ResourceManager* resourceManager, TextureManager* textureManager, float startX, float startY);
};

#endif // HUD_H

