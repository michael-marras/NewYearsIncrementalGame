#include "GodState.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_keycode.h>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <unordered_map>
#include <vector>
#include <memory>
#include "utils/constants.h"
#include "states/GameContext.h"
#include "entities/player.h"
#include "world/planet.h"
#include "world/planet_tree.h"
#include "core/textures.h"
#include "world/tiles.h"
#include "world/objects.h"
#include "items/resources.h"
#include "core/camera.h"
#include "core/Animations.h"
#include "world/BigBangEngine.h"
#include "core/input_manager.h"

GodState::GodState() {

}

GodState::~GodState() {

}

void GodState::input() {
    SDL_Event event;
}

void GodState::update() {
    TextureManager* textureManager = context->getTextureManager();
    TileManager* tileManager = context->getTileManager();
    ObjectManager* objectManager = context->getObjectManager();
    Camera* camera = context->getCamera();
    std::unique_ptr<Animations> animation = std::make_unique<Animations>();

    const float moveSpeed = 3.0f;

    // Handle mouse wheel zoom (zooms toward screen center)
    float mouseWheelY = inputManager->GetMouseWheelY();
    if (mouseWheelY != 0.0f) {
        float zoomSpeed = 0.1f;
        if (mouseWheelY > 0.0f) {
            camera->ZoomIn(mouseWheelY * zoomSpeed);
        } else {
            camera->ZoomOut(-mouseWheelY * zoomSpeed);
        }
    }

    float mouseScreenX = inputManager->GetMouseX();
    float mouseScreenY = inputManager->GetMouseY();
    float mouseRenderX, mouseRenderY;
    SDL_RenderCoordinatesFromWindow(renderer, mouseScreenX, mouseScreenY, &mouseRenderX, &mouseRenderY);
    
    float worldX, worldY;
    camera->RenderToWorld(mouseRenderX, mouseRenderY, worldX, worldY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    int gridX = (int)(worldX / TILE_RENDER_SIZE);
    int gridY = (int)(worldY / TILE_RENDER_SIZE);

    ObjectInfo* objInfo = objectManager->GetObjectAt(context->getObjectMap(), gridX, gridY);

    float moveX = 0.0f;
    float moveY = 0.0f;

    // Check vertical movement (W/S)
    if (inputManager->IsKeyHeld(SDLK_W)) {
        moveY -= 1.0f;
    }
    if (inputManager->IsKeyHeld(SDLK_S)) {
        moveY += 1.0f;
    }
    if (inputManager->IsKeyHeld(SDLK_A)) {
        moveX -= 1.0f;
    }
    if (inputManager->IsKeyHeld(SDLK_D)) {
        moveX += 1.0f;
    }

    if (moveX != 0.0f || moveY != 0.0f) {
        float length = sqrtf(moveX * moveX + moveY * moveY);
        if (length > 0.0f) {
            moveX /= length;
            moveY /= length;
        }

        moveX *= moveSpeed;
        moveY *= moveSpeed;
        
        camera->Move(moveX, moveY);
    }
        
}

void GodState::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    TextureManager* textureManager = context->getTextureManager();
    TileManager* tileManager = context->getTileManager();
    ObjectManager* objectManager = context->getObjectManager();
    ResourceManager* resourceManager = context->getResourceManager();
    Camera* camera = context->getCamera();

    float cameraX = camera->GetX();
    float cameraY = camera->GetY();
    
    // Cull tiles: calculate which tiles to render
    float zoom = camera->GetZoom();
    float effectiveWidth = VIRTUAL_WIDTH / zoom;
    float effectiveHeight = VIRTUAL_HEIGHT / zoom;
    
    // Calculate camera bounds in universe space
    float minUniverseX = cameraX - effectiveWidth / 2.0f;
    float maxUniverseX = cameraX + effectiveWidth / 2.0f;
    float minUniverseY = cameraY - effectiveHeight / 2.0f;
    float maxUniverseY = cameraY + effectiveHeight / 2.0f;
    
    // Render all planets in universe space
    PlanetTree* planetTree = context->getPlanetTree();
    if (planetTree) {
        std::vector<Planet*> allPlanets;
        planetTree->GetAllPlanets(allPlanets);
        
        // Debug: log how many planets we found
        static bool loggedOnce = false;
        if (!loggedOnce) {
            SDL_Log("GodState: Found %zu planets to render", allPlanets.size());
            loggedOnce = true;
        }
        
        for (Planet* planet : allPlanets) {
            if (!planet) continue;
            
            float planetUniverseX = planet->GetUniverseX();
            float planetUniverseY = planet->GetUniverseY();
            
            int planetRadius = planet->getRadius();
            int planetWidth = planetRadius * 2;
            float planetSize = planetWidth * TILE_RENDER_SIZE;
            
            float planetCenterLocalX = planetRadius * TILE_RENDER_SIZE;
            float planetCenterLocalY = planetRadius * TILE_RENDER_SIZE;
            
            float planetCenterUniverseX, planetCenterUniverseY;
            planet->LocalToUniverse(planetCenterLocalX, planetCenterLocalY, planetCenterUniverseX, planetCenterUniverseY);
            
            if (planetCenterUniverseX + planetSize / 2.0f >= minUniverseX && 
                planetCenterUniverseX - planetSize / 2.0f <= maxUniverseX &&
                planetCenterUniverseY + planetSize / 2.0f >= minUniverseY && 
                planetCenterUniverseY - planetSize / 2.0f <= maxUniverseY) {
                
                float renderX, renderY;
                camera->WorldToRender(planetCenterUniverseX, planetCenterUniverseY, renderX, renderY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                
                float renderSize = planetSize * zoom;
                
                Planet* currentPlanet = context->getCurrentPlanet();
                Player* playerForRendering = nullptr;
                if (currentPlanet == planet && player) {
                    playerForRendering = player;
                }
                
                if (planet->IsDirty() || !planet->GetCachedTexture()) {
                    planet->RenderToTexture(renderer, tileManager, textureManager, 
                                           objectManager, resourceManager, playerForRendering);
                }
                
                // Try to render using cached texture
                SDL_Texture* cachedTex = planet->GetCachedTexture();
                if (cachedTex) {
                    SDL_FRect dstRect;
                    dstRect.x = renderX - renderSize / 2.0f;
                    dstRect.y = renderY - renderSize / 2.0f;
                    dstRect.w = renderSize;
                    dstRect.h = renderSize;
                    SDL_RenderTexture(renderer, cachedTex, nullptr, &dstRect);
                } else {
                    // Fallback to colored rectangle if texture not available
                    SDL_SetRenderDrawColor(renderer, 100, 150, 255, SDL_ALPHA_OPAQUE);
                    SDL_FRect planetRect;
                    planetRect.x = renderX - renderSize / 2.0f;
                    planetRect.y = renderY - renderSize / 2.0f;
                    planetRect.w = renderSize;
                    planetRect.h = renderSize;
                    SDL_RenderFillRect(renderer, &planetRect);
                    
                    // Draw outline
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                    SDL_RenderRect(renderer, &planetRect);
                }
            }
        }
    }
    
    // Render BigBangEngine if viewing TOP face
    if (context->getCurrentPlanet() && context->getCurrentPlanetFace() == 4 /* TOP */) {
        Planet* planet = context->getCurrentPlanet();
        BigBangEngine* engine = planet->GetPortalEngine();
        if (engine) {
            // Ensure the portal texture is loaded
            engine->EnsureTextureLoaded(textureManager);
            
            // Set target scale based on current energy ratio
            float currentEnergy = planet->GetCurrentEnergy();
            float energyCost = planet->GetEnergyCost();
            float energyRatio = (energyCost > 0.0f) ? (currentEnergy / energyCost) : 0.0f;
            if (energyRatio > 1.0f) energyRatio = 1.0f;
            engine->SetTargetEnergyRatio(energyRatio);
            
            // Update animation and growth interpolation
            engine->Update((float)context->getDeltaTime());
            
            // Compute center of current grid (TOP face) - match player spawn calculation
            int mapId = context->getMap();
            TileGrid* topGrid = tileManager->GetTileGrid(mapId);
            if (topGrid) {
                // Use same calculation as player spawn: (width * TILE_RENDER_SIZE) / 2.0f
                float centerX = (topGrid->width * TILE_RENDER_SIZE) / 2.0f;
                float centerY = (topGrid->height * TILE_RENDER_SIZE) / 2.0f;
                
                float portalRenderX, portalRenderY;
                camera->WorldToRender(centerX, centerY, portalRenderX, portalRenderY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                
                // Use smoothly interpolated display scale
                float portalScale = engine->GetDisplayScale();
                engine->Render(textureManager, portalRenderX, portalRenderY, zoom * 0.5f * portalScale);
            }
        }
    }

    // Convert mouse window coordinates to render/logical coordinates
    float mouseScreenX = inputManager->GetMouseX();
    float mouseScreenY = inputManager->GetMouseY();
    float mouseRenderX, mouseRenderY;
    SDL_RenderCoordinatesFromWindow(renderer, mouseScreenX, mouseScreenY, &mouseRenderX, &mouseRenderY);

        float mouseWheelY = inputManager->GetMouseWheelY();
    
    // Convert render coordinates to world coordinates
    float worldX, worldY;
    camera->ScreenToWorld(mouseRenderX, mouseRenderY, worldX, worldY);
    int gridX = (int)(worldX / TILE_RENDER_SIZE);
    int gridY = (int)(worldY / TILE_RENDER_SIZE);

    // Get object at mouse position (if any)
    ObjectInfo* objInfo = objectManager->GetObjectAt(context->getObjectMap(), gridX, gridY);
    
    SDL_RenderPresent(renderer);
}

void GodState::onEnter() {
    if (player) {
        Camera* camera = context->getCamera();
        if (camera) {
            // In GodState, camera works in universe coordinates
            // Position camera at player's universe position (preserve player's world position)
            float playerUniverseX = context->GetPlayerUniverseX();
            float playerUniverseY = context->GetPlayerUniverseY();
            camera->SnapToTarget(playerUniverseX, playerUniverseY);
            camera->SetZoomBounds(0.005f, 0.15f);
            camera->SetZoom(0.15f);
        }
    }
}

void GodState::onExit() {

}