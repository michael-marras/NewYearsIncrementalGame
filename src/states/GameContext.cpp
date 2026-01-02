#include "GameContext.h"
#include "../constants.h"
#include "../textures.h"
#include "../tiles.h"
#include "../objects.h"
#include "../camera.h"
#include "../player.h"

GameContext:: GameContext() {
    window = nullptr;
    renderer = nullptr;
    textureManager = nullptr;
    tileManager = nullptr;
    objectManager = nullptr;
    player = nullptr;
    camera = nullptr;
    map = -1;
    objectMap = -1;
}

GameContext:: ~GameContext() {
    if (camera) {
        delete camera;
        camera = nullptr;
    }
    if (objectManager) {
        delete objectManager;
        objectManager = nullptr;
    }
    if (player) {
        delete player;
        player = nullptr;
    }
    if (tileManager) {
        delete tileManager;
        tileManager = nullptr;
    }
    if (textureManager) {
        delete textureManager;
        textureManager = nullptr;
    }
}

void GameContext:: InitializeManagers(SDL_Window* window, SDL_Renderer* renderer) {
    if (!window || !renderer) {
        SDL_Log("Warning: InitializeManagers called with nullptr window or renderer");
        return;
    }
    
    this->window = window;
    this->renderer = renderer;
    
    // Only initialize if not already initialized
    if (!textureManager) {
        textureManager = new TextureManager(renderer);
    }
    if (!tileManager) {
        tileManager = new TileManager();
    }
    if (!objectManager) {
        objectManager = new ObjectManager();
    }
    if (!player) {
        player = new Player();
    }
    if (!camera) {
        camera = new Camera(0.0f, 0.0f);
    }
}

void GameContext:: ResetGameWorld() {
    // TODO
}

void GameContext:: UpdateMouse() {
    // TODO
}

void GameContext:: ChangeResolution(int newIndex) {
    if (!window) {
        SDL_Log("Warning: ChangeResolution called but window is not initialized");
        return;
    }
    
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

void GameContext:: setDeltaTime(Uint64* currentTick) {
    this -> DeltaTime = SDL_GetTicks() - *currentTick;
}

Uint64 GameContext:: getDeltaTime() {
    return this -> DeltaTime;
}

TextureManager* GameContext:: getTextureManager() {
    return this -> textureManager;
}

TileManager* GameContext:: getTileManager() {
    return this -> tileManager;
}

ObjectManager* GameContext:: getObjectManager() {
    return this -> objectManager;
}

Camera* GameContext:: getCamera() {
    return this -> camera;
}

void GameContext:: setTextureManager(TextureManager* manager) {
    this -> textureManager = manager;
}

void GameContext:: setTileManager(TileManager* manager) {
    this -> tileManager = manager;
}

void GameContext:: setObjectManager(ObjectManager* manager) {
    this -> objectManager = manager;
}

void GameContext:: setCamera(Camera* camera) {
    this -> camera = camera;
}

int GameContext:: getMap() const {
    return map;
}

void GameContext:: setMap(int mapId) {
    map = mapId;
}

int GameContext:: getObjectMap() const {
    return objectMap;
}

void GameContext:: setObjectMap(int objectMapId) {
    objectMap = objectMapId;
}

Player* GameContext:: getPlayer() {
    return this -> player;
}