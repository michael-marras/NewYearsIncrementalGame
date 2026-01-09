#ifndef INVENTORY_H
#define INVENTORY_H

#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>
#include <functional>
#include "entities/player.h"
#include "items/resources.h"

class TextureManager;
class TextRenderer;
class InputManager;

enum class InventoryMode {
    NoValues,
    WithValues
};

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
     * Set the inventory mode (NoValues or WithValues)
     */
    void SetMode(InventoryMode mode);
    
    /**
     * Get the current inventory mode
     */
    InventoryMode GetMode() const { return mode; }
    
    /**
     * Get the current total value (for WithValues mode)
     * @return Total value calculated from input values, or 0.0f if not in WithValues mode
     */
    float GetCurrentTotalValue() const { return currentTotalValue; }
    
    /**
     * Set callback function to be called when submit button is clicked
     * @param callback Function that takes planet pointer, total value (float) and resources to consume (resourceId -> quantity)
     */
    void SetSubmitCallback(std::function<void(Planet*, float, const std::unordered_map<int, int>&)> callback) { submitCallback = callback; }
    
    /**
     * Update inventory state and handle mouse input
     * @param renderer SDL renderer for coordinate conversion
     * @param inputManager Input manager for mouse interactions
     * @param player Player reference for inventory data
     * @param resourceManager Resource manager to count valid resources for max scroll calculation
     */
    void Update(SDL_Renderer* renderer, InputManager* inputManager, Player* player, ResourceManager* resourceManager);
    
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
    InventoryMode mode = InventoryMode::NoValues;
    
    // Input box state
    int focusedResourceId = -1;
    std::unordered_map<int, std::string> inputTexts;
    
    // Track total value for WithValues mode (updated during render)
    float currentTotalValue = 0.0f;
    
    // Callback function for submit button (called with planet, total value and resources to consume)
    std::function<void(Planet*, float, const std::unordered_map<int, int>&)> submitCallback = nullptr;
    
    /**
     * Render text at specified position
     * @param renderer SDL renderer for rendering operations
     * @param text Text to render
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Text color
     */
    void RenderText(SDL_Renderer* renderer, const char* text, float x, float y, const std::string& alignment, SDL_Color color);
    
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
    
    /**
     * Render inventory item without value (for items with value = 0)
     * @param renderer SDL renderer for rendering operations
     * @param resourceInfo Resource information
     * @param quantity Quantity of the resource
     * @param textX X coordinate for text rendering
     * @param textY Y coordinate for text rendering
     */
    void RenderNoValue(SDL_Renderer* renderer, ResourceInfo* resourceInfo, int quantity, float textX, float textY);
    
    /**
     * Render inventory item with value
     * @param renderer SDL renderer for rendering operations
     * @param textureManager Texture manager for rendering input box
     * @param resourceInfo Resource information
     * @param resourceId Resource ID for input box tracking
     * @param quantity Quantity of the resource
     * @param textX X coordinate for text rendering
     * @param textY Y coordinate for text rendering
     */
    void RenderWithValue(SDL_Renderer* renderer, TextureManager* textureManager, ResourceInfo* resourceInfo, int resourceId, int quantity, float textX, float textY);
    
    /**
     * Handle text input for focused input box
     * @param textInput The text input character
     */
    void HandleTextInput(const char* textInput);
    
    /**
     * Handle backspace for focused input box
     */
    void HandleBackspace();
    
    /**
     * Check if mouse is over an input box and focus it
     * @param mouseX Mouse X coordinate
     * @param mouseY Mouse Y coordinate
     * @param resourceId Resource ID to check
     * @param inputBoxX Input box X coordinate
     * @param inputBoxY Input box Y coordinate
     * @param inputBoxWidth Input box width
     * @param inputBoxHeight Input box height
     * @return true if input box was focused
     */
    bool CheckAndFocusInputBox(float mouseX, float mouseY, int resourceId, float inputBoxX, float inputBoxY, float inputBoxWidth, float inputBoxHeight);
    
    /**
     * Get input value for a resource
     * @param resourceId Resource ID
     * @return Input value as integer (0 if invalid or empty)
     */
    int GetInputValue(int resourceId) const;
    
    /**
     * Get all input values (resourceId -> quantity to feed)
     * Only includes resources with value > 0 and input value > 0
     * @return Map of resource IDs to input quantities
     */
    std::unordered_map<int, int> GetInputValues() const;
    
    /**
     * Clear focus from input box
     */
    void ClearFocus();
};

#endif // INVENTORY_H

