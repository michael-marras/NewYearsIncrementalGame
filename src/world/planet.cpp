#include "world/planet.h"
#include <cmath>
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
    Player* player) {
    if (!renderer || !tileManager || !textureManager) return;

    int planetWidth = (radius * 2) * TILE_RENDER_SIZE;
    int planetHeight = planetWidth;

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
            SDL_Log("Failed to create planet cached texture: %s", SDL_GetError());
            SDL_SetRenderTarget(renderer, nullptr);  // Reset render target on error
            return;
        }
        cachedTextureWidth = planetWidth;
        cachedTextureHeight = planetHeight;
        isDirty = true;
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
    if (tileGridId >= 0) {
        TileGrid* grid = tileManager->GetTileGrid(tileGridId);
        if (grid) {
            for (int y = 0; y < grid->height; y++) {
                for (int x = 0; x < grid->width; x++) {
                    int tileId = grid->GetTile(x, y);
                    float tileX = x * TILE_RENDER_SIZE;
                    float tileY = y * TILE_RENDER_SIZE;
                    textureManager->RenderTile(tileManager, tileId, tileX, tileY, 1.0f);
                }
            }
        }
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

    // Render player if they're on this planet (optional)
    if (player) {
        float playerX = player->getX();
        float playerY = player->getY();
        // Check if player is within planet bounds
        if (playerX >= 0 && playerX < planetWidth && playerY >= 0 && playerY < planetHeight) {
            textureManager->RenderPlayer(player, playerX, playerY, player->getCurrentPlayerAnimation(), 1.0f);
        }
    }

    SDL_SetRenderTarget(renderer, nullptr);
    
    // Clear dirty flag after rendering
    isDirty = false;
}
    