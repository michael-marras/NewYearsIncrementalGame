#include "items/tools.h"
#include <cstdio>
#include <cstring>

ToolManager::ToolManager() {
}

ToolManager::~ToolManager() {
}

void ToolManager::RegisterTool(int id, const char* sheetName, int sheetX, int sheetY,
                               int width, int height, const char* type, int damage, int speed,
                               const char* name) {
    std::string toolName;

    // Use provided name, or generate default name if not provided
    if (name && name[0] != '\0') {
        toolName = name;
    } else {
        char defaultName[32];
        snprintf(defaultName, sizeof(defaultName), "tool_%d", id);
        toolName = defaultName;
    }

    // Create Tool Info
    ToolInfo tool;
    tool.id = id;
    tool.name = toolName;
    tool.sheetName = sheetName;
    tool.sheetX = sheetX;
    tool.sheetY = sheetY;
    tool.width = width;
    tool.height = height;
    tool.type = type;
    tool.damage = damage;
    tool.speed = speed;

    toolTypes[id] = tool;
    nameToId[toolName] = id;
}

ToolInfo* ToolManager::GetTool(int id) {
    auto it = toolTypes.find(id);
    if (it != toolTypes.end()) {
        return &it->second;
    }
    return nullptr;
}

ToolInfo* ToolManager::GetToolByName(const char* name) {
    auto it = nameToId.find(name);
    if (it != nameToId.end()) {
        return GetTool(it->second);
    }
    return nullptr;
}

bool ToolManager::HasTool(int id) {
    return toolTypes.find(id) != toolTypes.end();
}

