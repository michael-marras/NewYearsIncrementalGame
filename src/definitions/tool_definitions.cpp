#include "definitions/tool_definitions.h"

void SetupTools(ToolManager* toolManager, TextureManager* textureManager) {
    // Load tool sprite sheets
    textureManager->LoadImageFromRes("tools_sheet", "tools_sheet.png");

    // Format: RegisterTool(id, sheet, x, y, width, height, type, damage, speed_ms, name)
    // damage: damage per hit (higher = more damage)
    // speed: cooldown in milliseconds between hits (lower = faster mining)
    toolManager->RegisterTool(1, "tools_sheet", 0, 0, 16, 16, "axe", 25, 200, "Wood Axe");
    toolManager->RegisterTool(2, "tools_sheet", 16, 0, 16, 16, "pickaxe", 30, 150, "Iron Pickaxe");

}

