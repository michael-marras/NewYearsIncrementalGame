#include "Application.h"
#include "core/textures.h"
#include "ui/text_renderer.h"
#include "world/tiles.h"
#include "world/objects.h"
#include "definitions/tile_definitions.h"
#include "definitions/frame_definitions.h"
#include "definitions/object_definitions.h"
#include "definitions/resource_definitions.h"
#include "definitions/tool_definitions.h"
#include "utils/constants.h"
#include "core/camera.h"
#include "states/GameContext.h"
#include "core/input_manager.h"
#include "entities/player.h"
#include "items/resources.h"
#include "items/tools.h"
#include "ui/hud.h"
#include "ui/inventory.h"
#include "world/planet.h"
#include "core/Animations.h"
#include "states/BaseState.h"
#include "states/MortalState.h"
#include "states/GodState.h"
#include "utils/sound.h"

Application::Application(const char* title)
    : window(nullptr),
      renderer(nullptr),
      player(nullptr),
      currentState(nullptr) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    context = std::make_unique<GameContext>();
    player = context->getPlayer();
    const ResolutionPreset& initialPreset = RESOLUTION_PRESETS[context->getCurrentResolutionIndex()];
    window = SDL_CreateWindow(title, initialPreset.width, initialPreset.height, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, NULL);

    SDL_SetRenderLogicalPresentation(renderer, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    std::string windowTitle = std::string(title) + " - " + initialPreset.name +
                             " (" + std::to_string(initialPreset.width) + "x" +
                             std::to_string(initialPreset.height) + ")";
    SDL_SetWindowTitle(window, windowTitle.c_str());

    context->InitializeManagers(window, renderer);

    hud = std::make_unique<HUD>(renderer);
    inventory = std::make_unique<Inventory>(renderer);
}

Application::~Application() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool Application::Initialize() {
    TextureManager* textureManager = context->getTextureManager();
    TextRenderer* textRenderer = context->getTextRenderer();
    TileManager* tileManager = context->getTileManager();
    ObjectManager* objectManager = context->getObjectManager();
    ResourceManager* resourceManager = context->getResourceManager();
    ToolManager* toolManager = context->getToolManager();
    player = context->getPlayer();
    Camera* camera = context->getCamera();
    std::unique_ptr<Sound> sound = std::make_unique<Sound>("audio/coin-collect-retro-8-bit-sound-effect-145251.wav");
    sound->SetupDevice();

    SetupTiles(tileManager, textureManager);
    SetupResources(resourceManager, textureManager);
    SetupObjects(objectManager, textureManager, resourceManager);
    SetupTools(toolManager, textureManager);
    SetupAnimations(player, textureManager);

    textureManager->LoadImageFromRes("crosshairs", "ui/crosshairs.png");
    textRenderer->LoadFont("gamefont");

    if (player) {
        player->AddTool(7, 1);
        player->AddTool(8, 1);
        player->EquipTool(7);

        if (resourceManager) {
            std::srand(static_cast<unsigned int>(SDL_GetTicks()));
            for (int resourceId = 1; resourceId <= 13; resourceId++) {
                if (resourceManager->HasResource(resourceId)) {
                    int randomQuantity = (std::rand() % 10000) + 1;
                    player->AddResource(resourceId, randomQuantity);
                    ResourceInfo* info = resourceManager->GetResource(resourceId);
                    if (info) {
                        SDL_Log("TEMP: Added %d x %s to player inventory", randomQuantity, info->name.c_str());
                    }
                }
            }
            SDL_Log("TEMP: Finished adding random resources to player inventory");
        }
    }

    context->GeneratePlanetTree();
    context->setCurrentPlanetById(0);
    context->setCurrentPlanetFace(0);

    mortalState = std::make_unique<MortalState>();
    mortalState->setDependencies(context.get(), renderer, &input, player, std::move(hud), std::move(inventory), std::move(sound));

    godState = std::make_unique<GodState>();
    godState->setDependencies(context.get(), renderer, &input, player);

    ChangeState(StateType::MORTAL_STATE);

    return true;
}

void Application::ChangeState(StateType newStateType) {
    if (currentState) {
        currentState->onExit();
    }

    switch (newStateType) {
        case StateType::MORTAL_STATE:
            currentState = mortalState.get();
            break;
        case StateType::GOD_STATE:
            currentState = godState.get();
            break;
    }

    if (currentState) {
        currentState->onEnter();
    }
}

void Application::Tick() {
    Input();
    Update();
    Render();
    input.EndFrame();
}

void Application::Input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            context->Quit();
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            input.ProcessKeyDown(event.key.key);

            switch (event.key.key) {
                case SDLK_ESCAPE:
                    context->Quit();
                    break;
                case SDLK_G:
                    if (currentState == mortalState.get()) {
                        ChangeState(StateType::GOD_STATE);
                    } else if (currentState == godState.get()) {
                        ChangeState(StateType::MORTAL_STATE);
                    }
                    break;
            }
        } else if (event.type == SDL_EVENT_KEY_UP) {
            input.ProcessKeyUp(event.key.key);
        } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
            input.ProcessMouseMotion(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            input.ProcessMouseButtonDown(event.button.button);
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            input.ProcessMouseButtonUp(event.button.button);
        } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            input.ProcessMouseWheel(event.wheel.y);
        }
    }

    if (currentState) {
        currentState->input();
    }
}

void Application::Update() {
    if (currentState) {
        currentState->update();
    }
}

void Application::Render() {
    if (currentState) {
        currentState->render();
    }
}

void Application::FPSCount(Uint64* currentTick, Uint64* lastTime, Uint64* fps) {
    if (*currentTick > *lastTime + 1000) {
        *lastTime = *currentTick;
        const ResolutionPreset& preset = RESOLUTION_PRESETS[context->getCurrentResolutionIndex()];
        std::string title = std::string(preset.name) + " (" + std::to_string(preset.width) + "x" +
                           std::to_string(preset.height) + ") - FPS: " + std::to_string(*fps);
        SDL_SetWindowTitle(window, title.c_str());
        *fps = 0;
    }
}

void Application::TimePerFrame(Uint64* currentTick) {
    context->setDeltaTime(currentTick);
    if (context->getDeltaTime() < TARGETFRAMETIME) {
        SDL_Delay(TARGETFRAMETIME - context->getDeltaTime() - 1);
        while (SDL_GetTicks() - *currentTick < TARGETFRAMETIME) {
        }
    }
}

void Application::MainLoop() {
    Uint64 fps = 0;
    Uint64 lastTime = 0;

    while (context->isRunning()) {
        Uint64 currentTick = SDL_GetTicks();
        Tick();
        fps++;
        TimePerFrame(&currentTick);
        FPSCount(&currentTick, &lastTime, &fps);
    }
}
