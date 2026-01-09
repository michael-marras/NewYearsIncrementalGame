#ifndef INVENTORY_H
#define INVENTORY_H

#include <SDL3/SDL.h>
#include "entities/player.h"
#include "items/resources.h"

class TextureManager;
class TextRenderer;
class InputManager;

class Inventory {
public:
    Inventory(SDL_Renderer* renderer);
    ~Inventory();
    
    /**
     * Toggle inventory open/closed state
     */
    void Toggle();
    
    /**
     * Check if inventory is currently open
     * @return true if inventory is open, false otherwise
     */
    bool IsOpen() const;
    
    /**
     * Open the inventory
     */
    void Open();
    
    /**
     * Close the inventory
     */
    void Close();
    
    /**
     * Update inventory state and handle mouse input
     * @param inputManager Input manager for mouse interactions
     * @param player Player reference for inventory data
     * @param resourceManager Resource manager to count valid resources for max scroll calculation
     */
    void Update(InputManager* inputManager, Player* player, ResourceManager* resourceManager);
    
    /**
     * Render the inventory UI
     * @param renderer SDL renderer for rendering operations
     * @param player Player reference for inventory data
     * @param resourceManager Resource manager for resource info
     * @param textureManager Texture manager for rendering
     */
    void Render(SDL_Renderer* renderer, Player* player, ResourceManager* resourceManager, TextureManager* textureManager);
    
private:
    TextRenderer* textRenderer;
    bool isOpen;
    float yScroll = 0.0f;
    
    /**
     * Render text at specified position
     * @param renderer SDL renderer for rendering operations
     * @param text Text to render
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Text color
     */
    void RenderText(SDL_Renderer* renderer, const char* text, float x, float y, SDL_Color color);
    
    /**
     * Handle mouse click events within inventory
     * @param mouseX Mouse X coordinate
     * @param mouseY Mouse Y coordinate
     * @param player Player reference for inventory modifications
     */
    void HandleMouseClick(float mouseX, float mouseY, Player* player);
    
    /**
     * Check if mouse position is within inventory bounds
     * @param mouseX Mouse X coordinate
     * @param mouseY Mouse Y coordinate
     * @return true if mouse is within inventory UI bounds
     */
    bool IsMouseInInventory(float mouseX, float mouseY) const;
    
    /**
     * Handle scroll wheel input to change scroll Y position
     * @param scrollDelta
     * @param maxScrollY
     */
    void HandleScroll(float scrollDelta, float maxScrollY);
    
    /**
     * Calculate maximum scroll Y based on number of items
     * @param itemCount Number of items in the inventory
     * @param itemHeight Height of each item slot (itemSize + padding)
     * @param visibleHeight Visible height of the inventory panel
     * @return Maximum scroll Y value (0 if content fits without scrolling)
     */
    float CalculateMaxScrollY(int itemCount, float itemHeight, float visibleHeight) const;
    
    /**
     * Get current scroll Y position
     * @return Current scroll Y offset
     */
    float GetScrollY() const { return yScroll; }
};

#endif // INVENTORY_H

