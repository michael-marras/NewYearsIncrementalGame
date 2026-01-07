#include <SDL3/SDL.h>
#include <string>
#include <memory>
#include "core/textures.h"
#include "world/tiles.h"
#include "world/objects.h"
#include "definitions/tile_definitions.h"
#include "definitions/frame_definitions.h"
#include "definitions/object_definitions.h"
#include "definitions/resource_definitions.h"
#include "utils/constants.h"
#include "core/camera.h"
#include "states/GameContext.h"
#include "core/input_manager.h"
#include "entities/player.h"
#include "items/resources.h"
#include "ui/hud.h"
#include "world/planet.h"
#include "core/Animations.h"
#include "states/BaseState.h"
#include "states/MortalState.h"
#include <vector>

enum class StateType : uint8_t {
    GOD_STATE,
    MORTAL_STATE
};

struct SDLApplication {
    SDL_Window* window;
    SDL_Renderer* renderer;
    InputManager input;
    //to run indefinitely
    std::unique_ptr<GameContext> context = std::make_unique<GameContext>();
    Player* player = context -> getPlayer();
    HUD* hud = nullptr;

    // State management
    std::unique_ptr<MortalState> mortalState;
    
    BaseState* currentState = nullptr;

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
        
        // Initialize states
        mortalState = std::make_unique<MortalState>();
        mortalState->setDependencies(context.get(), renderer, &input, player, hud);
        // TODO: Initialize GodState when implemented
        
        // Set initial state
        ChangeState(StateType::MORTAL_STATE);
        
        return true;
    }
    
    void ChangeState(StateType newStateType) {
        // Exit current state
        if (currentState) {
            currentState->onExit();
        }
        
        // Switch to new state
        switch (newStateType) {
            case StateType::MORTAL_STATE:
                currentState = mortalState.get();
                break;
            case StateType::GOD_STATE:
                // TODO: Set to godState when implemented
                // currentState = godState.get();
                SDL_Log("GodState not yet implemented");
                return;
        }
        
        // Enter new state
        if (currentState) {
            currentState->onEnter();
        }
    }

    //Handle input events from I/O or networking devices
    void Tick(){
        Input();
        Update();
        Render();
        input.EndFrame();
    }
    
    void Input() {
        if (currentState) {
            currentState->input();
        }
    }

    void Update() {
        if (currentState) {
            currentState->update();
        }
    }
    

    void Render() {
        if (currentState) {
            currentState->render();
        }
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
        Uint64 fps = 0;
        Uint64 lastTime = 0;

        //inf loop
        while(context -> isRunning()) {
            Uint64 currentTick = SDL_GetTicks();
            Tick();
            fps++;
            
            TimePerFrame(&currentTick);

            FPSCount(&currentTick, &lastTime, &fps);
        }
    }          
};

// Entry Point
int main() {
    SDLApplication app("poopie");

    if (!app.Initialize()) {
        SDL_Log("Failed to initialize application!");
        return 1;
    }

    app.MainLoop();
    return 0;
}