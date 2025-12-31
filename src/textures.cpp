#include "textures.h"
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
