#include "world/tiles.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>
#include <utility>

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

int TileManager::CreateTileGridwGroupingsSeeded(int width, int height, unsigned int seed, PlanetBiome planetBiome){
    std::srand(seed);
    TileGrid* grid = new TileGrid(width, height);

    int offset;
    if (planetBiome == PlanetBiome::SUMMER) {
        offset = 35;
    } else if (planetBiome == PlanetBiome::WINTER) {
        offset = 0;
    } else {
        offset = 0;
    }

    const int high[] = {6 + offset, 13 + offset, 20 + offset};
    const int med[] = {5 + offset, 12 + offset, 19 + offset};
    const int low[] = {4 + offset, 11 + offset, 18 + offset};
    const int numHigh = sizeof(high) / sizeof(high[0]);
    const int numMed = sizeof(med) / sizeof(med[0]);
    const int numLow = sizeof(low) / sizeof(low[0]);

    const int numPatches = (width * height) / 80;
    const float baseHighRadius = 1.0f;
    const float baseMedRadius = 2.0f;
    const float baseLowRadius = 3.0f;

    struct Patch {
        int centerX, centerY;
        float highRadius, medRadius, lowRadius;
        int noiseSeed;
    };
    std::vector<Patch> patches;
    
    for (int i = 0; i < numPatches; i++) {
        Patch patch;
        patch.centerX = std::rand() % width;
        patch.centerY = std::rand() % height;
        float sizeVariation = 0.7f + (std::rand() % 60) / 100.0f;
        patch.highRadius = baseHighRadius * sizeVariation;
        patch.medRadius = baseMedRadius * sizeVariation;
        patch.lowRadius = baseLowRadius * sizeVariation;
        patch.noiseSeed = std::rand();
        patches.push_back(patch);
    }

    // Simple noise function for organic shape variation
    auto getNoise = [](int x, int y, int seed) -> float {
        // Simple hash-based noise
        int n = (x * 73856093) ^ (y * 19349663) ^ (seed * 83492791);
        n = (n << 13) ^ n;
        return ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 2147483648.0f;
    };

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float bestDistance = baseLowRadius * 1.5f; 
            int bestPatchIndex = -1;
            
            for (size_t i = 0; i < patches.size(); i++) {
                const Patch& patch = patches[i];
                int dx = x - patch.centerX;
                int dy = y - patch.centerY;
                
                float euclideanDist = std::sqrt(dx * dx + dy * dy);
                float manhattanDist = std::abs(dx) + std::abs(dy);
                float mixedDist = euclideanDist * 0.7f + manhattanDist * 0.3f;
                
                float noiseAmount = 0.4f * (1.0f - euclideanDist / (patch.lowRadius * 2.0f));
                noiseAmount = noiseAmount > 0 ? noiseAmount : 0;
                float noise = getNoise(x, y, patch.noiseSeed) * noiseAmount;
                mixedDist += noise;
                
                if (mixedDist < bestDistance) {
                    bestDistance = mixedDist;
                    bestPatchIndex = i;
                }
            }

            int tileId = 4 + offset;
            if (bestPatchIndex >= 0) {
                const Patch& patch = patches[bestPatchIndex];
                if (bestDistance <= patch.highRadius) {
                    tileId = high[std::rand() % numHigh];
                } else if (bestDistance <= patch.medRadius) {
                    tileId = med[std::rand() % numMed];
                } else if (bestDistance <= patch.lowRadius) {
                    tileId = low[std::rand() % numLow];
                }
            }
            
            grid->SetTile(x, y, tileId);
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

