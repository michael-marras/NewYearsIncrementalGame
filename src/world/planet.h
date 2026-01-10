#ifndef PLANET_H
#define PLANET_H

#include <unordered_map>
#include "world/BigBangEngine.h"
#include <entities/player.h>

// Forward declarations
struct SDL_Renderer;
struct SDL_Texture;
class TileManager;
class TextureManager;
class ObjectManager;
class ResourceManager;
class Player;
class ObjectNodeManager;

// Note: PlanetFace and PlanetBiome are defined in object_node.h to avoid circular dependency
// We need to include object_node.h before using these enums in implementations

// Enum for the 6 faces of a cube planet
enum class PlanetFace {
    FRONT,
    BACK,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

// Enum for planet size (radius in tiles for each face)
enum class PlanetSize {
    TINY,
    SMALL,
    MEDIUM,
    LARGE,
    EXTRA_LARGE,
    GIANT
};

// Enum for planet biomes (visual/seasonal theme)
enum class PlanetBiome {
    WINTER,
    SUMMER
};

// Enum for planet types (resource/object spawn characteristics)
enum class PlanetType {
    TREE_PLANET,
    IRON_PLANET,
    GOLD_PLANET,
    ROCK_PLANET
};

// Get the radius (in tiles) for a planet size
// Each face of the cube planet will be (radius * 2) x (radius * 2) tiles
int GetPlanetRadius(PlanetSize size);

// Data structure for each face of a planet
struct PlanetFaceData {
    int tileGridId;
    int objectGridId;
    int resourceArrayId;
    
    PlanetFaceData() : tileGridId(-1), objectGridId(-1), resourceArrayId(-1) {}
    PlanetFaceData(int tileId, int objId, int resId) 
        : tileGridId(tileId), objectGridId(objId), resourceArrayId(resId) {}
};

// Planet class to manage a cube planet with 6 faces
class Planet {
public:
    Planet();
    ~Planet();
    
    /**
     * Get face data for a specific face
     * Returns nullptr if face doesn't exist
     */
    PlanetFaceData* GetFaceData(PlanetFace face);
    const PlanetFaceData* GetFaceData(PlanetFace face) const;
    
    /**
     * Set face data for a specific face
     */
    void SetFaceData(PlanetFace face, int tileGridId, int objectGridId, int resourceArrayId);
    void SetFaceData(PlanetFace face, const PlanetFaceData& data);
    
    /**
     * Check if a face has been initialized
     */
    bool HasFace(PlanetFace face) const;
    
    /**
     * Get tile grid ID for a face
     */
    int GetTileGridId(PlanetFace face) const;
    
    /**
     * Get object grid ID for a face
     */
    int GetObjectGridId(PlanetFace face) const;
    
    /**
     * Get resource array ID for a face
     */
    int GetResourceArrayId(PlanetFace face) const;

    /**
     * Get planet's current energy
     */
    float GetCurrentEnergy() const;

    /**
     * Get cost of generating one child planet
     */
    float GetEnergyCost() const;
    
    /**
     * Set cost of generating one child planet
     */
    void SetEnergyCost();

    /**
     * Add energy when feeding resources
     */
    void AddEnergy(float amount);

    /**
     * Check if generated children is less than 2
     */
    bool CanGenerateChild() const;

    /**
     * True if this planet can still create more children (independent of energy amount).
     */
    bool HasChildCapacity() const;

    /**
     * Spend energy generating child planet
     */
    void ConsumeEnergyForChild();

    /**
     * Make child planet (consumes energy, increments counter)
     * Returns true if child was generated, false otherwise
     * Note: Actual planet creation happens in caller (needs tree/managers)
     */
    bool GenerateChild();

    /**
     * Get tier in the planet tree (0 = root)
     */
    int GetTier() const;
    
    /**
     * Set tier in the planet tree
     */
    void SetTier(int tierValue);

    /**
     * Get planet biome (WINTER or SUMMER)
     */
    PlanetBiome GetPlanetBiome() const;
    
    /**
     * Set planet biome (WINTER or SUMMER)
     */
    void SetPlanetBiome(PlanetBiome biome);
    
    /**
     * Get planet type (TREE_PLANET, IRON_PLANET, or GOLD_PLANET)
     */
    PlanetType GetPlanetType() const;
    
    /**
     * Set planet type (TREE_PLANET, IRON_PLANET, or GOLD_PLANET)
     */
    void SetPlanetType(PlanetType type);

    /**
     * Get universe X position (where this planet is in the universe)
     */
    float GetUniverseX() const;
    
    /**
     * Get universe Y position (where this planet is in the universe)
     */
    float GetUniverseY() const;
    
    /**
     * Set universe position (where this planet is in the universe)
     */
    void SetUniversePosition(float universeX, float universeY);
    
    /**
     * Convert local coordinates (on this planet) to universe coordinates
     */
    void LocalToUniverse(float localX, float localY, float& universeX, float& universeY) const;
    
    /**
     * Convert universe coordinates to local coordinates (on this planet)
     */
    void UniverseToLocal(float universeX, float universeY, float& localX, float& localY) const;

    void setRadius(int rad);

    int getRadius() const;

    void RenderToTexture(SDL_Renderer* renderer, 
                         TileManager* tileManager, 
                         TextureManager* textureManager,
                         ObjectManager* objectManager = nullptr,
                         ResourceManager* resourceManager = nullptr,
                         Player* player = nullptr,
                         float deltaTime = 0.0f,
                         int playerCurrentFace = -1);
    
    /**
     * Get the cached texture for rendering in GodState
     * Returns nullptr if texture hasn't been rendered yet
     */
    SDL_Texture* GetCachedTexture() const { return cachedTexture; }
    
    /**
     * Mark planet as dirty
     */
    void MarkDirty() { isDirty = true; }
    
    /**
     * Check if planet is dirty
     */
    bool IsDirty() const { return isDirty; }
    
    /**
     * Get object node manager for a specific face (creates one if it doesn't exist)
     */
    ObjectNodeManager* GetOrCreateObjectNodeManager(PlanetFace face);
    
    /**
     * Get object node manager for a specific face (returns nullptr if doesn't exist)
     */
    ObjectNodeManager* GetObjectNodeManager(PlanetFace face);
    const ObjectNodeManager* GetObjectNodeManager(PlanetFace face) const;
    
    /**
     * Update all object nodes on a specific face (call each frame)
     */
    void UpdateObjectNodes(PlanetFace face, float deltaTime, ObjectManager* objectManager);
    
    /**
     * Update all object nodes on all faces (call each frame)
     */
    void UpdateAllObjectNodes(float deltaTime, ObjectManager* objectManager);
    
    /**
     * Calculate the universe position for a child planet based on depth/index binary tree layout
     * @param parentDepth Depth of parent planet in tree (0 = root)
     * @param parentIndex Index of parent planet within its depth (0 to 2^depth - 1)
     * @param isLeftChild Whether this is the left child (true) or right child (false)
     * @param ringSpacing Distance between rings in universe units
     * @param outChildX Output parameter for child universe X position
     * @param outChildY Output parameter for child universe Y position
     */
    static void CalculateChildUniversePosition(int parentDepth, int parentIndex, bool isLeftChild, float ringSpacing, float& outChildX, float& outChildY);

    /**
     * Add new NPCs to a planet
     */
    void AddNPCs(int numNPCs, float x, float y);

private:
    std::unordered_map<PlanetFace, PlanetFaceData> faces;
    std::unordered_map<PlanetFace, ObjectNodeManager*> objectNodeManagers;
    PlanetBiome planetBiome = PlanetBiome::WINTER;
    PlanetType planetType = PlanetType::TREE_PLANET;
    int tier = 0;
    int radius = 0;
    float currentEnergy = 0.0f;
    float energyCost = 0.0f;
    int childrenGenerated = 0;
    BigBangEngine portalEngine;

    // Texture used for god state view
    SDL_Texture* cachedTexture = nullptr;
    int cachedTextureWidth = 0;
    int cachedTextureHeight = 0;
    bool isDirty = false;  // Set to true when planet needs to be re-rendered to texture
    
    // Universe position (where this planet exists in the universe)
    float universeX = 0.0f;
    float universeY = 0.0f;

    // NPC's
    std::vector<Player*> NPCs = {};
    
public:
    BigBangEngine* GetPortalEngine() { return &portalEngine; }
};

#endif // PLANET_H

