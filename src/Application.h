#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL3/SDL.h>
#include <string>
#include <memory>

#include "core/input_manager.h"

// Forward declarations
class GameContext;
class Player;
class HUD;
class Inventory;
class BaseState;
class MortalState;
class GodState;

enum class StateType : uint8_t {
    GOD_STATE,
    MORTAL_STATE
};

class Application {
public:
    Application(const char* title);
    ~Application();

    bool Initialize();
    void MainLoop();

private:
    void ChangeState(StateType newStateType);
    void Tick();
    void Input();
    void Update();
    void Render();
    void FPSCount(Uint64* currentTick, Uint64* lastTime, Uint64* fps);
    void TimePerFrame(Uint64* currentTick);

    SDL_Window* window;
    SDL_Renderer* renderer;
    InputManager input;
    std::unique_ptr<GameContext> context;
    Player* player;
    std::unique_ptr<HUD> hud;
    std::unique_ptr<Inventory> inventory;

    std::unique_ptr<MortalState> mortalState;
    std::unique_ptr<GodState> godState;
    BaseState* currentState;
};

#endif // APPLICATION_H
