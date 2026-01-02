#include "textures.h"
#include "tiles.h"
#include "player.h"
#include "objects.h"
#include <SDL3/SDL.h>

TextureManager::TextureManager(SDL_Renderer* renderer) 
    : renderer(renderer) {
}

TextureManager::~TextureManager() {
    Cleanup();
}

void TextureManager::Cleanup() {
    // Clean up all textures
    for (auto& pair : textures) {
        if (pair.second.texture) {
            SDL_DestroyTexture(pair.second.texture);
        }
    }
    textures.clear();
}

char* TextureManager::GetResourcePath(const char* filename) {
    const char* basePath = SDL_GetBasePath();
    if (basePath) {
        char* path;
        SDL_asprintf(&path, "%sres/%s", basePath, filename);
        return path;
    }
    // Fallback to relative path if base path not available
    char* path;
    SDL_asprintf(&path, "res/%s", filename);
    return path;
}

bool TextureManager::LoadImage(const char* name, const char* imagePath) {
    // Check if texture with this name already exists
    if (textures.find(name) != textures.end()) {
        SDL_Log("Texture '%s' already loaded!", name);
        return false;
    }
    
    SDL_Surface* surface = SDL_LoadSurface(imagePath);
    if (!surface) {
        SDL_Log("Couldn't load image %s: %s", imagePath, SDL_GetError());
        return false;
    }
    
    // Save dimensions before destroying surface
    int width = surface->w;
    int height = surface->h;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);  // Surface no longer needed after creating texture
    
    if (!texture) {
        SDL_Log("Couldn't create texture: %s", SDL_GetError());
        return false;
    }
    
    // Set texture to nearest neighbor filtering for pixel-perfect rendering (no gaps)
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    
    // Store texture with its dimensions
    TextureInfo info;
    info.texture = texture;
    info.width = width;
    info.height = height;
    textures[name] = info;
    
    SDL_Log("Loaded texture '%s' (%dx%d)", name, info.width, info.height);
    return true;
}

bool TextureManager::LoadImageFromRes(const char* name, const char* filename) {
    char* path = GetResourcePath(filename);
    bool result = LoadImage(name, path);
    SDL_free(path);
    return result;
}

TextureInfo* TextureManager::GetTexture(const char* name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return &it->second;
    }
    return nullptr;
}

bool TextureManager::HasTexture(const char* name) {
    return textures.find(name) != textures.end();
}

bool TextureManager::RenderSprite(const char* textureName, int srcX, int srcY, int srcW, int srcH, float dstX, float dstY, float scale) {
    TextureInfo* tex = GetTexture(textureName);
    if (!tex) {
        return false;
    }
    
    // Source rectangle (which part of the sprite sheet to use)
    SDL_FRect srcRect;
    srcRect.x = (float)srcX;
    srcRect.y = (float)srcY;
    srcRect.w = (float)srcW;
    srcRect.h = (float)srcH;
    
    // Destination rectangle (where and how big to draw on screen)
    SDL_FRect dstRect;
    dstRect.x = dstX;
    dstRect.y = dstY;
    dstRect.w = (float)srcW * scale;
    dstRect.h = (float)srcH * scale;
    
    // Render the sprite
    SDL_RenderTexture(renderer, tex->texture, &srcRect, &dstRect);
    return true;
}

bool TextureManager::RenderTile(TileManager* tileManager, int tileId, float dstX, float dstY, float scale) {
    if (!tileManager) return false;
    
    TileInfo* tile = tileManager->GetTile(tileId);
    if (!tile) return false;
    
    // Use RenderSprite with the tile's information
    return RenderSprite(tile->sheetName.c_str(), 
                       tile->sheetX, tile->sheetY, 
                       tile->width, tile->height, 
                       dstX, dstY, scale);
}

bool TextureManager::RenderObject(ObjectManager* objectManager, int objectId, float dstX, float dstY, float scale) {
    if (!objectManager) return false;
    
    ObjectInfo* obj = objectManager->GetObject(objectId);
    if (!obj) return false;
    
    // Convert center coordinates to top-left for SDL rendering
    float topLeftX = dstX - (obj->width * scale / 2.0f);
    float topLeftY = dstY - (obj->height * scale / 2.0f);
    
    // Use RenderSprite with the object's information
    return RenderSprite(obj->sheetName.c_str(), 
                       obj->sheetX, obj->sheetY, 
                       obj->width, obj->height, 
                       topLeftX, topLeftY, scale);
}

bool TextureManager::RenderPlayer(Player* player, float dstX, float dstY, PlayerAnimations frame) {
    if (!player) return false;

    frameInfo info = player->getFrame(frame);
    float scale = 1.0f;
    
    // Convert center coordinates to top-left for SDL rendering
    float topLeftX = dstX - (info.width * scale / 2.0f);
    float topLeftY = dstY - (info.height * scale / 2.0f);

    return RenderSprite(info.sheetName.c_str(), info.sheetX, info.sheetY, info.width, info.height, topLeftX, topLeftY, scale);
}
