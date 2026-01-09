#ifndef OBJECT_NODE_H
#define OBJECT_NODE_H

#include <vector>
#include <string>

// Forward declarations
class Planet;
class ObjectManager;

// Include planet.h to get PlanetFace and PlanetBiome enum definitions
// Note: planet.h should NOT include object_node.h to avoid circular dependency
// Instead, planet.cpp will include both headers
#include "planet.h"

// Enum for object node types
enum class ObjectNodeType {
    TREE_NODE,
    ROCK_NODE,
    IRON_NODE,
    GOLD_NODE
};

// Structure representing an object node that can regenerate objects
struct ObjectNode {
    // Node identity
    ObjectNodeType type;
    int nodeId;  // Unique ID for this node
    
    // Position on planet face
    PlanetFace face;
    int x, y;  // Position in tile coordinates on the face
    
    // Spawn parameters
    float range;           // Spawn radius in tiles
    float spawnRate;       // Objects per second that can spawn
    float energy;          // Energy reserve for spawning objects
    float maxEnergy;       // Maximum energy capacity
    float energyRegenRate; // Energy regeneration per second
    
    // Spawn control
    int maxObjects;        // Maximum number of objects allowed in range
    int currentObjectCount; // Current number of objects spawned in range
    float spawnCooldown;   // Time until next spawn attempt (in seconds)
    float baseSpawnCooldown; // Base cooldown between spawn attempts
    
    // Node state
    bool active;           // Whether the node is active (can spawn)
    bool depleted;         // Whether the node is depleted (temporarily or permanently)
    
    // Constructor
    ObjectNode();
    
    // Initialize node with specific values
    void Initialize(ObjectNodeType nodeType, PlanetFace planetFace, int posX, int posY, 
                    float spawnRange, float objectsPerSecond, float initialEnergy, 
                    float maxEnergyCapacity, float energyRegen, int maxSpawnedObjects);
    
    // Update node (call each frame with deltaTime)
    void Update(float deltaTime);
    
    // Try to spawn objects within range (batch spawn)
    // Returns the number of objects successfully spawned (0 if no spawn attempted)
    int TrySpawn(ObjectManager* objectManager, int objectGridId, int planetWidth, int planetHeight, PlanetBiome biome);
    
    // Spawn initial objects to fill up the node (call after node creation)
    // Spawns objects up to maxObjects limit, ignoring cooldown and energy constraints
    void SpawnInitialObjects(ObjectManager* objectManager, int objectGridId, int planetWidth, int planetHeight, PlanetBiome biome);
    
    // Get the object name that this node spawns based on type and biome
    std::string GetSpawnObjectName(PlanetBiome biome) const;
    
    // Check if a position is within this node's range
    bool IsInRange(int checkX, int checkY) const;
    
    // Calculate distance from node center to a position
    float DistanceTo(int checkX, int checkY) const;
};

// Manager class for object nodes (one per planet face)
class ObjectNodeManager {
public:
    ObjectNodeManager();
    ~ObjectNodeManager();
    
    // Add a node to the manager
    void AddNode(const ObjectNode& node);
    
    // Remove a node by ID
    void RemoveNode(int nodeId);
    
    // Get a node by ID (returns nullptr if not found)
    ObjectNode* GetNode(int nodeId);
    
    // Get all nodes
    std::vector<ObjectNode>& GetNodes() { return nodes; }
    const std::vector<ObjectNode>& GetNodes() const { return nodes; }
    
    // Get nodes of a specific type
    std::vector<ObjectNode*> GetNodesByType(ObjectNodeType type);
    
    // Get nodes within range of a position
    std::vector<ObjectNode*> GetNodesInRange(PlanetFace face, int x, int y, float range);
    
    // Update all nodes (call each frame)
    void UpdateAll(float deltaTime, ObjectManager* objectManager, int objectGridId, 
                   int planetWidth, int planetHeight, PlanetBiome biome);
    
    // Clear all nodes
    void Clear();
    
    // Get total number of nodes
    int GetNodeCount() const { return (int)nodes.size(); }
    
private:
    std::vector<ObjectNode> nodes;
    int nextNodeId = 1;  // Next available node ID
};

#endif // OBJECT_NODE_H

