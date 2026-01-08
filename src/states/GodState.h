#ifndef GODSTATE_H
#define GODSTATE_H

#include "BaseState.h"
#include <vector>
#include <utility>

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
        
    private:
        // Starfield data
        std::vector<std::pair<float, float>> stars;
        bool starsGenerated = false;
        
        /**
         * Generate starfield using root planet seed
         */
        void GenerateStarfield(unsigned int seed, int numStars = 1000);
};


#endif // GODSTATE_H