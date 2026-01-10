#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <tuple>

// Forward declarations
class TextureManager;
class TileManager;
class ObjectManager;
class ResourceManager;
class ToolManager;
class Camera;
class Player;
class Planet;
class PlanetTree;

class GameContext {
    private: 
        // Rendering infrastructure
        SDL_Window* window;
        SDL_Renderer* renderer;
        // Managers
        TextureManager* textureManager;
        TileManager* tileManager;
        ObjectManager* objectManager;
        ResourceManager* resourceManager;
        ToolManager* toolManager;
        Camera* camera;
        // Display/Rendering
        int currentResolutionIndex = 5;
        Uint64 DeltaTime = 0;
        Uint64 lastFrameTime = 0;

        // Game World
        bool Running = true;
        int map;
        int objectMap;
        int resourceArray;
        Player* player;
        Planet* currentPlanet;
        int currentPlanetId = 0;
        int currentPlanetFace = 0;
        Uint64 faceTransitionCooldown = 0;
        PlanetTree* planetTree;
        unsigned int rootPlanetSeed = 0;  // Seed used for root planet generation
        // Entities
            // TODO
        // UI
            // TODO

    public:
        /**
         * Constructor 
         */
        GameContext();
        
        /**
         * Destructor - cleans up managers
         */
        ~GameContext();
        
        /**
         * Initialize all managers and rendering infrastructure
         * 
         * @param window SDL window
         * @param renderer SDL renderer for TextureManager
         */
        void InitializeManagers(SDL_Window* window, SDL_Renderer* renderer);

        /**
         * Reset Game World to start
         */
        void ResetGameWorld();

        /**
         * Update Mouse Position Coordinates
         */
        void UpdateMouse();

        /** 
         * Changes resolution to a different presest
         *
         * @param newIndex resolution preset index
         */
        void ChangeResolution(int newIndex);

        /**
         * gets currentResolutionIndex
         */
        int getCurrentResolutionIndex();

        /**
         * Changes Running memeber vairable to false
         */
        void Quit();

        /**
         * Returns Running memeber variable
         */
        bool isRunning();

        /**
         * Set Delta Time in a tick
         *
         * @param currentTick the current tick
         */
        void setDeltaTime(Uint64* currentTick);

        /**
         * Get Delta Time
         */
        Uint64 getDeltaTime();
        
        /**
         * Get Texture Manager
         */
        TextureManager* getTextureManager();
        
        /**
         * Get Tile Manager
         */
        TileManager* getTileManager();
        
        /**
         * Get Object Manager
         */
        ObjectManager* getObjectManager();

        /**
         * Get Resource Manager
         */
         ResourceManager* getResourceManager();
        
        /**
         * Get Tool Manager
         */
        ToolManager* getToolManager();
        
        /**
         * Get Camera
         */
        Camera* getCamera();
        
        /**
         * Set Texture Manager
         */
        void setTextureManager(TextureManager* manager);
        
        /**
         * Set Tile Manager
         */
        void setTileManager(TileManager* manager);
        
        /**
         * Set Object Manager
         */
        void setObjectManager(ObjectManager* manager);

        /**
         * Set Resource Manager
         */
         void setResourceManager(ResourceManager* manager);
        
        /**
         * Set Tool Manager
         */
        void setToolManager(ToolManager* manager);
        
        /**
         * Set Camera
         */
        void setCamera(Camera* camera);
        /**
         * Get current map ID (tile grid)
         */
        int getMap() const;
        
        /**
         * Set current map ID (tile grid)
         */
        void setMap(int mapId);
        
        /**
         * Get current object map ID (object grid)
         */
        int getObjectMap() const;
        
        /**
         * Set current object map ID (object grid)
         */
        void setObjectMap(int objectMapId);

        /**
         * Get current resource array
         */
         int getResourceArray() const;
        
         /**
          * Set array object map id
          */
         void setResourceArray(int arrayMapId);

        /**
         * Get Player Pointer
         */ 
        Player* getPlayer();
        
        /**
         * Get current planet
         */
        Planet* getCurrentPlanet() const;
        
        /**
         * Set current planet
         */
        void setCurrentPlanet(Planet* planet);
        
        /**
         * Get current planet face (0=FRONT, 1=BACK, 2=LEFT, 3=RIGHT, 4=TOP, 5=BOTTOM)
         */
        int getCurrentPlanetFace() const;
        
        /**
         * Set current planet face (0=FRONT, 1=BACK, 2=LEFT, 3=RIGHT, 4=TOP, 5=BOTTOM)
         */
        void setCurrentPlanetFace(int face);
        
        /**
         * Update context maps to use current planet's current face
         */
        void updateMapsFromPlanet();
        
        /**
         * Check if player is off edge and handle face transition
         * Returns true if a transition occurred
         */
        bool checkAndHandleFaceTransition(Player* player);
        
        /**
         * Update face transition cooldown (call each frame)
         */
        void updateFaceTransitionCooldown();
        
        /**
         * Check if face transition cooldown is active
         * Returns true if player should not be able to move yet
         */
        bool isFaceTransitionCooldownActive() const;
        
        /**
         * TEMPORARY: Generate 50 planets in the planet tree
         * Call this after InitializeManagers
         */
        void GeneratePlanetTree();
        
        /**
         * Get the planet tree
         */
        PlanetTree* getPlanetTree() const;
        
        /**
         * Get current planet ID
         */
        int getCurrentPlanetId() const;
        
        /**
         * Set current planet by ID (finds planet in tree and sets it)
         */
        bool setCurrentPlanetById(int planetId);
        
        /**
         * Generate a new planet as a child of the specified parent planet
         * Consumes energy from parent planet if successful
         * 
         * @param parentId ID of the parent planet in the tree
         */
        void GeneratePlanetInTree(int parentId);
        
        /**
         * Get player's universe X position (local position + planet's universe position)
         */
        float GetPlayerUniverseX() const;
        
        /**
         * Get player's universe Y position (local position + planet's universe position)
         */
        float GetPlayerUniverseY() const;
        
        /**
         * Get the seed used for root planet generation (for starfield generation)
         */
        unsigned int GetRootPlanetSeed() const { return rootPlanetSeed; }
};
#endif // GAMECONTEXT_H