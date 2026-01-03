#include "world/objects.h"
#include "utils/constants.h"
#include "entities/player.h"
#include <SDL3/SDL.h>
#include <cstdio>
#include <cstring>
#include <cmath>

ObjectManager::ObjectManager() {
}

ObjectManager::~ObjectManager() {
    DestroyAllGrids();
}

void ObjectManager::RegisterObject(int id, const char* sheetName, int sheetX, int sheetY, 
                                  int width, int height, bool interactable, const char* name,
                                  int maxHealth, const char* replacement) {
    std::string objectName;
    
    // Use provided name, or generate default name if not provided
    if (name && name[0] != '\0') {
        objectName = name;
    } else {
        char defaultName[32];
        snprintf(defaultName, sizeof(defaultName), "object_%d", id);
        objectName = defaultName;
    }
    
    // Create object info
    ObjectInfo obj;
    obj.id = id;
    obj.name = objectName;
    obj.sheetName = sheetName;
    obj.sheetX = sheetX;
    obj.sheetY = sheetY;
    obj.width = width;
    obj.height = height;
    obj.interactable = interactable;
    obj.maxHealth = maxHealth;
    obj.death_replacement = replacement;
    
    objects[id] = obj;
    nameToId[objectName] = id;
}

void ObjectManager::RegisterObjectsFromSheet(const char* sheetName, const int* objectData, int count, bool interactable) {
    // Note: RegisterObjectsFromSheet doesn't support custom names per object
    // Use RegisterObject individually if you need custom names
    for (int i = 0; i < count; i++) {
        int id = objectData[i * 5 + 0];
        int x = objectData[i * 5 + 1];
        int y = objectData[i * 5 + 2];
        int w = objectData[i * 5 + 3];
        int h = objectData[i * 5 + 4];
        RegisterObject(id, sheetName, x, y, w, h, interactable, nullptr, -1);
    }
}

int ObjectManager::RegisterObjectsFromGrid(const char* sheetName, int cols, int rows, 
                                          int startX, int startY, int startId, 
                                          int objWidth, int objHeight, bool interactable) {
    int objectId = startId;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int x = startX + (col * objWidth);
            int y = startY + (row * objHeight);
            RegisterObject(objectId, sheetName, x, y, objWidth, objHeight, interactable, nullptr);
            objectId++;
        }
    }
    return objectId;
}

ObjectInfo* ObjectManager::GetObject(int id) {
    auto it = objects.find(id);
    if (it != objects.end()) {
        return &it->second;
    }
    return nullptr;
}

ObjectInfo* ObjectManager::GetObjectByName(const char* name) {
    auto it = nameToId.find(name);
    if (it != nameToId.end()) {
        return GetObject(it->second);
    }
    return nullptr;
}

bool ObjectManager::HasObject(int id) {
    return objects.find(id) != objects.end();
}

int ObjectManager::CreateObjectGrid(int width, int height) {
    ObjectGrid* grid = new ObjectGrid(width, height);
    int gridId = nextGridId++;
    grids[gridId] = grid;
    return gridId;
}

ObjectGrid* ObjectManager::GetObjectGrid(int gridId) {
    auto it = grids.find(gridId);
    if (it != grids.end()) {
        return it->second;
    }
    return nullptr;
}

ObjectInfo* ObjectManager::GetObjectAt(int gridId, int x, int y) {
    ObjectGrid* grid = GetObjectGrid(gridId);
    if (!grid || !grid->HasObject(x, y)) {
        return nullptr;
    }
    
    int objectId = grid->GetObject(x, y);
    return GetObject(objectId);
}

bool ObjectManager::ReplaceObjectAt(int gridId, int x, int y, int newObjectId) {
    ObjectGrid* grid = GetObjectGrid(gridId);
    if (!grid || !grid->IsValid(x, y)) {
        return false;
    }
    
    // Remove old instance data (if any)
    RemoveInstance(gridId, x, y);
    
    // Set new object (0 = empty, otherwise new object ID)
    grid->SetObject(x, y, newObjectId);
    
    return true;
}

void ObjectManager::DestroyAllGrids() {
    for (auto& pair : grids) {
        ClearInstances(pair.first);  // Clear instances before deleting grid
        delete pair.second;
    }
    grids.clear();
    instances.clear();  // Clear all instances
    nextGridId = 1;
}

void ObjectManager::DestroyObjectGrid(int gridId) {
    auto it = grids.find(gridId);
    if (it != grids.end()) {
        ClearInstances(gridId);  // Clear instances before deleting grid
        delete it->second;
        grids.erase(it);
    }
}

ObjectInstance* ObjectManager::GetOrCreateInstance(int gridId, int x, int y) {
    ObjectInstanceKey key;
    key.gridId = gridId;
    key.x = x;
    key.y = y;
    
    auto it = instances.find(key);
    if (it != instances.end()) {
        return &it->second;
    }
    
    // Get the object at this position to determine max health
    ObjectGrid* grid = GetObjectGrid(gridId);
    if (!grid) return nullptr;
    
    int objectId = grid->GetObject(x, y);
    if (objectId == 0) return nullptr;  // No object at this position
    
    ObjectInfo* objInfo = GetObject(objectId);
    if (!objInfo) return nullptr;
    
    // Only create instance if object has finite health
    if (objInfo->maxHealth <= 0) return nullptr;
    
    // Create new instance with full health
    ObjectInstance instance;
    instance.currentHealth = objInfo->maxHealth;
    instances[key] = instance;
    
    return &instances[key];
}

ObjectInstance* ObjectManager::GetInstance(int gridId, int x, int y) const {
    ObjectInstanceKey key;
    key.gridId = gridId;
    key.x = x;
    key.y = y;
    
    auto it = instances.find(key);
    if (it != instances.end()) {
        return const_cast<ObjectInstance*>(&it->second);
    }
    return nullptr;
}

bool ObjectManager::PlayerCanInteract(int gridId, int x, int y, Player* player) {
    ObjectInfo* objInfo = GetObjectAt(gridId, x, y);
    if (!objInfo || !objInfo->interactable) return false;

    float objectWorldX = x * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;
    float objectWorldY = y * TILE_RENDER_SIZE + TILE_RENDER_SIZE / 2.0f;

    float playerX = player->getX();
    float playerY = player->getY();

    float dx = objectWorldX - playerX;
    float dy = objectWorldY - playerY;
    float dist = sqrtf(dx * dx + dy * dy);

    return dist < 20;
}

void ObjectManager::SetInstanceHealth(int gridId, int x, int y, int health) {
    ObjectInstance* instance = GetOrCreateInstance(gridId, x, y);
    if (instance) {
        instance->currentHealth = health;
    }
}

bool ObjectManager::DamageInstance(int gridId, int x, int y, int damage) {
    ObjectInstance* instance = GetOrCreateInstance(gridId, x, y);
    if (!instance) {
        return false;
    }
    
    instance->currentHealth -= damage;
    SDL_Log("%d", instance->currentHealth);
    if (instance->currentHealth <= 0) {
        // Object destroyed - check for replacement
        ObjectGrid* grid = GetObjectGrid(gridId);
        if (!grid) return true;
        
        // Get current object ID before removing
        int currentObjectId = grid->GetObject(x, y);
        ObjectInfo* objInfo = GetObject(currentObjectId);
        
        RemoveInstance(gridId, x, y);
        
        if (objInfo && !objInfo->death_replacement.empty()) {
            ObjectInfo* replacementInfo = GetObjectByName(objInfo->death_replacement.c_str());
            if (replacementInfo) {
                ReplaceObjectAt(gridId, x, y, replacementInfo->id);
            } else {
                grid->ClearObject(x, y);
            }
        } else {
            grid->ClearObject(x, y);
        }
        return true;  // Object was destroyed
    }
    
    return false;  // Object still alive
}

int ObjectManager::GetInstanceHealth(int gridId, int x, int y) const {
    ObjectInstance* instance = GetInstance(gridId, x, y);
    if (instance) {
        return instance->currentHealth;
    }
    
    // No instance data - check if object has health tracking
    ObjectGrid* grid = const_cast<ObjectManager*>(this)->GetObjectGrid(gridId);
    if (!grid) return -1;
    
    int objectId = grid->GetObject(x, y);
    if (objectId == 0) return -1;
    
    ObjectInfo* objInfo = const_cast<ObjectManager*>(this)->GetObject(objectId);
    if (!objInfo) return -1;
    
    return objInfo->maxHealth > 0 ? objInfo->maxHealth : -1;
}

void ObjectManager::RemoveInstance(int gridId, int x, int y) {
    ObjectInstanceKey key;
    key.gridId = gridId;
    key.x = x;
    key.y = y;
    
    instances.erase(key);
}

void ObjectManager::ClearInstances(int gridId) {
    // Remove all instances for this grid
    auto it = instances.begin();
    while (it != instances.end()) {
        if (it->first.gridId == gridId) {
            it = instances.erase(it);
        } else {
            ++it;
        }
    }
}

