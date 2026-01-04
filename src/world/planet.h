#ifndef PLANET_H
#define PLANET_H

#include <unordered_map>

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

// Get the radius (width/height in tiles) for a planet size
// Each face of the cube planet will be radius x radius tiles
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

private:
    std::unordered_map<PlanetFace, PlanetFaceData> faces;
};

#endif // PLANET_H

