#include "world/planet_tree.h"

// Constructor
PlanetTree::PlanetTree(Planet* p) {
    root = new PlanetNode(0, p);
    nextPlanetId = 1;
}

// Destructor
PlanetTree::~PlanetTree() {
    DeleteNodeRecursive(root);
    root = nullptr;
}

void PlanetTree::DeleteNodeRecursive(PlanetNode* node) {
    if (!node) return; 

    DeleteNodeRecursive(node->left);
    
    DeleteNodeRecursive(node->right);
    
    // Delete the planet object first, then the node
    if (node->planet) {
        delete node->planet;
    }
    delete node;
}

// Add a child planet to a parent - takes ownership of the planet pointer
int PlanetTree::AddChild(int parentId, Planet* childPlanet) {
    PlanetNode* parent = FindPlanet(parentId);
    if (!parent) return -1;

    if (parent->left && parent->right) return -1;

    int childId;
    if (!parent->left) {
        childId = 2 * parentId + 1;
        parent->left = new PlanetNode(childId, childPlanet);
    } else {
        childId = 2 * parentId + 2;
        parent->right = new PlanetNode(childId, childPlanet);
    }
    return childId;
}

PlanetNode* PlanetTree::FindPlanet(int planetId) {
    if (!root) return nullptr;
    if (planetId == 0) return root;
    
    PlanetNode* current = root;
    int currentId = 0;
    
    while (currentId != planetId) {
        int leftChildId = 2 * currentId + 1;
        int rightChildId = 2 * currentId + 2;

        if (planetId == leftChildId) {
            return current->left ? current->left : nullptr;
        }
        if (planetId == rightChildId) {
            return current->right ? current->right : nullptr;
        }

        int ancestor = planetId;
        while (ancestor > rightChildId) {
            int parent = ancestor / 2;
            if (ancestor % 2 == 0) {
                parent--;
            }
            ancestor = parent;
        }

        if (ancestor == leftChildId) {
            if (!current->left) return nullptr;
            current = current->left;
            currentId = leftChildId;
        } else {
            if (!current->right) return nullptr;
            current = current->right;
            currentId = rightChildId;
        }
    }
    
    return current;
}

PlanetNode* PlanetTree::GetParent(int planetId) {
    if (!root || planetId == 0) return nullptr;
    return FindParentRecursive(root, planetId);
}

PlanetNode* PlanetTree::FindParentRecursive(PlanetNode* current, int targetPlanetId) {
    if (!current) return nullptr;
    
    if (current->left && current->left->planetId == targetPlanetId) {
        return current;
    }
    if (current->right && current->right->planetId == targetPlanetId) {
        return current;
    }
    
    PlanetNode* result = FindParentRecursive(current->left, targetPlanetId);
    if (result) return result;
    
    return FindParentRecursive(current->right, targetPlanetId);
}

int PlanetTree::GetLeftChildId(int planetId) {
    PlanetNode* node = FindPlanet(planetId);
    if (node && node->left) {
        return node->left->planetId;
    }
    return -1;
}

int PlanetTree::GetRightChildId(int planetId) {
    PlanetNode* node = FindPlanet(planetId);
    if (node && node->right) {
        return node->right->planetId;
    }
    return -1;
}

void PlanetTree::GetAllPlanets(std::vector<Planet*>& planets) const {
    planets.clear();
    CollectPlanetsRecursive(root, planets);
}

void PlanetTree::CollectPlanetsRecursive(PlanetNode* node, std::vector<Planet*>& planets) const {
    if (!node || !node->planet) return;
    
    planets.push_back(node->planet);
    
    if (node->left) {
        CollectPlanetsRecursive(node->left, planets);
    }
    if (node->right) {
        CollectPlanetsRecursive(node->right, planets);
    }
}

void PlanetTree::PlanetIdToDepthIndex(int planetId, int& outDepth, int& outIndex) {
    if (planetId == 0) {
        outDepth = 0;
        outIndex = 0;
        return;
    }
    
    // Calculate depth: find which depth this ID belongs to
    // Depth d: IDs from (2^d - 1) to (2^(d+1) - 2)
    int depth = 0;
    int minId = 0;
    int maxId = 0;
    
    while (planetId > maxId) {
        depth++;
        minId = maxId + 1;
        maxId = (1 << (depth + 1)) - 2;
    }
    
    outDepth = depth;
    outIndex = planetId - minId;
}
