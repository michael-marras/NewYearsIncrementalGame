#include "ui/engine_compass.h"
#include "entities/player.h"
#include "world/planet.h"
#include "core/textures.h"
#include "utils/constants.h"
#include <SDL3/SDL.h>
#include <cmath>

// Define M_PI if not defined (Windows/MSVC compatibility)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void EngineCompass::Update(Player* player, Planet* planet, int currentFace) {
    if (!player || !planet) {
        return;
    }
    
    float playerX = player->getX();
    float playerY = player->getY();
    int radius = planet->getRadius();
    
    // Center of the top face in local coordinates
    float topFaceCenterX = radius * TILE_RENDER_SIZE;
    float topFaceCenterY = radius * TILE_RENDER_SIZE;
    
    float dx, dy;
    
    if (currentFace == 4) {
        dx = topFaceCenterX - playerX;
        dy = topFaceCenterY - playerY;
        if (abs(dx) < 160 && abs(dy) < 90) {
            isEngineVisible = true;
        } else {
            isEngineVisible = false;
        }
    } else if (currentFace < 4) {
        float targetX = topFaceCenterX;
        float targetY = -topFaceCenterY;
        
        dx = targetX - playerX;
        dy = targetY - playerY;
    } else {
        // Bottom face (5) - find shortest path to portal (top face center)
        float faceDiameter = 2 * radius * TILE_RENDER_SIZE;
        
        // Calculate distance from player to each edge center
        float distToTopEdge = abs(playerY);
        float distToBottomEdge = abs(faceDiameter - playerY);
        float distToLeftEdge = abs(playerX);
        float distToRightEdge = abs(faceDiameter - playerX);
        
        float totalDistThroughTop = distToTopEdge + topFaceCenterY;
        float totalDistThroughBottom = distToBottomEdge + topFaceCenterY;
        float totalDistThroughLeft = distToLeftEdge + topFaceCenterX;
        float totalDistThroughRight = distToRightEdge + (faceDiameter - topFaceCenterX);
        
        float targetX, targetY;
        
        if (totalDistThroughTop <= totalDistThroughBottom && 
            totalDistThroughTop <= totalDistThroughLeft && 
            totalDistThroughTop <= totalDistThroughRight) {
            // Top edge provides shortest path
            targetX = topFaceCenterX;
            targetY = -3 * topFaceCenterY;
        } else if (totalDistThroughBottom <= totalDistThroughLeft && 
                   totalDistThroughBottom <= totalDistThroughRight) {
            // Bottom edge provides shortest path
            targetX = topFaceCenterX;
            targetY = 3 * topFaceCenterY;
        } else if (totalDistThroughLeft <= totalDistThroughRight) {
            // Left edge provides shortest path
            targetX = -3 * topFaceCenterX;
            targetY = topFaceCenterY;
        } else {
            // Right edge provides shortest path
            targetX = 3 * topFaceCenterX;
            targetY = topFaceCenterY;
        }
        
        dx = targetX - playerX;
        dy = targetY - playerY;
    }

    if (dx == 0.0f && dy == 0.0f) {
        direction = 0.0f;
        return;
    }
    
    float angleRad = atan2(dy, dx);
    float angleDeg = angleRad * 180.0f / M_PI;
    
    while (angleDeg < 0.0f) {
        angleDeg += 360.0f;
    }
    while (angleDeg >= 360.0f) {
        angleDeg -= 360.0f;
    }
    
    direction = angleDeg;
    
    // Window center (VIRTUAL_WIDTH=320, VIRTUAL_HEIGHT=180)
    const float windowCenterX = VIRTUAL_WIDTH / 2.0f;  // 160
    const float windowCenterY = VIRTUAL_HEIGHT / 2.0f; // 90
    
    // Arrow size at scale 1.0 (half-size for padding from edge)
    const float arrowHalfWidth = 6.0f;
    const float arrowHalfHeight = 4.0f;
    
    // Convert direction to radians for cos/sin
    float directionRad = direction * M_PI / 180.0f;
    float cosDir = cosf(directionRad);
    float sinDir = sinf(directionRad);
    
    // Calculate distance to edge along this direction
    // Account for arrow size by reducing the effective window size
    float maxX = (VIRTUAL_WIDTH / 2.0f) - arrowHalfWidth;   // Distance to horizontal edge
    float maxY = (VIRTUAL_HEIGHT / 2.0f) - arrowHalfHeight; // Distance to vertical edge
    
    // Handle division by zero and calculate distance to closest edge
    float distance;
    if (fabsf(cosDir) < 0.001f) {
        // Nearly vertical, use Y distance
        distance = maxY / fabsf(sinDir);
    } else if (fabsf(sinDir) < 0.001f) {
        // Nearly horizontal, use X distance
        distance = maxX / fabsf(cosDir);
    } else {
        // Use minimum of X and Y distances to hit the closest edge
        float distX = maxX / fabsf(cosDir);
        float distY = maxY / fabsf(sinDir);
        distance = (distX < distY) ? distX : distY;
    }
    
    // Position at edge of screen along the direction vector
    x = windowCenterX + cosDir * distance;
    y = windowCenterY + sinDir * distance;
}

void EngineCompass::Render(SDL_Renderer* renderer, TextureManager* textureManager) {
    if (!renderer || !textureManager || isEngineVisible) {
        return;
    }
    
    if (!textureManager->HasTexture("engine_arrow")) {
        textureManager->LoadImageFromRes("engine_arrow", "engine_arrow.png");
    }

    textureManager->RenderCompass(x, y, direction, 1.0f);
}

