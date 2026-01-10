#include "world/planet.h"
#include "world/object_node.h"
#include "world/BigBangEngine.h"
#include <cmath>
#include <cstdlib>
#include <unordered_map>
#include "utils/constants.h"
#include "world/tiles.h"
#include "core/textures.h"
#include "world/objects.h"
#include "items/resources.h"
#include "entities/player.h"
#include <SDL3/SDL.h>

int GetPlanetRadius(PlanetSize size) {
    switch (size) {
        case PlanetSize::TINY:
            return 12;  // Radius: 12 tiles -> 24x24 tiles total
        case PlanetSize::SMALL:
            return 25;  // Radius: 25 tiles -> 50x50 tiles total
        case PlanetSize::MEDIUM:
            return 50;  // Radius: 50 tiles -> 100x100 tiles total
        case PlanetSize::LARGE:
            return 100;  // Radius: 100 tiles -> 200x200 tiles total
        case PlanetSize::EXTRA_LARGE:
            return 150;  // Radius: 150 tiles -> 300x300 tiles total
        case PlanetSize::GIANT:
            return 250;  // Radius: 250 tiles -> 500x500 tiles total
        default:
            return 50;
    }
}

Planet::Planet() {
}

Planet::~Planet() {
    // Clean up cached texture
    if (cachedTexture) {
        SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
    }

    // CLean up NPCs TODO
    for (Player* NPC : this->NPCs) {
        delete NPC;
    }
    NPCs.clear();
    
    // Clean up object node managers
    objectNodeManagers.clear();

    // // Clean up NPCs TODO
    // unsigned int NPCsSize = this->NPCs.size();
    // if (NPCsSize > 0) {
    //     for (unsigned int i = 0; i < NPCsSize; i++) {
    //         Player* NPC = this->NPCs[i];
    //         if (NPC) {
    //             delete NPC;
    //             NPC = nullptr;
    //         }
    //     }
    // }
}

PlanetFaceData* Planet::GetFaceData(PlanetFace face) {
    auto it = faces.find(face);
    if (it != faces.end()) {
        return &it->second;
    }
    return nullptr;
}

const PlanetFaceData* Planet::GetFaceData(PlanetFace face) const {
    auto it = faces.find(face);
    if (it != faces.end()) {
        return &it->second;
    }
    return nullptr;
}

void Planet::SetFaceData(PlanetFace face, int tileGridId, int objectGridId, int resourceArrayId) {
    faces[face] = PlanetFaceData(tileGridId, objectGridId, resourceArrayId);
}

void Planet::SetFaceData(PlanetFace face, const PlanetFaceData& data) {
    faces[face] = data;
}

bool Planet::HasFace(PlanetFace face) const {
    return faces.find(face) != faces.end();
}

int Planet::GetTileGridId(PlanetFace face) const {
    const PlanetFaceData* data = GetFaceData(face);
    return data ? data->tileGridId : -1;
}

int Planet::GetObjectGridId(PlanetFace face) const {
    const PlanetFaceData* data = GetFaceData(face);
    return data ? data->objectGridId : -1;
}

int Planet::GetResourceArrayId(PlanetFace face) const {
    const PlanetFaceData* data = GetFaceData(face);
    return data ? data->resourceArrayId : -1;
}

float Planet::GetCurrentEnergy() const {
    return currentEnergy;
}

float Planet::GetEnergyCost() const {
    return energyCost;
}

void Planet::SetEnergyCost() {
    energyCost = 800.0f * powf(1.55f, static_cast<float>(tier)) * powf(static_cast<float>(tier + 1), 1.3f) + 400.0f;
}

void Planet::AddEnergy(float amount) {
    currentEnergy += amount;
}

bool Planet::CanGenerateChild() const {
    return childrenGenerated < 2 && currentEnergy >= energyCost;
}

bool Planet::HasChildCapacity() const {
    return childrenGenerated < 2;
}

void Planet::ConsumeEnergyForChild() {
    if (currentEnergy >= energyCost) {
        currentEnergy -= energyCost;
        childrenGenerated++;
    }
}

bool Planet::GenerateChild() {
    if (!CanGenerateChild()) {
        return false;
    }
    
    ConsumeEnergyForChild();
    
    return true;
}

int Planet::GetTier() const {
    return tier;
}

void Planet::SetTier(int tierValue) {
    tier = tierValue;
}

PlanetBiome Planet::GetPlanetBiome() const {
    return planetBiome;
}

void Planet::SetPlanetBiome(PlanetBiome biome) {
    planetBiome = biome;
}

PlanetType Planet::GetPlanetType() const {
    return planetType;
}

void Planet::SetPlanetType(PlanetType type) {
    planetType = type;
}

int Planet::getRadius() const {
    return radius;
}

void Planet::setRadius(int rad) {
    radius = rad;
}

float Planet::GetUniverseX() const {
    return universeX;
}

float Planet::GetUniverseY() const {
    return universeY;
}

void Planet::SetUniversePosition(float x, float y) {
    universeX = x;
    universeY = y;
}

void Planet::LocalToUniverse(float localX, float localY, float& outUniverseX, float& outUniverseY) const {
    outUniverseX = universeX + localX;
    outUniverseY = universeY + localY;
}

void Planet::UniverseToLocal(float inUniverseX, float inUniverseY, float& outLocalX, float& outLocalY) const {
    outLocalX = inUniverseX - universeX;
    outLocalY = inUniverseY - universeY;
}

ObjectNodeManager* Planet::GetOrCreateObjectNodeManager(PlanetFace face) {
    auto it = objectNodeManagers.find(face);
    if (it != objectNodeManagers.end()) {
        return it->second;
    }
    // Create new node manager for this face
    ObjectNodeManager* manager = new ObjectNodeManager();
    objectNodeManagers[face] = manager;
    return manager;
}

ObjectNodeManager* Planet::GetObjectNodeManager(PlanetFace face) {
    auto it = objectNodeManagers.find(face);
    if (it != objectNodeManagers.end()) {
        return it->second;
    }
    return nullptr;
}

const ObjectNodeManager* Planet::GetObjectNodeManager(PlanetFace face) const {
    auto it = objectNodeManagers.find(face);
    if (it != objectNodeManagers.end()) {
        return it->second;
    }
    return nullptr;
}

void Planet::UpdateObjectNodes(PlanetFace face, float deltaTime, ObjectManager* objectManager) {
    ObjectNodeManager* nodeManager = GetObjectNodeManager(face);
    if (!nodeManager || !objectManager) {
        return;
    }
    
    PlanetFaceData* faceData = GetFaceData(face);
    if (!faceData || faceData->objectGridId < 0) {
        return;
    }
    
    int planetWidth = radius * 2;
    int planetHeight = radius * 2;
    
    nodeManager->UpdateAll(deltaTime, objectManager, faceData->objectGridId, planetWidth, planetHeight, planetBiome);
}

void Planet::UpdateAllObjectNodes(float deltaTime, ObjectManager* objectManager) {
    if (!objectManager) {
        return;
    }
    
    PlanetFace faces[] = {
        PlanetFace::FRONT,
        PlanetFace::BACK,
        PlanetFace::LEFT,
        PlanetFace::RIGHT,
        PlanetFace::TOP,
        PlanetFace::BOTTOM
    };
    
    for (int i = 0; i < 6; i++) {
        UpdateObjectNodes(faces[i], deltaTime, objectManager);
    }
}

void Planet::CalculateChildUniversePosition(int parentDepth, int parentIndex, bool isLeftChild, float ringSpacing, float& outChildX, float& outChildY) {
    int childDepth = parentDepth + 1;
    int childIndex;
    
    if (isLeftChild) {
        childIndex = 2 * parentIndex;
    } else {
        childIndex = 2 * parentIndex + 1;
    }

    int N = (int)(1 << childDepth);
    float step = 360.0f / N;
    
    float childAngleDeg = 45.0f + (childIndex + 0.5f) * step;
    
    float childAngleRad = childAngleDeg * M_PI / 180.0f;
    
    float childRadius = childDepth * ringSpacing;
    
    outChildX = childRadius * cosf(childAngleRad);
    outChildY = childRadius * sinf(childAngleRad);
}

// Render planet to texture (top-down view)
void Planet::RenderToTexture(SDL_Renderer* renderer, 
    TileManager* tileManager,
    TextureManager* textureManager,
    ObjectManager* objectManager,
    ResourceManager* resourceManager,
    Player* player,
    float deltaTime,
    int playerCurrentFace) {
    if (!renderer || !tileManager || !textureManager) return;

    int planetWidth = (radius * 2) * TILE_RENDER_SIZE;
    int planetHeight = planetWidth;

    // Debug: check planet size
    if (planetWidth <= 0 || planetHeight <= 0) {
        SDL_Log("Error: Planet has invalid size: radius=%d, width=%d, height=%d", radius, planetWidth, planetHeight);
        return;
    }

    // Only re-render if texture doesn't exist, size changed, or planet is dirty
    if (!cachedTexture || cachedTextureWidth != planetWidth) {
        if (cachedTexture) {
            SDL_DestroyTexture(cachedTexture);
            cachedTexture = nullptr;
        }
        cachedTexture = SDL_CreateTexture(renderer, 
                        SDL_PIXELFORMAT_RGBA32,
                        SDL_TEXTUREACCESS_TARGET,
                        planetWidth, planetHeight);
        if (!cachedTexture) {
            SDL_Log("Error: Failed to create planet cached texture (size: %dx%d): %s", planetWidth, planetHeight, SDL_GetError());
            SDL_SetRenderTarget(renderer, nullptr);
            return;
        }
        cachedTextureWidth = planetWidth;
        cachedTextureHeight = planetHeight;
        isDirty = true;
        SDL_Log("Created planet cached texture: %dx%d (radius=%d)", planetWidth, planetHeight, radius);
    }

    // Only re-render if planet is dirty
    if (!isDirty && cachedTexture) {
        return;
    }

    SDL_SetRenderTarget(renderer, cachedTexture);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Render tiles from TOP face
    int tileGridId = GetTileGridId(PlanetFace::TOP);
    SDL_Log("Debug: Planet tile grid ID for TOP face: %d (radius=%d, size=%dx%d)", tileGridId, radius, planetWidth, planetHeight);
    if (tileGridId >= 0) {
        TileGrid* grid = tileManager->GetTileGrid(tileGridId);
        if (grid) {
            int tilesRendered = 0;
            int tilesFailed = 0;
            std::unordered_map<int, int> invalidTileIds;
            
            for (int y = 0; y < grid->height; y++) {
                for (int x = 0; x < grid->width; x++) {
                    int tileId = grid->GetTile(x, y);
                    
                    // Check if tile ID is valid before attempting to render
                    if (!tileManager->HasTile(tileId)) {
                        tilesFailed++;
                        invalidTileIds[tileId]++;
                        continue;
                    }
                    
                    float tileX = x * TILE_RENDER_SIZE;
                    float tileY = y * TILE_RENDER_SIZE;
                    bool rendered = textureManager->RenderTile(tileManager, tileId, tileX, tileY, 1.0f);
                    if (rendered) {
                        tilesRendered++;
                    } else {
                        tilesFailed++;
                        invalidTileIds[tileId]++;
                    }
                }
            }
            
            if (tilesFailed > 0) {
                SDL_Log("Warning: Failed to render %d tiles (rendered %d) for planet with grid ID %d", tilesFailed, tilesRendered, tileGridId);
                
                // Log which tile IDs are invalid
                if (!invalidTileIds.empty()) {
                    SDL_Log("Invalid tile IDs found:");
                    for (const auto& pair : invalidTileIds) {
                        SDL_Log("  Tile ID %d: %d occurrences", pair.first, pair.second);
                    }
                }
            }
            if (tilesRendered == 0 && tilesFailed > 0) {
                SDL_Log("Error: No tiles rendered! Grid ID: %d, Size: %dx%d, Total tiles: %d", tileGridId, grid->width, grid->height, grid->width * grid->height);
            }
        } else {
            SDL_Log("Error: Planet tile grid %d not found in TileManager", tileGridId);
        }
    } else {
        SDL_Log("Error: Planet has invalid tile grid ID: %d", tileGridId);
    }

    BigBangEngine* engine = GetPortalEngine();
    if (engine && textureManager) {
        // Ensure the portal texture is loaded
        engine->EnsureTextureLoaded(textureManager);
        
        // Set target scale based on current energy ratio
        float energyRatio = (energyCost > 0.0f) ? (currentEnergy / energyCost) : 0.0f;
        if (energyRatio > 1.0f) energyRatio = 1.0f;
        engine->SetTargetEnergyRatio(energyRatio);
        
        // Update animation and growth interpolation
        if (deltaTime > 0.0f) {
            engine->Update(deltaTime);
        }
        
        // Calculate center of planet (same as player spawn)
        float centerX = radius * TILE_RENDER_SIZE;
        float centerY = radius * TILE_RENDER_SIZE;
        
        // Use smoothly interpolated display scale with fixed scale factor
        float portalScale = engine->GetDisplayScale();
        engine->Render(textureManager, centerX, centerY, 0.5f * portalScale);
    }

    // Render objects from TOP face
    if (objectManager) {
        int objectGridId = GetObjectGridId(PlanetFace::TOP);
        if (objectGridId >= 0) {
            ObjectGrid* objectGrid = objectManager->GetObjectGrid(objectGridId);
            if (objectGrid) {
                for (int y = 0; y < objectGrid->height; y++) {
                    for (int x = 0; x < objectGrid->width; x++) {
                        if (objectGrid->HasObject(x, y)) {
                            int objectId = objectGrid->GetObject(x, y);
                            float objectX = x * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                            float objectY = y * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                            textureManager->RenderObject(objectManager, objectId, objectX, objectY, 1.0f);
                        }
                    }
                }
            }
        }
    }

    // Render resources from TOP face
    if (resourceManager) {
        int resourceArrayId = GetResourceArrayId(PlanetFace::TOP);
        if (resourceArrayId >= 0) {
            // Get all resources in the planet area (0 to planetWidth/Height)
            std::vector<ResourceInstance*> resources = resourceManager->GetResourcesInArea(
                resourceArrayId, 0.0f, 0.0f, (float)planetWidth, (float)planetHeight);
            for (ResourceInstance* resource : resources) {
                if (resource) {
                    textureManager->RenderResource(resourceManager, resource->resourceId, resource->x, resource->y, 0.8f);
                }
            }
        }
    }

    if (player && playerCurrentFace == 4 /* TOP */) {
        float playerX = player->getX();
        float playerY = player->getY();
        // Check if player is within planet bounds (TOP face bounds)
        if (playerX >= 0 && playerX < planetWidth && playerY >= 0 && playerY < planetHeight) {
            textureManager->RenderPlayer(player, playerX, playerY, player->getCurrentPlayerAnimation(), 1.0f);
        }
    }
    SDL_SetRenderTarget(renderer, nullptr);
    
    // Clear dirty flag after rendering
    isDirty = false;
}

void Planet::AddNPCs(int numNPCs, float x, float y) {
    for (int i = 0; i < numNPCs; i++) {
        Player* newNPC = new Player(x, y, false);
        (this->NPCs).push_back(newNPC);
    }
}
    