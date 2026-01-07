#ifndef PLANET_TREE_H
#define PLANET_TREE_H

#include "world/planet.h"

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

    ~PlanetTree();

    private:
    PlanetNode* root;
    int nextPlanetId;
    
    PlanetNode* FindPlanetRecursive(PlanetNode* node, int planetId);
    void DeleteNodeRecursive(PlanetNode* node);
    
    // Helper to find parent recursively
    PlanetNode* FindParentRecursive(PlanetNode* current, int targetPlanetId);
};

#endif // PLANET_TREE_H

