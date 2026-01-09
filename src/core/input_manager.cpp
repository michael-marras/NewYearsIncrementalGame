#include "core/input_manager.h"

void InputManager::ProcessKeyDown(SDL_Keycode key) {
    if (keysHeld.find(key) == keysHeld.end() || !keysHeld[key]) {
        keysPressedThisFrame[key] = true;
    }
    keysHeld[key] = true;
}

void InputManager::ProcessKeyUp(SDL_Keycode key) {
    keysHeld[key] = false;
    keysReleasedThisFrame[key] = true;
}

void InputManager::ProcessMouseMotion(float x, float y, float deltaX, float deltaY) {
    mouseX = x;
    mouseY = y;
    mouseDeltaX = deltaX;
    mouseDeltaY = deltaY;
}

void InputManager::ProcessMouseButtonDown(int button) {
    if (button >= 0 && button < 4) {
        if (!mouseButtons[button]) {
            mouseButtonsPressed[button] = true;
        }
        mouseButtons[button] = true;
    }
}

void InputManager::ProcessMouseButtonUp(int button) {
    if (button >= 0 && button < 4) {
        mouseButtons[button] = false;
        mouseButtonsReleased[button] = true;
    }
}

void InputManager::ProcessMouseWheel(float y) {
    mouseWheelY = y;
}

void InputManager::EndFrame() {
    // Clear per-frame flags
    keysPressedThisFrame.clear();
    keysReleasedThisFrame.clear();
    
    for (int i = 0; i < 4; i++) {
        mouseButtonsPressed[i] = false;
        mouseButtonsReleased[i] = false;
    }
    
    mouseDeltaX = 0.0f;
    mouseDeltaY = 0.0f;
    mouseWheelY = 0.0f;
}

bool InputManager::IsKeyHeld(SDL_Keycode key) const {
    auto it = keysHeld.find(key);
    return (it != keysHeld.end() && it->second);
}

bool InputManager::IsKeyPressed(SDL_Keycode key) const {
    auto it = keysPressedThisFrame.find(key);
    return (it != keysPressedThisFrame.end() && it->second);
}

bool InputManager::IsKeyReleased(SDL_Keycode key) const {
    auto it = keysReleasedThisFrame.find(key);
    return (it != keysReleasedThisFrame.end() && it->second);
}

bool InputManager::IsMouseButtonHeld(int button) const {
    if (button >= 0 && button < 4) {
        return mouseButtons[button];
    }
    return false;
}

bool InputManager::IsMouseButtonPressed(int button) const {
    if (button >= 0 && button < 4) {
        return mouseButtonsPressed[button];
    }
    return false;
}

bool InputManager::IsMouseButtonReleased(int button) const {
    if (button >= 0 && button < 4) {
        return mouseButtonsReleased[button];
    }
    return false;
}

