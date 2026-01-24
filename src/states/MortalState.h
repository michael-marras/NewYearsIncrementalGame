#ifndef MORTALSTATE_H
#define MORTALSTATE_H

#include "BaseState.h"
#include <cstdint>
#include <memory>
#include "../utils/sound.h"
#include "../ui/inventory.h"

typedef uint64_t Uint64;

class DamagePopupManager;
class EngineCompass;

class MortalState : public BaseState {
    private:
        Uint64 lastMiningTime = 0;
        std::unique_ptr<Inventory> inventory;
        DamagePopupManager* damagePopups = nullptr;
        EngineCompass* engineCompass = nullptr;
        
    public:

        MortalState();

        ~MortalState();

        void setDependencies(GameContext* ctx, SDL_Renderer* rend, InputManager* inp, Player* plyr, std::unique_ptr<HUD> h, std::unique_ptr<Inventory> inv, std::unique_ptr<Sound> snd) {
            context = ctx;
            renderer = rend;
            inputManager = inp;
            player = plyr;
            hud = std::move(h);
            inventory = std::move(inv);
            sound = std::move(snd);
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