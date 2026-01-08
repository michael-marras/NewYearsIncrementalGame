#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <unordered_map>

// Tool information structure (template/definition data - shared by all instances)
struct ToolInfo {
    int id;
    std::string name;
    std::string sheetName;
    int sheetX;
    int sheetY;
    int width;
    int height;
    std::string type;
    int damage;
    int speed;
};

class ToolManager {
public:
    ToolManager();
    ~ToolManager();

    // Register a tool type
    void RegisterTool(int id, const char* sheetName, int sheetX, int sheetY,
                     int width, int height, const char* type, int damage, int speed,
                     const char* name = nullptr);

    // Get tool info by ID
    ToolInfo* GetTool(int id);

    // Get tool info by name
    ToolInfo* GetToolByName(const char* name);

    // Check if tool type exists
    bool HasTool(int id);

    // Get total number of registered tool types
    int GetToolCount() const { return (int)toolTypes.size(); }

private:
    std::unordered_map<int, ToolInfo> toolTypes;      // Tool definitions by ID
    std::unordered_map<std::string, int> nameToId;    // Lookup by name
};

#endif // TOOLS_H
