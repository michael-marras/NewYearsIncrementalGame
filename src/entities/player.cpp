#include "entities/player.h"
#include "world/planet.h"

Player::Player() {
    this->posX = 0.0f;
    this->posY = 0.0f;
    this->currentPlanet = nullptr;
    this->currentAnimation = PlayerAnimations::STANDING_STILL_FORWARD;
    this->playerWalkingStage = WalkingStages:: NOT_WALKING;
    this->playerWalkingStage = WalkingStages:: NOT_WALKING;
    this->animationTime = 0;
    this->playerState=IDLE;
    this->equippedToolId = -1;
    this->isPlayer = false;
}

Player::Player(bool isPlayer) {
    this->posX = 0.0f;
    this->posY = 0.0f;
    this->currentPlanet = nullptr;
    this->currentAnimation = PlayerAnimations::STANDING_STILL_FORWARD;
    this->playerWalkingStage = WalkingStages:: NOT_WALKING;
    this->playerWalkingStage = WalkingStages:: NOT_WALKING;
    this->animationTime = 0;
    this->playerState=IDLE;
    this->equippedToolId = -1;
    this->isPlayer = isPlayer;
}

Player::Player(int posX, int posY) {
    this->posX = (float)posX;
    this->posY = (float)posY;
    this->currentPlanet = nullptr;
    this->equippedToolId = -1;
}

Player::Player(int posX, int posY, bool isPlayer) {
    this->posX = 0.0f;
    this->posY = 0.0f;
    this->currentPlanet = nullptr;
    this->currentAnimation = PlayerAnimations::STANDING_STILL_FORWARD;
    this->playerWalkingStage = WalkingStages:: NOT_WALKING;
    this->playerWalkingStage = WalkingStages:: NOT_WALKING;
    this->animationTime = 0;
    this->playerState=IDLE;
    this->equippedToolId = -1;
    this->isPlayer = isPlayer;
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

frameInfo Player:: getFrame(PlayerAnimations frame) {
    return this->Frames[frame];
}

WalkingStages Player:: getPlayerWalkingStage() {
    return this->playerWalkingStage;
}

void Player:: setPlayerWalkingStage(WalkingStages walkingStage) {
    this->playerWalkingStage = walkingStage;
}

bool Player::HasResource(int resourceId) const {
    auto it = inventory.find(resourceId);
    return it != inventory.end() && it->second > 0;
}

int Player::GetResourceQuantity(int resourceId) const {
    auto it = inventory.find(resourceId);
    return (it != inventory.end()) ? it->second : 0;
}

void Player::AddResource(int resourceId, int quantity) {
    inventory[resourceId] += quantity;
}

Uint64 Player::getAnimationTime() {
    return this->animationTime;
}

void Player::setAnimationTime(int animationTime) {
    this->animationTime = animationTime;
}

PlayerStates Player::getPlayerState() {
    return this->playerState;
}

void Player::setPlayerState(PlayerStates playerState) {
    this->playerState = playerState; 
}

void Player::incrementAnimationTime(Uint64 deltaTime) {
    this->animationTime += deltaTime;
}

std::unordered_map<int, int>* Player::getInventory() {
    return &this->inventory;
}

void Player::ClearInventory() {
    this->inventory.clear();
}

void Player::RemoveResource(int resourceId) {
    this->inventory.erase(resourceId);
}

void Player::ConsumeResource(int resourceId, int quantity) {
    if (quantity <= 0) {
        return;
    }
    auto it = this->inventory.find(resourceId);
    if (it == this->inventory.end()) {
        return;
    }
    if (it->second <= quantity) {
        this->inventory.erase(it);
    } else {
        it->second -= quantity;
    }
}

bool Player::EquipTool(int toolId) {
    // Check if player has the tool
    if (!HasTool(toolId)) {
        return false;
    }
    equippedToolId = toolId;
    return true;
}

void Player::UnequipTool() {
    equippedToolId = -1;
}

void Player::AddTool(int toolId, int quantity) {
    toolInventory[toolId] += quantity;
}

bool Player::HasTool(int toolId) const {
    auto it = toolInventory.find(toolId);
    return it != toolInventory.end() && it->second > 0;
}

int Player::GetToolQuantity(int toolId) const {
    auto it = toolInventory.find(toolId);
    return (it != toolInventory.end()) ? it->second : 0;
}

std::unordered_map<int, int>* Player::getToolInventory() {
    return &this->toolInventory;
}

