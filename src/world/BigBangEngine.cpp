#include "world/BigBangEngine.h"
#include "core/textures.h"

static constexpr const char* kPortalTextureName = "portal";
static constexpr const char* kPortalFile = "other/portal.png";
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
    
    // Update frame animation
    frameAccumulatorMs += deltaTimeMs;
    while (frameAccumulatorMs >= frameDurationMs) {
        frameAccumulatorMs -= frameDurationMs;
        currentFrame = (currentFrame + 1) % kTotalFrames;
    }
    
    // Smoothly interpolate display scale toward target scale
    // Growth rate: 4 scale units (5-1) over 4000ms = 1 scale unit per 1000ms
    float scalePerMs = 4.0f / kGrowthTimeMs;
    float maxChange = scalePerMs * deltaTimeMs;
    
    if (displayScale < targetScale) {
        displayScale += maxChange;
        if (displayScale > targetScale) displayScale = targetScale;
    } else if (displayScale > targetScale) {
        displayScale -= maxChange;
        if (displayScale < targetScale) displayScale = targetScale;
    }
}

void BigBangEngine::SetTargetEnergyRatio(float ratio) {
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;
    targetScale = 1.0f + 4.0f * ratio;
}

void BigBangEngine::ResetScale() {
    targetScale = 1.0f;
    displayScale = 1.0f;
}

void BigBangEngine::Render(TextureManager* textureManager, float worldX, float worldY, float scale) const {
    if (!textureManager) return;

    int srcX, srcY, srcW, srcH;
    GetFrameSrc(currentFrame, srcX, srcY, srcW, srcH);

    float topLeftX = worldX - (srcW * scale / 2.0f);
    float topLeftY = worldY - (srcH * scale / 2.0f);

    textureManager->RenderSprite(kPortalTextureName, srcX, srcY, srcW, srcH, topLeftX, topLeftY, scale);
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


