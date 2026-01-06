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

    ~PlanetTree();

    private:
    PlanetNode* root;
    int nextPlanetId;
    
    PlanetNode* FindPlanetRecursive(PlanetNode* node, int planetId);
    void DeleteNodeRecursive(PlanetNode* node);
};

#endif // PLANET_TREE_H

