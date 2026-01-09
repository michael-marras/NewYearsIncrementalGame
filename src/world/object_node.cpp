#include "object_node.h"
#include "objects.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ObjectNode::ObjectNode() 
    : type(ObjectNodeType::TREE_NODE),
      nodeId(0),
      face(PlanetFace::TOP),
      x(0), y(0),
      range(10.0f),
      spawnRate(1.0f),
      energy(0.0f),
      maxEnergy(100.0f),
      energyRegenRate(1.0f),
      maxObjects(5),
      currentObjectCount(0),
      spawnCooldown(0.0f),
      baseSpawnCooldown(1.0f),
      active(true),
      depleted(false) {
}

void ObjectNode::Initialize(ObjectNodeType nodeType, PlanetFace planetFace, int posX, int posY,
                            float spawnRange, float objectsPerSecond, float initialEnergy,
                            float maxEnergyCapacity, float energyRegen, int maxSpawnedObjects) {
    type = nodeType;
    face = planetFace;
    x = posX;
    y = posY;
    range = spawnRange;
    spawnRate = objectsPerSecond;
    energy = initialEnergy;
    maxEnergy = maxEnergyCapacity;
    energyRegenRate = energyRegen;
    maxObjects = maxSpawnedObjects;
    currentObjectCount = 0;
    baseSpawnCooldown = (spawnRate > 0.0f) ? (1.0f / spawnRate) : 1.0f;
    spawnCooldown = 0.0f;
    active = true;
    depleted = false;
}

void ObjectNode::Update(float deltaTime) {
    if (!active || depleted) {
        return;
    }
    
    // Regenerate energy
    energy = std::min(energy + (energyRegenRate * deltaTime), maxEnergy);
    
    // Update spawn cooldown
    if (spawnCooldown > 0.0f) {
        spawnCooldown -= deltaTime;
        if (spawnCooldown < 0.0f) {
            spawnCooldown = 0.0f;
        }
    }
    
    // Reset depleted state if energy is sufficient
    if (depleted && energy >= maxEnergy * 0.5f) {
        depleted = false;
    }
}

int ObjectNode::TrySpawn(ObjectManager* objectManager, int objectGridId, int planetWidth, int planetHeight, PlanetBiome biome) {
    if (!active || depleted || !objectManager) {
        return 0;
    }
    
    // Check cooldown
    if (spawnCooldown > 0.0f) {
        return 0;
    }
    
    // Check if we're at max objects
    if (currentObjectCount >= maxObjects) {
        return 0;
    }
    
    ObjectGrid* grid = objectManager->GetObjectGrid(objectGridId);
    if (!grid) {
        return 0;
    }
    
    float spawnEnergyCost = 10.0f;
    int objectsNeeded = maxObjects - currentObjectCount;
    int energyAffordable = static_cast<int>(energy / spawnEnergyCost);
    
    // Aim for 3-8 objects per batch depending on spawn rate
    int batchSize = static_cast<int>(spawnRate * 3.0f) + 2;  // 5-8 objects for spawnRate 1.0-2.0
    if (batchSize < 3) batchSize = 3;
    if (batchSize > 10) batchSize = 10;
    
    // Limit batch size by available capacity and energy
    if (batchSize > objectsNeeded) batchSize = objectsNeeded;
    if (batchSize > energyAffordable) batchSize = energyAffordable;
    
    if (batchSize <= 0 || energy < spawnEnergyCost) {
        return 0;
    }
    
    // Get object info once (all spawns will use the same type)
    std::string objectName = GetSpawnObjectName(biome);
    ObjectInfo* objInfo = objectManager->GetObjectByName(objectName.c_str());
    if (!objInfo) {
        return 0;
    }
    
    // Try to spawn multiple objects in this batch
    int objectsSpawned = 0;
    int maxAttempts = batchSize * 15;  // More attempts for larger batches
    
    for (int attempt = 0; attempt < maxAttempts && objectsSpawned < batchSize; attempt++) {
        // Generate random position within range
        float angle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f * M_PI;
        float distance = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
        
        int spawnX = x + static_cast<int>(std::cos(angle) * distance);
        int spawnY = y + static_cast<int>(std::sin(angle) * distance);
        
        // Check bounds
        if (spawnX < 0 || spawnX >= planetWidth || spawnY < 0 || spawnY >= planetHeight) {
            continue;
        }
        
        // Check if position is already occupied
        if (grid->HasObject(spawnX, spawnY)) {
            continue;
        }
        
        // Spawn the object
        grid->SetObject(spawnX, spawnY, objInfo->id);
        objectsSpawned++;
        currentObjectCount++;
    }
    
    // Consume energy for all spawned objects
    float totalEnergyCost = objectsSpawned * spawnEnergyCost;
    energy -= totalEnergyCost;
    if (energy < 0.0f) {
        energy = 0.0f;
    }
    
    // Set cooldown (longer for larger batches to maintain spawn rate)
    if (objectsSpawned > 0) {
        spawnCooldown = baseSpawnCooldown * (1.0f + objectsSpawned * 0.2f);  // Slightly longer cooldown for batches
    }
    
    // Check if depleted
    if (energy < spawnEnergyCost) {
        depleted = true;
    }
    
    return objectsSpawned;
}

void ObjectNode::SpawnInitialObjects(ObjectManager* objectManager, int objectGridId, int planetWidth, int planetHeight, PlanetBiome biome) {
    if (!active || !objectManager) {
        return;
    }
    
    ObjectGrid* grid = objectManager->GetObjectGrid(objectGridId);
    if (!grid) {
        return;
    }
    
    int attemptsPerObject = 50;
    int objectsSpawned = 0;
    
    // Get object name to spawn
    std::string objectName = GetSpawnObjectName(biome);
    ObjectInfo* objInfo = objectManager->GetObjectByName(objectName.c_str());
    if (!objInfo) {
        return;
    }
    
    // Count existing objects in range first
    int existingCount = 0;
    int rangeInt = static_cast<int>(range);
    for (int dy = -rangeInt; dy <= rangeInt; dy++) {
        for (int dx = -rangeInt; dx <= rangeInt; dx++) {
            int checkX = x + dx;
            int checkY = y + dy;
            if (grid->IsValid(checkX, checkY) && IsInRange(checkX, checkY)) {
                if (grid->HasObject(checkX, checkY)) {
                    existingCount++;
                }
            }
        }
    }
    
    // Spawn objects to fill up to maxObjects
    int objectsNeeded = maxObjects - existingCount;
    if (objectsNeeded <= 0) {
        currentObjectCount = existingCount;
        return;
    }
    
    for (int i = 0; i < objectsNeeded; i++) {
        bool spawned = false;
        
        // Try multiple random positions within range
        for (int attempt = 0; attempt < attemptsPerObject && !spawned; attempt++) {
            // Generate random position within range
            float angle = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f * M_PI;
            float distance = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * range;
            
            int spawnX = x + static_cast<int>(std::cos(angle) * distance);
            int spawnY = y + static_cast<int>(std::sin(angle) * distance);
            
            // Check bounds
            if (spawnX < 0 || spawnX >= planetWidth || spawnY < 0 || spawnY >= planetHeight) {
                continue;
            }
            
            // Check if position is already occupied
            if (grid->HasObject(spawnX, spawnY)) {
                continue;
            }
            
            // Spawn the object
            grid->SetObject(spawnX, spawnY, objInfo->id);
            spawned = true;
            objectsSpawned++;
        }
    }
    
    // Update current object count
    currentObjectCount = existingCount + objectsSpawned;
    
    // Set initial energy to be enough for future spawns (start with max energy)
    energy = maxEnergy;
}

std::string ObjectNode::GetSpawnObjectName(PlanetBiome biome) const {
    bool isSummer = (biome == PlanetBiome::SUMMER);
    
    switch (type) {
        case ObjectNodeType::TREE_NODE:
            return isSummer ? "summer_tree" : "winter_tree";
        case ObjectNodeType::ROCK_NODE:
            return isSummer ? "summer_medium_rock" : "winter_medium_rock";
        case ObjectNodeType::IRON_NODE:
            return isSummer ? "summer_medium_iron_rock" : "winter_medium_iron_rock";
        case ObjectNodeType::GOLD_NODE:
            return isSummer ? "summer_medium_gold_rock" : "winter_medium_gold_rock";
        default:
            return isSummer ? "summer_tree" : "winter_tree";
    }
}

bool ObjectNode::IsInRange(int checkX, int checkY) const {
    float dist = DistanceTo(checkX, checkY);
    return dist <= range;
}

float ObjectNode::DistanceTo(int checkX, int checkY) const {
    int dx = checkX - x;
    int dy = checkY - y;
    return std::sqrt(static_cast<float>(dx * dx + dy * dy));
}

// ObjectNodeManager implementation

ObjectNodeManager::ObjectNodeManager() {
}

ObjectNodeManager::~ObjectNodeManager() {
    Clear();
}

void ObjectNodeManager::AddNode(const ObjectNode& node) {
    ObjectNode newNode = node;
    newNode.nodeId = nextNodeId++;
    nodes.push_back(newNode);
}

void ObjectNodeManager::RemoveNode(int nodeId) {
    nodes.erase(
        std::remove_if(nodes.begin(), nodes.end(),
            [nodeId](const ObjectNode& node) { return node.nodeId == nodeId; }),
        nodes.end()
    );
}

ObjectNode* ObjectNodeManager::GetNode(int nodeId) {
    for (auto& node : nodes) {
        if (node.nodeId == nodeId) {
            return &node;
        }
    }
    return nullptr;
}

std::vector<ObjectNode*> ObjectNodeManager::GetNodesByType(ObjectNodeType type) {
    std::vector<ObjectNode*> result;
    for (auto& node : nodes) {
        if (node.type == type) {
            result.push_back(&node);
        }
    }
    return result;
}

std::vector<ObjectNode*> ObjectNodeManager::GetNodesInRange(PlanetFace face, int x, int y, float range) {
    std::vector<ObjectNode*> result;
    for (auto& node : nodes) {
        if (node.face == face && node.IsInRange(x, y)) {
            float dist = node.DistanceTo(x, y);
            if (dist <= range) {
                result.push_back(&node);
            }
        }
    }
    return result;
}

void ObjectNodeManager::UpdateAll(float deltaTime, ObjectManager* objectManager, int objectGridId,
                                  int planetWidth, int planetHeight, PlanetBiome biome) {
    for (auto& node : nodes) {
        node.Update(deltaTime);
        
        // Count current objects in range (this is a simple approximation)
        // In a full implementation, you'd want to track this more precisely
        ObjectGrid* grid = objectManager ? objectManager->GetObjectGrid(objectGridId) : nullptr;
        if (grid) {
            int count = 0;
            int rangeInt = static_cast<int>(node.range);
            for (int dy = -rangeInt; dy <= rangeInt; dy++) {
                for (int dx = -rangeInt; dx <= rangeInt; dx++) {
                    int checkX = node.x + dx;
                    int checkY = node.y + dy;
                    if (grid->IsValid(checkX, checkY) && node.IsInRange(checkX, checkY)) {
                        if (grid->HasObject(checkX, checkY)) {
                            count++;
                        }
                    }
                }
            }
            node.currentObjectCount = count;
        }
        
        // Try to spawn (pass biome parameter)
        node.TrySpawn(objectManager, objectGridId, planetWidth, planetHeight, biome);
    }
}

void ObjectNodeManager::Clear() {
    nodes.clear();
    nextNodeId = 1;
}

