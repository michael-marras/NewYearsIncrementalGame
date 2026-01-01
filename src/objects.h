#ifndef OBJECTS_H
#define OBJECTS_H

#include <string>
#include <unordered_map>
#include <vector>

// Object information structure
struct ObjectInfo {
    int id;                    // Unique object ID
    std::string name;          // Human-readable name (e.g., "tree", "rock", "chest")
    std::string sheetName;     // Which sprite sheet it's in
    int sheetX;                // X position in sprite sheet (in pixels)
    int sheetY;                // Y position in sprite sheet (in pixels)
    int width;                 // Object width
    int height;                // Object height
    bool interactable;         // Can player interact with this object?
    // Add more properties as needed (collision, interaction type, etc.)
};

// Object grid structure for game maps
struct ObjectGrid {
    int width;
    int height;
    std::vector<int> objects;  // 0 = no object, otherwise object ID
    
    ObjectGrid(int w, int h) : width(w), height(h) {
        objects.resize(w * h, 0);  // Initialize all to 0 (no object)
    }
    
    int GetObject(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return 0;
        return objects[y * width + x];
    }
    
    void SetObject(int x, int y, int objectId) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        objects[y * width + x] = objectId;
    }
    
    void ClearObject(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        objects[y * width + x] = 0;
    }
    
    bool IsValid(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
    
    bool HasObject(int x, int y) const {
        return IsValid(x, y) && GetObject(x, y) != 0;
    }
};

// Object manager to handle object definitions and lookups
class ObjectManager {
public:
    ObjectManager();
    ~ObjectManager();
    
    // Register an object with an optional custom name
    void RegisterObject(int id, const char* sheetName, int sheetX, int sheetY, 
                       int width, int height, bool interactable = false, const char* name = nullptr);
    
    // Register multiple objects from a sprite sheet at once
    void RegisterObjectsFromSheet(const char* sheetName, const int* objectData, int count, bool interactable = false);
    
    // Auto-generate objects from a grid layout
    int RegisterObjectsFromGrid(const char* sheetName, int cols, int rows, 
                               int startX = 0, int startY = 0, int startId = 0, 
                               int objWidth = 16, int objHeight = 16, bool interactable = false);
    
    // Get object info by ID
    ObjectInfo* GetObject(int id);
    
    // Get object info by name
    ObjectInfo* GetObjectByName(const char* name);
    
    // Check if object exists
    bool HasObject(int id);
    
    // Get total number of registered objects
    int GetObjectCount() const { return (int)objects.size(); }
    
    // Create an empty object grid
    int CreateObjectGrid(int width, int height);
    
    // Get object grid by ID
    ObjectGrid* GetObjectGrid(int gridId);
    
    // Destroy object grid
    void DestroyObjectGrid(int gridId);
    
    // Destroy all object grids
    void DestroyAllGrids();
    
    // Get number of object grids
    int GetGridCount() const { return (int)grids.size(); }

private:
    std::unordered_map<int, ObjectInfo> objects;           // Lookup by ID
    std::unordered_map<std::string, int> nameToId;         // Lookup by name
    std::unordered_map<int, ObjectGrid*> grids;            // Game maps/grids by ID
    int nextGridId = 1;                                    // Next available grid ID
};

#endif // OBJECTS_H

