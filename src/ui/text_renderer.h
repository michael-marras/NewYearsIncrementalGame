#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "core/textures.h"
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <string>
#include "core/json.hpp"

using json = nlohmann::json;

struct Rect {
    int x;
    int y;
    int width;
    int height;
};

class TextRenderer {
public:
    TextRenderer(SDL_Renderer* renderer, TextureManager* textureManager);
    ~TextRenderer();
    
    bool LoadFont(const std::string& fontPath);

    void RenderText(const std::string& text, float x, float y, float scale = 1.0f, const std::string& alignX = "left", const std::string& alignY = "top");

    // Check if font is loaded
    bool IsFontLoaded() const { return fontLoaded; }
    
private:
    SDL_Renderer* renderer;
    TextureManager* textureManager;
    int fontSize;
    bool fontLoaded;
    std::string fontTextureName;
  
    std::unordered_map<char, Rect> char_rects;
};

#endif // TEXT_RENDERER_H