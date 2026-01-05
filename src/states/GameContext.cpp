#include "GameContext.h"
#include "utils/constants.h"
#include "core/textures.h"
#include "world/tiles.h"
#include "world/objects.h"
#include "core/camera.h"
#include "entities/player.h"
#include "items/resources.h"
#include "world/planet.h"

GameContext:: GameContext() {
    window = nullptr;
    renderer = nullptr;
    textureManager = nullptr;
    tileManager = nullptr;
    objectManager = nullptr;
    resourceManager = nullptr;
    player = nullptr;
    camera = nullptr;
    map = -1;
    objectMap = -1;
    resourceArray = -1;
    currentPlanet = nullptr;
    currentPlanetFace = 0;
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
    if (resourceManager) {
        delete resourceManager;
        resourceManager = nullptr;
    }
    if (currentPlanet) {
        delete currentPlanet;
        currentPlanet = nullptr;
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
    if (!resourceManager) {
        resourceManager = new ResourceManager();
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

ResourceManager* GameContext:: getResourceManager() {
    return this -> resourceManager;
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

void GameContext:: setResourceManager(ResourceManager* manager) {
    this -> resourceManager = manager;
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

int GameContext:: getResourceArray() const {
    return resourceArray;
}

void GameContext:: setResourceArray(int arrayMapId) {
    resourceArray = arrayMapId;
}

Player* GameContext:: getPlayer() {
    return this -> player;
}

Planet* GameContext:: getCurrentPlanet() const {
    return currentPlanet;
}

void GameContext:: setCurrentPlanet(Planet* planet) {
    currentPlanet = planet;
}

int GameContext:: getCurrentPlanetFace() const {
    return currentPlanetFace;
}

void GameContext:: setCurrentPlanetFace(int face) {
    if (face >= 0 && face < 6) {
        currentPlanetFace = face;
        updateMapsFromPlanet();
    }
}

void GameContext:: updateMapsFromPlanet() {
    if (!currentPlanet) {
        return;
    }
    
    // Convert face index to PlanetFace enum
    PlanetFace faces[] = {
        PlanetFace::FRONT,
        PlanetFace::BACK,
        PlanetFace::LEFT,
        PlanetFace::RIGHT,
        PlanetFace::TOP,
        PlanetFace::BOTTOM
    };
    
    if (currentPlanetFace >= 0 && currentPlanetFace < 6) {
        PlanetFace face = faces[currentPlanetFace];
        const PlanetFaceData* faceData = currentPlanet->GetFaceData(face);
        if (faceData) {
            map = faceData->tileGridId;
            objectMap = faceData->objectGridId;
            resourceArray = faceData->resourceArrayId;
        }
    }
}

bool GameContext:: checkAndHandleFaceTransition(Player* player) {
    if (!currentPlanet || !player) {
        return false;
    }
    
    TileGrid* grid = tileManager->GetTileGrid(map);
    if (!grid) {
        return false;
    }
    
    int radius = grid->width;
    float maxCoord = radius * TILE_RENDER_SIZE;
    
    float playerX = player->getX();
    float playerY = player->getY();
    
    bool transitioned = false;
    int newFace = currentPlanetFace;
    float newX = playerX;
    float newY = playerY;
    int direction = 0;

    PlanetFace faces[] = {
        PlanetFace::FRONT,
        PlanetFace::BACK,
        PlanetFace::LEFT,
        PlanetFace::RIGHT,
        PlanetFace::TOP,
        PlanetFace::BOTTOM
    };
    
    PlanetFace currentFaceEnum = faces[currentPlanetFace];
    if (playerY < 0) {
        direction = 90;
    } else if (playerY >= maxCoord) {
        direction = 270;
    } else if (playerX < 0) {
        direction = 180;
    } else if (playerX >= maxCoord) {
        direction = 0;
    } else {
        direction = -1;
    }
    
    // Only process transitions if we actually crossed an edge
    if (direction != -1) {
        if (currentFaceEnum == PlanetFace::FRONT) {
            switch (direction) {
                case 0: newFace = 3; newX = 0; newY = playerY; break;
                case 90: newFace = 4; newX = playerX; newY = maxCoord - 1; break;
                case 180: newFace = 2; newX = maxCoord - 1; newY = playerY; break;
                case 270: newFace = 5; newX = playerX; newY = 0; break;
            }
            transitioned = true;
        } else if (currentFaceEnum == PlanetFace::BACK) {
            switch (direction) {
                case 0: newFace = 2; newX = 0; newY = playerY; break;
                case 90: newFace = 4; newX = playerX; newY = 0; break;
                case 180: newFace = 3; newX = maxCoord - 1; newY = playerY; break;
                case 270: newFace = 5; newX = playerX; newY = maxCoord - 1; break;
            }
            transitioned = true;
        } else if (currentFaceEnum == PlanetFace::LEFT) {
            switch (direction) {
                case 0: newFace = 0; newX = 0; newY = playerY; break;
                case 90: newFace = 4; newX = 0; newY = playerX; break;
                case 180: newFace = 1; newX = maxCoord - 1; newY = playerY; break;
                case 270: newFace = 5; newX = 0; newY = maxCoord - playerX; break;
            }
            transitioned = true;
        } else if (currentFaceEnum == PlanetFace::RIGHT) {
            switch (direction) {
                case 0: newFace = 1; newX = 0; newY = playerY; break;
                case 90: newFace = 4; newX = maxCoord - 1; newY = maxCoord - playerX; break;
                case 180: newFace = 0; newX = maxCoord - 1; newY = playerY; break;
                case 270: newFace = 5; newX = maxCoord - 1; newY = playerX; break;
            }
            transitioned = true;
        } else if (currentFaceEnum == PlanetFace::TOP) {
            switch (direction) {
                case 0: newFace = 3; newX = maxCoord - playerY; newY = 0; break;
                case 90: newFace = 1; newX = playerX; newY = 0; break;
                case 180: newFace = 2; newX = playerY; newY = 0; break;
                case 270: newFace = 0; newX = playerX; newY = 0; break;
            }
            transitioned = true;
        } else if (currentFaceEnum == PlanetFace::BOTTOM) {
            switch (direction) {
                case 0: newFace = 3; newX = playerY; newY = maxCoord - 1; break;
                case 90: newFace = 0; newX = playerX; newY = maxCoord - 1; break;
                case 180: newFace = 2; newX = maxCoord - playerY; newY = maxCoord - 1; break;
                case 270: newFace = 1; newX = playerX; newY = maxCoord - 1; break;
            }
            transitioned = true;
        }
    }
    
    if (transitioned) {
        currentPlanetFace = newFace;
        updateMapsFromPlanet();
        player->setX(newX);
        player->setY(newY);
        
        // Set player direction based on which edge they appear on
        const float epsilon = 0.5f;
        if (newY < epsilon) {
            player->setPlayerDirection(Direction::FORWARD);
        } else if (newY > maxCoord - 1 - epsilon) {
            player->setPlayerDirection(Direction::BACK);
        } else if (newX < epsilon) {
            player->setPlayerDirection(Direction::RIGHT);
        } else if (newX > maxCoord - 1 - epsilon) {
            player->setPlayerDirection(Direction::LEFT);
        }
        
        // Set cooldown to 30ms after face transition
        faceTransitionCooldown = 30;
        
        return true;
    }
    
    return false;
}

void GameContext::updateFaceTransitionCooldown() {
    if (faceTransitionCooldown > 0) {
        Uint64 deltaTime = getDeltaTime();
        if (faceTransitionCooldown > deltaTime) {
            faceTransitionCooldown -= deltaTime;
        } else {
            faceTransitionCooldown = 0;
        }
    }
}

bool GameContext::isFaceTransitionCooldownActive() const {
    return faceTransitionCooldown > 0;
}

