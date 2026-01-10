#ifndef MORTALSTATE_H
#define MORTALSTATE_H

#include "BaseState.h"
#include <cstdint>

typedef uint64_t Uint64;

class Inventory;
class DamagePopupManager;
class EngineCompass;

class MortalState : public BaseState {
    private:
        Uint64 lastMiningTime = 0;
        Inventory* inventory = nullptr;
        DamagePopupManager* damagePopups = nullptr;
        EngineCompass* engineCompass = nullptr;
        
    public:

        MortalState();

        ~MortalState();

        void setDependencies(GameContext* ctx, SDL_Renderer* rend, InputManager* inp, Player* plyr, HUD* h, Inventory* inv) {
            context = ctx;
            renderer = rend;
            inputManager = inp;
            player = plyr;
            hud = h;
            inventory = inv;
        }

        /**
         * Handle the state's inputs
         */
        void input();

         /**
          * The state's update logic
          */
        void update();
 
         /**
          * Handle the state's rendering
          */
        void render();
         
         /**
          * Handle entering the state
          */
        void onEnter();
 
         /**
          * Handle exiting the state
          */
        void onExit();
};


#endif // MORTALSTATE_H