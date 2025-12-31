#ifndef TILES_H
#define TILES_H

#include <string>
#include <unordered_map>
#include <vector>

// Tile information structure
struct TileInfo {
    int id;                    // Unique tile ID
    std::string name;          // Human-readable name (e.g., "grass", "stone")
    std::string sheetName;     // Which sprite sheet it's in
    int sheetX;                // X position in sprite sheet (in pixels)
    int sheetY;                // Y position in sprite sheet (in pixels)
    int width;                 // Tile width
    int height;                // Tile height
    bool walkable;             // Optional: can player walk on this?
    // Add more properties as needed (collision, animation, etc.)
};

// Tile grid structure for game maps
struct TileGrid {
    int width;
    int height;
    std::vector<int> tiles;
    
    TileGrid(int w, int h) : width(w), height(h) {
        tiles.resize(w * h, 0);
    }
    
    int GetTile(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return 0;
        return tiles[y * width + x];
    }
    
    void SetTile(int x, int y, int tileId) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        tiles[y * width + x] = tileId;
    }
    
    bool IsValid(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
};

// Tile manager to handle tile definitions and lookups
class TileManager {
public:
    TileManager();
    ~TileManager();
    
    // Simplified: Register a tile with default 16x16 size (no name needed)
    void RegisterTile(int id, const char* sheetName, int sheetX, int sheetY, bool walkable = true);
    
    // Register multiple tiles from a sprite sheet at once
    void RegisterTilesFromSheet(const char* sheetName, const int* tileData, int count, bool walkable = true);
    
    // Auto-generate tiles from a grid layout
    int RegisterTilesFromGrid(const char* sheetName, int cols, int rows, 
                              int startX = 0, int startY = 0, int startId = 0, bool walkable = true);
    
    // Get tile info by ID
    TileInfo* GetTile(int id);
    
    // Get tile info by name
    TileInfo* GetTileByName(const char* name);
    
    // Check if tile exists
    bool HasTile(int id);
    
    // Get total number of registered tiles
    int GetTileCount() const { return (int)tiles.size(); }
    
    int CreateTileGrid(int width, int height);
    TileGrid* GetTileGrid(int gridId);
    void DestroyTileGrid(int gridId);
    void DestroyAllGrids();
    int GetGridCount() const { return (int)grids.size(); }

private:
    std::unordered_map<int, TileInfo> tiles;           // Lookup by ID
    std::unordered_map<std::string, int> nameToId;     // Lookup by name
    std::unordered_map<int, TileGrid*> grids;           // Game maps/grids by ID
    int nextGridId = 1;                                 // Next available grid ID
};

#endif // TILES_H

