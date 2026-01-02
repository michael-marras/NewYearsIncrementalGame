#include "player.h"

player:: player() {
    this -> posX = 800;
    this -> posY = 800;
    this -> currentAnimation = PlayerAnimations:: StandingStillForward;
}

player:: player(int posX, int posY) {
    this -> posX = posX;
    this -> posY = posY;
}

void player:: setX(int X) {
    this -> posX = X;
}

int player:: getX() {
    return this -> posX;
}

void player:: setY(int Y) {
    this -> posY = Y;
}

int player:: getY() {
    return this -> posY;
}

PlayerAnimations player::getCurrentPlayerAnimation() {
    return this -> currentAnimation;
}

PlayerAnimations player:: RegisterPlayerAnimationsFromGrid(const char* sheetName, int cols, int rows, 
    int startX, int startY, PlayerAnimations startId) {
    PlayerAnimations animationId = startId;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int x = startX + (col * 15);  // 15 = TILE_SIZE
            int y = startY + (row * 15);
            RegisterPlayerAnimation(animationId, sheetName, x, y);
            animationId = static_cast<PlayerAnimations>(static_cast<int>(animationId) + 1);
        }
    }
    return animationId;
}

void player:: RegisterPlayerAnimation(PlayerAnimations id, const char* sheetName, int sheetX, int sheetY) {
    // Create frame info directly
    frameInfo frame;
    frame.frameId = id;
    frame.sheetName = sheetName;
    frame.sheetX = sheetX;
    frame.sheetY = sheetY;
    frame.width = 15;
    frame.height = 15;

    Frames[id] = frame;
}



