#include "ui/text_renderer.h"
#include <cstring>
#include <algorithm>
#include <SDL3/SDL.h>

TextRenderer::TextRenderer(SDL_Renderer* renderer) 
    : renderer(renderer), library(nullptr), face(nullptr), fontSize(16), fontLoaded(false) {
    
    // Initialize FreeType library
    FT_Error error = FT_Init_FreeType(&library);
    if (error) {
        SDL_Log("Error initializing FreeType library: %d", error);
    }
}

TextRenderer::~TextRenderer() {
    ClearGlyphCache();
    
    if (face) {
        FT_Done_Face(face);
        face = nullptr;
    }
    
    if (library) {
        FT_Done_FreeType(library);
        library = nullptr;
    }
}

bool TextRenderer::LoadFont(const char* fontPath, int fontSize) {
    if (!library) {
        SDL_Log("FreeType library not initialized");
        return false;
    }
    
    // Clean up previous font if loaded
    ClearGlyphCache();
    if (face) {
        FT_Done_Face(face);
        face = nullptr;
    }
    
    // Load font face
    FT_Error error = FT_New_Face(library, fontPath, 0, &face);
    if (error) {
        SDL_Log("Error loading font '%s': %d", fontPath, error);
        return false;
    }
    
    // Set font size (using pixel size, not point size)
    error = FT_Set_Pixel_Sizes(face, 0, fontSize);
    if (error) {
        SDL_Log("Error setting font size: %d", error);
        FT_Done_Face(face);
        face = nullptr;
        return false;
    }
    
    this->fontSize = fontSize;
    this->fontLoaded = true;
    
    SDL_Log("Loaded font '%s' at size %d", fontPath, fontSize);
    return true;
}

CachedGlyph* TextRenderer::GetGlyph(unsigned long charCode) {
    // Check cache first
    auto it = glyphCache.find(charCode);
    if (it != glyphCache.end()) {
        return &it->second;
    }
    
    if (!face || !fontLoaded) {
        return nullptr;
    }
    
    // Load glyph index
    FT_UInt glyphIndex = FT_Get_Char_Index(face, charCode);
    if (glyphIndex == 0) {
        // Character not found in font, return null
        return nullptr;
    }
    
    // Load glyph into face's glyph slot with MONO target for crisp pixel fonts
    FT_Error error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_TARGET_MONO);
    if (error) {
        SDL_Log("Error loading glyph for character %lu: %d", charCode, error);
        return nullptr;
    }
    
    // Render glyph to bitmap using MONO mode for crisp pixel fonts (no anti-aliasing)
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
    if (error) {
        SDL_Log("Error rendering glyph for character %lu: %d", charCode, error);
        return nullptr;
    }
    
    // Create texture from bitmap
    SDL_Texture* texture = CreateGlyphTexture(face->glyph->bitmap);
    if (!texture) {
        return nullptr;
    }
    
    // Cache the glyph
    CachedGlyph glyph;
    glyph.texture = texture;
    glyph.width = face->glyph->bitmap.width;
    glyph.height = face->glyph->bitmap.rows;
    glyph.advanceX = face->glyph->advance.x >> 6;  // Convert from 26.6 fixed point to pixels
    glyph.bearingX = face->glyph->bitmap_left;
    glyph.bearingY = face->glyph->bitmap_top;
    
    glyphCache[charCode] = glyph;
    return &glyphCache[charCode];
}

SDL_Texture* TextRenderer::CreateGlyphTexture(const FT_Bitmap& bitmap) {
    if (bitmap.width == 0 || bitmap.rows == 0) {
        // Create a 1x1 transparent texture for empty glyphs
        SDL_Surface* surface = SDL_CreateSurface(1, 1, SDL_PIXELFORMAT_RGBA32);
        if (!surface) {
            return nullptr;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        return texture;
    }
    
    // Create surface for the glyph
    // FreeType bitmaps are grayscale, we need to convert to RGBA
    SDL_Surface* surface = SDL_CreateSurface(bitmap.width, bitmap.rows, SDL_PIXELFORMAT_RGBA32);
    if (!surface) {
        SDL_Log("Error creating surface for glyph: %s", SDL_GetError());
        return nullptr;
    }
    
    // Copy bitmap data to surface
    unsigned char* pixels = (unsigned char*)surface->pixels;
    const unsigned char* bitmapBuffer = bitmap.buffer;
    
    int pitch = bitmap.pitch;
    if (pitch < 0) {
        pitch = -pitch;
    }
    
    // Handle mono (1-bit) bitmaps for crisp pixel fonts
    if (bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
        // Mono mode: 1 bit per pixel, packed 8 pixels per byte
        for (unsigned int y = 0; y < bitmap.rows; y++) {
            for (unsigned int x = 0; x < bitmap.width; x++) {
                int byteIndex = (int)(y * pitch + (x / 8));
                int bitIndex = 7 - (x % 8);  // Bits are MSB first
                unsigned char byte = bitmapBuffer[byteIndex];
                unsigned char alpha = (byte & (1 << bitIndex)) ? 255 : 0;
                
                int dstIdx = (int)((y * bitmap.width + x) * 4);
                pixels[dstIdx + 0] = 255;  // R
                pixels[dstIdx + 1] = 255;  // G
                pixels[dstIdx + 2] = 255;  // B
                pixels[dstIdx + 3] = alpha; // A
            }
        }
    } else {
        // Grayscale mode (8 bits per pixel) - fallback
        for (unsigned int y = 0; y < bitmap.rows; y++) {
            for (unsigned int x = 0; x < bitmap.width; x++) {
                int srcIdx = (int)(y * pitch + x);
                int dstIdx = (int)((y * bitmap.width + x) * 4);
                
                unsigned char alpha = bitmapBuffer[srcIdx];
                
                // Set RGBA values (white text with alpha from bitmap)
                pixels[dstIdx + 0] = 255;  // R
                pixels[dstIdx + 1] = 255;  // G
                pixels[dstIdx + 2] = 255;  // B
                pixels[dstIdx + 3] = alpha; // A
            }
        }
    }
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Error creating texture from surface: %s", SDL_GetError());
        SDL_DestroySurface(surface);
        return nullptr;
    }
    
    // Set texture to nearest neighbor filtering for crisp pixel-perfect rendering
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    
    // Set texture blend mode for alpha blending
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    SDL_DestroySurface(surface);
    return texture;
}

void TextRenderer::RenderText(const char* text, float x, float y, const std::string& alignment, SDL_Color color) {
    if (!fontLoaded || !text || !renderer) {
        return;
    }
    
    // Calculate text dimensions for alignment
    int textWidth = GetTextWidth(text);
    
    // Calculate actual text bounds (min/max Y from glyphs)
    int maxTop = 0;
    int maxBottom = 0;
    const char* p = text;
    while (*p) {
        unsigned long charCode = (unsigned char)*p;
        if (charCode >= 32) {
            CachedGlyph* glyph = GetGlyph(charCode);
            if (glyph) {
                int top = glyph->bearingY;
                int bottom = glyph->height - glyph->bearingY;
                maxTop = std::max(maxTop, top);
                maxBottom = std::max(maxBottom, bottom);
            }
        }
        p++;
    }
    int actualHeight = maxTop + maxBottom;
    
    // Parse alignment string (format: "horizontal-vertical" or "horizontal vertical")
    std::string align = alignment;
    std::transform(align.begin(), align.end(), align.begin(), ::tolower);
    
    std::string hAlign = "left";
    std::string vAlign = "top";
    
    // Find separator
    size_t dashPos = align.find('-');
    size_t spacePos = align.find(' ');
    size_t sepPos = (dashPos != std::string::npos) ? dashPos : spacePos;
    
    if (sepPos != std::string::npos) {
        hAlign = align.substr(0, sepPos);
        vAlign = align.substr(sepPos + 1);
    } else {
        // Single word - try to match as vertical or horizontal
        if (align == "top" || align == "center" || align == "bottom") {
            vAlign = align;
        } else if (align == "left" || align == "right") {
            hAlign = align;
        }
    }
    
    // Adjust X based on horizontal alignment
    float adjustedX = x;
    if (hAlign == "center") {
        adjustedX = x - textWidth / 2.0f;
    } else if (hAlign == "right") {
        adjustedX = x - textWidth;
    }
    // "left" uses x as-is
    
    // Adjust Y based on vertical alignment
    // Note: adjustedY is the baseline position for rendering
    // Glyphs are positioned as: glyphY = adjustedY - bearingY
    float adjustedY = y;
    if (vAlign == "center") {
        // Center: text center should be at y
        // Center from baseline = (maxBottom - maxTop) / 2
        // So baseline = y - (maxBottom - maxTop) / 2
        adjustedY = y - (maxBottom - maxTop) / 2.0f;
    } else if (vAlign == "bottom") {
        // Bottom: text bottom should be at y
        // Bottom from baseline = maxBottom
        // So baseline = y - maxBottom
        adjustedY = y - maxBottom;
    } else {
        // "top" - y is the top of the text
        // Top from baseline = maxTop (bearingY points up from baseline)
        // So baseline = y + maxTop (in screen coords, Y increases downward)
        adjustedY = y + maxTop;
    }
    
    float currentX = adjustedX;
    p = text;
    
    // First pass: Render black outline by rendering with offsets
    while (*p) {
        unsigned long charCode = (unsigned char)*p;
        
        if (charCode < 32) {
            p++;
            continue;
        }
        
        CachedGlyph* glyph = GetGlyph(charCode);
        if (glyph && glyph->texture) {
            float glyphX = currentX + glyph->bearingX;
            float glyphY = adjustedY - glyph->bearingY;
            
            SDL_FRect dstRect;
            dstRect.w = (float)glyph->width;
            dstRect.h = (float)glyph->height;
            
            for (int i = 0; i < 8; i++) {
                dstRect.x = glyphX;
                dstRect.y = glyphY;
                SDL_RenderTexture(renderer, glyph->texture, nullptr, &dstRect);
            }
            
            currentX += glyph->advanceX;
        }
        
        p++;
    }
    
    // Second pass: Render the actual colored text on top
    currentX = adjustedX;
    p = text;
    
    while (*p) {
        unsigned long charCode = (unsigned char)*p;
        
        if (charCode < 32) {
            p++;
            continue;
        }
        
        CachedGlyph* glyph = GetGlyph(charCode);
        if (glyph && glyph->texture) {
            float glyphX = currentX + glyph->bearingX;
            float glyphY = adjustedY - glyph->bearingY;
            
            // Set texture color modulation for color tinting
            SDL_SetTextureColorMod(glyph->texture, color.r, color.g, color.b);
            SDL_SetTextureAlphaMod(glyph->texture, color.a);
            
            // Render the glyph
            SDL_FRect dstRect;
            dstRect.x = glyphX;
            dstRect.y = glyphY;
            dstRect.w = (float)glyph->width;
            dstRect.h = (float)glyph->height;
            
            SDL_RenderTexture(renderer, glyph->texture, nullptr, &dstRect);
            
            // Advance to next character position
            currentX += glyph->advanceX;
        }
        
        p++;
    }
}

int TextRenderer::GetTextWidth(const char* text) {
    if (!fontLoaded || !text) {
        return 0;
    }
    
    int width = 0;
    const char* p = text;
    
    while (*p) {
        unsigned long charCode = (unsigned char)*p;
        if (charCode < 32) {
            p++;
            continue;
        }
        
        CachedGlyph* glyph = GetGlyph(charCode);
        if (glyph) {
            width += glyph->advanceX;
        }
        p++;
    }
    
    return width;
}

void TextRenderer::ClearGlyphCache() {
    for (auto& pair : glyphCache) {
        if (pair.second.texture) {
            SDL_DestroyTexture(pair.second.texture);
        }
    }
    glyphCache.clear();
}
