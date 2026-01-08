#ifndef ANIMATIONS_H
#define ANIMATIONS_H
#include "../entities/player.h"
#include "../states/GameContext.h"
#include "../states/GameContext.h"

typedef uint64_t Uint64;

class Animations {
    public:
        /**
         * Constructor
         */
        Animations();

        /**
         * Animate Player given an animation 
         */
        void AnimatePlayer(Player* player, PlayerStates playerState);

        /**
         * Get firstTime
         */
        bool getFirstTime();

        /**
         * Set firstTime
         */
        void setFirstTime(bool firstTime);

        /**
         * Increment AnimationTime
         */
         void incrementAnimationTime(Uint64 deltaTime);

        /**
         * Set AnimationTime
         */
        void setAnimationTime(Uint64 animationTime);

    private:
        bool firstTime;
        Uint64 animationTime;
};
#endif