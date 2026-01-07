#include "Animations.h"

Animations::Animations() {
    this->firstTime     = true;
    this->animationTime = 0;
}

void Animations::AnimatePlayer(Player* player, PlayerStates playerState) {
    if (playerState == PUNCHING && player->getPlayerDirection() == Direction:: BACK) {
        if (this -> firstTime) {
            player->setCurrentPlayerAnimation(PlayerAnimations:: StandingStillBackRightHandUp);
            return;
        }
        else if (player->getCurrentPlayerAnimation() == PlayerAnimations:: StandingStillBack && player->getAnimationTime() >= 100) {
            player->setCurrentPlayerAnimation(PlayerAnimations:: StandingStillBackRightHandUp);

            player->setAnimationTime(0);
        }
        else if (player->getCurrentPlayerAnimation() == PlayerAnimations:: StandingStillBackRightHandUp && player->getAnimationTime() >= 100) {
            player->setCurrentPlayerAnimation(PlayerAnimations:: StandingStillBack);

            player->setAnimationTime(0);
        }

        this->firstTime = false;
    }
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