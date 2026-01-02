#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <unordered_map>

// Forward Declarations
class TextureManager;

// animations
enum class PlayerAnimations: uint8_t {
    // Standing Still
    StandingStillForward,
    StandingStillBackward,
    StandingStillLeft,
    StandingStillRight,

    // Walking Forward
    WalkingForwardLeftFoot,
    WalkingForwardRightFoot,
    
    // Walking Back
    WalkingBackLeftFoot,
    WalkingBackRightFoot,
    
    // Walking Left
    WalkingLeftRightFoot,
    WalkingLeftLeftFoot,

    // Walking Right
    WalkingRightLeftFoot,
    WalkingRightRightFoot,

    //Count of Player Animations
    PlayerAnimationsCount
};

// Frame information structure
struct frameInfo {
    PlayerAnimations frameId;   // Unique frame id
    std::string sheetName;     // Which sprite sheet its in
    int sheetX;                // X position in sprite sheet (in pixels)
    int sheetY;                // Y position in sprite sheet (in pixels)
    int width;                 // Tile width
    int height; 
};

class player {
    private:
        // Position
        int posX;
        int posY;

        // Player Animation
        PlayerAnimations currentAnimation;
        int curretnAnimationDepth = 0;

        // Frames
        std::unordered_map<PlayerAnimations, frameInfo> Frames;

    public:
        /**
         * Default Constructor
         */
        player();

        /**
         * Constructor
         */
        player(int posX, int posY);

        /**
         * Set player X position
         */
        void setX(int X);

        /**
         * Get player X position
         */
        int getX();
        
        /**
         * Set player Y position
         */
        void setY(int Y);

        /**
         * Get player Y position
         */
        int getY();

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
};
#endif