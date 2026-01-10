#ifndef BASESTATE_H
#define BASESTATE_H
#include<unordered_map>

// Forward declarations
class GameContext;
class InputManager;
class Player;
class HUD;
class Sound;

struct SDL_Renderer;

class BaseState {
    public:
        virtual ~BaseState() = default;

        /**
         * Set the state's dependencies
         */
        void setDependencies(GameContext* ctx, SDL_Renderer* rend, InputManager* inp, Player* plyr, HUD* h) {
            context = ctx;
            renderer = rend;
            inputManager = inp;
            player = plyr;
            hud = h;
        }
        
        /**
         * Handle the state's inputs
         */
        virtual void input() = 0;

        /**
         * The state's update logic
         */
        virtual void update() = 0;

        /**
         * Handle the state's rendering
         */
        virtual void render() = 0;
        
        /**
         * Handle entering the state
         */
        virtual void onEnter() {}

        /**
         * Handle exiting the state
         */
        virtual void onExit() {}

    protected:
        GameContext* context = nullptr;
        SDL_Renderer* renderer = nullptr;
        InputManager* inputManager = nullptr;
        Player* player = nullptr;
        HUD* hud = nullptr;
        Sound* sound;
};


#endif // BASESTATE_H