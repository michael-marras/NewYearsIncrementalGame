#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL3/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <unordered_map>

// Structure to hold a cached glyph texture
struct CachedGlyph {
    SDL_Texture* texture;
    int width;
    int height;
    int advanceX;
    int bearingX;
    int bearingY;
};

// Text renderer using FreeType for TTF/OTF font rendering
class TextRenderer {
public:
    TextRenderer(SDL_Renderer* renderer);
    ~TextRenderer();
    
    // Load a font from file (TTF or OTF)
    bool LoadFont(const char* fontPath, int fontSize);
    
    // Render text at position (x, y) with color and alignment
    // alignment format: "horizontal-vertical" (e.g., "center-center", "top-right", "bottom-left")
    // horizontal: "left", "center", "right"
    // vertical: "top", "center", "bottom"
    void RenderText(const char* text, float x, float y, const std::string& alignment, SDL_Color color);
    
    // Get the width in pixels of a text string
    int GetTextWidth(const char* text);
    
    // Get the height in pixels (based on font size)
    int GetTextHeight() const { return fontSize; }
    
    // Check if font is loaded
    bool IsFontLoaded() const { return fontLoaded; }
    
private:
    SDL_Renderer* renderer;
    FT_Library library;
    FT_Face face;
    int fontSize;
    bool fontLoaded;
    
    // Glyph cache: maps character code to cached glyph texture
    std::unordered_map<unsigned long, CachedGlyph> glyphCache;
    
    // Get or create a glyph texture for a character
    CachedGlyph* GetGlyph(unsigned long charCode);
    
    // Create a texture from FreeType glyph bitmap
    SDL_Texture* CreateGlyphTexture(const FT_Bitmap& bitmap);
    
    // Clean up glyph cache
    void ClearGlyphCache();
};

#endif // TEXT_RENDERER_H
