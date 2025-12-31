#include "tiles.h"
#include <cstdio>
#include <cstdlib>

TileManager::TileManager() {
}

TileManager::~TileManager() {
    DestroyAllGrids();
}

void TileManager::RegisterTile(int id, const char* sheetName, int sheetX, int sheetY, bool walkable) {
    char name[32];
    snprintf(name, sizeof(name), "tile_%d", id);
    
    // Create tile info directly
    TileInfo tile;
    tile.id = id;
    tile.name = name;
    tile.sheetName = sheetName;
    tile.sheetX = sheetX;
    tile.sheetY = sheetY;
    tile.width = 16;
    tile.height = 16;
    tile.walkable = walkable;
    
    tiles[id] = tile;
    nameToId[name] = id;
}

void TileManager::RegisterTilesFromSheet(const char* sheetName, const int* tileData, int count, bool walkable) {
    for (int i = 0; i < count; i++) {
        int id = tileData[i * 3 + 0];
        int x = tileData[i * 3 + 1];
        int y = tileData[i * 3 + 2];
        RegisterTile(id, sheetName, x, y, walkable);
    }
}

int TileManager::RegisterTilesFromGrid(const char* sheetName, int cols, int rows, 
                                       int startX, int startY, int startId, bool walkable) {
    int tileId = startId;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int x = startX + (col * 16);  // 16 = TILE_SIZE
            int y = startY + (row * 16);
            RegisterTile(tileId, sheetName, x, y, walkable);
            tileId++;
        }
    }
    return tileId;
}

TileInfo* TileManager::GetTile(int id) {
    auto it = tiles.find(id);
    if (it != tiles.end()) {
        return &it->second;
    }
    return nullptr;
}

TileInfo* TileManager::GetTileByName(const char* name) {
    auto it = nameToId.find(name);
    if (it != nameToId.end()) {
        return GetTile(it->second);
    }
    return nullptr;
}

bool TileManager::HasTile(int id) {
    return tiles.find(id) != tiles.end();
}

int TileManager::CreateTileGrid(int width, int height) {
    TileGrid* grid = new TileGrid(width, height);
    
    // Random tile IDs to choose from
    const int tileOptions[] = {4, 5, 6, 11, 12, 13, 18, 19, 20, 25, 26};
    const int numOptions = sizeof(tileOptions) / sizeof(tileOptions[0]);
    
    // Fill grid with random tiles
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int randomIndex = std::rand() % numOptions;
            grid->SetTile(x, y, tileOptions[randomIndex]);
        }
    }
    
    int gridId = nextGridId++;
    grids[gridId] = grid;
    return gridId;
}

TileGrid* TileManager::GetTileGrid(int gridId) {
    auto it = grids.find(gridId);
    if (it != grids.end()) {
        return it->second;
    }
    return nullptr;
}

void TileManager::DestroyTileGrid(int gridId) {
    auto it = grids.find(gridId);
    if (it != grids.end()) {
        delete it->second;
        grids.erase(it);
    }
}

void TileManager::DestroyAllGrids() {
    for (auto& pair : grids) {
        delete pair.second;
    }
    grids.clear();
    nextGridId = 1;
}

