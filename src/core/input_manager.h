#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <SDL3/SDL.h>
#include <unordered_map>

class InputManager {
    private:
        // Keyboard state
        std::unordered_map<SDL_Keycode, bool> keysHeld;
        std::unordered_map<SDL_Keycode, bool> keysPressedThisFrame;
        std::unordered_map<SDL_Keycode, bool> keysReleasedThisFrame;
        
        // Mouse state
        float mouseX = 0.0f;
        float mouseY = 0.0f;
        float mouseDeltaX = 0.0f;
        float mouseDeltaY = 0.0f;
        bool mouseButtons[4] = {false, false, false, false};  // 0-3: left, middle, right, extra
        bool mouseButtonsPressed[4] = {false, false, false, false};
        bool mouseButtonsReleased[4] = {false, false, false, false};
        
        // Mouse wheel
        float mouseWheelY = 0.0f;
    
    public:
        // Update methods
        void ProcessKeyDown(SDL_Keycode key);
        void ProcessKeyUp(SDL_Keycode key);
        void ProcessMouseMotion(float x, float y, float deltaX, float deltaY);
        void ProcessMouseButtonDown(int button);
        void ProcessMouseButtonUp(int button);
        void ProcessMouseWheel(float y);
        
        // Reset per-frame flags
        void EndFrame();
        
        // Query methods
        bool IsKeyHeld(SDL_Keycode key) const;
        bool IsKeyPressed(SDL_Keycode key) const;
        bool IsKeyReleased(SDL_Keycode key) const;
        
        // Mouse queries
        float GetMouseX() const { return mouseX; }
        float GetMouseY() const { return mouseY; }
        float GetMouseDeltaX() const { return mouseDeltaX; }
        float GetMouseDeltaY() const { return mouseDeltaY; }
        bool IsMouseButtonHeld(int button) const;
        bool IsMouseButtonPressed(int button) const;
        bool IsMouseButtonReleased(int button) const;
        float GetMouseWheelY() const { return mouseWheelY; }
};

#endif // INPUT_MANAGER_H