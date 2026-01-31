#include "ui/text_renderer.h"
#include <cstring>
#include <algorithm>
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>
#include <stdexcept>

TextRenderer::TextRenderer(SDL_Renderer* renderer, TextureManager* textureManager) 
    : renderer(renderer), textureManager(textureManager), fontSize(16), fontLoaded(false) {
    
}

TextRenderer::~TextRenderer() {

}

bool TextRenderer::LoadFont(const std::string& fontPath) {
    const char* basePath = SDL_GetBasePath();
    std::string jsonPath;
    
    if (basePath) {
        jsonPath = std::string(basePath) + "data/fonts/" + fontPath + ".json";
    } else {
        jsonPath = "data/fonts/" + fontPath + ".json";
    }
    
    SDL_Log("Attempting to load font from: %s", jsonPath.c_str());
    
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        SDL_Log("Failed to open JSON file: %s", jsonPath.c_str());
        return false;
    }
    
    json data;
    try {
        data = json::parse(file);
    } catch (json::parse_error& e) {
        SDL_Log("Error: JSON parsing failed: %s", e.what());
        return false;
    }
    
    std::string imageName = data["image"].get<std::string>();
    fontTextureName = fontPath;
    
    if (!textureManager->LoadImageFromRes(fontTextureName.c_str(), ("ui/" + imageName).c_str())) {
        SDL_Log("Failed to load font texture: %s", imageName.c_str());
        return false;
    }

    json chars = data["characters"];

    for (auto& [key, value] : chars.items()) {
        Rect rect;
        rect.x = value["x"].get<int>();
        rect.y = value["y"].get<int>();
        rect.width = value["w"].get<int>();
        rect.height = value["h"].get<int>();

        if (!key.empty()) {
            char_rects[key[0]] = rect;
        }
    }
    
    fontLoaded = true;
    SDL_Log("Font loaded successfully: %d characters", (int)char_rects.size());
    return true;
}

void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, const std::string& alignX, const std::string& alignY) {
    if (!fontLoaded || text.empty()) {
        return;
    }
    
    int totalWidth = 0;
    int maxHeight = 0;
    const int charSpacing = 1;
    
    for (char c : text) {
        c = std::tolower(c);
        if (c == ' ') {
            totalWidth += 9 * scale;
            continue;
        }
        auto it = char_rects.find(c);
        if (it != char_rects.end()) {
            totalWidth += (it->second.width + charSpacing) * scale;
            if (maxHeight < it->second.height * scale) {
                maxHeight = it->second.height * scale;
            }
        }
    }
    
    float startX = x;
    float startY = y;
    
    if (alignX == "center") {
        startX = x - float(totalWidth) / 2.0f;
    } else if (alignX == "right") {
        startX = x - totalWidth;
    }
    
    if (alignY == "center") {
        startY = y - float(maxHeight) / 2.0f;
    } else if (alignY == "bottom") {
        startY = y - maxHeight;
    }
    
    float currentX = startX;
    for (char c : text) {
        c = std::tolower(c);
        if (c == ' ') {
            currentX += 9 * scale;
            continue;
        }
        auto it = char_rects.find(c);
        if (it != char_rects.end()) {
            const Rect& charRect = it->second;
            textureManager->RenderSprite(
                fontTextureName.c_str(),
                charRect.x, charRect.y,
                charRect.width, charRect.height,
                currentX, startY,
                scale
            );
            
            currentX += (charRect.width + charSpacing) * scale;
        }
    }
}

