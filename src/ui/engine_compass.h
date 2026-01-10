#ifndef ENGINE_COMPASS_H
#define ENGINE_COMPASS_H

// Forward declarations
class Player;
class Planet;
class TextureManager;
struct SDL_Renderer;

class EngineCompass {
    public:
        /**
         * Update the compass direction to point toward the center of the top face
         * @param player Player to get position from
         * @param planet Planet to get radius from
         * @param currentFace Current planet face (0=FRONT, 1=BACK, 2=LEFT, 3=RIGHT, 4=TOP, 5=BOTTOM)
         */
        void Update(Player* player, Planet* planet, int currentFace);
        
        /**
         * Get the current direction in degrees (0-360)
         */
        float GetDirection() const { return direction; }
        
        /**
         * Get the compass position X (for rendering)
         */
        float GetX() const { return x; }
        
        /**
         * Get the compass position Y (for rendering)
         */
        float GetY() const { return y; }
        
        /**
         * Set the compass position (for rendering)
         */
        void SetPosition(float posX, float posY) { x = posX; y = posY; }

        /**
         * Render the compass
         */
        void Render(SDL_Renderer* renderer, TextureManager* textureManager);

    private:
        float direction = 0.0f;
        float x = 0.0f;
        float y = 0.0f;
        bool isEngineVisible = false;
};


#endif // ENGINE_COMPASS_H