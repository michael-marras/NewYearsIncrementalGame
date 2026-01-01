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
#include "object_definitions.h"
#include "map_generation.h"
#include "constants.h"
#include "camera.h"
#include "GameContext.h"

struct SDLApplication {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TextureManager* textureManager;
    TileManager* tileManager;
    ObjectManager* objectManager;
    Camera* camera;
    int currentResolutionIndex = 5;
    std::unordered_map<SDL_Keycode, bool> keysHeld;
    //to run indefinitely
    bool running = true;
    int map;
    int objectMap;

    //constructor
    SDLApplication(const char* title) {
        SDL_Init(SDL_INIT_VIDEO);
        const ResolutionPreset& initialPreset = RESOLUTION_PRESETS[currentResolutionIndex];
        window = SDL_CreateWindow(title, initialPreset.width, initialPreset.height, SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, NULL);
        
        SDL_SetRenderLogicalPresentation(renderer, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        
        std::string windowTitle = std::string(title) + " - " + initialPreset.name + 
                                 " (" + std::to_string(initialPreset.width) + "x" + 
                                 std::to_string(initialPreset.height) + ")";
        SDL_SetWindowTitle(window, windowTitle.c_str());
        
        textureManager = new TextureManager(renderer);
        tileManager = new TileManager();
        objectManager = new ObjectManager();
        camera = new Camera(0.0f, 0.0f);
        
    }

    //destructor
    ~SDLApplication() {
        delete camera;
        delete objectManager;
        delete tileManager;
        delete textureManager;
        
        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        SDL_Quit();
    }

    bool Initialize() {
        SetupTiles(tileManager, textureManager);
        SetupObjects(objectManager, textureManager);
        
        // Generate map from seed (change seed for different maps, or use time for random)
        unsigned int mapSeed = 12345;  // Use same seed for same map, or use time(nullptr) for random
        GeneratedMap generated = GenerateMapFromSeed(mapSeed, tileManager, objectManager, 100, 100);
        map = generated.tileGridId;
        objectMap = generated.objectGridId;


        camera->SnapToTarget(800, 800); // this is center of the map, can we done differently later
        
        return true;
    }

    // Change resolution to a different preset
    void ChangeResolution(int newIndex) {
        if (newIndex < 0) newIndex = 0;
        if (newIndex >= RESOLUTION_PRESET_COUNT) newIndex = RESOLUTION_PRESET_COUNT - 1;
        
        if (newIndex == currentResolutionIndex) return;
        
        currentResolutionIndex = newIndex;
        const ResolutionPreset& preset = RESOLUTION_PRESETS[currentResolutionIndex];
        
        // Resize the window
        SDL_SetWindowSize(window, preset.width, preset.height);
        
        // Center the window on the screen
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        
        SDL_Log("Changed resolution to %s (%dx%d)", preset.name, preset.width, preset.height);
    }

    //Handle input events from I/O or networking devices
    void Tick(){
        Input();
        Update();
        Render();
    }

    //Advances out loop one iteration
    void Input() {
        SDL_Event event;

        //Event Handling loop
        while (SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT){
                running = false;
            }
            else if(event.type == SDL_EVENT_KEY_DOWN) {
                keysHeld[event.key.key] = true;
                
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                }
                if (event.key.key == SDLK_UP) {
                    ChangeResolution(currentResolutionIndex - 1);
                }
                if (event.key.key == SDLK_DOWN) {
                    ChangeResolution(currentResolutionIndex + 1);
                }
            }
            else if(event.type == SDL_EVENT_KEY_UP) {
                keysHeld[event.key.key] = false;
            }
            else if(event.type == SDL_EVENT_MOUSE_MOTION) {
            }
            else if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if(event.button.button == SDL_BUTTON_LEFT) {
                }
                if(event.button.button == SDL_BUTTON_MIDDLE) {
                }
                if(event.button.button == SDL_BUTTON_RIGHT) {
                }
            }
        }
    }

    void Update() {
        const float moveSpeed = 1.0f;
        
        float moveX = 0.0f;
        float moveY = 0.0f;
        
        if (IsKeyHeld(SDLK_W)) {
            moveY -= 1.0f;
        }
        if (IsKeyHeld(SDLK_S)) {
            moveY += 1.0f;
        }
        if (IsKeyHeld(SDLK_A)) {
            moveX -= 1.0f;
        }
        if (IsKeyHeld(SDLK_D)) {
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
    
    bool IsKeyHeld(SDL_Keycode key) const {
        auto it = keysHeld.find(key);
        return (it != keysHeld.end() && it->second);
    }

    void Render() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        TileGrid* grid = tileManager->GetTileGrid(map);
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
        ObjectGrid* objectGrid = objectManager->GetObjectGrid(objectMap);
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
        
        SDL_RenderPresent(renderer);
    }

    void FPSCount(Uint64* currentTick, Uint64* lastTime, Uint64* fps) {
        if (*currentTick > *lastTime + 1000) {
            *lastTime = *currentTick;
            const ResolutionPreset& preset = RESOLUTION_PRESETS[currentResolutionIndex];
            std::string title = std::string(preset.name) + " (" + std::to_string(preset.width) + "x" + 
                              std::to_string(preset.height) + ") - FPS: " + std::to_string(*fps);
            SDL_SetWindowTitle(window, title.c_str());
            *fps = 0;
        }
    }
    
    void TimePerFrame(Uint64* currentTick) {
        Uint64 deltaTime = SDL_GetTicks() - *currentTick;
        if (deltaTime < TARGETFRAMETIME) {
            SDL_Delay(TARGETFRAMETIME - deltaTime - 1);
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
        while(running) {
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