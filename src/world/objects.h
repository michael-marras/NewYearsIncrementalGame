#ifndef OBJECTS_H
#define OBJECTS_H

#include <string>
#include <unordered_map>
#include <vector>

// Forward declaration
class Player;

struct DropInstance {
    int resourceId;
    int quantity;
};
// Object information structure (template/definition data - shared by all instances)
struct ObjectInfo {
    int id;
    std::string name;
    std::string sheetName;
    int sheetX;
    int sheetY;
    int width;
    int height;
    bool interactable;
    int maxHealth = -1;
    std::string death_replacement = "";
    std::vector<DropInstance> drops;
};

// Key for identifying object instances (grid position)
struct ObjectInstanceKey {
    int gridId;
    int x, y;
    
    bool operator==(const ObjectInstanceKey& other) const {
        return gridId == other.gridId && x == other.x && y == other.y;
    }
};

// Hash function for ObjectInstanceKey
struct ObjectInstanceKeyHash {
    std::size_t operator()(const ObjectInstanceKey& key) const {
        // Combine hash of gridId, x, y
        return std::hash<int>()(key.gridId) ^ 
               (std::hash<int>()(key.x) << 1) ^ 
               (std::hash<int>()(key.y) << 2);
    }
};

// Instance data for individual objects (per-instance state)
struct ObjectInstance {
    int currentHealth;
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
                       int width, int height, bool interactable = false, const char* name = nullptr,
                       int maxHealth = -1, const char* replacement = "",
                       const std::vector<DropInstance>& drops = {});
    
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
    
    // Get object info at a grid position (returns nullptr if no object at position)
    ObjectInfo* GetObjectAt(int gridId, int x, int y);
    
    /**
     * Replace object at a grid position with a new object
     * Removes old instance data and sets new object (new object starts fresh)
     * Returns true if replacement succeeded, false if position is invalid
     */
    bool ReplaceObjectAt(int gridId, int x, int y, int newObjectId);

    // Destroy object grid
    void DestroyObjectGrid(int gridId);
    
    // Destroy all object grids
    void DestroyAllGrids();
    
    // Get number of object grids
    int GetGridCount() const { return (int)grids.size(); }
    
    // Instance data management (health/durability)
    
    /**
     * Get or create instance data for an object at a grid position
     * If instance doesn't exist, creates it with full health based on ObjectInfo
     */
    ObjectInstance* GetOrCreateInstance(int gridId, int x, int y);
    
    /**
     * Get instance data for an object at a grid position (returns nullptr if not found)
     */
    ObjectInstance* GetInstance(int gridId, int x, int y) const;

    /**
     * Check if the object if a player can interact with the object
     */
    bool PlayerCanInteract(int gridId, int x, int y, Player* player);
    
    /**
     * Set instance health directly
     */
    void SetInstanceHealth(int gridId, int x, int y, int health);
    
    /**
     * Damage an object instance (returns true if object was destroyed)
     * When destroyed, replaces with object specified in death_replacement field (empty string = remove)
     */
    bool DamageInstance(int gridId, int x, int y, int damage);
    
    /**
     * Get current health of an object instance (returns -1 if not found or indestructible)
     */
    int GetInstanceHealth(int gridId, int x, int y) const;
    
    /**
     * Remove instance data (when object is destroyed/removed)
     */
    void RemoveInstance(int gridId, int x, int y);
    
    /**
     * Clear all instances for a grid (call when destroying grid)
     */
    void ClearInstances(int gridId);

private:
    std::unordered_map<int, ObjectInfo> objects;           // Lookup by ID
    std::unordered_map<std::string, int> nameToId;         // Lookup by name
    std::unordered_map<int, ObjectGrid*> grids;            // Game maps/grids by ID
    std::unordered_map<ObjectInstanceKey, ObjectInstance, ObjectInstanceKeyHash> instances;  // Instance data
    int nextGridId = 1;                                    // Next available grid ID
};

#endif // OBJECTS_H

