#include <SDL3/SDL.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include <vector>
#include "SDL3/SDL_keycode.h"
#include "core/textures.h"
#include "world/tiles.h"
#include "world/objects.h"
#include "definitions/tile_definitions.h"
#include "definitions/frame_definitions.h"
#include "definitions/object_definitions.h"
#include "definitions/resource_definitions.h"
#include "world/map_generation.h"
#include "utils/constants.h"
#include "core/camera.h"
#include "states/GameContext.h"
#include "core/input_manager.h"
#include <memory>
#include "entities/player.h"
#include "items/resources.h"
#include "ui/hud.h"
#include "world/planet.h"
#include "core/Animations.h"
#include "world/planet_tree.h"
#include "world/BigBangEngine.h"

struct SDLApplication {
    SDL_Window* window;
    SDL_Renderer* renderer;
    InputManager input;
    //to run indefinitely
    std::unique_ptr<GameContext> context = std::make_unique<GameContext>();
    Player* player = context -> getPlayer();
    HUD* hud = nullptr;

    //constructor
    SDLApplication(const char* title) {
        SDL_Init(SDL_INIT_VIDEO);
        const ResolutionPreset& initialPreset = RESOLUTION_PRESETS[context->getCurrentResolutionIndex()];
        window = SDL_CreateWindow(title, initialPreset.width, initialPreset.height, SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, NULL);
        
        SDL_SetRenderLogicalPresentation(renderer, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        
        std::string windowTitle = std::string(title) + " - " + initialPreset.name + 
                                 " (" + std::to_string(initialPreset.width) + "x" + 
                                 std::to_string(initialPreset.height) + ")";
        SDL_SetWindowTitle(window, windowTitle.c_str());
        
        context->InitializeManagers(window, renderer);
        
        // Initialize HUD
        hud = new HUD(renderer);
    }

    //destructor
    ~SDLApplication() {
        // Clean up HUD
        if (hud) {
            delete hud;
            hud = nullptr;
        }
        
        // Managers are cleaned up by GameContext destructor
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        SDL_Quit();
    }

    bool Initialize() {
        TextureManager* textureManager = context->getTextureManager();
        TileManager* tileManager = context->getTileManager();
        ObjectManager* objectManager = context->getObjectManager();
        ResourceManager* resourceManager = context->getResourceManager();
        player = context->getPlayer();
        Camera* camera = context->getCamera();
        
        // Allow zoom out to 0.01 (100x zoomed out)
        if (camera) {
            camera->SetZoomBounds(0.8f, 10.0f);
        }
        
        SetupTiles(tileManager, textureManager);
        SetupResources(resourceManager, textureManager);
        SetupObjects(objectManager, textureManager, resourceManager);
        SetupAnimations(player, textureManager);
        
        context->GeneratePlanetTree();
        
        context->setCurrentPlanetById(0);
        context->setCurrentPlanetFace(0);
        
        // Spawn player in the center of the starting face
        if (player) {
            int radius = GetPlanetRadius(PlanetSize::TINY);
            float centerX = (radius * TILE_RENDER_SIZE) / 2.0f;
            float centerY = (radius * TILE_RENDER_SIZE) / 2.0f;
            player->setX(centerX);
            player->setY(centerY);
            camera->SnapToTarget(centerX, centerY);
        }
        
        return true;
    }

    //Handle input events from I/O or networking devices
    void Tick(){
        Input();
        Update();
        Render();
        input.EndFrame();
    }

    //Advances out loop one iteration
    void Input() {
        SDL_Event event;

        //Event Handling loop
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT){
                context -> Quit();
            }
            else if(event.type == SDL_EVENT_KEY_DOWN) {
                input.ProcessKeyDown(event.key.key);

                switch (event.key.key) {
                    case SDLK_ESCAPE: context -> Quit(); break;
                    case SDLK_UP: {
                        // Navigate to parent planet
                        PlanetTree* tree = context->getPlanetTree();
                        if (tree) {
                            int currentPlanetId = context->getCurrentPlanetId();
                            PlanetNode* parent = tree->GetParent(currentPlanetId);
                            if (parent && context->setCurrentPlanetById(parent->planetId)) {
                                // Set to TOP face so portal is visible
                                context->setCurrentPlanetFace(4); // TOP
                                // Respawn player at center of new planet
                                int radius = GetPlanetRadius(PlanetSize::TINY);
                                float centerX = (radius * TILE_RENDER_SIZE) / 2.0f;
                                float centerY = (radius * TILE_RENDER_SIZE) / 2.0f;
                                player->setX(centerX);
                                player->setY(centerY);
                                Camera* camera = context->getCamera();
                                if (camera) camera->SnapToTarget(centerX, centerY);
                            }
                        }
                        break;
                    }
                    case SDLK_DOWN: {
                        // Cycle through all resolutions
                        int currentIndex = context->getCurrentResolutionIndex();
                        int nextIndex = (currentIndex + 1) % RESOLUTION_PRESET_COUNT;
                        context->ChangeResolution(nextIndex);
                        break;
                    }
                    case SDLK_LEFT: {
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
                                float centerX = (radius * TILE_RENDER_SIZE) / 2.0f;
                                float centerY = (radius * TILE_RENDER_SIZE) / 2.0f;
                                player->setX(centerX);
                                player->setY(centerY);
                                Camera* camera = context->getCamera();
                                if (camera) camera->SnapToTarget(centerX, centerY);
                            }
                        }
                        break;
                    }
                    case SDLK_RIGHT: {
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
                                float centerX = (radius * TILE_RENDER_SIZE) / 2.0f;
                                float centerY = (radius * TILE_RENDER_SIZE) / 2.0f;
                                player->setX(centerX);
                                player->setY(centerY);
                                Camera* camera = context->getCamera();
                                if (camera) camera->SnapToTarget(centerX, centerY);
                            }
                        }
                        break;
                    }
                    case SDLK_E: {
                        Player* player = context->getPlayer();
                        Planet* currentPlanet = context->getCurrentPlanet();
                        ResourceManager* resourceManager = context -> getResourceManager();

                        if (!player || !currentPlanet || !resourceManager) {
                            return;
                        }

                        // Only feed resources for ONE child per E press
                        // Check if planet can accept more energy (has capacity and not already full)
                        if (!currentPlanet->HasChildCapacity()) {
                            SDL_Log("Planet has already generated maximum children");
                            break;
                        }
                        
                        if (currentPlanet->CanGenerateChild()) {
                            // Already has enough energy - generate the child
                            context->GeneratePlanetInTree(context->getCurrentPlanetId());
                            SDL_Log("Generated child planet!");
                            break;
                        }
                        
                        std::unordered_map<int, int>* inventory = player->getInventory();
                        std::vector<std::pair<int, int>> entries(inventory->begin(), inventory->end());
                        
                        float energyCost = currentPlanet->GetEnergyCost();
                        float missing = energyCost - currentPlanet->GetCurrentEnergy();
                        
                        // Process each resource type; only accept what is needed to reach ONE child
                        for (const auto& entry : entries) {
                            if (missing <= 0.0f) break; // Already have enough for this child
                            
                            int resourceId = entry.first;
                            int remainingQty = entry.second;
                            
                            float energyPerUnit = resourceManager->GetResourceValue(resourceId);
                            
                            // Skip non-energy resources
                            if (energyPerUnit <= 0.0f) {
                                continue;
                            }
                            
                            // Calculate exact units needed to fill the gap (not more)
                            int unitsNeeded = (int)ceilf(missing / energyPerUnit);
                            if (unitsNeeded < 1) unitsNeeded = 1;
                            int consumeNow = (remainingQty < unitsNeeded) ? remainingQty : unitsNeeded;
                            
                            // Only add the exact energy needed, not more
                            float addEnergy = energyPerUnit * (float)consumeNow;
                            if (addEnergy > missing) {
                                addEnergy = missing;
                            }
                            
                            currentPlanet->AddEnergy(addEnergy);
                            player->ConsumeResource(resourceId, consumeNow);
                            missing -= addEnergy;
                        }
                        
                        char energyMsg[128];
                        snprintf(energyMsg, sizeof(energyMsg), "Your planet now has %.1f/%.1f energy", 
                                currentPlanet->GetCurrentEnergy(), currentPlanet->GetEnergyCost());
                        SDL_Log("%s", energyMsg);
                        
                        // Check if we now have enough to generate
                        if (currentPlanet->CanGenerateChild()) {
                            SDL_Log("Energy requirement met! Press E again to generate child planet.");
                        }

                    }
                }
            }
            else if(event.type == SDL_EVENT_KEY_UP) {
                input.ProcessKeyUp(event.key.key);
            }
            else if(event.type == SDL_EVENT_MOUSE_MOTION) {
                input.ProcessMouseMotion(event.motion.x, event.motion.y, 
                                         event.motion.xrel, event.motion.yrel);
            }
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                input.ProcessMouseButtonDown(event.button.button);
            }
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                input.ProcessMouseButtonUp(event.button.button);
            }
            else if(event.type == SDL_EVENT_MOUSE_WHEEL) {
                input.ProcessMouseWheel(event.wheel.y);
            }
        }
    }

    void Update() {
        // Update face transition cooldown
        context->updateFaceTransitionCooldown();
        
        TextureManager* textureManager = context->getTextureManager();
        TileManager* tileManager = context->getTileManager();
        ObjectManager* objectManager = context->getObjectManager();
        Camera* camera = context->getCamera();
        std::unique_ptr<Animations> animation = std::make_unique<Animations>();

        const float moveSpeed = 1.5f;
        
        // Handle mouse wheel zoom (zooms toward screen center)
        float mouseWheelY = input.GetMouseWheelY();
        if (mouseWheelY != 0.0f) {
            float zoomSpeed = 0.1f;
            if (mouseWheelY > 0.0f) {
                camera->ZoomIn(mouseWheelY * zoomSpeed);
            } else {
                camera->ZoomOut(-mouseWheelY * zoomSpeed);
            }
        }

        float mouseScreenX = input.GetMouseX();
        float mouseScreenY = input.GetMouseY();
        float mouseRenderX, mouseRenderY;
        SDL_RenderCoordinatesFromWindow(renderer, mouseScreenX, mouseScreenY, &mouseRenderX, &mouseRenderY);
        
        float worldX, worldY;
        camera->RenderToWorld(mouseRenderX, mouseRenderY, worldX, worldY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
        int gridX = (int)(worldX / TILE_RENDER_SIZE);
        int gridY = (int)(worldY / TILE_RENDER_SIZE);

        ObjectInfo* objInfo = objectManager->GetObjectAt(context->getObjectMap(), gridX, gridY);

        if (input.IsMouseButtonHeld(1)) {
            if (objectManager->PlayerCanInteract(context->getObjectMap(), gridX, gridY, context->getPlayer())) {
                // Get object info before damaging (to check for drops)
                ObjectInfo* objBeforeDamage = objectManager->GetObjectAt(context->getObjectMap(), gridX, gridY);
                
                // Damage the object
                bool wasDestroyed = objectManager->DamageInstance(context->getObjectMap(), gridX, gridY, 1);
                
                // If object was destroyed, drop resources
                if (wasDestroyed && objBeforeDamage) {
                    ResourceManager* resourceManager = context->getResourceManager();
                    int resourceArrayId = context->getResourceArray();
                    
                    if (resourceManager && resourceArrayId >= 0) {
                        // Calculate world position (center of tile)
                        float baseX = gridX * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                        float baseY = gridY * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                        
                        // Drop all resources with initial velocity
                        const float INITIAL_SPEED = 1000.0f;  // Initial velocity (pixels per second)
                        
                        for (const DropInstance& drop : objBeforeDamage->drops) {
                            // Drop each resource with random direction
                            for (int i = 0; i < drop.quantity; i++) {
                                // Random angle for direction
                                float angle = (float)(std::rand() % 360) * M_PI / 180.0f;
                                
                                // Calculate initial velocity components
                                float vx = cosf(angle) * INITIAL_SPEED;
                                float vy = sinf(angle) * INITIAL_SPEED;
                                
                                resourceManager->AddResource(resourceArrayId, baseX, baseY, drop.resourceId, 1, vx, vy);
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
            float moveX = 0.0f;
            float moveY = 0.0f;
            
            // Check vertical movement (W/S)
            if (input.IsKeyHeld(SDLK_W)) {
                moveY -= 1.0f;
                player->setCurrentPlayerAnimation(PlayerAnimations::WalkingBackLeftFoot);
                player->setPlayerDirection(Direction::BACK);
            }
            else if (input.IsKeyHeld(SDLK_S)) {
                moveY += 1.0f;
                player->setCurrentPlayerAnimation(PlayerAnimations::WalkingForwardRightFoot);
                player->setPlayerDirection(Direction::FORWARD);
            }
            
            // Check horizontal movement (A/D) - independent of vertical
            if (input.IsKeyHeld(SDLK_A)) {
                moveX -= 1.0f;
                player->setCurrentPlayerAnimation(PlayerAnimations::WalkingLeftLeftFoot);
                player->setPlayerDirection(Direction::LEFT);
            }
            else if (input.IsKeyHeld(SDLK_D)) {
                moveX += 1.0f;
                player->setCurrentPlayerAnimation(PlayerAnimations::WalkingRightRightFoot);
                player->setPlayerDirection(Direction::RIGHT);
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
                player->setPlayerState(WALKING);
                
                // Check for face transitions when player walks off edge
                context->checkAndHandleFaceTransition(player);
            }
            // Idle animations
            else {
                if ((currentAnimation != PlayerAnimations:: StandingStillBack   || 
                    currentAnimation != PlayerAnimations:: StandingStillForward ||
                    currentAnimation != PlayerAnimations:: StandingStillLeft    ||
                    currentAnimation != PlayerAnimations:: StandingStillRight)  &&
                    player->getPlayerState() != PUNCHING) 
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
                    else {
                        SDL_Log("test");
                    }
                }
                // Idle punching animation
                if (input.IsMouseButtonHeld(1)) {
                    bool firstPunch = false;

                    // Check Animation Time Requirements
                    if (player->getPlayerState() == PUNCHING) {
                        player->incrementAnimationTime(context->getDeltaTime());
                        firstPunch = false;
                        animation->setFirstTime(false);
                    } 
                    else if (player->getPlayerState()){
                        player->setAnimationTime(0);
                        firstPunch = true;
                    }
                    player->setPlayerState(PUNCHING);

                    animation->AnimatePlayer(player, PUNCHING);
                }
                else if (!input.IsMouseButtonHeld(1)) {
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
            
            resourceManager->Update(resourceArrayId, playerX, playerY, deltaTime, player);
        }
        
        Planet* currentPlanet = context->getCurrentPlanet();
        if (currentPlanet && currentPlanet->CanGenerateChild()) {
            BigBangEngine* engine = currentPlanet->GetPortalEngine();
            if (engine && engine->IsFullyGrown()) {
                int currentPlanetId = context->getCurrentPlanetId();
                context->GeneratePlanetInTree(currentPlanetId);
                engine->ResetScale();
            }
        }
        
        if (player) {
            camera->SnapToTarget(player->getX(), player->getY());
        }
    }
    

    void Render() {
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
            textureManager->RenderPlayer(player, renderX, renderY, player -> getCurrentPlayerAnimation(), zoom);
        }
        
        // Render objects in front of player
        if (objectManager) {
            for (const ObjectToRender& obj : objectsInFront) {
                textureManager->RenderObject(objectManager, obj.objectId, obj.renderX, obj.renderY, zoom);
            }
        }

        // Convert mouse window coordinates to render/logical coordinates
        float mouseScreenX = input.GetMouseX();
        float mouseScreenY = input.GetMouseY();
        float mouseRenderX, mouseRenderY;
        SDL_RenderCoordinatesFromWindow(renderer, mouseScreenX, mouseScreenY, &mouseRenderX, &mouseRenderY);

        float mouseWheelY = input.GetMouseWheelY();
        
        // Convert render coordinates to world coordinates
        float worldX, worldY;
        camera->ScreenToWorld(mouseRenderX, mouseRenderY, worldX, worldY);
        int gridX = (int)(worldX / TILE_RENDER_SIZE);
        int gridY = (int)(worldY / TILE_RENDER_SIZE);

        // Get object at mouse position (if any)
        ObjectInfo* objInfo = objectManager->GetObjectAt(context->getObjectMap(), gridX, gridY);
        
        // Render HUD overlay (on top of everything)
        // resourceManager is already defined above, reuse it
        if (hud && player && resourceManager && textureManager) {
            hud->Render(player, resourceManager, textureManager);
        }
        
        SDL_RenderPresent(renderer);
    }

    void FPSCount(Uint64* currentTick, Uint64* lastTime, Uint64* fps) {
        if (*currentTick > *lastTime + 1000) {
            *lastTime = *currentTick;
            const ResolutionPreset& preset = RESOLUTION_PRESETS[context->getCurrentResolutionIndex()];
            std::string title = std::string(preset.name) + " (" + std::to_string(preset.width) + "x" + 
                              std::to_string(preset.height) + ") - FPS: " + std::to_string(*fps);
            SDL_SetWindowTitle(window, title.c_str());
            *fps = 0;
        }
    }
    
    void TimePerFrame(Uint64* currentTick) {
        context -> setDeltaTime(currentTick);
        if (context -> getDeltaTime() < TARGETFRAMETIME) {
            SDL_Delay(TARGETFRAMETIME - context -> getDeltaTime() - 1);
            while (SDL_GetTicks() - *currentTick < TARGETFRAMETIME) {
                // Tight loop for precision (usually < 1ms)
            }
        }
    }

    //Main Application Loop
    void MainLoop() {
        Uint64 fps = 0; //number of frames per second
        Uint64 lastTime = 0;

        //inf loop
        while(context -> isRunning()) {
            Uint64 currentTick = SDL_GetTicks();
            Tick();
            fps++;
            
            //Per frame calculation of elapsed time
            TimePerFrame(&currentTick);

            //FPS Calculation
            FPSCount(&currentTick, &lastTime, &fps);
        }
    }          
};

// Entry Point
int main() {
    SDLApplication app("poop");

    if (!app.Initialize()) {
        SDL_Log("Failed to initialize application!");
        return 1;
    }

    app.MainLoop();
    return 0;
}