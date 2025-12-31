#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>
#include <tuple>

class GameContext {
    private: 
        // Display/Rendering
        Uint64 FPS;

        // Input
        std::unordered_map<SDL_Keycode, bool> KeyHeld;
        std::unordered_map<SDL_Keycode, std::tuple<int, int>> KeyMap;
        bool mouseHeld = false;
        std::tuple<int, int> mousePos {0, 0};

        // Game World
            // TODO
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
         * Changes resolution up or down

         * @param direction direction to scale
         */
        void ChangeResolution(std::string direction);

};
#endif // GAMECONTEXT_H