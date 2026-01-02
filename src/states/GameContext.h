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
class Camera;

class GameContext {
    private: 
        // Rendering infrastructure
        SDL_Window* window;
        SDL_Renderer* renderer;
        // Managers
        TextureManager* textureManager;
        TileManager* tileManager;
        ObjectManager* objectManager;
        Camera* camera;
        // Display/Rendering
        int currentResolutionIndex = 5;
        Uint64 DeltaTime = 0;
        // Game World
        bool Running = true;
        int map;
        int objectMap;
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
};
#endif // GAMECONTEXT_H