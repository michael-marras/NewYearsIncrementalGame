#ifndef PLANET_TREE_H
#define PLANET_TREE_H

#include "world/planet.h"
#include <vector>

struct PlanetNode {
    int planetId;
    Planet* planet;
    PlanetNode* left;
    PlanetNode* right;

    PlanetNode(int id, Planet* p) {
        planetId = id;
        planet = p;
        left = nullptr;
        right = nullptr;
    }
};

class PlanetTree {
    public:
    PlanetTree(Planet* p);

    int AddChild(int parentId, Planet* childPlanet);

    PlanetNode* FindPlanet(int planetId);
    
    // Get parent of a planet (returns nullptr if root or not found)
    PlanetNode* GetParent(int planetId);
    
    // Get left child planet ID (returns -1 if no left child)
    int GetLeftChildId(int planetId);
    
    // Get right child planet ID (returns -1 if no right child)
    int GetRightChildId(int planetId);
    
    /**
     * Collect all planets in the tree into a vector
     * @param planets Output vector to fill with all planets
     */
    void GetAllPlanets(std::vector<Planet*>& planets) const;
    
    /**
     * Convert planet ID to depth and index in binary tree layout
     * Binary tree layout: root ID 0, left child = 2*parent+1, right child = 2*parent+2
     * Depth d has IDs from (2^d - 1) to (2^(d+1) - 2), with indices 0 to (2^d - 1)
     * @param planetId The planet ID to convert
     * @param outDepth Output parameter for the depth (0 = root)
     * @param outIndex Output parameter for the index within that depth
     */
    static void PlanetIdToDepthIndex(int planetId, int& outDepth, int& outIndex);

    ~PlanetTree();

    private:
    PlanetNode* root;
    int nextPlanetId;
    
    PlanetNode* FindPlanetRecursive(PlanetNode* node, int planetId);
    void DeleteNodeRecursive(PlanetNode* node);
    
    // Helper to find parent recursively
    PlanetNode* FindParentRecursive(PlanetNode* current, int targetPlanetId);
    
    // Helper to collect all planets recursively
    void CollectPlanetsRecursive(PlanetNode* node, std::vector<Planet*>& planets) const;
};

#endif // PLANET_TREE_H

