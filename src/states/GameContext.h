#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <tuple>

class GameContext {
    private: 
        // Display/Rendering
        int currentResolutionIndex = 5;
        Uint64 DeltaTime = 0;
        // Input
        std::unordered_map<SDL_Keycode, bool> KeyHeld;
        std::unordered_map<SDL_Keycode, std::tuple<int, int>> KeyMap;
        bool mouseHeld = false;
        std::tuple<int, int> mousePos {0, 0};

        // Game World
        bool Running = true;
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
         * @param direction direction to scale
         */
        void ChangeResolution(int newIndex, SDL_Window* window);

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
        

};
#endif // GAMECONTEXT_H