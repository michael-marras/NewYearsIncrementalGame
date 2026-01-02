#include <SDL3/SDL.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include "SDL3/SDL_keycode.h"
#include "textures.h"
#include "tiles.h"
#include "objects.h"
#include "tile_definitions.h"
#include "frame_definitions.h"
#include "object_definitions.h"
#include "map_generation.h"
#include "constants.h"
#include "camera.h"
#include "states/GameContext.h"
#include "input_manager.h"
#include <memory>
#include "player.h"

struct SDLApplication {
    SDL_Window* window;
    SDL_Renderer* renderer;
    InputManager input;
    //to run indefinitely
    std::unique_ptr<GameContext> context = std::make_unique<GameContext>();

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
        player* Player = context->getPlayer();
        Camera* camera = context->getCamera();
        
        SetupTiles(tileManager, textureManager);
        SetupObjects(objectManager, textureManager);
        SetupAnimations(Player, textureManager);
        
        // Generate map from seed (change seed for different maps, or use time for random)
        unsigned int mapSeed = time(nullptr);  // Use same seed for same map, or use time(nullptr) for random
        GeneratedMap generated = GenerateMapFromSeed(mapSeed, tileManager, objectManager, 100, 100);
        
        // Set the generated map as the current active map in context
        context->setMap(generated.tileGridId);
        context->setObjectMap(generated.objectGridId);


        camera->SnapToTarget(800, 800); // this is center of the map, can we done differently later
        
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
                if(event.button.button == SDL_BUTTON_LEFT) {
                }
                if(event.button.button == SDL_BUTTON_MIDDLE) {
                }
                if(event.button.button == SDL_BUTTON_RIGHT) {
                }
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
        const float moveSpeed = 1.0f;
        
        float moveX = 0.0f;
        float moveY = 0.0f;
        
        if (input.IsKeyHeld(SDLK_W)) {
            moveY -= 1.0f;
        }
        if (input.IsKeyHeld(SDLK_S)) {
            moveY += 1.0f;
        }
        if (input.IsKeyHeld(SDLK_A)) {
            moveX -= 1.0f;
        }
        if (input.IsKeyHeld(SDLK_D)) {
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
            
            context->getCamera()->Move(moveX, moveY);
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
        int startGridX = (int)(cameraX / TILE_RENDER_SIZE);
        int startGridY = (int)(cameraY / TILE_RENDER_SIZE);
        
        // Add +2 buffer tiles
        int tilesX = (VIRTUAL_WIDTH / TILE_RENDER_SIZE) + 2;
        int tilesY = (VIRTUAL_HEIGHT / TILE_RENDER_SIZE) + 2;
        
        // Render tiles
        for (int y = startGridY; y < startGridY + tilesY; y++) {
            for (int x = startGridX; x < startGridX + tilesX; x++) {
                if (grid->IsValid(x, y)) {
                    float screenX = (x * TILE_RENDER_SIZE) - cameraX;
                    float screenY = (y * TILE_RENDER_SIZE) - cameraY;

                    int tileId = grid->GetTile(x, y);
                    
                    textureManager->RenderTile(tileManager, tileId, screenX, screenY, 1.0f);
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
                        float screenX = (x * TILE_RENDER_SIZE) - cameraX;
                        float screenY = (y * TILE_RENDER_SIZE) - cameraY;

                        int objectId = objectGrid->GetObject(x, y);
                        ObjectInfo* objInfo = objectManager->GetObject(objectId);
                        
                        if (objInfo) {
                            float objectBottomY = screenY + objInfo->height;
                            float objectRightX = screenX + objInfo->width;
                            
                            if (objectRightX > 0 && screenX < VIRTUAL_WIDTH && 
                                objectBottomY > 0 && screenY < VIRTUAL_HEIGHT) {
                                textureManager->RenderObject(objectManager, objectId, screenX, screenY, 1.0f);
                            }
                        }
                    }
                }
            }
        }

        //Render Player
        player* Player = context -> getPlayer();
        if (Player) {
            float screenX = Player -> getX() - cameraX;
            float screenY = Player -> getY() - cameraY;
            textureManager->RenderPlayer(Player, screenX, screenY, PlayerAnimations::StandingStillForward);
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