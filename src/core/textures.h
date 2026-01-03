#ifndef TEXTURES_H
#define TEXTURES_H

#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

// Forward Declarations
enum class PlayerAnimations : uint8_t;

// Structure to hold texture and its dimensions
struct TextureInfo {
    SDL_Texture* texture;
    int width;
    int height;
};

// Texture manager class to handle loading and managing multiple textures
class TextureManager {
public:
    TextureManager(SDL_Renderer* renderer);
    ~TextureManager();
    
    // Load an image file and create a texture from it, stored by name
    bool LoadImage(const char* name, const char* imagePath);
    
    // Convenience method to load image from res folder
    bool LoadImageFromRes(const char* name, const char* filename);
    
    // Get a texture by name (returns nullptr if not found)
    TextureInfo* GetTexture(const char* name);
    
    // Check if a texture exists
    bool HasTexture(const char* name);
    
    // Render a portion of a texture (for sprite sheets)
    bool RenderSprite(const char* textureName, int srcX, int srcY, int srcW, int srcH, float dstX, float dstY, float scale = 1.0f);
    
    // Render a tile by ID (requires TileManager - see tiles.h)
    // This is a convenience function that looks up tile info and renders it
    bool RenderTile(class TileManager* tileManager, int tileId, float dstX, float dstY, float scale = 1.0f);
    
    // Render an object by ID (requires ObjectManager - see objects.h)
    // This is a convenience function that looks up object info and renders it
    bool RenderObject(class ObjectManager* objectManager, int objectId, float dstX, float dstY, float scale = 1.0f);

    // Render the player by the frame
    bool RenderPlayer(class Player* player, float dstX, float dstY, PlayerAnimations frame);
    
    // Clean up all textures (called automatically in destructor)
    void Cleanup();

private:
    SDL_Renderer* renderer;
    std::unordered_map<std::string, TextureInfo> textures;
    
    // Helper function to get resource path
    static char* GetResourcePath(const char* filename);
};

#endif // TEXTURES_H

