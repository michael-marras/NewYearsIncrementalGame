#ifndef CAMERA_H
#define CAMERA_H

// Camera class to handle viewport positioning and scrolling
class Camera {
public:
    Camera();
    Camera(float x, float y);
    
    // Position
    float GetX() const { return x; }
    float GetY() const { return y; }
    void SetPosition(float newX, float newY);
    void Move(float deltaX, float deltaY);
    
    // Clamping (boundaries)
    void SetBounds(float minX, float minY, float maxX, float maxY);
    void ClearBounds();
    void ClampToBounds();
    
    // Following/Snapping to target
    void SnapToTarget(float targetX, float targetY);
    void FollowTarget(float targetX, float targetY, float speed = 1.0f);
    void SetFollowTarget(float targetX, float targetY);
    void UpdateFollow(float deltaTime);

    // Zoom control
    void SetZoom(float zoom);
    float GetZoom() const;
    void ZoomIn(float amount);
    void ZoomOut(float amount);
    void SetZoomBounds(float minZoom, float maxZoom);
    
    // Get world-to-screen conversion
    float WorldToScreenX(float worldX) const;
    float WorldToScreenY(float worldY) const;
    void WorldToScreen(float worldX, float worldY, float& screenX, float& screenY) const;
    
    // Get screen-to-world conversion
    float ScreenToWorldX(float screenX) const;
    float ScreenToWorldY(float screenY) const;
    void ScreenToWorld(float screenX, float screenY, float& worldX, float& worldY) const;
    
    // Convert center-relative screen coordinates to top-left-relative render coordinates
    // (Camera uses center-relative, SDL uses top-left-relative)
    void WorldToRender(float worldX, float worldY, float& renderX, float& renderY, float screenWidth, float screenHeight) const;
    float WorldToRenderX(float worldX, float screenWidth) const;
    float WorldToRenderY(float worldY, float screenHeight) const;
    
    // Convert top-left-relative render coordinates to world coordinates
    void RenderToWorld(float renderX, float renderY, float& worldX, float& worldY, float screenWidth, float screenHeight) const;
    
    // Center camera on a point
    void CenterOn(float worldX, float worldY);
    
private:
    float x, y;  // Camera position in world coordinates
    
    // Clamping bounds
    bool hasBounds;
    float minX, minY, maxX, maxY;
    
    // Following target
    bool isFollowing;
    float targetX, targetY;
    float followSpeed;

    // Zoom
    float zoom = 1.0f;
    float minZoom = 0.25f;
    float maxZoom = 4.0f;
};

#endif // CAMERA_H

