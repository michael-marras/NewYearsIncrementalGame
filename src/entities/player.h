#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <unordered_map>
#include <cstdint>

// Forward Declarations
class TextureManager;

// animations
enum class PlayerAnimations: uint8_t {
    StandingStillForward,
    WalkingForwardLeftFoot,
    WalkingForwardIntermediary,
    WalkingForwardRightFoot,
    WalkingBackRightFoot,
    WalkingBackIntermediary,
    WalkingBackLeftFoot,
    StandingStillBack,
    WalkingLeftLeftFoot,
    WalkingLeftIntermediary,
    WalkingLeftRightFoot,
    StandingStillLeft,
    WalkingRightRightFoot,
    WalkingRightIntermediary,
    WalkingRightLeftFoot,
    StandingStillRight,
    StandingStillForwardLeftHandUp,
    StandingStillLeftRightHandUp,
    StandingStillRightLeftHandUp,
    StandingStillBackRightHandUp,

    //Count of Player Animations
    PlayerAnimationsCount
};

enum class Direction: uint8_t {LEFT, RIGHT, FORWARD, BACK};

enum class WalkingStages : uint8_t {NOT_WALKING, LEFT_FOOT, RIGHT_FOOT, INTERMEDIARY};

enum class WalkingStages : uint8_t {NOT_WALKING, LEFT_FOOT, RIGHT_FOOT, INTERMEDIARY};

// Frame information structure
struct frameInfo {
    PlayerAnimations frameId;   // Unique frame id
    std::string sheetName;     // Which sprite sheet its in
    int sheetX;                // X position in sprite sheet (in pixels)
    int sheetY;                // Y position in sprite sheet (in pixels)
    int width;                 // Tile width
    int height; 
};

class Player {
    private:
        // Position
        float posX;
        float posY;

        // Player Animation
        PlayerAnimations currentAnimation;
        Direction playerDirection;
        WalkingStages playerWalkingStage;
        WalkingStages playerWalkingStage;

        // Frames
        std::unordered_map<PlayerAnimations, frameInfo> Frames;

        // Inventory (resourceId -> quantity)
        std::unordered_map<int, int> inventory;

    public:
        /**
         * Default Constructor
         */
        Player();

        /**
         * Constructor
         */
        Player(int posX, int posY);

        /**
         * Set player X position
         */
        void setX(float X);

        /**
         * Get player X position
         */
        float getX();
        
        /**
         * Set player Y position
         */
        void setY(float Y);

        /**
         * Get player Y position
         */
        float getY();

        /**
         * Combined method to change both x and y
         */
        void move(float dx, float dy);

        /**
         * Get Player Direction
         */
        Direction getPlayerDirection();

        /**
         * Set Player Direction
         *
         * @param playerDirection last direction player was facing
         */
        void setPlayerDirection(Direction playerDirection);

        /**
         * Set the current player animation
         */
        void setCurrentPlayerAnimation(PlayerAnimations animation);

        /** 
         * Get current Player Animation
         */
        PlayerAnimations getCurrentPlayerAnimation();
        
        /**
         *  Register the player animation from the sprite sheet
         */
        PlayerAnimations RegisterPlayerAnimationsFromGrid(const char* sheetName, int cols, int rows, 
            int startX = 0, int startY = 0, PlayerAnimations startId = PlayerAnimations::StandingStillForward);

        /**
         *
         */
        void RegisterPlayerAnimation(PlayerAnimations id, const char* sheetName, int sheetX, int sheetY); 

        /**
         * get frameInfo structure from Frames
         */
        frameInfo getFrame(PlayerAnimations frame) {
            return this -> Frames[frame];
        }

        /**
         * check if player has a resource
         */
        bool HasResource(int resourceId) const;

        /**
         * check how much of a resource the player has
         */
        int GetResourceQuantity(int resourceId) const;

        /**
         * add resources into the player's inventory, handles lazy init
         */
        void AddResource(int resourceId, int quantity);
};
#endif