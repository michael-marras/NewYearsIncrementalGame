#include "GameContext.h"
#include "utils/constants.h"
#include "core/textures.h"
#include "ui/text_renderer.h"
#include "world/tiles.h"
#include "world/objects.h"
#include "core/camera.h"
#include "entities/player.h"
#include "items/resources.h"
#include "items/tools.h"
#include "world/planet.h"
#include "world/planet_tree.h"
#include "world/map_generation.h"
#include <ctime>
#include <cmath>

GameContext:: GameContext() {
    window = nullptr;
    renderer = nullptr;
    textureManager = nullptr;
    textRenderer = nullptr;
    tileManager = nullptr;
    objectManager = nullptr;
    resourceManager = nullptr;
    toolManager = nullptr;
    player = nullptr;
    camera = nullptr;
    map = -1;
    objectMap = -1;
    resourceArray = -1;
    currentPlanet = nullptr;
    currentPlanetFace = 0;
    planetTree = nullptr;
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
    if (textRenderer) {
        delete textRenderer;
        textRenderer = nullptr;
    }
    if (resourceManager) {
        delete resourceManager;
        resourceManager = nullptr;
    }
    if (toolManager) {
        delete toolManager;
        toolManager = nullptr;
    }
    if (planetTree) {
        delete planetTree;
        planetTree = nullptr;
        currentPlanet = nullptr;
    }
    currentPlanet = nullptr;
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
    if (!textRenderer) {
        textRenderer = new TextRenderer(renderer, textureManager);
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
    if (!toolManager) {
        toolManager = new ToolManager();
    }
    if (!player) {
        player = new Player(true);
    }
    if (!camera) {
        camera = new Camera(0.0f, 0.0f);
    }
}

void GameContext::GeneratePlanetTree() {
    if (!tileManager || !objectManager || !resourceManager) {
        SDL_Log("Error: Managers not initialized. Cannot generate planets.");
        return;
    }
    
    // Generate root planet (Planet 0)
    rootPlanetSeed = static_cast<unsigned int>(time(nullptr));
    Planet* planet0 = GeneratePlanetFromSeed(rootPlanetSeed, tileManager, objectManager, resourceManager, 0);
    planet0->SetEnergyCost();  // Calculate energy cost based on tier
    planet0->SetUniversePosition(0.0f, 0.0f);  // Root planet at universe origin
    
    // Create tree with root planet
    planetTree = new PlanetTree(planet0);
    
    // Initialize player position and planet reference
    if (player) {
        int radius = planet0->getRadius();
        float centerX = radius * TILE_RENDER_SIZE;
        float centerY = radius * TILE_RENDER_SIZE;
        player->setX(centerX);
        player->setY(centerY);
        player->SetPlanet(planet0);
    }
    
    // Render planet to texture after player is positioned
    // Player starts on TOP face (face 4) by default, so pass that
    if (renderer && planet0) {
        int playerFace = 4; // TOP face
        planet0->RenderToTexture(renderer, tileManager, textureManager, objectManager, resourceManager, player, 0.0f, playerFace);
    }
    
    // Not being used, can be used to generate extra planets off the start, just increase 0 in i < 0
    for (int i = 1; i < 0; i++) {
        // Find a parent that has an available slot
        int parentId = -1;
        for (int j = 0; j < i; j++) {
            PlanetNode* node = planetTree->FindPlanet(j);
            if (node && (!node->left || !node->right)) {
                parentId = j;
                break;
            }
        }
        
        if (parentId == -1) {
            SDL_Log("Warning: Could not find available parent for planet %d", i);
            continue;
        }
        
        // Generate new planet with unique seed
        unsigned int seed = static_cast<unsigned int>(time(nullptr) + i);
        
        // Calculate tier: parent's tier + 1
        PlanetNode* parentNode = planetTree->FindPlanet(parentId);
        int childTier = 1;
        if (parentNode && parentNode->planet) {
            int parentTier = parentNode->planet->GetTier();
            childTier = parentTier + 1;
        }
        
        Planet* newPlanet = GeneratePlanetFromSeed(seed, tileManager, objectManager, resourceManager, childTier);
        newPlanet->SetEnergyCost();
        
        // Add to tree
        int childId = planetTree->AddChild(parentId, newPlanet);
        if (childId == -1) {
            SDL_Log("Warning: Failed to add planet %d to tree", i);
            delete newPlanet;
        } else {
            // Position child planet to create ring structure using depth/index layout
            int leftChildId = planetTree->GetLeftChildId(parentId);
            bool isLeftChild = (childId == leftChildId);
            
            // Convert parent ID to depth and index
            int parentDepth, parentIndex;
            PlanetTree::PlanetIdToDepthIndex(parentId, parentDepth, parentIndex);
            
            const float ringSpacing = 2000.0f;
            float childUniverseX, childUniverseY;
            Planet::CalculateChildUniversePosition(parentDepth, parentIndex, isLeftChild, ringSpacing, childUniverseX, childUniverseY);
            
            newPlanet->SetUniversePosition(childUniverseX, childUniverseY);
        }
    }
}

void GameContext::GeneratePlanetInTree(int parentId) {
    if (!tileManager || !objectManager || !resourceManager) {
        SDL_Log("Error: Managers not initialized. Cannot generate planet.");
        return;
    }

    if (!planetTree) {
        SDL_Log("Error: Planet tree not initialized.");
        return;
    }

    PlanetNode* parent = planetTree->FindPlanet(parentId);
    if (!parent) {
        SDL_Log("Error: Couldn't find parent planet.");
        return;
    }
    
    if (!parent->planet) {
        SDL_Log("Error: Parent planet is null.");
        return;
    }
    
    if (parent->planet->CanGenerateChild()) {
        unsigned int seed = static_cast<unsigned int>(time(nullptr));
        
        int parentTier = parent->planet->GetTier();
        int childTier = parentTier + 1;
        
        Planet* child = GeneratePlanetFromSeed(seed, tileManager, objectManager, resourceManager, childTier);
        child->SetEnergyCost();
        
        int childId = planetTree->AddChild(parentId, child);
        if (childId == -1) {
            SDL_Log("Warning: Failed to add planet to tree");
            delete child;
        } else {
            // Position child planet to create ring structure using depth/index layout
            int leftChildId = planetTree->GetLeftChildId(parentId);
            bool isLeftChild = (childId == leftChildId);
            
            // Convert parent ID to depth and index
            int parentDepth, parentIndex;
            PlanetTree::PlanetIdToDepthIndex(parentId, parentDepth, parentIndex);
            
            const float ringSpacing = 2000.0f;
            float childUniverseX, childUniverseY;
            Planet::CalculateChildUniversePosition(parentDepth, parentIndex, isLeftChild, ringSpacing, childUniverseX, childUniverseY);
            
            child->SetUniversePosition(childUniverseX, childUniverseY);
            
            parent->planet->ConsumeEnergyForChild();
            SDL_Log("Successfully generated child planet %d (tier %d) from parent %d (tier %d) at universe (%.1f, %.1f)", 
                    childId, child->GetTier(), parentId, parentTier, childUniverseX, childUniverseY);
        }
    } else {
        SDL_Log("Parent planet %d cannot generate child (insufficient energy or max children reached)", parentId);
    }
}

PlanetTree* GameContext::getPlanetTree() const {
    return planetTree;
}

int GameContext::getCurrentPlanetId() const {
    return currentPlanetId;
}

bool GameContext::setCurrentPlanetById(int planetId) {
    if (!planetTree) {
        return false;
    }
    
    PlanetNode* node = planetTree->FindPlanet(planetId);
    if (node && node->planet) {
        currentPlanetId = planetId;
        currentPlanet = node->planet;
        
        // Update player's planet reference
        if (player) {
            player->SetPlanet(node->planet);
        }
        updateMapsFromPlanet();
        return true;
    }
    
    return false;
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
    Uint64 now = SDL_GetTicks();
    if (lastFrameTime == 0) {
        // First frame - initialize
        lastFrameTime = now;
        this->DeltaTime = 16; // Default to ~60 FPS for first frame
    } else {
        // Calculate actual frame-to-frame delta
        this->DeltaTime = now - lastFrameTime;
        lastFrameTime = now;
    }
}

Uint64 GameContext:: getDeltaTime() {
    return this -> DeltaTime;
}

TextureManager* GameContext:: getTextureManager() {
    return this -> textureManager;
}

TextRenderer* GameContext:: getTextRenderer() {
    return this->textRenderer;
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

ToolManager* GameContext:: getToolManager() {
    return this -> toolManager;
}

Camera* GameContext:: getCamera() {
    return this -> camera;
}

void GameContext:: setTextureManager(TextureManager* manager) {
    this -> textureManager = manager;
}

void GameContext:: setTextRenderer(TextRenderer* manager) {
    this -> textRenderer = manager;
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

void GameContext:: setToolManager(ToolManager* manager) {
    this -> toolManager = manager;
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

        faceTransitionCooldown = 500;
        
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

float GameContext::GetPlayerUniverseX() const {
    if (!player || !currentPlanet) {
        return 0.0f;
    }
    float universeX, universeY;
    currentPlanet->LocalToUniverse(player->getX(), player->getY(), universeX, universeY);
    return universeX;
}

float GameContext::GetPlayerUniverseY() const {
    if (!player || !currentPlanet) {
        return 0.0f;
    }
    float universeX, universeY;
    currentPlanet->LocalToUniverse(player->getX(), player->getY(), universeX, universeY);
    return universeY;
}

