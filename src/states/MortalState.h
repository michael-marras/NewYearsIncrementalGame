#ifndef MORTALSTATE_H
#define MORTALSTATE_H

#include "BaseState.h"
#include <cstdint>

typedef uint64_t Uint64;

class MortalState : public BaseState {
    private:
        Uint64 lastMiningTime = 0;
        
    public:

        MortalState();

        ~MortalState();

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