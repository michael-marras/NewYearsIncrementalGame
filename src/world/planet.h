#ifndef PLANET_H
#define PLANET_H

#include <unordered_map>
#include "world/BigBangEngine.h"

// Forward declarations
struct SDL_Renderer;
struct SDL_Texture;
class TileManager;
class TextureManager;
class ObjectManager;
class ResourceManager;
class Player;

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
                         Player* player = nullptr);
    
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
     * Calculate the universe position for a child planet based on depth/index binary tree layout
     * @param parentDepth Depth of parent planet in tree (0 = root)
     * @param parentIndex Index of parent planet within its depth (0 to 2^depth - 1)
     * @param isLeftChild Whether this is the left child (true) or right child (false)
     * @param ringSpacing Distance between rings in universe units
     * @param outChildX Output parameter for child universe X position
     * @param outChildY Output parameter for child universe Y position
     */
    static void CalculateChildUniversePosition(int parentDepth, int parentIndex, bool isLeftChild, float ringSpacing, float& outChildX, float& outChildY);

private:
    std::unordered_map<PlanetFace, PlanetFaceData> faces;
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
    
public:
    BigBangEngine* GetPortalEngine() { return &portalEngine; }
};

#endif // PLANET_H

