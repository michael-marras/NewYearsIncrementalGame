#include "player.h"

player:: player() {
    this -> posX = 0;
    this -> posY = 0;
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

void player:: RegisterPlayerAnimationsFromGrid(const char* sheetName, int cols, int rows) {

}

void player:: RegisterPlayerAnimation(PlayerAnimations id, const char* sheetName, int sheetX, int sheetY) {
    frameInfo frame;
    // Create frame info directly
    frame.frameId = id;
    frame.sheetName = sheetName;
    frame.sheetX = sheetX;
    frame.sheetY = sheetY;
    frame.width = 15;
    frame.height = 15;
}