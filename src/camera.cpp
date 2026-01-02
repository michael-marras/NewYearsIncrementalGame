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
    return (worldX - x) * zoom;
}

float Camera::WorldToScreenY(float worldY) const {
    return (worldY - y) * zoom;
}

void Camera::WorldToScreen(float worldX, float worldY, float& screenX, float& screenY) const {
    screenX = (worldX - x) * zoom;
    screenY = (worldY - y) * zoom;
}

float Camera::ScreenToWorldX(float screenX) const {
    return (screenX / zoom) + x;
}

float Camera::ScreenToWorldY(float screenY) const {
    return (screenY / zoom) + y;
}

void Camera::ScreenToWorld(float screenX, float screenY, float& worldX, float& worldY) const {
    worldX = (screenX / zoom) + x;
    worldY = (screenY / zoom) + y;
}

float Camera::WorldToRenderX(float worldX, float screenWidth) const {
    return WorldToScreenX(worldX) + (screenWidth / 2.0f);
}

float Camera::WorldToRenderY(float worldY, float screenHeight) const {
    return WorldToScreenY(worldY) + (screenHeight / 2.0f);
}

void Camera::WorldToRender(float worldX, float worldY, float& renderX, float& renderY, float screenWidth, float screenHeight) const {
    renderX = WorldToScreenX(worldX) + (screenWidth / 2.0f);
    renderY = WorldToScreenY(worldY) + (screenHeight / 2.0f);
}

void Camera::RenderToWorld(float renderX, float renderY, float& worldX, float& worldY, float screenWidth, float screenHeight) const {
    // Convert render coordinates (top-left origin) to screen coordinates (center origin)
    float screenX = renderX - (screenWidth / 2.0f);
    float screenY = renderY - (screenHeight / 2.0f);
    // Convert to world coordinates
    ScreenToWorld(screenX, screenY, worldX, worldY);
}

void Camera::CenterOn(float worldX, float worldY) {
    x = worldX;
    y = worldY;
    ClampToBounds();
}

float Camera::GetZoom() const {
    return zoom;
}

void Camera::SetZoom(float newZoom) {
    zoom = std::max(minZoom, std::min(maxZoom, newZoom));
}

void Camera::ZoomIn(float amount) {
    SetZoom(zoom + amount);
}

void Camera::ZoomOut(float amount) {
    SetZoom(zoom - amount);
}

void Camera::SetZoomBounds(float newMinZoom, float newMaxZoom) {
    minZoom = newMinZoom;
    maxZoom = newMaxZoom;
    SetZoom(zoom);
}
