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
    // name = "portal", file = "other/portal.png" in res/
    void EnsureTextureLoaded(TextureManager* textureManager);

    // Advance animation by deltaTimeMs
    void Update(float deltaTimeMs);

    // Render at world position (centered) with scale
    // Uses TextureManager::RenderSprite on texture name "portal"
    void Render(TextureManager* textureManager, float worldX, float worldY, float scale = 1.0f) const;

    // Configure animation speed (milliseconds per frame)
    void SetFrameDurationMs(float ms) { frameDurationMs = ms; }

    // Set the target scale based on energy ratio (0.0 to 1.0)
    // Portal will grow from 1x to 5x over time
    void SetTargetEnergyRatio(float ratio);
    
    // Get current display scale (smoothly interpolated)
    float GetDisplayScale() const { return displayScale; }
    
    // Check if portal has finished growing to full size (5x)
    bool IsFullyGrown() const { return displayScale >= 4.99f; }
    
    // Reset portal scale to 1x (after generation)
    void ResetScale();

    // Accessors
    int GetCurrentFrame() const { return currentFrame; }

private:
    // Calculate source rectangle for a given frame index (0..6)
    void GetFrameSrc(int frameIndex, int& srcX, int& srcY, int& srcW, int& srcH) const;

private:
    int currentFrame = 0;
    float frameAccumulatorMs = 0;
    float frameDurationMs = 100.0f;
    
    float targetScale = 1.0f;
    float displayScale = 1.0f;
    static constexpr float kGrowthTimeMs = 4000.0f;
};

#endif // BIG_BANG_ENGINE_H


