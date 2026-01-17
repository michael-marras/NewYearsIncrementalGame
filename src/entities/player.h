#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <unordered_map>
#include <cstdint>
#include <vector>

// Forward Declarations
class TextureManager;
class Planet;

typedef uint64_t Uint64;

enum PlayerStates: uint8_t {
    IDLE,
    WALKING,
    PUNCHING,
    SWINGING
};

// animations
enum class PlayerAnimations: uint8_t {
    STANDING_STILL_FORWARD,
    WALKING_FORWARD_LEFT_FOOT,
    WALKING_FORWARD_INTERMEDIARY,
    WALKING_FORWARD_RIGHT_FOOT,
    WALKING_BACK_RIGHT_FOOT,
    WALKING_BACK_INTERMEDIARY,
    WALKING_BACK_LEFT_FOOT,
    STANDING_STILL_BACK,
    WALKING_LEFT_LEFT_FOOT,
    WALKING_LEFT_INTERMEDIARY,
    WALKING_LEFT_RIGHT_FOOT,
    STANDING_STILL_LEFT,
    WALKING_RIGHT_RIGHT_FOOT,
    WALKING_RIGHT_INTERMEDIARY,
    WALKING_RIGHT_LEFT_FOOT,
    STANDING_STILL_RIGHT,
    STANDING_STILL_FORWARD_LEFT_HAND_UP,
    STANDING_STILL_LEFT_RIGHT_HAND_UP,
    STANDING_STILL_RIGHT_LEFT_HAND_UP,
    STANDING_STILL_BACK_RIGHT_HAND_UP,
    SWINGING_FORWARD_TOOL_UP,
    SWINGING_FORWARD_TOOL_DOWN,
    SWINGING_BACK_TOOL_UP,
    SWINGING_BACK_TOOL_DOWN,
    SWINGING_RIGHT_TOOL_UP,
    SWINGING_RIGHT_TOOL_DOWN,
    SWINGING_LEFT_TOOL_UP,
    SWINGING_LEFT_TOOL_DOWN,

    //Count of Player Animations
    PlayerAnimationsCount
};

/* This was my idea for a way to store a bit simpler, but you don't have to do it this way (sudo code)

playerAnimation = { <- map
    WALKING : { <- map
        Front: Array[3] <- these would be arrays with each frame, so like the 3 forward walking frames
        BACK: Array[3]
        LEFT: Array[3]
        RIGHT: Array[3]
        }
}

That way when you want to do the player walking forward animation you can get it by playerAnimation.WALKING.FRONT
and it gives you the 3 frames for it instead of having to type out all 3 frame enums

*/

enum class Direction: uint8_t {
    FORWARD,
    BACK,
    LEFT,
    RIGHT,
    FORWARD_LEFT,
    FORWARD_RIGHT,
    BACK_LEFT,
    BACK_RIGHT
};

enum class WalkingStages : uint8_t {
    NOT_WALKING, 
    LEFT_FOOT, 
    RIGHT_FOOT, 
    INTERMEDIARY
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

class Player {

    public:
        /**
         * Default Constructor
         */
        Player();

        /**
         * Constructor
         */
        Player(bool isPlayer);

        /**
         * Constructor
         */
        Player(int posX, int posY);

        /**
         * Constructor
         */
        Player(int posX, int posY, bool isPlayer);
        
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
            int startX = 0, int startY = 0, PlayerAnimations startId = PlayerAnimations::STANDING_STILL_FORWARD);

        /**
         *
         */
        void RegisterPlayerAnimation(PlayerAnimations id, const char* sheetName, int sheetX, int sheetY); 

        /**
         * get frameInfo structure from Frames
         */
        frameInfo getFrame(PlayerAnimations frame);

        /**
         * Get playerWalkingStage
         */
        WalkingStages getPlayerWalkingStage();

        /**
         * Set playerWalkingStage
         */
        void setPlayerWalkingStage(WalkingStages walkingStage);

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

        /**
         * Get playerIdlePuncingDeltaTime
         */
        Uint64 getAnimationTime();

        /**
        * Set playerIdlePuncingDeltaTime
        */
        void setAnimationTime(int animationTime);

        /**
         * Get playerState
         */
        PlayerStates getPlayerState();

        /**
         * Set playerState
         */
        void setPlayerState(PlayerStates playerState);

        /**
         * Increment animationTime
         */
        void incrementAnimationTime(Uint64 deltaTime);

        /**
         * Get pointer to player's inventory (resourceId -> quantity)
         * Can be used to access or clear inventory
         */
        std::unordered_map<int, int>* getInventory();
        
        /**
         * Clear all resources from player's inventory
         */
        void ClearInventory();
        
        /**
         * Remove all of a specific resource type from inventory
         * 
         * @param resourceId The resource ID to remove
         */
        void RemoveResource(int resourceId);

        /**
         * Consume a specific quantity of a resource.
         * If quantity exceeds what's available, removes the entry.
         *
         * @param resourceId The resource ID to consume
         * @param quantity The amount to consume (>= 0)
         */
        void ConsumeResource(int resourceId, int quantity);
        
        /**
         * Get the planet the player is currently on
         */
        Planet* GetPlanet() const { return currentPlanet; }
        
        /**
         * Set the planet the player is currently on
         */
        void SetPlanet(Planet* planet) { currentPlanet = planet; }
        
        /**
         * Equip a tool by ID (tool must be in inventory)
         * @param toolId The tool ID to equip
         * @return true if tool was equipped, false if tool not in inventory
         */
        bool EquipTool(int toolId);
        
        /**
         * Unequip the currently equipped tool
         */
        void UnequipTool();
        
        /**
         * Get the ID of the currently equipped tool (-1 if none)
         */
        int GetEquippedToolId() const { return equippedToolId; }
        
        /**
         * Add a tool to inventory
         * @param toolId The tool ID to add
         * @param quantity The quantity to add (usually 1 for tools)
         */
        void AddTool(int toolId, int quantity = 1);
        
        /**
         * Check if player has a tool in inventory
         * @param toolId The tool ID to check
         * @return true if player has the tool
         */
        bool HasTool(int toolId) const;
        
        /**
         * Get tool quantity in inventory
         * @param toolId The tool ID to check
         * @return Quantity of tool in inventory
         */
        int GetToolQuantity(int toolId) const;
        
        /**
         * Get pointer to player's tool inventory (toolId -> quantity)
         */
        std::unordered_map<int, int>* getToolInventory();
        
        /**
         * Get the player's reach/interact range
         * @return Reach range in pixels
         */
        float GetReach() const { return reach; }
        
        /**
         * Set the player's reach/interact range
         * @param range Reach range in pixels
         */
        void SetReach(float range) { reach = range; }
        
        /**
         * Get the player's resource pickup range
         * @return Pickup range in pixels
         */
        float GetPickupRange() const { return pickupRange; }
        
        /**
         * Set the player's resource pickup range
         * @param range Pickup range in pixels
         */
        void SetPickupRange(float range) { pickupRange = range; }
        
    private:
        // Position
        float posX;
        float posY;
        
        // Planet reference (which planet the player is currently on)
        Planet* currentPlanet = nullptr;

        // Player Animation
        PlayerAnimations currentAnimation;
        Direction playerDirection;
        PlayerStates playerState;
        WalkingStages playerWalkingStage;
        Uint64 animationTime;

        // Frames
        std::unordered_map<PlayerAnimations, frameInfo> Frames;

        // Inventory (resourceId -> quantity)
        std::unordered_map<int, int> inventory;
        
        // Tool inventory (toolId -> quantity)
        std::unordered_map<int, int> toolInventory;
        
        // Currently equipped tool ID (-1 if none)
        int equippedToolId = -1;
        
        // Interaction ranges (upgradeable)
        float reach = 20.0f;
        float pickupRange = 20.0f;
        
        // distinguish between playable and non playable characters
        bool isPlayer;

};
#endif