#include "items/resources.h"
#include "utils/constants.h"
#include "entities/player.h"
#include "ui/hud.h"
#include <SDL3/SDL.h>
#include <cstdio>
#include <cstring>
#include <cmath>

ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
    DestroyAllArrays();
}

void ResourceManager::RegisterResource(int id, const char* sheetName, int sheetX, int sheetY,
                                       int width, int height, bool pickupable, const char* name, 
                                       float value, const char* displayName) {
    std::string resourceName;

    // Use provided name, or generate default name if not provided
    if (name && name[0] != '\0') {
        resourceName = name;
    } else {
        char defaultName[32];
        snprintf(defaultName, sizeof(defaultName), "object_%d", id);
        resourceName = defaultName;
    }

    // Create Resource Info
    ResourceInfo resource;
    resource.id = id;
    resource.name = resourceName;
    resource.sheetName = sheetName;
    resource.sheetX = sheetX;
    resource.sheetY = sheetY;
    resource.width = width;
    resource.height = height;
    resource.pickupable = pickupable;
    resource.value = value;
    resource.displayName = displayName;

    resourceTypes[id] = resource;
    nameToId[resourceName] = id;
}

ResourceInfo* ResourceManager::GetResource(int id) {
    auto it = resourceTypes.find(id);
    if (it != resourceTypes.end()) {
        return &it->second;
    }
    return nullptr;
}

ResourceInfo* ResourceManager::GetResourceByName(const char* name) {
    auto it = nameToId.find(name);
    if (it != nameToId.end()) {
        return GetResource(it->second);
    }
    return nullptr;
}

float ResourceManager::GetResourceValue(int resourceId) const {
    auto it = resourceTypes.find(resourceId);
    if (it == resourceTypes.end()) {
        return 0.0f;
    }
    return it->second.value;
}

float ResourceManager::GetResourceValue(int resourceId, int quantity) const {
    if (quantity <= 0) return 0.0f;
    
    auto it = resourceTypes.find(resourceId);
    if (it == resourceTypes.end()) {
        return 0.0f;
    }
    return it->second.value * static_cast<float>(quantity);
}

bool ResourceManager::HasResource(int id) {
    return resourceTypes.find(id) != resourceTypes.end();
}

int ResourceManager::CreateResourceArray() {
    ResourceArray* array = new ResourceArray();
    int arrayId = nextArrayId++;
    resourceArrays[arrayId] = array;
    return arrayId;
}

ResourceArray* ResourceManager::GetResourceArray(int arrayId) {
    auto it = resourceArrays.find(arrayId);
    if (it != resourceArrays.end()) {
        return it->second;
    }
    return nullptr;
}

void ResourceManager::AddResource(int arrayId, float x, float y, int resourceId, int quantity, float vx, float vy) {
    ResourceArray* array = GetResourceArray(arrayId);

    if (!array) return;

    ResourceInstance instance;
    instance.x = x;
    instance.y = y;
    instance.resourceId = resourceId;
    instance.quantity = quantity;
    instance.vx = vx;
    instance.vy = vy;

    array->resources.push_back(instance);
}

ResourceInstance* ResourceManager::GetResourceNear(int arrayId, float worldX, float worldY, float pickupRange) {
    ResourceArray* array = GetResourceArray(arrayId);
    if (!array) return nullptr;

    ResourceInstance* closest = nullptr;
    float closestDist = pickupRange;

    for (ResourceInstance& resource : array->resources) {
        float dx = resource.x - worldX;
        float dy = resource.y - worldY;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < closestDist) {
            closestDist = dist;
            closest = &resource;
        }
    }
    return closest;
}

int ResourceManager::PickupResource(int arrayId, float worldX, float worldY, float pickupRange) {
    ResourceInstance* resource = GetResourceNear(arrayId, worldX, worldY, pickupRange);
    if (!resource) return 0;

    int quantity = resource->quantity;

    ResourceArray* array = GetResourceArray(arrayId);
    if (!array) return 0;

    for (size_t i = 0; i < array->resources.size(); i++) {
        if (&array->resources[i] == resource) {
            array->resources.erase(array->resources.begin() + i);
            break;
        }
    }

    return quantity;
}

std::vector<ResourceInstance*> ResourceManager::GetResourcesInArea(int arrayId, float minX, float minY, float maxX, float maxY) {
    std::vector<ResourceInstance*> result;
    ResourceArray* array = GetResourceArray(arrayId);
    if (!array) return result;

    for (ResourceInstance& resource : array->resources) {
        if (resource.x >= minX && resource.x <= maxX
        && resource.y >= minY && resource.y <= maxY) {
            result.push_back(&resource);
        }
    }

    return result;
}

void ResourceManager::DestroyResourceArray(int arrayId) {
    auto it = resourceArrays.find(arrayId);

    if (it != resourceArrays.end()) {
        delete it->second;
        resourceArrays.erase(it);
    }

}

void ResourceManager::DestroyAllArrays() {
    for (auto& pair : resourceArrays) {
        delete pair.second;
    }
    resourceArrays.clear();
}

void ResourceManager::Update(int arrayId, float playerX, float playerY, float deltaTimeMs, Player* player, HUD* hud) {
    ResourceArray* resourceArray = GetResourceArray(arrayId);
    if (!resourceArray) {
        return;
    }
    
    float pickupRange = player ? player->GetPickupRange() : 10.0f;
    const float SHOOT_OUT_DURATION = 150.0f;
    const float IDLE_DURATION = 200.0f;
    const float MAGNETIC_RANGE = pickupRange;
    const float ACTUAL_PICKUP_DISTANCE = 10.0f;
    const float MAGNETIC_ACCELERATION = 1000.0f;
    const float MAX_MAGNETIC_SPEED = 100.0f;
    const float VELOCITY_CORRECTION_STRENGTH = 15.0f;
    const float DELTA_TIME_SEC = deltaTimeMs / 1000.0f;

    
    for (size_t i = resourceArray->resources.size(); i > 0; i--) {
        size_t idx = i - 1;
        ResourceInstance& resource = resourceArray->resources[idx];
        
        float dx = playerX - resource.x;
        float dy = playerY - resource.y;
        float distance = sqrtf(dx * dx + dy * dy);
        if (resource.state == ResourceState::MAGNETIC) {
            if (distance < ACTUAL_PICKUP_DISTANCE) {
                if (player) {
                    player->AddResource(resource.resourceId, resource.quantity);
                    
                    // Notify HUD about pickup
                    if (hud) {
                        hud->OnResourcePickedUp(resource.resourceId, resource.quantity);
                    }
                    
                    ResourceInfo* resourceInfo = GetResource(resource.resourceId);
                    if (resourceInfo) {
                        SDL_Log("Picked up %d x %s (Resource ID: %d)", 
                                resource.quantity, 
                                resourceInfo->name.c_str(), 
                                resource.resourceId);
                    } else {
                        SDL_Log("Picked up %d x Resource ID: %d", resource.quantity, resource.resourceId);
                    }
                }
                
                resourceArray->resources.erase(resourceArray->resources.begin() + idx);
                continue;
            }
        }
            
        
        switch (resource.state) {
            case ResourceState::SHOOT_OUT: {
                resource.shootOutTimer += deltaTimeMs;
                
                resource.x += resource.vx * DELTA_TIME_SEC;
                resource.y += resource.vy * DELTA_TIME_SEC;

                if (resource.shootOutTimer >= SHOOT_OUT_DURATION) {
                    resource.state = ResourceState::IDLE;
                    resource.vx = 0.0f;
                    resource.vy = 0.0f;
                    resource.idleTimer = 0.0f;
                }
                break;
            }
            
            case ResourceState::IDLE: {
                resource.idleTimer += deltaTimeMs;
                
                if (resource.idleTimer >= IDLE_DURATION && distance < MAGNETIC_RANGE) {
                    resource.state = ResourceState::MAGNETIC;
                    resource.vx = 0.0f;
                    resource.vy = 0.0f;
                }
                break;
            }
            
            case ResourceState::MAGNETIC: {
                if (distance > 0.001f) {
                    float normalizedX = dx / distance;
                    float normalizedY = dy / distance;

                    float idealVx = normalizedX * MAX_MAGNETIC_SPEED;
                    float idealVy = normalizedY * MAX_MAGNETIC_SPEED;

                    float correctionFactor = VELOCITY_CORRECTION_STRENGTH * DELTA_TIME_SEC;
                    resource.vx = resource.vx * (1.0f - correctionFactor) + idealVx * correctionFactor;
                    resource.vy = resource.vy * (1.0f - correctionFactor) + idealVy * correctionFactor;

                    resource.vx += normalizedX * MAGNETIC_ACCELERATION * DELTA_TIME_SEC;
                    resource.vy += normalizedY * MAGNETIC_ACCELERATION * DELTA_TIME_SEC;
                    
                    // Cap speed
                    float speed = sqrtf(resource.vx * resource.vx + resource.vy * resource.vy);
                    if (speed > MAX_MAGNETIC_SPEED) {
                        float scale = MAX_MAGNETIC_SPEED / speed;
                        resource.vx *= scale;
                        resource.vy *= scale;
                    }
                    
                    resource.x += resource.vx * DELTA_TIME_SEC;
                    resource.y += resource.vy * DELTA_TIME_SEC;
                }
                break;
            }
        }
    }
}

