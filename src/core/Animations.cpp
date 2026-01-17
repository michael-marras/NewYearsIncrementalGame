#include "Animations.h"
#include <array>

Animations::Animations() {
    this->firstTime     = true;
    this->animationTime = 0;
}
/*
This is what I was mentioning about making a list of the frames for one specific animation group
It just makes it easier because you can go through it and the only diff is which array to use
You don't need a whole new if elif else conditional for each direction
*/
void Animations::AnimatePlayer(Player* player, PlayerStates playerState) {
    if (playerState == PUNCHING) {

        /*
        These could even prob be defined somewhere else
        */
        std::array<PlayerAnimations, 2> frames;
        switch (player->getPlayerDirection()) {
            case Direction::BACK:
                frames = {
                    PlayerAnimations::STANDING_STILL_BACK,
                    PlayerAnimations::STANDING_STILL_BACK_RIGHT_HAND_UP
                };
                break;
            case Direction::FORWARD:
                frames = {
                    PlayerAnimations::STANDING_STILL_FORWARD,
                    PlayerAnimations::STANDING_STILL_FORWARD_LEFT_HAND_UP
                };
                break;
            case Direction::LEFT:
                frames = {
                    PlayerAnimations::STANDING_STILL_LEFT,
                    PlayerAnimations::STANDING_STILL_LEFT_RIGHT_HAND_UP
                };
                break;
            case Direction::RIGHT:
                frames = {
                    PlayerAnimations::STANDING_STILL_RIGHT,
                    PlayerAnimations::STANDING_STILL_RIGHT_LEFT_HAND_UP
                };
                break;
        }
        
        {
            if (this->firstTime) {
                player->setCurrentPlayerAnimation(frames[1]);
                return;
            }
            else if (player->getCurrentPlayerAnimation() == frames[0] && player->getAnimationTime() >= 100) {
                player->setCurrentPlayerAnimation(frames[1]);
                player->setAnimationTime(0);
            }
            else if (player->getCurrentPlayerAnimation() == frames[1] && player->getAnimationTime() >= 100) {
                player->setCurrentPlayerAnimation(frames[0]);
                player->setAnimationTime(0);
            }
            
            this->firstTime = false;
        }
    }
    else if (playerState == WALKING) {

        /*
        These could even prob be defined somewhere else
        */
        std::array<PlayerAnimations, 3> frames;
        switch (player->getPlayerDirection()) {
            case Direction::BACK:
                frames = {
                    PlayerAnimations::WALKING_BACK_RIGHT_FOOT,
                    PlayerAnimations::WALKING_BACK_INTERMEDIARY,
                    PlayerAnimations::WALKING_BACK_LEFT_FOOT,
                };
                break;
            case Direction::FORWARD:
                frames = {
                    PlayerAnimations::WALKING_FORWARD_LEFT_FOOT,
                    PlayerAnimations::WALKING_FORWARD_INTERMEDIARY,
                    PlayerAnimations::WALKING_FORWARD_RIGHT_FOOT
                };
                break;
            case Direction::LEFT:
            case Direction::BACK_LEFT:
            case Direction::FORWARD_LEFT:
                frames = {
                    PlayerAnimations::WALKING_LEFT_LEFT_FOOT,
                    PlayerAnimations::WALKING_LEFT_INTERMEDIARY,
                    PlayerAnimations::WALKING_LEFT_RIGHT_FOOT
                };
                break;
            case Direction::RIGHT:
            case Direction::BACK_RIGHT:
            case Direction::FORWARD_RIGHT:
                frames = {
                    PlayerAnimations::WALKING_RIGHT_RIGHT_FOOT,
                    PlayerAnimations::WALKING_RIGHT_INTERMEDIARY,
                    PlayerAnimations::WALKING_RIGHT_LEFT_FOOT
                };
                break;
        }
        
        {
            if (this->firstTime) {
                player->setCurrentPlayerAnimation(frames[0]);
            }
            else if (player->getCurrentPlayerAnimation() == frames[0] && player->getAnimationTime() >= 100) {
                player->setCurrentPlayerAnimation(frames[1]);
                player->setAnimationTime(0);
            }
            else if (player->getCurrentPlayerAnimation() == frames[1] && player->getAnimationTime() >= 100) {
                player->setCurrentPlayerAnimation(frames[2]);
                player->setAnimationTime(0);
            }
            else if (player->getCurrentPlayerAnimation() == frames[2] && player->getAnimationTime() >= 100) {
                player->setCurrentPlayerAnimation(frames[0]);
                player->setAnimationTime(0);
            }
            
            this->firstTime = false;
        }
    }
    else if (playerState == SWINGING) {
        /*
        These could even prob be defined somewhere else
        */
        std::array<PlayerAnimations, 2> frames;
        switch (player->getPlayerDirection()) {
            case Direction::BACK:
                frames = {
                    PlayerAnimations::SWINGING_BACK_TOOL_UP,
                    PlayerAnimations::SWINGING_BACK_TOOL_DOWN
                };
                break;
            case Direction::FORWARD:
                frames = {
                    PlayerAnimations::SWINGING_FORWARD_TOOL_UP,
                    PlayerAnimations::SWINGING_FORWARD_TOOL_DOWN
                };
                break;
            case Direction::LEFT:
                frames = {
                    PlayerAnimations::SWINGING_LEFT_TOOL_UP,
                    PlayerAnimations::SWINGING_LEFT_TOOL_DOWN
                };
                break;
            case Direction::RIGHT:
                frames = {
                    PlayerAnimations::SWINGING_RIGHT_TOOL_UP,
                    PlayerAnimations::SWINGING_RIGHT_TOOL_DOWN
                };
                break;
        }
        
        {
            if (this->firstTime) {
                player->setCurrentPlayerAnimation(frames[1]);
                return;
            }
            else if (player->getCurrentPlayerAnimation() == frames[0] && player->getAnimationTime() >= 100) {
                player->setCurrentPlayerAnimation(frames[1]);
                player->setAnimationTime(0);
            }
            else if (player->getCurrentPlayerAnimation() == frames[1] && player->getAnimationTime() >= 100) {
                player->setCurrentPlayerAnimation(frames[0]);
                player->setAnimationTime(0);
            }
            
            this->firstTime = false;
        }
    }
}

bool Animations::getFirstTime() {
    return this->firstTime;
}

void Animations::setFirstTime(bool firstTime) {
    this->firstTime = firstTime;
}

void Animations::incrementAnimationTime(Uint64 deltaTime) {
    this->animationTime += deltaTime;
}

void Animations::setAnimationTime(Uint64 animationTime) {
    this->animationTime = animationTime;
}