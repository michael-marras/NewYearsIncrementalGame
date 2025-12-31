#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <SDL3/SDL.h>

// Game constants
const Uint64 FPS = 100;
const Uint64 TARGETFRAMETIME = 1000 / FPS;

// Window settings
const int VIRTUAL_WIDTH = 320;
const int VIRTUAL_HEIGHT =  180;

// Resolution preset structure
struct ResolutionPreset {
    int width;
    int height;
    const char* name;
};

// Resolution presets (organized by aspect ratio, then by resolution)
const ResolutionPreset RESOLUTION_PRESETS[] = {
    // 16:9 (most common)
    {3840, 2160, "4K"},
    {2560, 1440, "1440p"},
    {1920, 1080, "1080p"},
    {1600, 900,  "900p"},
    {1366, 768,  "768p"},
    {1280, 720,  "720p"},
    
    // 16:10
    {2560, 1600, "1600p"},
    {1920, 1200, "1200p"},
    {1680, 1050, "1050p"},
    
    // 5:4 (close to 4:3)
    {1280, 1024, "1024p"},
    
    // 4:3
    {1600, 1200, "1200p 4:3"},
    {1024, 768,  "768p 4:3"}
};

const int RESOLUTION_PRESET_COUNT = sizeof(RESOLUTION_PRESETS) / sizeof(RESOLUTION_PRESETS[0]);

// Tile settings
const int TILE_SIZE = 16;
const int TILE_RENDER_SIZE = 16;

#endif // CONSTANTS_H

