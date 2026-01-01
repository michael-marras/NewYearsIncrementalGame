#include "GameContext.h"
#include "../constants.h"

GameContext:: GameContext() {
    // TODO
}

void GameContext:: ResetGameWorld() {
    // TODO
}

void GameContext:: UpdateMouse() {
    // TODO
}

void GameContext:: ChangeResolution(int newIndex, SDL_Window* window) {
    if (newIndex < 0) newIndex = 0;
    if (newIndex >= RESOLUTION_PRESET_COUNT) newIndex = RESOLUTION_PRESET_COUNT - 1;
    
    if (newIndex == this -> currentResolutionIndex) return;
    
    this -> currentResolutionIndex = newIndex;
    const ResolutionPreset& preset = RESOLUTION_PRESETS[this -> currentResolutionIndex];
    
    // Resize the window
    SDL_SetWindowSize(window, preset.width, preset.height);
    
    // Center the window on the screen
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    
    SDL_Log("Changed resolution to %s (%dx%d)", preset.name, preset.width, preset.height);
}

int GameContext:: getCurrentResolutionIndex() {
    return this -> currentResolutionIndex;
}

void GameContext:: Quit() {
    this -> Running = false;
}

bool GameContext:: isRunning() {
    return this -> Running;
}

