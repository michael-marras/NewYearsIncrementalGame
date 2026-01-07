#ifndef BIG_BANG_ENGINE_H
#define BIG_BANG_ENGINE_H

#include <cstdint>

// Forward declarations
class TextureManager;

/**
 * BigBangEngine
 * - Handles loading, animating, and rendering the planet "portal" sprite.
 * - Sprite sheet layout (portal.png):
 *   - 2 rows of 64x64 frames
 *   - 4 frames on the top row (indices 0..3)
 *   - 3 frames on the bottom row (indices 4..6)
 */
class BigBangEngine {
public:
    BigBangEngine();
    ~BigBangEngine() = default;

    // Ensure the portal sprite is loaded (idempotent)
    // name = "portal", file = "portal.png" in res/
    void EnsureTextureLoaded(TextureManager* textureManager);

    // Advance animation by deltaTimeMs
    void Update(float deltaTimeMs);

    // Render at world position (centered) with scale
    // Uses TextureManager::RenderSprite on texture name "portal"
    void Render(TextureManager* textureManager, float worldX, float worldY, float scale = 1.0f) const;

    // Configure animation speed (milliseconds per frame)
    void SetFrameDurationMs(float ms) { frameDurationMs = ms; }

    // Accessors
    int GetCurrentFrame() const { return currentFrame; }

private:
    // Calculate source rectangle for a given frame index (0..6)
    void GetFrameSrc(int frameIndex, int& srcX, int& srcY, int& srcW, int& srcH) const;

private:
    int currentFrame = 0;          // 0..6
    float frameAccumulatorMs = 0;  // time accumulator for frame stepping
    float frameDurationMs = 100.0f; // default: 10 FPS
};

#endif // BIG_BANG_ENGINE_H


