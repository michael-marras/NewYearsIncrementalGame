#ifndef MORTALSTATE_H
#define MORTALSTATE_H

#include "BaseState.h"

class MortalState : public BaseState {
    public:

        MortalState();

        ~MortalState();
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