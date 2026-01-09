#ifndef RESOURCES_H
#define RESOURCES_H

#include <vector>
#include <string>
#include <unordered_map>

// Resource information structure (template/definition data - shared by all instances)
struct ResourceInfo {
    int id;
    std::string name;
    std::string sheetName;
    int sheetX;
    int sheetY;
    int width;
    int height;
    bool pickupable;
    float value;
    std::string displayName;
};

// Resource pickup stages
enum class ResourceState {
    SHOOT_OUT,
    IDLE,
    MAGNETIC
};

struct ResourceInstance {
    float x;
    float y;
    int resourceId;
    int quantity;
    float vx;
    float vy;
    ResourceState state = ResourceState::SHOOT_OUT;
    float shootOutTimer = 0.0f;
    float idleTimer = 0.0f;
};

// Resource array for resources on the map
struct ResourceArray {
    std::vector<ResourceInstance> resources;

    ResourceArray() = default;
};

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    // Register a resource type (like RegisterObject)
    void RegisterResource(int id, const char* sheetName, int sheetX, int sheetY,
                         int width, int height, bool pickupable = true, const char* name = nullptr, float value = 0.0f, const char* displayName = nullptr);

    // Get resource info by ID
    ResourceInfo* GetResource(int id);

    // Get resource info by name
    ResourceInfo* GetResourceByName(const char* name);

    // Check if resource type exists
    bool HasResource(int id);

    // Get total number of registered resource types
    int GetResourceCount() const { return (int)resourceTypes.size(); }

    // Get value of a resource
    float GetResourceValue(int resourceId) const;

    // Get value of a quantity of a resource
    float GetResourceValue(int resourceId, int quantity) const;

    // Create a resource array for a map
    int CreateResourceArray();

    // Get resource array by ID
    ResourceArray* GetResourceArray(int arrayId);

    // Add a resource instance to an array (when object is destroyed)
    void AddResource(int arrayId, float x, float y, int resourceId, int quantity = 1, float vx = 0.0f, float vy = 0.0f);

    // Find resource near a world position (for pickup)
    ResourceInstance* GetResourceNear(int arrayId, float worldX, float worldY, float pickupRange);

    // Pickup and remove a resource (returns quantity picked up)
    int PickupResource(int arrayId, float worldX, float worldY, float pickupRange);

    // Get all resources in an area (for rendering/culling)
    std::vector<ResourceInstance*> GetResourcesInArea(int arrayId, float minX, float minY, float maxX, float maxY);

    // Destroy resource array
    void DestroyResourceArray(int arrayId);

    // Destroy all resource arrays
    void DestroyAllArrays();

    // Update resources (falling animation and magnetic pickup)
    // Adds picked up resources to player inventory and logs pickup info
    // Optional HUD parameter to notify about pickups
    void Update(int arrayId, float playerX, float playerY, float deltaTimeMs, class Player* player, class HUD* hud = nullptr);

private:
    std::unordered_map<int, ResourceInfo> resourceTypes;      // Resource definitions by ID
    std::unordered_map<std::string, int> nameToId;            // Lookup by name
    std::unordered_map<int, ResourceArray*> resourceArrays;   // Resource arrays by map ID
    int nextArrayId = 1;                                      // Next available array ID
};

#endif // RESOURCES_H