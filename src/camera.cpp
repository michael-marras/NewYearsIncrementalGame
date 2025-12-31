#include "camera.h"
#include <algorithm>

Camera::Camera() : x(0.0f), y(0.0f), hasBounds(false), isFollowing(false), followSpeed(1.0f) {
}

Camera::Camera(float x, float y) : x(x), y(y), hasBounds(false), isFollowing(false), followSpeed(1.0f) {
}

void Camera::SetPosition(float newX, float newY) {
    x = newX;
    y = newY;
    ClampToBounds();
}

void Camera::Move(float deltaX, float deltaY) {
    x += deltaX;
    y += deltaY;
    ClampToBounds();
}

void Camera::SetBounds(float minX, float minY, float maxX, float maxY) {
    hasBounds = true;
    this->minX = minX;
    this->minY = minY;
    this->maxX = maxX;
    this->maxY = maxY;
    ClampToBounds();
}

void Camera::ClearBounds() {
    hasBounds = false;
}

void Camera::ClampToBounds() {
    if (hasBounds) {
        x = std::max(minX, std::min(maxX, x));
        y = std::max(minY, std::min(maxY, y));
    }
}

void Camera::SnapToTarget(float targetX, float targetY) {
    x = targetX;
    y = targetY;
    ClampToBounds();
}

void Camera::FollowTarget(float targetX, float targetY, float speed) {
    followSpeed = speed;
    SetFollowTarget(targetX, targetY);
}

void Camera::SetFollowTarget(float targetX, float targetY) {
    isFollowing = true;
    this->targetX = targetX;
    this->targetY = targetY;
}

void Camera::UpdateFollow(float deltaTime) {
    if (!isFollowing) return;
    
    // Linear interpolation towards target
    float dx = targetX - x;
    float dy = targetY - y;
    
    x += dx * followSpeed * deltaTime;
    y += dy * followSpeed * deltaTime;
    
    ClampToBounds();
}

float Camera::WorldToScreenX(float worldX) const {
    return worldX - x;
}

float Camera::WorldToScreenY(float worldY) const {
    return worldY - y;
}

void Camera::WorldToScreen(float worldX, float worldY, float& screenX, float& screenY) const {
    screenX = worldX - x;
    screenY = worldY - y;
}

float Camera::ScreenToWorldX(float screenX) const {
    return screenX + x;
}

float Camera::ScreenToWorldY(float screenY) const {
    return screenY + y;
}

void Camera::ScreenToWorld(float screenX, float screenY, float& worldX, float& worldY) const {
    worldX = screenX + x;
    worldY = screenY + y;
}

void Camera::CenterOn(float worldX, float worldY) {
    x = worldX;
    y = worldY;
    ClampToBounds();
}

