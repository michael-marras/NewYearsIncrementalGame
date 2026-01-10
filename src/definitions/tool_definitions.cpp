#include "definitions/tool_definitions.h"

void SetupTools(ToolManager* toolManager, TextureManager* textureManager) {
    // Load tool sprite sheets (32x16 each: axes on left, picks on right)
    textureManager->LoadImageFromRes("wood_tools", "wood_tools.png");
    textureManager->LoadImageFromRes("stone_tools", "stone_tools.png");
    textureManager->LoadImageFromRes("iron_tools", "iron_tools.png");
    textureManager->LoadImageFromRes("gold_tools", "gold_tools.png");

    // Format: RegisterTool(id, sheet, x, y, width, height, type, damage, speed_ms, name)
    // damage: damage per hit (higher = more damage)
    // speed: cooldown in milliseconds between hits (lower = faster mining)
    // Each sheet is 32x16: axes at x=0-15, picks at x=16-31
    
    // Wood tools
    toolManager->RegisterTool(1, "wood_tools", 0, 0, 16, 16, "axe", 25, 200, "Wood Axe");
    toolManager->RegisterTool(2, "wood_tools", 16, 0, 16, 16, "pickaxe", 30, 180, "Wood Pickaxe");
    
    // Stone tools
    toolManager->RegisterTool(3, "stone_tools", 0, 0, 16, 16, "axe", 40, 180, "Stone Axe");
    toolManager->RegisterTool(4, "stone_tools", 16, 0, 16, 16, "pickaxe", 50, 150, "Stone Pickaxe");
    
    // Iron tools
    toolManager->RegisterTool(5, "iron_tools", 0, 0, 16, 16, "axe", 60, 150, "Iron Axe");
    toolManager->RegisterTool(6, "iron_tools", 16, 0, 16, 16, "pickaxe", 75, 120, "Iron Pickaxe");
    
    // Gold tools
    toolManager->RegisterTool(7, "gold_tools", 0, 0, 16, 16, "axe", 85, 120, "Gold Axe");
    toolManager->RegisterTool(8, "gold_tools", 16, 0, 16, 16, "pickaxe", 100, 100, "Gold Pickaxe");

}

