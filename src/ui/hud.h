#ifndef HUD_H
#define HUD_H

#include <SDL3/SDL.h>
#include <vector>
#include <string>

class Player;
class ResourceManager;
class TextureManager;
class TextRenderer;

// Resource pickup event tracking
struct ResourcePickupEvent {
    int resourceId;
    int totalQuantity; 
    float timer;
    float lifetime; 
    
    ResourcePickupEvent(int id, int qty) 
        : resourceId(id), totalQuantity(qty), timer(6.0f), lifetime(6.0f) {}
};

class HUD {
public:
    HUD(SDL_Renderer* renderer);
    ~HUD();
    
    void Render(Player* player, ResourceManager* resourceManager, TextureManager* textureManager);
    
    // Update pickup events (call each frame with deltaTime in seconds)
    void Update(float deltaTime);
    
    // Notify HUD that a resource was picked up
    void OnResourcePickedUp(int resourceId, int quantity);
    
    // Get text renderer for use by other systems
    TextRenderer* GetTextRenderer() const { return textRenderer; }
    
private:
    SDL_Renderer* renderer;
    TextRenderer* textRenderer;
    std::vector<ResourcePickupEvent> pickupEvents;
    
    void RenderText(const char* text, float x, float y, SDL_Color color);
    
    void RenderResourceCounter(Player* player, ResourceManager* resourceManager, TextureManager* textureManager, float startX, float startY);
    
    void RenderPickupEvents(ResourceManager* resourceManager, TextureManager* textureManager);
};

#endif // HUD_H

