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
                    PlayerAnimations::StandingStillBack,
                    PlayerAnimations::StandingStillBackRightHandUp
                };
                break;
            case Direction::FORWARD:
                frames = {
                    PlayerAnimations::StandingStillForward,
                    PlayerAnimations::StandingStillForwardLeftHandUp
                };
                break;
            case Direction::LEFT:
                frames = {
                    PlayerAnimations::StandingStillLeft,
                    PlayerAnimations::StandingStillLeftRightHandUp
                };
                break;
            case Direction::RIGHT:
                frames = {
                    PlayerAnimations::StandingStillRight,
                    PlayerAnimations::StandingStillRightLeftHandUp
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
                    PlayerAnimations:: WalkingBackRightFoot,
                    PlayerAnimations:: WalkingBackIntermediary,
                    PlayerAnimations:: WalkingBackLeftFoot,
                };
                break;
            case Direction::FORWARD:
                frames = {
                    PlayerAnimations:: WalkingForwardLeftFoot,
                    PlayerAnimations:: WalkingForwardIntermediary,
                    PlayerAnimations:: WalkingForwardRightFoot
                };
                break;
            case Direction::LEFT:
                frames = {
                    PlayerAnimations:: WalkingLeftLeftFoot,
                    PlayerAnimations:: WalkingLeftIntermediary,
                    PlayerAnimations:: WalkingLeftRightFoot
                };
                break;
            case Direction::RIGHT:
                frames = {
                    PlayerAnimations:: WalkingRightRightFoot,
                    PlayerAnimations:: WalkingRightIntermediary,
                    PlayerAnimations:: WalkingRightLeftFoot
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