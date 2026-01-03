#include <SDL3/SDL.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include "SDL3/SDL_keycode.h"
#include "core/textures.h"
#include "world/tiles.h"
#include "world/objects.h"
#include "definitions/tile_definitions.h"
#include "definitions/frame_definitions.h"
#include "definitions/object_definitions.h"
#include "world/map_generation.h"
#include "utils/constants.h"
#include "core/camera.h"
#include "states/GameContext.h"
#include "core/input_manager.h"
#include <memory>
#include "entities/player.h"

struct SDLApplication {
    SDL_Window* window;
    SDL_Renderer* renderer;
    InputManager input;
    //to run indefinitely
    std::unique_ptr<GameContext> context = std::make_unique<GameContext>();
    Player* player = context -> getPlayer();

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
        
    }

    //destructor
    ~SDLApplication() {
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
        player = context->getPlayer();
        Camera* camera = context->getCamera();
        
        SetupTiles(tileManager, textureManager);
        SetupObjects(objectManager, textureManager);
        SetupAnimations(player, textureManager);
        
        // Generate map from seed (change seed for different maps, or use time for random)
        unsigned int mapSeed = time(nullptr);  // Use same seed for same map, or use time(nullptr) for random
        GeneratedMap generated = GenerateMapFromSeed(mapSeed, tileManager, objectManager, 100, 100);
        
        // Set the generated map as the current active map in context
        context->setMap(generated.tileGridId);
        context->setObjectMap(generated.objectGridId);

        if (player) {
            camera->SnapToTarget(player->getX(), player->getY());
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
                
                if (event.key.key == SDLK_ESCAPE) {
                    context -> Quit();
                }
                if (event.key.key == SDLK_UP) {
                    context -> ChangeResolution(context -> getCurrentResolutionIndex() - 1);
                }
                if (event.key.key == SDLK_DOWN) {
                    context -> ChangeResolution(context -> getCurrentResolutionIndex() + 1);
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
        TextureManager* textureManager = context->getTextureManager();
        TileManager* tileManager = context->getTileManager();
        ObjectManager* objectManager = context->getObjectManager();
        Camera* camera = context->getCamera();

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
                objectManager->DamageInstance(context->getObjectMap(), gridX, gridY, 1);
            }
        }
        
        // Update Player Movement and current animations

        float moveX = 0.0f;
        float moveY = 0.0f;
        
        if (input.IsKeyHeld(SDLK_W)) {
            moveY -= 1.0f;
            player->setCurrentPlayerAnimation(PlayerAnimations::WalkingBackLeftFoot);
            player->setPlayerDirection(Direction::BACK);
        }
        if (input.IsKeyHeld(SDLK_S)) {
            moveY += 1.0f;
            player->setCurrentPlayerAnimation(PlayerAnimations::WalkingForwardRightFoot);
            player->setPlayerDirection(Direction::FORWARD);
        }
        if (input.IsKeyHeld(SDLK_A)) {
            moveX -= 1.0f;
            player->setCurrentPlayerAnimation(PlayerAnimations::WalkingLeftLeftFoot);
            player->setPlayerDirection(Direction::LEFT);
        }
        if (input.IsKeyHeld(SDLK_D)) {
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
        }
        else {
            if (player->getCurrentPlayerAnimation() != PlayerAnimations:: StandingStillBack    || 
                player->getCurrentPlayerAnimation() != PlayerAnimations:: StandingStillForward ||
                player->getCurrentPlayerAnimation() != PlayerAnimations:: StandingStillLeft    ||
                player->getCurrentPlayerAnimation() != PlayerAnimations:: StandingStillRight) 
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
                }
            }
        }
        
        // Make camera follow the player
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
        
        // Render objects (on top of tiles)
        ObjectGrid* objectGrid = objectManager->GetObjectGrid(context->getObjectMap());
        if (objectGrid) {
            const int objectBufferTiles = 3;
            
            int objectStartGridY = startGridY - objectBufferTiles;
            int objectEndGridY = startGridY + tilesY;
            int objectStartGridX = startGridX;
            int objectEndGridX = startGridX + tilesX;
            
            for (int y = objectStartGridY; y < objectEndGridY; y++) {
                for (int x = objectStartGridX; x < objectEndGridX; x++) {
                    if (objectGrid->IsValid(x, y) && objectGrid->HasObject(x, y)) {
                        // Convert from tile top-left to tile center for center-based rendering
                        float worldX = x * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                        float worldY = y * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
                        float renderX, renderY;
                        camera->WorldToRender(worldX, worldY, renderX, renderY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

                        int objectId = objectGrid->GetObject(x, y);
                        ObjectInfo* objInfo = objectManager->GetObject(objectId);
                        
                        if (objInfo) {
                            // Check if object is visible (using center-based bounds)
                            float halfWidth = (objInfo->width * zoom) / 2.0f;
                            float halfHeight = (objInfo->height * zoom) / 2.0f;
                            
                            if (renderX + halfWidth > 0 && renderX - halfWidth < VIRTUAL_WIDTH && 
                                renderY + halfHeight > 0 && renderY - halfHeight < VIRTUAL_HEIGHT) {
                                textureManager->RenderObject(objectManager, objectId, renderX, renderY, zoom);
                            }
                        }
                    }
                }
            }
        }

        //Render Player
        if (player) {
            float renderX, renderY;
            camera->WorldToRender(player->getX(), player->getY(), renderX, renderY, VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
            textureManager->RenderPlayer(player, renderX, renderY, player -> getCurrentPlayerAnimation());
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