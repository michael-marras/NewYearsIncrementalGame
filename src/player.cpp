#include "player.h"

Player::Player() {
    this->posX = 800.0f;
    this->posY = 800.0f;
    this->currentAnimation = PlayerAnimations:: StandingStillForward;
}

Player::Player(int posX, int posY) {
    this->posX = (float)posX;
    this->posY = (float)posY;
}

void Player::setX(float X) {
    this->posX = X;
}

float Player::getX() {
    return this->posX;
}

void Player::setY(float Y) {
    this->posY = Y;
}

float Player::getY() {
    return this->posY;
}

void Player::move(float dx, float dy) {
    this->posX += dx;
    this->posY += dy;
}

Direction Player::getPlayerDirection() {
    return this->playerDirection;
}

void Player::setPlayerDirection(Direction playerDirection) {
    this->playerDirection = playerDirection;
}

void Player::setCurrentPlayerAnimation(PlayerAnimations animation) {
    this->currentAnimation = animation;
}

PlayerAnimations Player::getCurrentPlayerAnimation() {
    return this->currentAnimation;
}

PlayerAnimations Player:: RegisterPlayerAnimationsFromGrid(const char* sheetName, int cols, int rows, 
    int startX, int startY, PlayerAnimations startId) {
    PlayerAnimations animationId = startId;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int x = startX + (col * 16);  // 16a = TILE_SIZE
            int y = startY + (row * 16);
            RegisterPlayerAnimation(animationId, sheetName, x, y);
            animationId = static_cast<PlayerAnimations>(static_cast<int>(animationId) + 1);
        }
    }
    return animationId;
}

void Player:: RegisterPlayerAnimation(PlayerAnimations id, const char* sheetName, int sheetX, int sheetY) {
    // Create frame info directly
    frameInfo frame;
    frame.frameId = id;
    frame.sheetName = sheetName;
    frame.sheetX = sheetX;
    frame.sheetY = sheetY;
    frame.width = 16;
    frame.height = 16;

    Frames[id] = frame;
}



