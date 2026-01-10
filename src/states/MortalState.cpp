#include "MortalState.h"
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
#include "items/tools.h"
#include "core/camera.h"
#include "core/Animations.h"
#include "world/BigBangEngine.h"
#include "ui/hud.h"
#include "ui/inventory.h"
#include "ui/damage_popup.h"
#include "ui/text_renderer.h"
#include "ui/engine_compass.h"
#include "core/input_manager.h"

MortalState::MortalState() {
    damagePopups = new DamagePopupManager();
    engineCompass = new EngineCompass();
    // Set compass position (top-right corner of screen)
    engineCompass->SetPosition(VIRTUAL_WIDTH - 20.0f, 20.0f);
}

MortalState::~MortalState() {
    if (damagePopups) {
        delete damagePopups;
        damagePopups = nullptr;
    }
    if (engineCompass) {
        delete engineCompass;
        engineCompass = nullptr;
    }
}

void MortalState::input() {
    if (inputManager->IsKeyPressed(SDLK_E)) {
        if (inventory) {
            inventory->SetMode(InventoryMode::NoValues);
            inventory->Toggle();
        }
    }

    if (inputManager->IsKeyPressed(SDLK_UP)) {
        PlanetTree* tree = context->getPlanetTree();
        if (tree) {
            int currentPlanetId = context->getCurrentPlanetId();
            PlanetNode* parent = tree->GetParent(currentPlanetId);
            if (parent && context->setCurrentPlanetById(parent->planetId)) {
                // Set to TOP face so portal is visible
                context->setCurrentPlanetFace(4); // TOP
                // Respawn player at center of new planet
                int radius = GetPlanetRadius(PlanetSize::TINY);
                float centerX = radius * TILE_RENDER_SIZE;
                float centerY = radius * TILE_RENDER_SIZE;
                player->setX(centerX);
                player->setY(centerY);
                Camera* camera = context->getCamera();
                if (camera) camera->SnapToTarget(centerX, centerY);
            }
        }
    }
    
    if (inputManager->IsKeyPressed(SDLK_DOWN)) {
        // Cycle through all resolutions
        int currentIndex = context->getCurrentResolutionIndex();
        int nextIndex = (currentIndex + 1) % RESOLUTION_PRESET_COUNT;
        context->ChangeResolution(nextIndex);
    }
    
    if (inputManager->IsKeyPressed(SDLK_LEFT)) {
        // Navigate to left child planet
        PlanetTree* tree = context->getPlanetTree();
        if (tree) {
            int currentPlanetId = context->getCurrentPlanetId();
            int leftChildId = tree->GetLeftChildId(currentPlanetId);
            if (leftChildId >= 0 && context->setCurrentPlanetById(leftChildId)) {
                // Set to TOP face so portal is visible
                context->setCurrentPlanetFace(4); // TOP
                // Respawn player at center of new planet
                int radius = GetPlanetRadius(PlanetSize::TINY);
                float centerX = radius * TILE_RENDER_SIZE;
                float centerY = radius * TILE_RENDER_SIZE;
                player->setX(centerX);
                player->setY(centerY);
                Camera* camera = context->getCamera();
                if (camera) camera->SnapToTarget(centerX, centerY);
            }
        }
    }
    
    if (inputManager->IsKeyPressed(SDLK_RIGHT)) {
        // Navigate to right child planet
        PlanetTree* tree = context->getPlanetTree();
        if (tree) {
            int currentPlanetId = context->getCurrentPlanetId();
            int rightChildId = tree->GetRightChildId(currentPlanetId);
            if (rightChildId >= 0 && context->setCurrentPlanetById(rightChildId)) {
                // Set to TOP face so portal is visible
                context->setCurrentPlanetFace(4); // TOP
                // Respawn player at center of new planet
                int radius = GetPlanetRadius(PlanetSize::TINY);
                float centerX = radius * TILE_RENDER_SIZE;
                float centerY = radius * TILE_RENDER_SIZE;
                player->setX(centerX);
                player->setY(centerY);
                Camera* camera = context->getCamera();
                if (camera) camera->SnapToTarget(centerX, centerY);
            }
        }
    }
}

void MortalState::update() {
    // Update face transition cooldown
    context->updateFaceTransitionCooldown();
    
    // Update damage popups
    if (damagePopups) {
        float deltaTimeSeconds = (float)context->getDeltaTime() / 1000.0f;
        damagePopups->Update(deltaTimeSeconds);
    }
    
    // Update HUD pickup events
    if (hud) {
        float deltaTimeSeconds = (float)context->getDeltaTime() / 1000.0f;
        hud->Update(deltaTimeSeconds);
    }
    
    // Update engine compass
    if (engineCompass && player) {
        Planet* currentPlanet = context->getCurrentPlanet();
        if (currentPlanet) {
            int currentFace = context->getCurrentPlanetFace();
            engineCompass->Update(player, currentPlanet, currentFace);
        }
    }
        
    TextureManager* textureManager = context->getTextureManager();
    TileManager* tileManager = context->getTileManager();
    ObjectManager* objectManager = context->getObjectManager();
    Camera* camera = context->getCamera();
    std::unique_ptr<Animations> animation = std::make_unique<Animations>();

    const float moveSpeed = 1.5f;
    
    // Handle mouse wheel zoom (zooms toward screen center)
    // Only zoom if inventory is closed, otherwise let inventory handle scrolling
    float mouseWheelY = inputManager->GetMouseWheelY();
    if (inventory && inventory->IsOpen()) {
        // Inventory is open - let it handle scrolling
        ResourceManager* resourceManager = context->getResourceManager();
        inventory->Update(renderer, inputManager, player, resourceManager);
    } else if (mouseWheelY != 0.0f) {
        // Inventory is closed - handle camera zoom
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

    if (inputManager->IsMouseButtonPressed(3) && inventory) {
        Planet* currentPlanet = context->getCurrentPlanet();
        if (currentPlanet && context->getCurrentPlanetFace() == 4 && tileManager && camera && currentPlanet->HasChildCapacity()) {
            BigBangEngine* engine = currentPlanet->GetPortalEngine();
            if (engine) {
                int mapId = context->getMap();
                TileGrid* topGrid = tileManager->GetTileGrid(mapId);
                if (topGrid) {
                    float portalCenterWorldX = (topGrid->width * TILE_RENDER_SIZE) / 2.0f;
                    float portalCenterWorldY = (topGrid->height * TILE_RENDER_SIZE) / 2.0f;
                    
                    // Convert portal center to render coordinates
                    float portalRenderX, portalRenderY;
                    float zoom = camera->GetZoom();
                    camera->WorldToRender(portalCenterWorldX, portalCenterWorldY, portalRenderX, portalRenderY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                    
                    // Calculate portal size (based on scale and frame size)
                    float portalScale = engine->GetDisplayScale();
                    const float kFrameSize = 64.0f;
                    float portalSize = zoom * 0.5f * portalScale * kFrameSize;
                    float portalRadius = portalSize / 2.0f;
                    
                    // Check if mouse is within portal bounds (using already calculated mouseRenderX/Y)
                    float dx = mouseRenderX - portalRenderX;
                    float dy = mouseRenderY - portalRenderY;
                    float distance = sqrtf(dx * dx + dy * dy);
                    
                    if (distance <= portalRadius) {
                        // Right-clicked on BigBangEngine - open inventory in WithValues mode
                        inventory->SetMode(InventoryMode::WithValues);
                        inventory->Open();
                    }
                }
            }
        }
    }

    ObjectInfo* objInfo = objectManager->GetObjectAt(context->getObjectMap(), gridX, gridY);

    if (inputManager->IsMouseButtonHeld(1) && inventory && !inventory->IsOpen()) {
        if (objectManager->PlayerCanInteract(context->getObjectMap(), gridX, gridY, context->getPlayer())) {
            // Get object info to check tool requirement
            ObjectInfo* objectInfo = objectManager->GetObjectAt(context->getObjectMap(), gridX, gridY);
            
            if (objectInfo) {
                SDL_Log("Attempting to mine: %s at (%d, %d), required tool: %s", 
                        objectInfo->name.c_str(), gridX, gridY, 
                        objectInfo->requiredToolType.empty() ? "none" : objectInfo->requiredToolType.c_str());
            } else {
                SDL_Log("No object found at grid position (%d, %d)", gridX, gridY);
            }
            
            // Get tool info if tool is equipped
            ToolManager* toolManager = context->getToolManager();
            Player* player = context->getPlayer();
            int equippedToolId = player ? player->GetEquippedToolId() : -1;
            ToolInfo* toolInfo = (toolManager && equippedToolId >= 0) ? toolManager->GetTool(equippedToolId) : nullptr;
            
            // Check if object requires a specific tool type and auto-swap if needed
            bool toolRequirementMet = true;
            if (objectInfo && !objectInfo->requiredToolType.empty()) {
                // Check if equipped tool matches required type
                if (!toolInfo || toolInfo->type != objectInfo->requiredToolType) {
                    // Try to find and equip a tool of the required type from inventory
                    bool foundTool = false;
                    if (toolManager && player) {
                        std::unordered_map<int, int>* toolInventory = player->getToolInventory();
                        if (toolInventory) {
                            // Search through tool inventory for a tool of the required type
                            for (const auto& toolEntry : *toolInventory) {
                                if (toolEntry.second > 0) {
                                    ToolInfo* candidateTool = toolManager->GetTool(toolEntry.first);
                                    if (candidateTool && candidateTool->type == objectInfo->requiredToolType) {
                                        player->EquipTool(toolEntry.first);
                                        toolInfo = candidateTool;
                                        equippedToolId = toolEntry.first;
                                        foundTool = true;
                                        SDL_Log("Auto-equipped %s for mining", candidateTool->name.c_str());
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    
                    if (!foundTool) {
                        toolRequirementMet = false;
                        SDL_Log("Cannot mine %s - requires %s tool (none found in inventory)", 
                                objectInfo->name.c_str(), objectInfo->requiredToolType.c_str());
                    }
                }
            }
            
            // Only mine if tool requirement is met
            if (toolRequirementMet) {
                
                // Get current time
                Uint64 currentTime = SDL_GetTicks();
                
                // Calculate damage and speed from tool, or use defaults
                int damage = 1;
                int miningSpeed = 100;
                
                if (toolInfo) {
                    damage = toolInfo->damage;
                    miningSpeed = toolInfo->speed;
                }
                
                // Check if cooldown has elapsed
                bool canMine = (currentTime - lastMiningTime) >= miningSpeed;
                
                if (canMine) {
                    SDL_Log("Mining attempt: damage=%d, speed=%dms, cooldown=%llu", 
                            damage, miningSpeed, (unsigned long long)(currentTime - lastMiningTime));
                    // Get object info before damaging (to check for drops)
                    ObjectInfo* objBeforeDamage = objectManager->GetObjectAt(context->getObjectMap(), gridX, gridY);
                    
                    // Damage the object with tool damage
                    bool wasDestroyed = objectManager->DamageInstance(context->getObjectMap(), gridX, gridY, damage);
                    
                    // Create damage popup at object position
                    if (damagePopups) {
                        float randomOffsetX = (std::rand() % 11) - 5;
                        float worldX = gridX * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f + randomOffsetX;
                        float worldY = gridY * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                        SDL_Color damageColor = {255, 255, 255, 255};
                        damagePopups->AddPopup(worldX, worldY, damage, damageColor);
                    }
                    
                    // Update last mining time
                    lastMiningTime = currentTime;
            
            // If object was destroyed, drop resources
            if (wasDestroyed && objBeforeDamage) {
                ResourceManager* resourceManager = context->getResourceManager();
                int resourceArrayId = context->getResourceArray();
                
                if (resourceManager && resourceArrayId >= 0) {
                    // Calculate world position (center of tile)
                    float baseX = gridX * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                    float baseY = gridY * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                    
                    // Drop all resources with initial velocity
                    const float INITIAL_SPEED = 50.0f;  // Initial velocity (pixels per second)
                            const int MAX_DROPS = 10;
                            
                            std::unordered_map<int, int> resourceQuantities;
                    for (const DropInstance& drop : objBeforeDamage->drops) {
                                resourceQuantities[drop.resourceId] += drop.quantity;
                            }
                            
                            int totalQuantity = 0;
                            for (const auto& pair : resourceQuantities) {
                                totalQuantity += pair.second;
                            }
                            
                            if (resourceQuantities.size() == 1) {
                                int resourceId = resourceQuantities.begin()->first;
                                int quantity = resourceQuantities.begin()->second;
                                
                                int numDrops = std::min(MAX_DROPS, quantity);
                                int baseQuantityPerDrop = quantity / numDrops;
                                int remainder = quantity % numDrops;
                                
                                for (int i = 0; i < numDrops; i++) {
                                    int dropQuantity = baseQuantityPerDrop + (i < remainder ? 1 : 0);
                                    
                                    float angle = (float)(std::rand() % 360) * M_PI / 180.0f;
                                    
                                    float vx = cosf(angle) * INITIAL_SPEED;
                                    float vy = sinf(angle) * INITIAL_SPEED;
                                    
                                    resourceManager->AddResource(resourceArrayId, baseX, baseY, resourceId, dropQuantity, vx, vy);
                                }
                            } else {
                                std::vector<std::pair<int, int>> dropsToCreate;
                                
                                int totalDropsCreated = 0;
                                for (const auto& pair : resourceQuantities) {
                                    int resourceId = pair.first;
                                    int quantity = pair.second;
                                    
                                    float proportion = (float)quantity / (float)totalQuantity;
                                    int numDropsForResource = std::max(1, (int)std::round(proportion * MAX_DROPS));
                                    
                                    if (totalDropsCreated + numDropsForResource > MAX_DROPS) {
                                        numDropsForResource = MAX_DROPS - totalDropsCreated;
                                    }
                                    
                                    if (numDropsForResource > 0) {
                                        int baseQuantityPerDrop = quantity / numDropsForResource;
                                        int remainder = quantity % numDropsForResource;
                                        
                                        for (int i = 0; i < numDropsForResource; i++) {
                                            int dropQuantity = baseQuantityPerDrop + (i < remainder ? 1 : 0);
                                            dropsToCreate.push_back({resourceId, dropQuantity});
                                        }
                                        
                                        totalDropsCreated += numDropsForResource;
                                    }
                                    
                                    if (totalDropsCreated >= MAX_DROPS) break;
                                }
                                
                                // Create the drops
                                for (const auto& drop : dropsToCreate) {
                            // Random angle for direction
                            float angle = (float)(std::rand() % 360) * M_PI / 180.0f;
                            
                            // Calculate initial velocity components
                            float vx = cosf(angle) * INITIAL_SPEED;
                            float vy = sinf(angle) * INITIAL_SPEED;
                            
                                    resourceManager->AddResource(resourceArrayId, baseX, baseY, drop.first, drop.second, vx, vy);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Update Player Movement and current animations
    // Only allow movement if face transition cooldown is not active
    PlayerAnimations currentAnimation = player->getCurrentPlayerAnimation();

    if (!context->isFaceTransitionCooldownActive()) {
        static float lastMoveX = 0.0f;
        static float lastMoveY = 0.0f;
        
        float moveX = 0.0f;
        float moveY = 0.0f;
        
        if (inventory && !inventory->IsOpen() && !(player->getPlayerState() == SWINGING || player->getPlayerState() == PUNCHING)) {
            if (inputManager->IsKeyHeld(SDLK_W)) {
                moveY -= 1.0f;
            }
            if (inputManager->IsKeyHeld(SDLK_S)) {
                moveY += 1.0f;
            }
            
            // Check horizontal movement (A/D)
            if (inputManager->IsKeyHeld(SDLK_A)) {
                moveX -= 1.0f;
            }
            if (inputManager->IsKeyHeld(SDLK_D)) {
                moveX += 1.0f;
            }
        }
        
        if (moveX != 0.0f || moveY != 0.0f) {
            Direction newDirection;
            
            if (moveY > 0.0f) {
                if (moveX < 0.0f) {
                    newDirection = Direction::FORWARD_LEFT;
                } else if (moveX > 0.0f) {
                    newDirection = Direction::FORWARD_RIGHT;
                } else {
                    newDirection = Direction::FORWARD;
                }
            } else if (moveY < 0.0f) { 
                if (moveX < 0.0f) {
                    newDirection = Direction::BACK_LEFT;
                } else if (moveX > 0.0f) {
                    newDirection = Direction::BACK_RIGHT;
                } else {
                    newDirection = Direction::BACK;
                }
            } else {
                if (moveX < 0.0f) {
                    newDirection = Direction::LEFT;
                } else {
                    newDirection = Direction::RIGHT;
                }
            }
            
            // Normalize diagonal movement (make it same speed as cardinal directions)
            if (moveX != 0.0f && moveY != 0.0f) {
                float length = std::sqrt(moveX * moveX + moveY * moveY);
                moveX = (moveX / length) * 1.0f;
                moveY = (moveY / length) * 1.0f;
            }
            
            player->setPlayerDirection(newDirection);
        }

        if (moveX != lastMoveX || moveY != lastMoveY) {
            animation->setFirstTime(true);
            player->setAnimationTime(0);
        }
        else if (moveX != 0.0f || moveY != 0.0f) {
            player->incrementAnimationTime(context->getDeltaTime());
            animation->setFirstTime(false);
        }
        
        lastMoveX = moveX;
        lastMoveY = moveY;

        // Check Animation Time Reqs for Walking
        if (inputManager->IsKeyHeld(SDLK_W) || inputManager->IsKeyHeld(SDLK_S) ||
            inputManager->IsKeyHeld(SDLK_A) || inputManager->IsKeyHeld(SDLK_D)) {
        }
        

        if (moveX != 0.0f || moveY != 0.0f) {
            float length = sqrtf(moveX * moveX + moveY * moveY);
            if (length > 0.0f) {
                moveX /= length;
                moveY /= length;
            }

            moveX *= moveSpeed;
            moveY *= moveSpeed;
            
            player->move(moveX, moveY);

            // animate player walking
            animation->AnimatePlayer(player, WALKING);
            
            // Mark current planet as dirty when player moves
            Planet* currentPlanet = context->getCurrentPlanet();
            if (currentPlanet) {
                currentPlanet->MarkDirty();
            }
            
            // Check for face transitions when player walks off edge
            context->checkAndHandleFaceTransition(player);
        }
        // Idle animations
        else {
            if ((currentAnimation != PlayerAnimations:: StandingStillBack   || 
                currentAnimation != PlayerAnimations:: StandingStillForward ||
                currentAnimation != PlayerAnimations:: StandingStillLeft    ||
                currentAnimation != PlayerAnimations:: StandingStillRight)  &&
                player->getPlayerState() != PUNCHING && player->getPlayerState() != SWINGING) 
            {
                if (player->getPlayerDirection() == Direction::BACK) {
                    player->setCurrentPlayerAnimation(PlayerAnimations:: StandingStillBack);
                }
                else if (player->getPlayerDirection() == Direction::FORWARD) {
                    player->setCurrentPlayerAnimation(PlayerAnimations:: StandingStillForward);
                }
                else if (player->getPlayerDirection() == Direction::LEFT) {
                    player->setCurrentPlayerAnimation(PlayerAnimations:: StandingStillLeft);
                }
                else if (player->getPlayerDirection() == Direction::RIGHT) {
                    player->setCurrentPlayerAnimation(PlayerAnimations:: StandingStillRight);
                }
            }
            // Idle punching animation
            if (inputManager->IsMouseButtonHeld(1)) {

                if (player->getPlayerState() != PUNCHING && player->getPlayerState() != SWINGING) {
                    player->setAnimationTime(0);
                }


                if (player->GetEquippedToolId() == -1) {
                // Check Animation Time Requirements
                    if (player->getPlayerState() == PUNCHING) {
                        player->incrementAnimationTime(context->getDeltaTime());
                        animation->setFirstTime(false);
                    } 
                    else if (player->getPlayerState() != PUNCHING){
                        player->setAnimationTime(0);
                    }
                    player->setPlayerState(PUNCHING);
                    SDL_Log("%d", player->getAnimationTime());
                    animation->AnimatePlayer(player, PUNCHING);
                }
                else if (player->GetEquippedToolId() != -1) {
                    if (player->getPlayerState() == SWINGING) {
                        player->incrementAnimationTime(context->getDeltaTime());
                        animation->setFirstTime(false);
                    } 
                    else if (player->getPlayerState() != SWINGING){
                        player->setAnimationTime(0);
                    }
                    player->setPlayerState(SWINGING);

                    SDL_Log("%d", player->getAnimationTime());
                    animation->AnimatePlayer(player, SWINGING);
                }
            }
            else if (!inputManager->IsMouseButtonHeld(1)) {
                player->setAnimationTime(0);
                player->setPlayerState(IDLE);
            }
        }
    }
    
    
    // Update resources (falling animation and magnetic pickup)
    ResourceManager* resourceManager = context->getResourceManager();
    int resourceArrayId = context->getResourceArray();
    if (resourceManager && resourceArrayId >= 0 && player) {
        float playerX = player->getX();
        float playerY = player->getY();
        float deltaTime = (float)context->getDeltaTime();
        
        resourceManager->Update(resourceArrayId, playerX, playerY, deltaTime, player, hud ? hud : nullptr);
    }
    
    // Update object nodes for regenerating objects
    Planet* currentPlanet = context->getCurrentPlanet();
    if (currentPlanet && objectManager) {
        float deltaTimeSeconds = (float)context->getDeltaTime() / 1000.0f;
        // Update nodes for all faces (or just current face for performance)
        // For now, update all faces - can optimize to only update current face later
        currentPlanet->UpdateAllObjectNodes(deltaTimeSeconds, objectManager);
    }
    if (currentPlanet && currentPlanet->CanGenerateChild()) {
        BigBangEngine* engine = currentPlanet->GetPortalEngine();
        if (engine && engine->IsFullyGrown()) {
            int currentPlanetId = context->getCurrentPlanetId();
            context->GeneratePlanetInTree(currentPlanetId);
            engine->ResetScale();
        }
    }
    
    if (player && camera) {
        camera->SnapToTarget(player->getX(), player->getY());
    }
}

void MortalState::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    TextureManager* textureManager = context->getTextureManager();
    TileManager* tileManager = context->getTileManager();
    ObjectManager* objectManager = context->getObjectManager();
    Camera* camera = context->getCamera();

    TileGrid* grid = tileManager->GetTileGrid(context->getMap());
    if (!grid) return;

    float cameraX = camera->GetX();
    float cameraY = camera->GetY();
    
    // Cull tiles: calculate which tiles to render
    float zoom = camera->GetZoom();
    float effectiveWidth = VIRTUAL_WIDTH / zoom;
    float effectiveHeight = VIRTUAL_HEIGHT / zoom;
    
    // Start from the left/top edge of the visible area (camera is at center)
    int startGridX = (int)((cameraX - effectiveWidth / 2.0f) / TILE_RENDER_SIZE);
    int startGridY = (int)((cameraY - effectiveHeight / 2.0f) / TILE_RENDER_SIZE);
    
    // Add +2 buffer tiles
    int tilesX = (int)(effectiveWidth / TILE_RENDER_SIZE) + 2;
    int tilesY = (int)(effectiveHeight / TILE_RENDER_SIZE) + 2;
    
    // Render tiles
    for (int y = startGridY; y < startGridY + tilesY; y++) {
        for (int x = startGridX; x < startGridX + tilesX; x++) {
            if (grid->IsValid(x, y)) {
                float renderX = camera->WorldToRenderX(x * TILE_RENDER_SIZE, VIRTUAL_WIDTH);
                float renderY = camera->WorldToRenderY(y * TILE_RENDER_SIZE, VIRTUAL_HEIGHT);

                int tileId = grid->GetTile(x, y);
                
                textureManager->RenderTile(tileManager, tileId, renderX, renderY, zoom);
            }
        }
    }
    
    // Render BigBangEngine if viewing TOP face
    if (context->getCurrentPlanet() && context->getCurrentPlanetFace() == 4) {
        Planet* planet = context->getCurrentPlanet();
        if (planet->HasChildCapacity()) {
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
    }
    
    struct ObjectToRender {
        int objectId;
        float renderX;
        float renderY;
    };
    std::vector<ObjectToRender> objectsInFront;
    
    ObjectGrid* objectGrid = objectManager->GetObjectGrid(context->getObjectMap());
    if (objectGrid && player) {
        const int objectBufferTiles = 3;
        
        int objectStartGridY = startGridY - objectBufferTiles;
        int objectEndGridY = startGridY + tilesY;
        int objectStartGridX = startGridX;
        int objectEndGridX = startGridX + tilesX;
        
        float playerY = player->getY();
        
        for (int y = objectStartGridY; y < objectEndGridY; y++) {
            for (int x = objectStartGridX; x < objectEndGridX; x++) {
                if (objectGrid->IsValid(x, y) && objectGrid->HasObject(x, y)) {
                    float worldX = x * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                    float worldY = y * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                    float renderX, renderY;
                    camera->WorldToRender(worldX, worldY, renderX, renderY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

                    int objectId = objectGrid->GetObject(x, y);
                    ObjectInfo* objInfo = objectManager->GetObject(objectId);
                    
                    if (objInfo) {
                        float halfWidth = (objInfo->width * zoom) / 2.0f;
                        float halfHeight = (objInfo->height * zoom) / 2.0f;
                        
                        if (renderX + halfWidth > 0 && renderX - halfWidth < VIRTUAL_WIDTH && 
                            renderY + halfHeight > 0 && renderY - halfHeight < VIRTUAL_HEIGHT) {
                            
                            if (worldY > playerY) {
                                objectsInFront.push_back({objectId, renderX, renderY});
                            } else {
                                textureManager->RenderObject(objectManager, objectId, renderX, renderY, zoom);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Render resources
    ResourceManager* resourceManager = context->getResourceManager();
    if (resourceManager) {
        int resourceArrayId = context->getResourceArray();
        if (resourceArrayId >= 0) {
            float minX = cameraX - effectiveWidth / 2.0f;
            float maxX = cameraX + effectiveWidth / 2.0f;
            float minY = cameraY - effectiveHeight / 2.0f;
            float maxY = cameraY + effectiveHeight / 2.0f;
            
            std::vector<ResourceInstance*> resources = resourceManager->GetResourcesInArea(
                resourceArrayId, minX, minY, maxX, maxY);
            
            for (ResourceInstance* resource : resources) {
                if (resource) {
                    float renderX, renderY;
                    camera->WorldToRender(resource->x, resource->y, renderX, renderY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
                    textureManager->RenderResource(resourceManager, resource->resourceId, renderX, renderY, zoom * 0.8);
                }
            }
        }
    }
    

    //Render Player
    if (player) {
        float renderX, renderY;
        camera->WorldToRender(player->getX(), player->getY(), renderX, renderY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
        
        // Check if player is punching
        bool isSwinging = (player->getPlayerState() == SWINGING);
        int equippedToolId = player->GetEquippedToolId();
        Direction playerDir = player->getPlayerDirection();
        bool isFacingBack = (playerDir == Direction::BACK);
        
        if (!isSwinging && equippedToolId >= 0 && !isFacingBack) {
            ToolManager* toolManager = context->getToolManager();
            if (toolManager) {
                ToolInfo* toolInfo = toolManager->GetTool(equippedToolId);
                if (toolInfo) {
                    float toolOffsetX = 0.0f;
                    
                    if (playerDir == Direction::LEFT) {
                        toolOffsetX = 3.0f * zoom;
                    } else if (playerDir == Direction::RIGHT) {
                        toolOffsetX = -3.0f * zoom;
                    }
                    
                    float toolX = renderX + toolOffsetX;
                    float toolY = renderY + -3.0f * zoom;
                    
                    bool flipTool = (playerDir == Direction::LEFT);
                    
                    textureManager->RenderTool(toolManager, equippedToolId, toolX, toolY, zoom, flipTool, 180.0);
                }
            }
        }
        
        textureManager->RenderPlayer(player, renderX, renderY, player -> getCurrentPlayerAnimation(), zoom);
        
        if (!isSwinging && equippedToolId >= 0 && isFacingBack) {
            ToolManager* toolManager = context->getToolManager();
            if (toolManager) {
                ToolInfo* toolInfo = toolManager->GetTool(equippedToolId);
                if (toolInfo) {
                    float toolX = renderX;
                    float toolY = renderY + -3.0f * zoom;
                    
                    bool flipTool = true;
                    
                    textureManager->RenderTool(toolManager, equippedToolId, toolX, toolY, zoom, flipTool, 180.0);
                }
            }
        }
        
        // Render equipped tool in player's hand when punching
        if (isSwinging && equippedToolId >= 0) {
            ToolManager* toolManager = context->getToolManager();
            if (toolManager) {
                ToolInfo* toolInfo = toolManager->GetTool(equippedToolId);
                if (toolInfo) {
                    Direction playerDir = player->getPlayerDirection();
                    float toolOffsetX = 0.0f;
                    float toolOffsetY = 0.0f;
                    
                    switch (playerDir) {
                        case Direction::RIGHT:
                            toolOffsetX = 6.0f * zoom;
                            toolOffsetY = -2.0f * zoom;
                            break;
                        case Direction::LEFT:
                            toolOffsetX = -6.0f * zoom;
                            toolOffsetY = -2.0f * zoom;
                            break;
                        case Direction::FORWARD:
                        case Direction::FORWARD_LEFT:
                        case Direction::FORWARD_RIGHT:
                            toolOffsetX = 5.0f * zoom;
                            toolOffsetY = -4.0f * zoom;
                            break;
                        case Direction::BACK:
                        case Direction::BACK_LEFT:
                        case Direction::BACK_RIGHT:
                            toolOffsetX = -4.0f * zoom;
                            toolOffsetY = -10.0f * zoom;
                            break;
                        default:
                            toolOffsetX = 4.0f * zoom;
                            toolOffsetY = -2.0f * zoom;
                            break;
                    }
                    
                    bool flipTool = (playerDir == Direction::LEFT || playerDir == Direction::BACK);
                    
                    float hiltX = renderX + toolOffsetX;
                    float hiltY = renderY + toolOffsetY;
                    
                    Uint64 animTime = player->getAnimationTime();
                    PlayerAnimations currentAnim = player->getCurrentPlayerAnimation();
                    
                    bool handRaised = (currentAnim == PlayerAnimations::SwingingForwardToolUp ||
                                      currentAnim == PlayerAnimations::SwingingBackToolup ||
                                      currentAnim == PlayerAnimations::SwingingLeftToolUp ||
                                      currentAnim == PlayerAnimations::SwingingRightToolUp);
                    
                    float frameProgress = (float)animTime / 100.0f;
                    if (frameProgress > 1.0f) frameProgress = 1.0f;

                    double arcAngle;
                    
                    if (handRaised) {
                        float t = frameProgress;
                        float eased = t * t * (3.0f - 2.0f * t);
                        arcAngle = -30.0 + (90.0 * eased);
                    } else {
                        float t = frameProgress;
                        float eased = t * t * (3.0f - 2.0f * t);
                        arcAngle = 60.0 - (90.0 * eased);
                    }
                    
                    if (playerDir == Direction::LEFT || playerDir == Direction::BACK) {
                        arcAngle = -arcAngle;
                    }
                    
                    float hiltPosX = (float)toolInfo->width / 2.0f;
                    float hiltPosY = (float)toolInfo->height - 2.0f;
                    
                    textureManager->RenderTool(toolManager, equippedToolId, hiltX, hiltY, zoom, flipTool, arcAngle, hiltPosX, hiltPosY);
                }
            }
        }
    }
    
    // Render objects in front of player
    if (objectManager) {
        for (const ObjectToRender& obj : objectsInFront) {
            textureManager->RenderObject(objectManager, obj.objectId, obj.renderX, obj.renderY, zoom);
        }
    }

    // Convert mouse window coordinates to render/logical coordinates
    float mouseScreenX = inputManager->GetMouseX();
    float mouseScreenY = inputManager->GetMouseY();
    float mouseRenderX, mouseRenderY;
    SDL_RenderCoordinatesFromWindow(renderer, mouseScreenX, mouseScreenY, &mouseRenderX, &mouseRenderY);

    float mouseWheelY = inputManager->GetMouseWheelY();
    
    float worldX, worldY;
    camera->RenderToWorld(mouseRenderX, mouseRenderY, worldX, worldY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    int gridX = (int)(worldX / TILE_RENDER_SIZE);
    int gridY = (int)(worldY / TILE_RENDER_SIZE);

    ObjectInfo* objInfo = objectManager->GetObjectAt(context->getObjectMap(), gridX, gridY);
    
    int crosshairIndex = 0;
    Player* currentPlayer = context->getPlayer();
    if (currentPlayer && objectManager->PlayerCanInteract(context->getObjectMap(), gridX, gridY, currentPlayer) && inventory && !inventory->IsOpen()) {
        crosshairIndex = 1;
    }
    
    if (hud && player && resourceManager && textureManager) {
        hud->Render(player, resourceManager, textureManager);
    }

    if (inventory && renderer && player && resourceManager && textureManager && context) {
        ToolManager* toolManager = context->getToolManager();
        if (toolManager) {
            inventory->Render(renderer, player, resourceManager, textureManager, toolManager);
        }
    }
    
    // Render engine compass
    if (engineCompass && renderer && textureManager) {
        engineCompass->Render(renderer, textureManager);
    }
    
    // Render damage popups
    if (damagePopups && hud) {
        TextRenderer* textRenderer = hud->GetTextRenderer();
        if (textRenderer && camera) {
            damagePopups->Render(textRenderer, camera, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
        }
    }
    
    const int crosshairSize = 7;
    int srcX = crosshairIndex * crosshairSize;
    int srcY = 0;
    
    // Center the crosshair on the mouse position
    float crosshairX = mouseRenderX - crosshairSize / 2.0f;
    float crosshairY = mouseRenderY - crosshairSize / 2.0f;
    
    textureManager->RenderSprite("crosshairs", srcX, srcY, crosshairSize, crosshairSize, crosshairX, crosshairY, 1.0f);
    
    SDL_RenderPresent(renderer);
}

void MortalState::onEnter() {
    if (inventory && context) {
        inventory->SetSubmitCallback([this](Planet* planet, float totalValue, const std::unordered_map<int, int>& resourcesToConsume) {
            // Add null check to prevent segfault
            if (!context) {
                SDL_Log("Error: context is null in submit callback");
                return;
            }
            if (planet) {
                // Use the planet that was used for calculation (passed from inventory)
                planet->AddEnergy(totalValue);
                
                // Consume resources from player inventory
                Player* currentPlayer = context->getPlayer();
                if (currentPlayer) {
                    for (const auto& entry : resourcesToConsume) {
                        int resourceId = entry.first;
                        int quantity = entry.second;
                        if (quantity > 0) {
                            currentPlayer->ConsumeResource(resourceId, quantity);
                        }
                    }
                }
            } else {
                SDL_Log("Warning: Planet is null in submit callback");
            }
        });
    }
    // Hide system cursor when entering MortalState
    SDL_HideCursor();
    
    if (player) {
        Camera* camera = context->getCamera();
        if (camera) {
            // Preserve player's current position, just snap camera to it
            float playerX = player->getX();
            float playerY = player->getY();
            
            // Only reset to center if player is at invalid position (0,0)
            if (playerX == 0.0f && playerY == 0.0f) {
                int radius = GetPlanetRadius(PlanetSize::TINY);
                playerX = radius * TILE_RENDER_SIZE;
                playerY = radius * TILE_RENDER_SIZE;
                player->setX(playerX);
                player->setY(playerY);
            }
            
            camera->SnapToTarget(playerX, playerY);
            camera->SetZoomBounds(0.8f, 10.0f);
        }
    }
}

void MortalState:: onExit() {
    // Show system cursor when exiting MortalState
    SDL_ShowCursor();
}