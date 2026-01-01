#include "objects.h"
#include <cstdio>
#include <cstring>

ObjectManager::ObjectManager() {
}

ObjectManager::~ObjectManager() {
    DestroyAllGrids();
}

void ObjectManager::RegisterObject(int id, const char* sheetName, int sheetX, int sheetY, 
                                  int width, int height, bool interactable, const char* name) {
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
        RegisterObject(id, sheetName, x, y, w, h, interactable, nullptr);
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

void ObjectManager::DestroyObjectGrid(int gridId) {
    auto it = grids.find(gridId);
    if (it != grids.end()) {
        delete it->second;
        grids.erase(it);
    }
}

void ObjectManager::DestroyAllGrids() {
    for (auto& pair : grids) {
        delete pair.second;
    }
    grids.clear();
    nextGridId = 1;
}

