#ifndef GODSTATE_H
#define GODSTATE_H

#include "BaseState.h"

class GodState : public BaseState {
    public:

        GodState();

        ~GodState();

        void setDependencies(GameContext* ctx, SDL_Renderer* rend, InputManager* inp, Player* plyr) {
            context = ctx;
            renderer = rend;
            inputManager = inp;
            player = plyr;
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


#endif // GODSTATE_H