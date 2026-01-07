#include "world/BigBangEngine.h"
#include "core/textures.h"

static constexpr const char* kPortalTextureName = "portal";
static constexpr const char* kPortalFile = "portal.png";
static constexpr int kFrameSize = 64;   // 64x64 frames
static constexpr int kTopRowFrames = 4; // frames 0..3
static constexpr int kTotalFrames = 7;  // 4 top + 3 bottom

BigBangEngine::BigBangEngine() {
}

void BigBangEngine::EnsureTextureLoaded(TextureManager* textureManager) {
    if (!textureManager) return;
    if (!textureManager->HasTexture(kPortalTextureName)) {
        textureManager->LoadImageFromRes(kPortalTextureName, kPortalFile);
    }
}

void BigBangEngine::Update(float deltaTimeMs) {
    if (deltaTimeMs <= 0.0f) return;
    frameAccumulatorMs += deltaTimeMs;
    while (frameAccumulatorMs >= frameDurationMs) {
        frameAccumulatorMs -= frameDurationMs;
        currentFrame = (currentFrame + 1) % kTotalFrames;
    }
}

void BigBangEngine::Render(TextureManager* textureManager, float worldX, float worldY, float scale) const {
    if (!textureManager) return;

    int srcX, srcY, srcW, srcH;
    GetFrameSrc(currentFrame, srcX, srcY, srcW, srcH);

    // The engine should render centered on worldX/worldY
    // TextureManager::RenderSprite treats dstX/dstY as the center
    textureManager->RenderSprite(kPortalTextureName, srcX, srcY, srcW, srcH, worldX, worldY, scale);
}

void BigBangEngine::GetFrameSrc(int frameIndex, int& srcX, int& srcY, int& srcW, int& srcH) const {
    if (frameIndex < 0) frameIndex = 0;
    if (frameIndex >= kTotalFrames) frameIndex = kTotalFrames - 1;

    int row = (frameIndex < kTopRowFrames) ? 0 : 1;
    int col = (frameIndex < kTopRowFrames) ? frameIndex : (frameIndex - kTopRowFrames);

    srcX = col * kFrameSize;
    srcY = row * kFrameSize;
    srcW = kFrameSize;
    srcH = kFrameSize;
}


